#ifndef UTILS_H
#define UTILS_H

char unescape(char c);
inline std::string escape(char c);
const std::string unescape(const std::string& in);
const std::string escape(const std::string& in);

#endif // UTILS_H