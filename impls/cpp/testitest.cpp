#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>

enum class testCase {
    STEP_0,
    STEP_1,
    STEP_2,
    STEP_3,
    STEP_4,
    STEP_5,
    STEP_6,
    STEP_7,
    STEP_8,
    STEP_9,
    STEP_10,
    ALL
};

std::vector<std::string> read_file(const std::string& filename)
{
    std::vector<std::string> input_vec;
    std::fstream file(filename);

    while ( !file.eof() ) {
        std::string line;
        file.getline(line);
        input_vec.push_back(line);
    }

    return input_vec;
}

void run_test(std::vector<std::string>& cases)
{
    auto it = cases.begin();

    while ( it != cases.end() ) {
        if ( it->front() == ';' ) {
            // comment or expected result
        }
        else {
            // pass testcase to testfile
        }
    }
}

int main(int argc, char* argv[])
{

    return 0;
}