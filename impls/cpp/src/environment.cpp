#include "environment.h"
#include <cassert>

Env::Env(EnvPtr outer,
       const std::vector<std::string>& bindings,
       AST_iter argsBegin, AST_iter argsEnd)
    : m_outer_env(outer)
{
    const int n = bindings.size();
    auto it = argsBegin;
    for ( int i = 0; i < n; ++i ) {
        if ( bindings[i] == "&" ) {
            assert(i == n - 2 && "There must be one parameter after the &");
            set(bindings[n-1], type::list(it, argsEnd));
            return;
        }

        assert(it != argsEnd && "Not enough parameters");
        set(bindings[i], *it);
        ++it;
    }

    assert(it == argsEnd && "Too many parameters");
}

AST Env::get(const std::string& symbol)
{
    for ( EnvPtr env = this; env; env = env->m_outer_env ) {
        auto it = env->m_map.find(symbol);
        if ( it != env->m_map.end() ) {
            return it->second;
        }
    }

    throw LISP_ERROR(symbol, " not found!");
}

EnvPtr Env::find(const std::string& symbol)
{
    for ( EnvPtr env = this; env; env = env->m_outer_env ) {
        if ( env->m_map.find(symbol) != env->m_map.end() ) {
            return env;
        }
    }

    return NULL;
}

AST Env::set(const std::string& symbol, AST value)
{
    m_map[symbol] = value;
    return value;
}

EnvPtr Env::getRoot()
{
    for ( EnvPtr env = this; env; env = env->m_outer_env ) {
        if ( !env->m_outer_env ) {
            return env;
        }
    }

    throw LISP_ERROR("this should never ever happen - env has no root");
}
