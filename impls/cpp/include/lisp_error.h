#ifndef LISP_ERROR_H
#define LISP_ERROR_H

#include <sstream>
#include <string>

inline std::string concatArgs(std::initializer_list<std::string> args)
{
    std::stringstream ss;
    for ( const auto& arg : args ) {
        ss << arg;
    }

    return ss.str();
}

#define LISP_ERROR(...) concatArgs({__VA_ARGS__})

// Base class for all Lisp interpreter exceptions
class LispException : public std::runtime_error {
public:
    explicit LispException(const std::string& message)
        : std::runtime_error(message) { }
};

// Specific error class for parser/tokenizer errors
class ParserError : public LispException {
public:
    explicit ParserError(const std::string& message)
        : LispException(message)
    { }
};

#endif // LISP_ERROR_H