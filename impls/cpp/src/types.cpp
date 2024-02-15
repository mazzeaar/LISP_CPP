#include "types.h"

#include <cassert>

namespace type {
    AST macro(const Lambda& lambda)
    {
        return AST(new Lambda(lambda, true));
    }

    AST lambda(const std::vector<std::string>& bindings, AST body, EnvPtr env)
    {
        return AST(new Lambda(bindings, body, env));
    }

    AST builtin(const std::string& name, BuiltIn::ApplyFunc handler)
    {
        return AST(new BuiltIn(name, handler));
    }

    AST boolean(bool value)
    {
        return value ? trueValue() : falseValue();
    }

    AST integer(int64_t value)
    {
        return AST(new Integer(value));
    }

    AST integer(const std::string& token)
    {
        return integer(std::stoi(token));
    }

    AST keyword(const std::string& token)
    {
        return AST(new Keyword(token));
    }

    AST symbol(const std::string& token)
    {
        return AST(new Symbol(token));
    }

    AST falseValue()
    {
        static AST False(new Constant("false"));
        return AST(False);
    }

    AST nilValue()
    {
        static AST True(new Constant("nil"));
        return AST(True);
    }

    AST trueValue()
    {
        static AST Nil(new Constant("true"));
        return AST(Nil);
    }

    AST hash(AST_iter begin, AST_iter end, bool isEvaluated)
    {
        return AST(new Hash(begin, end, isEvaluated));
    }

    AST hash(const Hash::Map& map)
    {
        return AST(new Hash(map));
    }

    AST hash(AST_vec* items, bool isEvaluated)
    {
        return hash(items->begin(), items->end(), isEvaluated);
    }

    AST vector(AST_vec* items)
    {
        return AST(new Vector(items));
    }

    AST vector(AST_iter begin, AST_iter end)
    {
        return AST(new Vector(begin, end));
    }

    AST atom(AST value)
    {
        return AST(new Atom(value));
    }

    AST list(AST_vec* items)
    {
        return AST(new List(items));
    }

    AST list(AST_iter begin, AST_iter end)
    {
        return AST(new List(begin, end));
    }

    AST list(AST a)
    {
        AST_vec* items = new AST_vec(1);
        items->at(0) = a;
        return AST(new List(items));
    }

    AST list(AST a, AST b)
    {
        AST_vec* items = new AST_vec(2);
        items->at(0) = a;
        items->at(1) = b;
        return AST(new List(items));
    }

    AST list(AST a, AST b, AST c)
    {
        AST_vec* items = new AST_vec(3);
        items->at(0) = a;
        items->at(1) = b;
        items->at(2) = c;
        return AST(new List(items));
    }

    AST string(const std::string& token)
    {
        return AST(new String(token));
    }
} // namespace type


// ================================
// EXPRESSION
bool Expression::isEqualTo(const Expression* rhs) const
{
    bool types_match = (typeid(*this) == typeid(*rhs))
        || (dynamic_cast<const Sequence*>(this) && dynamic_cast<const Sequence*>(rhs));

    return types_match && ((*this) == rhs);
}

bool Expression::isTrue() const
{
    return this != type::falseValue().ptr()
        && this != type::nilValue().ptr();
}

AST Expression::eval(EnvPtr env)
{
    return AST(this);
}

AST Expression::withMeta(AST meta) const
{
    return doWithMeta(meta);
}

AST Expression::meta() const
{
    return m_meta.ptr() == NULL ? type::nilValue() : m_meta;
}

// ================================
// ATOM
const std::string Atom::toString(bool readably) const
{
    return "(atom " + m_atom->toString(readably) + ")";
}

bool Atom::operator==(const Expression* rhs) const
{
    return this->m_atom->isEqualTo(rhs);
}


// ================================
// CONSTANT
const std::string Constant::toString(bool readably) const
{
    return m_name;
}

bool Constant::operator==(const Expression* rhs) const
{
    return this == rhs;
}


// ================================
// INTEGER
bool Integer::operator==(const Expression* rhs) const
{
    return m_val == static_cast<const Integer*>(rhs)->m_val;
}

const std::string Integer::toString(bool readably) const
{
    return std::to_string(m_val);
}


// ================================
// STRING
const std::string String::toString(bool readably) const
{
    return readably ? escape(value()) : value();
}

bool String::operator==(const Expression* rhs) const
{
    return value() == static_cast<const String*>(rhs)->value();
}

std::string String::escapedValue() const
{
    return escape(value());
}


// ================================
// SEQUENCE
const std::string Sequence::toString(bool readably) const
{
    std::string res;
    AST_vec::const_iterator iter = m_items->cbegin();
    AST_vec::const_iterator end = m_items->cend();

    if ( iter != end ) {
        res += (*iter)->toString(readably);
        ++iter;
    }

    while ( iter != end ) {
        res += " ";
        res += (*iter)->toString(readably);
        ++iter;
    }

    return res;
}

bool Sequence::operator==(const Expression* rhs) const
{
    const Sequence* rhs_seq = static_cast<const Sequence*>(rhs);

    if ( count() != rhs_seq->count() ) {
        return false;
    }

    AST_iter this_it = m_items->begin(),
        rhs_it = rhs_seq->begin(),
        end = m_items->end();

    while ( this_it != end ) {
        if ( !(*this_it)->isEqualTo((*rhs_it).ptr()) ) {
            return false;
        }

        ++this_it;
        ++rhs_it;
    }

    return true;
}

AST_vec* Sequence::evalItems(EnvPtr env) const
{
    AST_vec* items = new AST_vec;;
    items->reserve(count());
    for ( AST_iter it = m_items->begin(); it != m_items->end(); ++it ) {
        items->push_back(EVAL(*it, env));
    }

    return items;
}

AST Sequence::first() const
{
    return count() == 0 ? type::nilValue() : this->item(0);
}

AST Sequence::rest() const
{
    AST_iter start = (count() > 0) ? begin() + 1 : end();
    return type::list(start, end());
}


// ================================
// KEYWORD
bool Keyword::operator==(const Expression* rhs) const
{
    return value() == static_cast<const Keyword*>(rhs)->value();
}


// ================================
// SYMBOL
bool Symbol::operator==(const Expression* rhs) const
{
    return value() == static_cast<const Symbol*>(rhs)->value();
}

AST Symbol::eval(EnvPtr env)
{
    return env->get(value());
}


// ================================
// LIST
const std::string List::toString(bool readably) const
{
    return '(' + Sequence::toString(readably) + ')';
}

AST List::eval(EnvPtr env)
{
    if ( count() == 0 ) {
        return AST(this);
    }

    std::unique_ptr<AST_vec> items(evalItems(env));
    auto it = items->begin();
    AST op = *it;
    return APPLY(op, ++it, items->end());
}

AST List::conj(AST_iter argsBegin, AST_iter argsEnd) const
{
    int oldItemCount = std::distance(begin(), end());
    int newItemCount = std::distance(argsBegin, argsEnd);

    AST_vec* items = new AST_vec(oldItemCount + newItemCount);
    std::reverse_copy(argsBegin, argsEnd, items->begin());
    std::copy(begin(), end(), items->begin() + newItemCount);

    return type::list(items);
}


// ================================
// VECTOR
const std::string Vector::toString(bool readably) const
{
    return '[' + Sequence::toString(readably) + ']';
}

AST Vector::eval(EnvPtr env)
{
    return type::vector(evalItems(env));
}

AST Vector::conj(AST_iter begin_iter, AST_iter end_iter) const
{
    int oldItemCount = std::distance(begin(), end());
    int newItemCount = std::distance(begin_iter, end_iter);

    AST_vec* items = new AST_vec(oldItemCount + newItemCount);
    std::copy(begin(), end(), items->begin());
    std::copy(begin_iter, end_iter, items->begin() + oldItemCount);

    return type::vector(items);
}


// ================================
// HASH
std::string Hash::makeHashKey(AST key)
{
    if ( const String* skey = dynamic_cast<String*>(key.ptr()) ) {
        return skey->toString(true);
    }
    else if ( const Keyword* kkey = dynamic_cast<Keyword*>(key.ptr()) ) {
        return kkey->toString(true);
    }

    throw std::string("not a string or keyword");
}

Hash::Map Hash::addToMap(Hash::Map& map, AST_iter begin, AST_iter end)
{
    for ( AST_iter it = begin; it != end; ++it ) {
        std::string key = makeHashKey(*it++);
        map[key] = *it;
    }

    return map;
}

Hash::Map Hash::createMap(AST_iter begin, AST_iter end)
{
    assert(std::distance(begin, end) % 2 == 0 && "hash map must be even sized!\n");
    Hash::Map map;
    return addToMap(map, begin, end);
}

const std::string Hash::toString(bool readably) const
{
    std::string res = "{";
    auto it = m_map.begin(),
        end = m_map.end();

    if ( it != end ) {
        res += it->first + " " + it->second->toString(readably);
        ++it;
    }

    for ( ; it != end; ++it ) {
        res += " " + it->first + " " + it->second->toString(readably);
    }

    return res + "}";
}

bool Hash::operator==(const Expression* rhs) const
{
    const Hash::Map& rhs_map = static_cast<const Hash*>(rhs)->m_map;

    if ( m_map.size() != rhs_map.size() ) {
        return false;
    }

    auto this_it = m_map.begin(),
        rhs_it = rhs_map.begin(),
        end = m_map.end();

    while ( this_it != end ) {
        if ( this_it->first != rhs_it->first ) {
            return false;
        }

        if ( !this_it->second->isEqualTo(rhs_it->second.ptr()) ) {
            return false;
        }

        ++this_it;
        ++rhs_it;
    }

    return true;
}

AST Hash::assoc(AST_iter begin, AST_iter end) const
{
    if ( std::distance(begin, end) % 2 != 0 ) {
        throw "assoc requires even-sized lists";
    }
    Hash::Map map(m_map);
    return type::hash(addToMap(map, begin, end));
}

AST Hash::dissoc(AST_iter begin, AST_iter end) const
{
    Hash::Map map(m_map);
    for ( auto it = begin; it != end; ++it ) {
        std::string key = makeHashKey(*it);
        map.erase(key);
    }

    return type::hash(map);
}

bool Hash::contains(AST key) const
{
    return m_map.find(makeHashKey(key)) != m_map.end();
}

AST Hash::eval(EnvPtr env)
{
    if ( m_isEval ) {
        return AST(this);
    }

    Hash::Map map;

    for ( auto it = m_map.begin(); it != m_map.end(); ++it ) {
        map[it->first] = EVAL(it->second, env);
    }
    return type::hash(map);
}

AST Hash::get(AST key) const
{
    auto it = m_map.find(makeHashKey(key));
    return it == m_map.end() ? type::nilValue() : it->second;
}

AST Hash::keys() const
{
    AST_vec* keys = new AST_vec();
    keys->reserve(m_map.size());
    for ( auto it = m_map.begin(), end = m_map.end(); it != end; ++it ) {
        if ( it->first[0] == '"' ) {
            keys->push_back(type::string(unescape(it->first)));
        }
        else {
            keys->push_back(type::keyword(it->first));
        }
    }
    return type::list(keys);
}

AST Hash::values() const
{
    AST_vec* keys = new AST_vec();
    keys->reserve(m_map.size());
    for ( auto it = m_map.begin(), end = m_map.end(); it != end; ++it ) {
        keys->push_back(it->second);
    }
    return type::list(keys);
}


// ================================
// BUILTIN
AST BuiltIn::apply(AST_iter argsBegin, AST_iter argsEnd) const
{
    return m_handler(m_name, argsBegin, argsEnd);
}

// ================================
// LAMBDA
Lambda::Lambda(const std::vector<std::string>& bindings, AST body, EnvPtr env)
    : m_bindings(bindings), m_body(body),
    m_env(env), m_isMacro(false)
{ }

Lambda::Lambda(const Lambda& that, AST meta)
    : Applicable(meta),
    m_bindings(that.m_bindings), m_body(that.m_body),
    m_env(that.m_env), m_isMacro(that.m_isMacro)
{ }

Lambda::Lambda(const Lambda& that, bool isMacro)
    : Applicable(that.m_meta),
    m_bindings(that.m_bindings), m_body(that.m_body),
    m_env(that.m_env), m_isMacro(isMacro)
{ }

AST Lambda::doWithMeta(AST meta) const
{
    return new Lambda(*this, meta);
}

AST Lambda::apply(AST_iter argsBegin, AST_iter argsEnd) const
{
    return EVAL(m_body, makeEnv(argsBegin, argsEnd));
}

EnvPtr Lambda::makeEnv(AST_iter argsBegin, AST_iter argsEnd) const
{
    return EnvPtr(new Env(m_env, m_bindings, argsBegin, argsEnd));
}

bool Lambda::operator==(const Expression* rhs) const
{
    return this == rhs;
}

const std::string Lambda::toString(bool readably) const
{
    std::ostringstream oss;
    if ( m_isMacro ) {
        oss << "#user-macro(" << this << ")";
    }
    else {
        oss << "#user-function(" << this << ")";
    }

    return oss.str();
}