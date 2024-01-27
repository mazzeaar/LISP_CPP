#include <iostream>
#include <string>

#include "../include/reader.h"

std::string READ(const std::string& param);
std::string EVAL(const std::string& param);
std::string PRINT(const std::string& param);
std::string REP(const std::string& param);

static std::shared_ptr<types::Object> tokenize_string(const std::string& input);
static std::shared_ptr<types::Object> read_form(Reader& tokeniser);
static std::shared_ptr<types::Object> read_atom(Reader& tokeniser);
static void read_list(Reader& tokeniser, std::shared_ptr<types::List> list, const char closing_bracket);

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
        //std::cout << REP(line) << std::endl;

        std::shared_ptr<types::Object> tokens;
        try {
            tokens = tokenize_string(line);
        }
        catch ( std::exception& e ) {
            std::cout << e.what() << std::endl;
            return 1;
        }

        std::cout << tokens->toString() << std::endl;
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

std::shared_ptr<types::Object> tokenize_string(const std::string& input)
{
    Reader tokeniser(input);
    std::shared_ptr<types::Object> tokens = read_form(tokeniser);

    return tokens;
}

std::shared_ptr<types::Object> read_form(Reader& tokeniser)
{
    if ( tokeniser.eof() ) {
        return nullptr;
    }

    std::string token = tokeniser.peek();

    switch ( token[0] ) {
        case '[':
        case '(':
        case '{': {
            char closing_bracket = (token[0] == '[') ? ']' : (token[0] == '(') ? ')' : '}';
            tokeniser.next(); // consume bracket
            std::shared_ptr<types::List> list(new types::List());
            read_list(tokeniser, list, closing_bracket);
            return list;
        }
        default:
            return read_atom(tokeniser);
    }
}

std::shared_ptr<types::Object> read_atom(Reader& tokeniser)
{
    std::string atom_value;
    atom_value = tokeniser.next(); // Consume and get the atom value
    return std::make_shared<types::Atom>(atom_value);
}


void read_list(Reader& tokeniser, std::shared_ptr<types::List> list, const char closing_bracket)
{
    while ( !tokeniser.eof() && tokeniser.peek() != std::string(1, closing_bracket) ) {
        list->push_back(read_form(tokeniser));
    }

    if ( tokeniser.eof() ) {
        throw error::eof(); // TODO: define errors
    }

    tokeniser.next(); // consume closing bracket
}