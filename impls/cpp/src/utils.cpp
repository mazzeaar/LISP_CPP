#include "../include/utils.h"

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