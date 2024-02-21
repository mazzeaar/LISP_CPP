#ifndef VALIDATION_H
#define VALIDATION_H

#include <string>
#include "mal_errors.h"

extern int checkArgsIs(const std::string& name, int expected, int got);
extern int checkArgsBetween(const std::string& name, int min, int max, int got);
extern int checkArgsAtLeast(const std::string& name, int min, int got);
extern int checkArgsEven(const std::string& name, int got);

#endif // VALIDATION_H