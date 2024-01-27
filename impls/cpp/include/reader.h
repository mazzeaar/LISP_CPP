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
    Reader(const std::string& line);

    std::string peek() const;
    std::string next();
    inline bool eof() const { return m_iter == m_end; }

private:
    void nextToken();
    void skipWhitespace();
    bool matchRegex(const std::regex& regex);

    std::string m_token;
    std::string::const_iterator m_iter;
    std::string::const_iterator m_end;
};

#endif // READER_H