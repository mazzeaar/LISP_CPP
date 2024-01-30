#include <iostream>
#include <string>

#include "reader.h"
#include "types.h"
#include "environment.h"

ValuePtr READ(std::string& input);
ValuePtr EVAL(ValuePtr tokens, EnvPtr env);
std::string PRINT(ValuePtr tokens);
std::string read_eval_print(const std::string& param, EnvPtr env);

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

    EnvPtr env;
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
    return ast->toString();
}

std::string read_eval_print(const std::string& input, EnvPtr env)
{
    return PRINT(EVAL(READ(input), env));
}