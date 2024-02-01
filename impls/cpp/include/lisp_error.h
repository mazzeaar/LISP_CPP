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

#endif // LISP_ERROR_H