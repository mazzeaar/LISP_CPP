#include "../include/reader.h"

Reader::Reader(const std::string& line)
    : m_iter(line.begin()), m_end(line.end())
{
    nextToken();
}

std::string Reader::peek() const
{
    assert(!eof() && "Reader::peek() reading past EOF\n");
    return m_token;
}

std::string Reader::next()
{
    assert(!eof() && "Reader::next() reading past EOF\n");
    std::string token = peek();
    nextToken();
    return token;
}

void Reader::nextToken()
{
    m_iter += m_token.size();
    skipWhitespace();

    if ( eof() ) {
        return;
    }

    for ( const std::regex& token_regex : TOKEN_REGEXES ) {
        if ( matchRegex(token_regex) ) {
            return;
        }
    }
}

void Reader::skipWhitespace()
{
    static const std::regex whitespace_regex("[\\s,]+|;.*");
    while ( matchRegex(whitespace_regex) ) {
        m_iter += m_token.size();
    }
}

bool Reader::matchRegex(const std::regex& regex)
{
    if ( eof() ) {
        return false;
    }

    std::smatch regex_match;
    if ( !std::regex_search(m_iter, m_end, regex_match, regex, std::regex_constants::match_continuous) ) {
        return false;
    }

    assert(regex_match.size() == 1 && "Should only have one submatch!\n");
    assert(regex_match.length(0) > 0 && "Match is empty!\n");
    assert(regex_match.length(0) > 0 && "Matched an empty string\n");

    m_token = regex_match.str(0);

    return true;
}

// reader.cpp
static std::shared_ptr<types::Object> tokenize_string(const std::string& input);
static std::shared_ptr<types::Object> read_form(Reader& tokeniser);
static std::shared_ptr<types::Object> read_atom(Reader& tokeniser);
static void read_list(Reader& tokeniser, std::shared_ptr<types::List> list, const char closing_bracket);

std::shared_ptr<types::Object> tokenize_string(const std::string& input)
{
    Reader tokeniser(input);
    std::shared_ptr<types::Object> tokens = read_form(tokeniser);

    return tokens;
}

std::shared_ptr<types::Object> read_form(Reader& tokeniser)
{
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
    //static const std::regex string_regex("\"(?:\\.|[^\\\"])*\"?");
    //static const std::regex integer_regex("^[-+]?\\d+$");

    std::string atom_value;
    atom_value = tokeniser.next(); // Consume and get the atom value
    return std::make_shared<types::Atom>(atom_value);
}


void read_list(Reader& tokeniser, std::shared_ptr<types::List> list, const char closing_bracket)
{
    while ( tokeniser.peek() != std::string(1, closing_bracket) ) {
        list->push_back(read_form(tokeniser));
    }

    tokeniser.next(); // consume closing bracket
}