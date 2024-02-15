#ifndef DEF_H
#define DEF_H

#include "type_base.h"

class Expression;
typedef RefCountedPtr<Expression>   AST;
typedef std::vector<AST>            AST_vec;
typedef AST_vec::iterator           AST_iter;

class Env;
typedef RefCountedPtr<Env>          EnvPtr;

// step*.cpp
extern AST READ(std::string& input);
extern AST EVAL(AST tokens, EnvPtr env);
extern std::string PRINT(AST tokens);
extern std::string rep(const std::string& param, EnvPtr env);
extern AST APPLY(AST op, AST_iter argsBegin, AST_iter argsEnd);

#endif // DEF_H