#include "reader.h"

Tokeniser::Tokeniser(const std::string& line)
    : m_iter(line.begin()), m_end(line.end())
{
    nextToken();
}

std::string Tokeniser::peek() const
{
    assert(!eof() && "Reader::peek() reading past EOF\n");
    return m_token;
}

std::string Tokeniser::next()
{
    assert(!eof() && "Reader::next() reading past EOF\n");
    std::string token = peek();
    nextToken();
    return token;
}

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
}

void Tokeniser::skipWhitespace()
{
    static const std::regex whitespaces("[\\s,]+|;.*");

    while ( matchRegex(whitespaces) ) {
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

    m_token = regex_match.str(0);

    return true;
}

#define SYNTAX_TREE void*

static SYNTAX_TREE read_form(Tokeniser& tokeniser);
static SYNTAX_TREE read_atom(Tokeniser& tokeniser);
static void read_list(Tokeniser& tokeniser, SYNTAX_TREE TREE, const char closing_bracket);

static SYNTAX_TREE tokenize_string(const std::string& input)
{
    std::vector<std::string> tokens;
    Tokeniser tokeniser(input);

    while ( !tokeniser.eof() ) {
        tokens.push_back(tokeniser.next());
    }

    return nullptr;
}

// todo: pointer shenenigans, this is inefficient as fuck.
static SYNTAX_TREE read_form(Tokeniser& tokeniser)
{
    char token = tokeniser.peek()[0]; // probably sketchy idk

    switch ( token ) {
    case '[':
        tokeniser.next();
        read_list(tokeniser, nullptr, ']');
        return nullptr;
    case '(':
        tokeniser.next();
        read_list(tokeniser, nullptr, ')');
        return nullptr;
    case '{':
        tokeniser.next();
        read_list(tokeniser, nullptr, '}');
        return nullptr;
    default:
        return read_atom(tokeniser);
    }
}

static SYNTAX_TREE read_atom(Tokeniser& tokeniser)
{
    /**
     * atom values are:
     *
     * http://watson.latech.edu/book/objects/objectsFunctional.html#:~:text=There%20are%20two%20kinds%20of,that%20do%20not%20include%20spaces.
     *
     * There are two kinds of atoms in Lisp: numeric atoms and symbolic atoms.
     * Numeric atoms are simply numbers, both integer and real, like “27”or “3.14159”.
     * Symbolic atoms, also called symbols, consist of a string of
     * one or more characters that do not include spaces.
     */

     /**
      * todo:
      *
      * if number (-N, N, N.NN)
      *  integer or double?
      *
      * if operator (+ - * /)
      *
      * if string with/without spaces?
      *
      */
}

static void read_list(Tokeniser& tokeniser, void* SOME_LIST, const char closing_bracket)
{
    std::vector<std::string> list;
    while ( true ) {
        if ( tokeniser.peek()[0] == closing_bracket ) {
            tokeniser.next();
            return;
        }

        /*
        * list.push_back(readForm(tokeniser));
        * the list must be something else. need to call readForm recursively
        * to capture lists in lists...
        *
        * -> need a datatype (probably pointer?) to store the syntax tree
        *
        *

        For example, if the string is "(+ 2 (* 3 4))" then
        the read function will process this into a tree structure
        that looks like this:

                           List
                          / |  \
                         /  |   \
                        /   |    \
                    Sym:+  Int:2  List
                                 / |  \
                                /  |   \
                               /   |    \
                           Sym:*  Int:3  Int:4

        */
    }
}