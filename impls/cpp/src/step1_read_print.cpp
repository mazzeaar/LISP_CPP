#include <iostream>
#include <string>

#include "../include/reader.h"
#include "../include/types.h"

ValuePtr READ(const std::string& input);
ValuePtr EVAL(ValuePtr tokens);
std::string PRINT(ValuePtr tokens);
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
        catch ( ParseException& e ) {
            out = e.what();
        }

        std::cout << out << std::endl;
    }

    return 0;
}

ValuePtr READ(const std::string& input)
{
    return tokenize_string(input);
}

ValuePtr EVAL(ValuePtr tokens)
{
    return tokens;
}

std::string PRINT(ValuePtr tokens)
{
    return tokens->toString(true);
}

std::string REP(const std::string& input)
{
    return PRINT(EVAL(READ(input)));
}