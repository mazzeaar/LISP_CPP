#include "tokenizer.h"
#include "lisp_error.h"

void Tokeniser::nextToken()
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
        throw LISP_ERROR("unbalanced");
    }
    else {
        throw LISP_ERROR("unexpected: " + mismatch);
    }
}

void Tokeniser::skipWhitespace()
{
    static const std::regex whitespace_regex("[\\s,]+|;.*");
    while ( matchRegex(whitespace_regex) ) {
        m_iter += m_token.size();
    }
}

bool Tokeniser::matchRegex(const std::regex& regex)
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