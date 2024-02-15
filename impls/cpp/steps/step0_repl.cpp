#include <iostream>
#include <string>

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

int main()
{
    const std::string prompt = "user> ";
    std::string line;

    while ( true ) {
        std::cout << prompt;
        std::getline(std::cin, line);
        std::cout << REP(line) << std::endl;
    }

    return 0;
}