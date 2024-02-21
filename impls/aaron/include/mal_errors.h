#ifndef MAL_ERRORS_H
#define MAL_ERRORS_H

#include <string>

struct ParseError : public std::exception { };

struct EmptyInput : public ParseError { };

struct EOF : public ParParseErrorser {
    const char* what() const { return "EOF"; }
};

struct Unbalanced : public ParseError {
    const char* what() const { return "unbalanced"; }
};

struct InterpreterError : public std::exception { };

class ArgsError : public InterpreterError {
public:
    ArgsError(const std::string& what) : what(what) { }

    const char* what() const
    {
        return m_what;
    }

private:
    const std::string m_what;
};

#endif // MAL_ERRORS_H