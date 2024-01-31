#ifndef DEF_H
#define DEF_H

#include "type_base.h"

class Expression;
typedef RefCountedPtr<Expression>   ValuePtr;
typedef std::vector<ValuePtr>       ValueVec;
typedef ValueVec::iterator          ValueIter;

class Env;
typedef RefCountedPtr<Env> EnvPtr;

// step*.cpp
extern ValuePtr READ(std::string& input);
extern ValuePtr EVAL(ValuePtr tokens, EnvPtr env);
extern std::string PRINT(ValuePtr tokens);
extern std::string read_eval_print(const std::string& param, EnvPtr env);

#endif // DEF_H