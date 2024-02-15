#include <iostream>
#include <string>

#include "def.h"
#include "parser.h"
#include "types.h"
#include "environment.h"
#include "lisp_error.h"
#include "core.h"

static EnvPtr rootEnv(new Env);

AST READ(const std::string& input);
std::string PRINT(AST ast);
std::string rep(const std::string& input, EnvPtr env);
std::string safe_rep(const std::string& input, EnvPtr env);
AST quasiquote(AST obj);

static const char* malFunctionTable[] = {
    "(def! not (fn* (cond) (if cond false true)))",
    "(def! load-file (fn* (filename) \
        (eval (read-string (str \"(do \" (slurp filename) \"\nnil)\")))))",
    "(def! fib (fn* [n] (if (= n 0) 1 (if (= n 1) 1 (+ (fib (-n 1)) (fib(-n 2)))))))"
};

bool read_line(const std::string& prompt, std::string& line)
{
    std::cout << prompt;

    if ( !std::getline(std::cin, line) ) {
        std::cout << "exiting...\n";
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    const std::string prompt = "user> ";
    std::string line;

    installCore(rootEnv);

    // install functions
    for ( auto& function : malFunctionTable ) {
        rep(function, rootEnv);
    }

    // make argv
    AST_vec* args = new AST_vec();
    for ( int i = 2; i < argc; ++i ) {
        args->push_back(type::string(argv[i]));
    }
    rootEnv->set("*ARGV*", type::list(args));

    if ( argc > 1 ) {
        const std::string input = "(load-file " + escape(argv[1]) + ")";
        safe_rep(input, rootEnv);
        return 0;
    }

    while ( read_line(prompt, line) ) {
        std::string out = safe_rep(line, rootEnv);
        if ( out.length() > 0 ) {
            std::cout << out << std::endl;
        }
    }

    return 0;
}

AST EVAL(AST ast, EnvPtr env)
{
    if ( !env ) {
        env = rootEnv;
    }
    while ( true ) {
        const List* list = DYNAMIC_CAST(List, ast);
        if ( !list  || list->count() == 0 ) {
            return ast->eval(env);
        }

        if ( const Symbol* symbol = DYNAMIC_CAST(Symbol, list->item(0)) ) {
            std::string special = symbol->value();
            int argCount = list->count() - 1;

            if ( special == "def!" ) {
                if ( argCount != 2 ) {
                    throw LISP_ERROR("\"def!\" expects 2 args, got" + std::to_string(argCount));
                }

                const Symbol* id = VALUE_CAST(Symbol, list->item(1));
                return env->set(id->value(), EVAL(list->item(2), env));
            }
            else if ( special == "let*" ) {
                if ( argCount != 2 ) {
                    throw LISP_ERROR("\"let*\" expects 2 args, got" + std::to_string(argCount));
                }

                const Sequence* bindings = VALUE_CAST(Sequence, list->item(1));

                int count = bindings->count();
                if ( count % 2 != 0 ) {
                    throw LISP_ERROR("\"let*\" expects an even number of args");
                }

                EnvPtr inner(new Env(env));
                for ( int i = 0; i < count; i += 2 ) {
                    const Symbol* var = VALUE_CAST(Symbol, bindings->item(i));
                    inner->set(var->value(), EVAL(bindings->item(i + 1), inner));
                }
                ast = list->item(2);
                env = inner;
                continue;
            }
            else if ( special == "do" ) {
                if ( argCount < 1 ) {
                    throw LISP_ERROR("\"do\" expects at least 1 arg, got" + std::to_string(argCount));
                }

                for ( int i = 1; i < argCount; i++ ) {
                    EVAL(list->item(i), env);
                }
                ast = list->item(argCount);
                continue;
            }
            else if ( special == "fn*" ) {
                if ( argCount != 2 ) {
                    throw LISP_ERROR("\"fn*\" expects 2 args, got" + std::to_string(argCount));
                }

                const Sequence* bindings = VALUE_CAST(Sequence, list->item(1));
                std::vector<std::string> params;
                for ( int i = 0; i < bindings->count(); i++ ) {
                    const Symbol* sym = VALUE_CAST(Symbol, bindings->item(i));
                    params.push_back(sym->value());
                }

                return type::lambda(params, list->item(2), env);
            }
            else if ( special == "if" ) {
                if ( argCount < 2 || argCount > 3 ) {
                    throw LISP_ERROR("\"if\" expects 2-3 args, got" + std::to_string(argCount));
                }

                bool isTrue = EVAL(list->item(1), env)->isTrue();
                if ( !isTrue && argCount == 2 ) {
                    return type::nilValue();
                }
                ast = list->item(isTrue ? 2 : 3);
                continue;
            }

            if ( special == "quasiquoteexpand" ) {
                checkArgsIs("quasiquote", 1, argCount);
                return quasiquote(list->item(1));
            }

            if ( special == "quasiquote" ) {
                checkArgsIs("quasiquote", 1, argCount);
                ast = quasiquote(list->item(1));
                continue;
            }

            if ( special == "quote" ) {
                checkArgsIs("quote", 1, argCount);
                return list->item(1);
            }
        }

        std::unique_ptr<AST_vec> items(list->evalItems(env));
        AST op = items->at(0);
        if ( const Lambda* lambda = DYNAMIC_CAST(Lambda, op) ) {
            ast = lambda->getBody();
            env = lambda->makeEnv(items->begin() + 1, items->end());
            continue;
        }
        else {
            return APPLY(op, items->begin() + 1, items->end());
        }
    }
}

static bool isSymbol(AST obj, const std::string& text)
{
    const Symbol* sym = DYNAMIC_CAST(Symbol, obj);
    return sym && (sym->value() == text);
}

//  Return arg when ast matches ('sym, arg), else NULL.
static AST starts_with(const AST ast, const char* sym)
{
    const List* list = DYNAMIC_CAST(List, ast);
    if ( !list || list->isEmpty() || !isSymbol(list->item(0), sym) )
        return NULL;
    checkArgsIs(sym, 1, list->count() - 1);
    return list->item(1);
}

AST quasiquote(AST obj)
{
    if ( DYNAMIC_CAST(Symbol, obj) || DYNAMIC_CAST(Hash, obj) )
        return type::list(type::symbol("quote"), obj);

    const Sequence* seq = DYNAMIC_CAST(Sequence, obj);
    if ( !seq )
        return obj;

    const AST unquoted = starts_with(obj, "unquote");
    if ( unquoted )
        return unquoted;

    AST res = type::list(new AST_vec(0));
    for ( int i = seq->count()-1; 0<=i; i-- ) {
        const AST elt = seq->item(i);
        const AST spl_unq = starts_with(elt, "splice-unquote");
        if ( spl_unq )
            res = type::list(type::symbol("concat"), spl_unq, res);
        else
            res = type::list(type::symbol("cons"), quasiquote(elt), res);
    }
    if ( DYNAMIC_CAST(Vector, obj) )
        res = type::list(type::symbol("vec"), res);
    return res;
}

AST READ(const std::string& input)
{
    return tokenize_string(input);
}

std::string PRINT(AST ast)
{
    return ast->toString(true);
}

AST APPLY(AST op, AST_iter argsBegin, AST_iter argsEnd)
{
    const Applicable* handler = DYNAMIC_CAST(Applicable, op);

    if ( handler == NULL ) {
        throw LISP_ERROR(op->toString(true), " not applicable");
    }

    return handler->apply(argsBegin, argsEnd);
}

std::string safe_rep(const std::string& input, EnvPtr env)
{
    try {
        return rep(input, env);
    }
    catch ( EmptyInputException& e ) {
        return "";
    }
    catch ( std::string& s ) {
        return s;
    };
}

std::string rep(const std::string& input, EnvPtr env)
{
    return PRINT(EVAL(READ(input), env));
}