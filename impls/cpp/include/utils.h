#ifndef UTILS_H
#define UTILS_H

#include "lisp_error.h"
#include <string>

char unescape(char c);
inline std::string escape(char c);
const std::string unescape(const std::string& in);
const std::string escape(const std::string& in);

int checkArgsIs(const std::string& name, int expected, int got);
int checkArgsBetween(const std::string& name, int min, int max, int got);
int checkArgsAtLeast(const std::string& name, int min, int got);
int checkArgsEven(const std::string& name, int got);

#endif // UTILS_H