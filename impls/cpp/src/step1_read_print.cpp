#include <iostream>
#include <string>

#include "../include/reader.h"
#include "../include/types.h"

ValuePtr READ(const std::string& input);
ValuePtr EVAL(const ValuePtr& tokens);
void PRINT(const ValuePtr& tokens);
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
        PRINT(EVAL(READ(line)));
    }

    return 0;
}

ValuePtr READ(const std::string& input)
{
    ValuePtr tokenized_input;

    try {
        tokenized_input = tokenize_string(input);
    }
    catch ( ParseException& e ) {
        return type::error(e);
    }

    return tokenized_input;
}

ValuePtr EVAL(const ValuePtr& tokens)
{
    return tokens;
}

void PRINT(const ValuePtr& tokens)
{
    std::cout << tokens << std::endl;
}

std::string REP(const std::string& param)
{
    return "";
}