#include "utils.h"

int checkArgsIs(const std::string& name, int expected, int got)
{
    if ( got != expected ) {
        throw LISP_ERROR("\"", name, "\" expects ", std::to_string(expected), " args, ",
            std::to_string(got), " supplied");
    }

    return got;
}

int checkArgsBetween(const std::string& name, int min, int max, int got)
{
    if ( got < min || got > max ) {
        throw LISP_ERROR("\"", name, "\" expects between ", std::to_string(min),
         " and ", std::to_string(max), " args, ",
            std::to_string(got), " supplied");
    }

    return got;
}

int checkArgsAtLeast(const std::string& name, int min, int got)
{
    if ( got < min ) {
        throw LISP_ERROR("\"", name, "\" expects at least ", std::to_string(min), " args, ",
            std::to_string(got), " supplied");
    }

    return got;
}

int checkArgsEven(const std::string& name, int got)
{
    if ( got % 2 != 0 ) {
        throw LISP_ERROR("\"", name, "\" expects even number of args");
    }

    return got;
}

inline char unescape(char c)
{
    switch ( c ) {
        case '\\':  return '\\';
        case 'n':   return '\n';
        case '"':   return '"';
        default:    return c;
    }
}

inline std::string escape(char c)
{
    switch ( c ) {
        case '\\': return "\\\\";
        case '\n': return "\\n";
        case '"':  return "\\\"";
        default:   return std::string(1, c);
    };
}

const std::string unescape(const std::string& in)
{
    std::string out;
    out.reserve(in.size()); // unescaped string will be shorter

    for ( auto it = in.begin() + 1, end = in.end() - 1; it != end; ++it ) {
        char c = *it;
        if ( c == '\\' ) {
            ++it;
            if ( it != end ) {
                out += unescape(*it);
            }
        }
        else {
            out += c;
        }
    }

    out.shrink_to_fit();
    return out;
}

const std::string escape(const std::string& in)
{
    std::string out;
    out.reserve(in.size() * 2 + 2); // each char may get escaped + two "'s
    out += '"';
    for ( auto it = in.begin(), end = in.end(); it != end; ++it ) {
        out += escape(*it);
    }

    out += '"';
    out.shrink_to_fit();
    return out;
}