#include <iostream>
#include <string>

#include "../include/reader.h"
#include "../include/types.h"

types::ValuePtr READ(const std::string& input);
types::ValuePtr EVAL(const types::ValuePtr& tokens);
void PRINT(const types::ValuePtr& tokens);
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

types::ValuePtr READ(const std::string& input)
{
    types::ValuePtr tokenized_input;

    try {
        tokenized_input = tokenize_string(input);
    }
    catch ( ParseException& e ) {
        return types::ValuePtr(new types::Atom(e.what()));
    }

    return tokenized_input;
}

types::ValuePtr EVAL(const types::ValuePtr& tokens)
{
    return tokens;
}

void PRINT(const types::ValuePtr& tokens)
{
    std::cout << tokens << std::endl;
}

std::string REP(const std::string& param)
{
    return "";
}