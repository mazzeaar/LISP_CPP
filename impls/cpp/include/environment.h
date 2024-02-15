#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "def.h"
#include "type_base.h"
#include "types.h"

#include <map>
#include <string>

class Env : public ReferenceCounter {
public:
    Env(EnvPtr outer = NULL) : m_outer_env(outer) { }
    Env(EnvPtr outer,
           const std::vector<std::string>& bindings,
           AST_iter argsBegin, AST_iter argsEnd);

    ~Env() { }

    AST get(const std::string& symbol);
    EnvPtr find(const std::string& symbol);
    AST set(const std::string& symbol, AST value);
    EnvPtr getRoot();

private:
    typedef std::map<std::string, AST> Map;
    Map m_map;
    EnvPtr m_outer_env;
};

#endif // ENVIRONMENT_H