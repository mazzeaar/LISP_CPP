#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cassert>
#include <string>
#include <regex>

static const std::regex TOKEN_REGEXES[] = {
    std::regex("~@"),
    std::regex("[\\[\\]{}()'`~^@]"),
    std::regex("\"(?:\\\\.|[^\\\\\"])*\""),
    std::regex("[^\\s\\[\\]{}('\"`,;)]+"),
};

class Tokeniser {
public:
    Tokeniser(const std::string& line)
        : m_iter(line.begin()), m_end(line.end())
    {
        nextToken();
    }

    inline std::string peek() const
    {
        assert(!eof() && "Tokeniser::peek() reading past EOF\n");
        return m_token;
    }

    inline std::string next()
    {
        assert(!eof() && "Tokeniser::next() reading past EOF\n");
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

#endif // TOKENIZER_H