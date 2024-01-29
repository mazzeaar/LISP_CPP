#ifndef READER_H
#define READER_H

#include <cassert>
#include <string>
#include <regex>
#include <memory>

#include "../include/types.h"

static const std::regex TOKEN_REGEXES[] = {
    std::regex("~@"),
    std::regex("[\\[\\]{}()'`~^@]"),
    std::regex("\"(?:\\\\.|[^\\\\\"])*\""),
    std::regex("[^\\s\\[\\]{}('\"`,;)]+"),
};

class Reader {
public:
    Reader(const std::string& line)
        : m_iter(line.begin()), m_end(line.end())
    {
        nextToken();
    }

    std::string peek() const
    {
        assert(!eof() && "Reader::peek() reading past EOF\n");
        return m_token;
    }

    std::string next()
    {
        assert(!eof() && "Reader::next() reading past EOF\n");
        std::string token = peek();
        nextToken();
        return token;
    }

    inline bool eof() const { return m_iter == m_end; }

private:
    void nextToken();
    void skipWhitespace();
    bool matchRegex(const std::regex& regex);

    std::string m_token;
    std::string::const_iterator m_iter;
    std::string::const_iterator m_end;
};

types::ValuePtr read_atom(Reader& reader);
types::ValuePtr read_form(Reader& reader);
types::ValuePtr tokenize_string(const std::string& input);
void read_list(Reader& reader, types::ValueVec* items, char closing_bracket);

#endif // READER_H