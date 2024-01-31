#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "def.h"
#include "type_base.h"
#include "types.h"

#include <map>
#include <string>

class Env : public ReferenceCounter {
public:
    Env(EnvPtr outer = NULL) : m_outer(outer) { }
    Env(EnvPtr outer,
           const std::vector<std::string>& bindings,
           ValueIter argsBegin, ValueIter argsEnd);

    ~Env() { }

    ValuePtr get(const std::string& symbol);
    EnvPtr find(const std::string& symbol);
    ValuePtr set(const std::string& symbol, ValuePtr value);
    EnvPtr getRoot();

private:
    typedef std::map<std::string, ValuePtr> Map;
    Map m_map;
    EnvPtr m_outer;
};

#endif // ENVIRONMENT_H
