#include <iostream>
#include <string>

#include "def.h"
#include "parser.h"
#include "types.h"
#include "environment.h"
#include "lisp_error.h"

static BuiltIn::ApplyFunc builtIn_add;
static BuiltIn::ApplyFunc builtIn_sub;
static BuiltIn::ApplyFunc builtIn_mul;
static BuiltIn::ApplyFunc builtIn_div;
static BuiltIn::ApplyFunc fib;

static EnvPtr rootEnv(new Env);

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

    rootEnv->set("+", type::builtin("+", &builtIn_add));
    rootEnv->set("-", type::builtin("-", &builtIn_sub));
    rootEnv->set("*", type::builtin("+", &builtIn_mul));
    rootEnv->set("/", type::builtin("/", &builtIn_div));
    rootEnv->set("fib", type::builtin("fib", &fib));

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

AST READ(const std::string& input)
{
    return tokenize_string(input);
}

AST APPLY(AST op, AST_iter argsBegin, AST_iter argsEnd)
{
    const Applicable* handler = dynamic_cast<Applicable*>(op.ptr());

    if ( handler == NULL ) {
        throw LISP_ERROR(op->toString(true), " not applicable");
    }

    return handler->apply(argsBegin, argsEnd);
}

AST EVAL(AST ast, EnvPtr env)
{
    if ( env == nullptr ) {
        env = rootEnv;
    }

    const List* list = dynamic_cast<List*>(ast.ptr());
    if ( !list || list->count() == 0 ) {
        return ast->eval(env);
    }

    if ( const Symbol* symbol = dynamic_cast<Symbol*>(list->item(0).ptr()) ) {
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
    }

    std::unique_ptr<AST_vec> items(list->evalItems(env));
    AST op = items->at(0);
    return APPLY(op, items->begin() + 1, items->end());
}

std::string PRINT(AST ast)
{
    return ast->toString(true);
}

std::string rep(const std::string& input, EnvPtr env)
{
    return PRINT(EVAL(READ(input), env));
}

static AST fib(const std::string& name,
    AST_iter argsBegin, AST_iter argsEnd)
{
    if ( int dist = std::distance(argsBegin, argsEnd); dist != 1 ) {
        throw LISP_ERROR("\"", name, "\" expects 1 args, ",
            std::to_string(dist), " supplied");
    }

    Integer* arg = VALUE_CAST(Integer, *argsBegin++);
    int n = arg->value();

    if ( n < 0 ) {
        throw LISP_ERROR("Fibonacci number cannot be negative");
    }

    else if ( n == 0 ) return type::integer(0);
    else if ( n == 1 ) return type::integer(1);

    AST_vec args1 = { type::integer(n - 1) };
    AST_vec args2 = { type::integer(n - 2) };

    AST fib_n_1 = APPLY(type::builtin("fib", &fib), args1.begin(), args1.end());
    AST fib_n_2 = APPLY(type::builtin("fib", &fib), args2.begin(), args2.end());

    return type::integer(VALUE_CAST(Integer, fib_n_1)->value()
                       + VALUE_CAST(Integer, fib_n_2)->value());
}

static AST builtIn_add(const std::string& name,
    AST_iter argsBegin, AST_iter argsEnd)
{
    if ( int dist = std::distance(argsBegin, argsEnd); dist != 2 ) {
        throw LISP_ERROR("\"", name, "\" expects 2 args, ",
            std::to_string(dist), " supplied");
    }

    Integer* lhs = VALUE_CAST(Integer, *argsBegin++);
    Integer* rhs = VALUE_CAST(Integer, *argsBegin++);

    return type::integer(lhs->value() + rhs->value());
}

static AST builtIn_sub(const std::string& name,
    AST_iter argsBegin, AST_iter argsEnd)
{
    int argCount = std::distance(argsBegin, argsEnd);
    if ( argCount < 1 || argCount > 2 ) {
        throw LISP_ERROR("\"", name, "\" expects between 1 and 2 args, ",
            std::to_string(argCount), " supplied");
    }

    Integer* lhs = VALUE_CAST(Integer, *argsBegin++);
    if ( argCount == 1 ) {
        return type::integer(-lhs->value());
    }

    Integer* rhs = VALUE_CAST(Integer, *argsBegin++);
    return type::integer(lhs->value() - rhs->value());
}

static AST builtIn_mul(const std::string& name,
    AST_iter argsBegin, AST_iter argsEnd)
{
    if ( int dist = std::distance(argsBegin, argsEnd); dist != 2 ) {
        throw LISP_ERROR("\"", name, "\" expects 2 args, ", std::to_string(dist), " supplied");
    }

    Integer* lhs = VALUE_CAST(Integer, *argsBegin++);
    Integer* rhs = VALUE_CAST(Integer, *argsBegin++);
    return type::integer(lhs->value() * rhs->value());
}

static AST builtIn_div(const std::string& name,
    AST_iter argsBegin, AST_iter argsEnd)
{
    if ( int dist = std::distance(argsBegin, argsEnd); dist != 2 ) {
        throw LISP_ERROR("\"", name, "\" expects 2 args, ", std::to_string(dist), " supplied");
    }

    Integer* lhs = VALUE_CAST(Integer, *argsBegin++);
    Integer* rhs = VALUE_CAST(Integer, *argsBegin++);

    if ( rhs->value() == 0 ) {
        throw LISP_ERROR("Division by zero");
    }

    return type::integer(lhs->value() / rhs->value());
}