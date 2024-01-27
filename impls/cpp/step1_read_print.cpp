#include <iostream>
#include <string>
#include "reader.cpp"

std::string READ(const std::string& param);
std::string EVAL(const std::string& param);
std::string PRINT(const std::string& param);
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
        std::cout << REP(line) << std::endl;
    }

    return 0;
}

std::string READ(const std::string& param)
{
    return param;
}

std::string EVAL(const std::string& param)
{
    return param;
}

std::string PRINT(const std::string& param)
{
    return param;
}

std::string REP(const std::string& param)
{
    return PRINT(EVAL(READ(param)));
}