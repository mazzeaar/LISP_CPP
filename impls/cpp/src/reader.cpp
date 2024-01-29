#include "../include/reader.h"

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

types::ValuePtr tokenize_string(const std::string& input)
{
    Reader reader(input);

    if ( reader.eof() ) {
        throw ParseException("empty line!", "EOF");
    }

    return read_form(reader);
}

types::ValuePtr read_form(Reader& reader)
{
    assert(!reader.eof() && "read_form failed - reader reached EOF\n");

    char token = reader.peek().front();
    switch ( token ) {
        case '(': {
            reader.next();
            std::shared_ptr<types::ValueVec> list(new types::ValueVec);
            read_list(reader, list.get(), ')');
            return types::ValuePtr(new types::List(list.get()));
        }
                //case '[': // vector
                //case '{': // hash 
        default:
            return read_atom(reader);
    }
}

types::ValuePtr read_atom(Reader& reader)
{
    if ( reader.eof() ) {
        throw ParseException("huh?");
    }

    std::string atom_value;
    atom_value = reader.next();
    return types::ValuePtr(new types::Atom(atom_value));
}

void read_list(Reader& reader, types::ValueVec* list, char closing_bracket)
{
    while ( !reader.eof() && reader.peek() != std::string(1, closing_bracket) ) {
        list->push_back(read_form(reader));
    }

    if ( reader.eof() ) {
        throw ParseException("EOF", "unbalanced ')");
    }

    reader.next(); // consume bracket
}