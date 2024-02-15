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

static const char* malFunctionTable[] = {
    "(def! not (fn* (cond) (if cond false true)))"
};

static void installFunctions(EnvPtr env)
{
    for ( auto& function : malFunctionTable ) {
        rep(function, env);
    }
}

bool read_line(const std::string& prompt, std::string& line)
{
    std::cout << prompt;

    if ( !std::getline(std::cin, line) ) {
        std::cout << "exiting...\n";
        return false;
    }

    return true;
}

int main()
{
    const std::string prompt = "user> ";
    std::string line;

    installCore(rootEnv);
    installFunctions(rootEnv);
    while ( read_line(prompt, line) ) {
        std::string out = "";

        try {
            out = rep(line, rootEnv);
        }
        catch ( EmptyInputException& e ) {
            continue;
        }
        catch ( std::string& s ) {
            out = s;
        }

        std::cout << out << std::endl;
    }

    return 0;
}

AST EVAL(AST ast, EnvPtr env)
{
    //std::cout << "EVAL: " << ast->toString(true) << std::endl;
    if ( !env ) {
        env = rootEnv;
    }

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

        if ( special == "let*" ) {
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

            return EVAL(list->item(2), inner);
        }

        if ( special == "do" ) {
            if ( argCount < 1 ) {
                throw LISP_ERROR("\"do\" expects at least 1 arg, got" + std::to_string(argCount));
            }

            for ( int i = 1; i < argCount; i++ ) {
                EVAL(list->item(i), env);
            }

            return EVAL(list->item(argCount), env);
        }

        if ( special == "fn*" ) {
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

        if ( special == "if" ) {
            if ( argCount < 2 || argCount > 3 ) {
                throw LISP_ERROR("\"if\" expects 2-3 args, got" + std::to_string(argCount));
            }

            bool isTrue = EVAL(list->item(1), env)->isTrue();
            if ( !isTrue && argCount == 2 ) {
                return type::nilValue();
            }

            return EVAL(list->item(isTrue ? 2 : 3), env);
        }
    }

    std::unique_ptr<AST_vec> items(list->evalItems(env));
    AST op = items->at(0);
    if ( const Lambda* lambda = DYNAMIC_CAST(Lambda, op) ) {
        //return EVAL(lambda->getBody(), lambda->makeEnv(items->begin() + 1, items->end()));
        return lambda->apply(items->begin() + 1, items->end());
    }
    else {
        return APPLY(op, items->begin() + 1, items->end());
    }
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

std::string rep(const std::string& input, EnvPtr env)
{
    return PRINT(EVAL(READ(input), env));
}
