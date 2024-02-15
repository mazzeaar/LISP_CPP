#include <iostream>
#include <string>

#include "parser.h"
#include "types.h"
#include "def.h"
#include "environment.h"

std::string REP(const std::string& param);

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

    while ( read_line(prompt, line) ) {
        std::string out = "";
        try {
            out = REP(line);
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

AST EVAL(AST tokens)
{
    return tokens;
}

std::string PRINT(AST tokens)
{
    return tokens->toString(true);
}

std::string REP(const std::string& input)
{
    return PRINT(EVAL(READ(input)));
}

// for the linker ^^
AST EVAL(AST ast, EnvPtr env)
{
    return type::nilValue();
}

AST APPLY(AST op, AST_iter argsBegin, AST_iter argsEnd)
{
    return type::nilValue();
}