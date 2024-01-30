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

    std::string mismatch(m_iter, m_end);
    if ( mismatch[0] == '"' ) {
        throw ParseException("unbalanced");
    }
    else {
        throw ParseException("unexpected '%s", mismatch.c_str());
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

ValuePtr tokenize_string(const std::string& input)
{
    Reader reader(input);

    if ( reader.eof() ) {
        throw ParseException("empty line!", "EOF");
    }

    return read_form(reader);
}

ValuePtr read_form(Reader& reader)
{
    assert(!reader.eof() && "read_form failed - reader reached EOF\n");

    const auto init_sequence = [&reader] (char closing_bracket) -> ValueVec*
    {
        reader.next();
        std::unique_ptr<ValueVec> items(new ValueVec);
        read_list(reader, items.get(), closing_bracket);
        return items.release();
    };

    switch ( reader.peek().front() ) {
        case '(': {
            return type::list(init_sequence(')'));
        }
        case '[': {
            return type::vector(init_sequence(']'));
        }
        case '{': {
            return type::hash(init_sequence('}'), false);
        }
        default:
            return read_atom(reader);
    }
}

ValuePtr read_atom(Reader& reader)
{
    static const std::unordered_map<std::string, ValuePtr> constantTable = {
        {"false", type::falseValue()},
        {"nil", type::nilValue()},
        {"true", type::trueValue()}
    };

    static const std::unordered_map<std::string, std::string> macroTable = {
        {"@", "deref"},
        {"`", "quasiquote"},
        {"'", "quote"},
        {"~@", "splice-unquote"},
        {"~", "unquote"}
    };

    std::string token = reader.next();
    if ( token[0] == '"' ) {
        return type::string(unescape(token));
    }

    if ( token[0] == ':' ) {
        return type::keyword(token);
    }

    if ( token == "^" ) {
        ValuePtr meta = read_form(reader);
        ValuePtr value = read_form(reader);

        return type::list(type::symbol("with-meta"), value, meta);
    }

    if ( auto constIt = constantTable.find(token); constIt != constantTable.end() ) {
        return constIt->second;
    }

    if ( auto macroIt = macroTable.find(token); macroIt != macroTable.end() ) {
        return process_macro(reader, macroIt->second);
    }

    static const std::regex int_regex("^[-+]?\\d+$");
    if ( std::regex_match(token, int_regex) ) {
        return type::integer(token);
    }

    return type::symbol(token);
}

void read_list(Reader& reader, ValueVec* items, char closing_bracket)
{
    while ( !reader.eof() && reader.peek() != std::string(1, closing_bracket) ) {
        items->push_back(read_form(reader));
    }

    if ( reader.eof() ) {
        throw ParseException("EOF", "unbalanced ')");
    }

    reader.next(); // consume bracket
}

ValuePtr process_macro(Reader& reader, const std::string& symbol)
{
    return type::list(type::symbol(symbol), read_form(reader));
}
