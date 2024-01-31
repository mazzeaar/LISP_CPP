#include <iostream>
#include <string>

#include "def.h"
#include "reader.h"
#include "types.h"
#include "environment.h"

static BuiltIn::ApplyFunc builtIn_add, builtIn_sub, builtIn_mul, builtIn_div;
static BuiltIn::ApplyFunc fib;

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

    EnvPtr env(new Env);
    env->set("+", type::builtin("+", &builtIn_add));
    env->set("-", type::builtin("-", &builtIn_sub));
    env->set("*", type::builtin("+", &builtIn_mul));
    env->set("/", type::builtin("/", &builtIn_div));
    env->set("fib", type::builtin("fib", &fib));

    while ( read_line(prompt, line) ) {
        std::string out = "";

        try {
            out = read_eval_print(line, env);
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

ValuePtr READ(const std::string& input)
{
    return tokenize_string(input);
}

ValuePtr EVAL(ValuePtr ast, EnvPtr env)
{
    return ast->eval(env);
}

std::string PRINT(ValuePtr ast)
{
    return ast->toString(true);
}

std::string read_eval_print(const std::string& input, EnvPtr env)
{
    return PRINT(EVAL(READ(input), env));
}

ValuePtr APPLY(ValuePtr op, ValueIter argsBegin, ValueIter argsEnd)
{
    const Applicable* handler = dynamic_cast<Applicable*>(op.ptr());

    if ( handler == NULL ) {
        throw op->toString(true) + " not applicable";
    }

    return handler->apply(argsBegin, argsEnd);
}

template<class T>
T* value_cast(ValuePtr obj, const char* typeName)
{
    T* dest = dynamic_cast<T*>(obj.ptr());
    if ( dest == NULL ) {
        throw obj->toString(true) + " != " + std::string(typeName);
    }
    return dest;
}

#define VALUE_CAST(Type, Value) value_cast<Type>(Value, #Type)
#define ARG(type, name)         type* name = VALUE_CAST(type, *argsBegin++)

void check_args_is(int expected, const std::string& name, ValueIter argsBegin, ValueIter argsEnd)
{
    int dist = std::distance(argsBegin, argsEnd);
    if ( dist != expected ) {
        throw "\"" + name + "\" expects "
            + std::to_string(expected)
            + "args, " + std::to_string(dist) + " supplied";
    }
}

int check_args_between(int min, int max, const std::string& name, ValueIter argsBegin, ValueIter argsEnd)
{
    int dist = std::distance(argsBegin, argsEnd);

    if ( dist < 1 || dist > 2 ) {
        throw "\"" + name + "\" expects between "
            + std::to_string(min) + " and " + std::to_string(max)
            + " args, " + std::to_string(dist) + " supplied";
    }

    return dist;
}

static ValuePtr fib(const std::string& name,
    ValueIter argsBegin, ValueIter argsEnd)
{
    check_args_is(1, name, argsBegin, argsEnd);

    ARG(Integer, arg);
    int n = arg->value();

    if ( n < 0 ) {
        throw "fibonacci number cannot be negative";
    }
    else if ( n == 0 ) return type::integer(0);
    else if ( n == 1 ) return type::integer(1);

    ValueVec args1 = { type::integer(n - 1) };
    ValueVec args2 = { type::integer(n - 2) };

    ValuePtr fib_n_1 = APPLY(type::builtin("fib", &fib), args1.begin(), args1.end());
    ValuePtr fib_n_2 = APPLY(type::builtin("fib", &fib), args2.begin(), args2.end());

    return type::integer(VALUE_CAST(Integer, fib_n_1)->value()
                       + VALUE_CAST(Integer, fib_n_2)->value());
}

static ValuePtr builtIn_add(const std::string& name,
    ValueIter argsBegin, ValueIter argsEnd)
{
    check_args_is(2, name, argsBegin, argsEnd);

    ARG(Integer, lhs);
    ARG(Integer, rhs);
    return type::integer(lhs->value() + rhs->value());
}

static ValuePtr builtIn_sub(const std::string& name,
    ValueIter argsBegin, ValueIter argsEnd)
{
    int argCount = check_args_between(1, 2, name, argsBegin, argsEnd);

    ARG(Integer, lhs);

    if ( argCount == 1 ) {
        return type::integer(-lhs->value());
    }

    ARG(Integer, rhs);
    return type::integer(lhs->value() - rhs->value());
}

static ValuePtr builtIn_mul(const std::string& name,
    ValueIter argsBegin, ValueIter argsEnd)
{
    check_args_is(2, name, argsBegin, argsEnd);

    ARG(Integer, lhs);
    ARG(Integer, rhs);
    return type::integer(lhs->value() * rhs->value());
}

static ValuePtr builtIn_div(const std::string& name,
    ValueIter argsBegin, ValueIter argsEnd)
{
    check_args_is(2, name, argsBegin, argsEnd);

    ARG(Integer, lhs);
    ARG(Integer, rhs);

    if ( rhs->value() == 0 ) {
        throw "Division by zero";
    }

    return type::integer(lhs->value() / rhs->value());
}