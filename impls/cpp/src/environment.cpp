#include "environment.h"
#include <cassert>

Env::Env(EnvPtr outer,
       const std::vector<std::string>& bindings,
       ValueIter argsBegin, ValueIter argsEnd)
    :m_outer(outer)
{
    const int n = bindings.size();
    ValueIter it = argsBegin;

    for ( int i = 0; i < n; ++i ) {
        if ( bindings[i] == "&" ) {
            assert(i == n - 2 && "There must be one parameter after the &");
            set(bindings[n-1], type::list(it, argsEnd));
        }

        assert(it != argsEnd && "Not enough parameters");
        set(bindings[i], *it);
        ++it;
    }

    assert(it == argsEnd && "Too many parameters");
}

ValuePtr Env::get(const std::string& symbol)
{

    for ( EnvPtr env = this; env; env = env->m_outer ) {
        auto it = env->m_map.find(symbol);
        if ( it != env->m_map.end() ) {
            return it->second;
        }
    }
    throw ("'%s' not found", symbol.c_str());
}

EnvPtr Env::find(const std::string& symbol)
{
    for ( EnvPtr env = this; env; env = env->m_outer ) {
        if ( env->m_map.find(symbol) != env->m_map.end() ) {
            return env;
        }
    }

    return NULL;
}

ValuePtr Env::set(const std::string& symbol, ValuePtr value)
{
    m_map[symbol] = value;
    return value;
}

EnvPtr Env::getRoot()
{
    for ( EnvPtr env = this; env; env = env->m_outer ) {
        if ( !env->m_outer ) {
            return env;
        }
    }

    throw std::string("this should never ever happen - env has no root");
}
