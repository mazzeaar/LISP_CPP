#include "../include/types.h"

#include <cassert>

namespace type {
    // ValuePtr lambda(const std::vector<std::string>&, ValuePtr, malEnvPtr);
    // ValuePtr macro(const Lambda& lambda);

    ValuePtr builtin(const std::string& name, BuiltIn::ApplyFunc handler)
    {
        return ValuePtr(new BuiltIn(name, handler));
    }

    ValuePtr boolean(bool value)
    {
        return value ? trueValue() : falseValue();
    }

    ValuePtr integer(int64_t value)
    {
        return ValuePtr(new Integer(value));
    }

    ValuePtr integer(const std::string& token)
    {
        return integer(std::stoi(token));
    }

    ValuePtr keyword(const std::string& token)
    {
        return ValuePtr(new Keyword(token));
    }

    ValuePtr symbol(const std::string& token)
    {
        return ValuePtr(new Symbol(token));
    }

    ValuePtr falseValue()
    {
        static ValuePtr False(new Constant("false"));
        return ValuePtr(False);
    }

    ValuePtr nilValue()
    {
        static ValuePtr True(new Constant("nil"));
        return ValuePtr(True);
    }

    ValuePtr trueValue()
    {
        static ValuePtr Nil(new Constant("true"));
        return ValuePtr(Nil);
    }

    ValuePtr hash(ValueIter begin, ValueIter end, bool isEvaluated)
    {
        return ValuePtr(new Hash(begin, end, isEvaluated));
    }

    ValuePtr hash(const Hash::Map& map)
    {
        return ValuePtr(new Hash(map));
    }

    ValuePtr hash(ValueVec* items, bool isEvaluated)
    {
        return hash(items->begin(), items->end(), isEvaluated);
    }

    ValuePtr vector(ValueVec* items)
    {
        return ValuePtr(new Vector(items));
    }

    ValuePtr vector(ValueIter begin, ValueIter end)
    {
        return ValuePtr(new Vector(begin, end));
    }

    ValuePtr atom(ValuePtr value)
    {
        return ValuePtr(new Atom(value));
    }

    ValuePtr list(ValueVec* items)
    {
        return ValuePtr(new List(items));
    }

    ValuePtr list(ValueIter begin, ValueIter end)
    {
        return ValuePtr(new List(begin, end));
    }

    ValuePtr list(ValuePtr a)
    {
        ValueVec* items = new ValueVec(1);
        items->at(0) = a;
        return ValuePtr(new List(items));
    }

    ValuePtr list(ValuePtr a, ValuePtr b)
    {
        ValueVec* items = new ValueVec(2);
        items->at(0) = a;
        items->at(1) = b;
        return ValuePtr(new List(items));
    }

    ValuePtr list(ValuePtr a, ValuePtr b, ValuePtr c)
    {
        ValueVec* items = new ValueVec(3);
        items->at(0) = a;
        items->at(1) = b;
        items->at(2) = c;
        return ValuePtr(new List(items));
    }

    ValuePtr string(const std::string& token)
    {
        return ValuePtr(new String(token));
    }
} // namespace type

// ================================
// EXPRESSION
bool Expression::isEqualTo(const Expression* rhs) const
{
    bool types_match = (typeid(*this) == typeid(*rhs))
        || (dynamic_cast<const Sequence*>(this) && dynamic_cast<const Sequence*>(rhs));

    return types_match && (this == rhs);
}

bool Expression::isTrue() const
{
    return (this != type::falseValue().ptr() && this != type::nilValue().ptr());
}

ValuePtr Expression::eval(EnvPtr env)
{
    return ValuePtr(this);
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

// ================================
// SEQUENCE
const std::string Sequence::toString(bool readably) const
{
    std::string res;
    ValueVec::const_iterator iter = m_items->cbegin();
    ValueVec::const_iterator end = m_items->cend();

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

    ValueIter this_it = m_items->begin(),
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

ValueVec* Sequence::evalItems(EnvPtr env) const
{
    ValueVec* items = new ValueVec;;
    items->reserve(count());
    for ( auto it = m_items->begin(), end = m_items->end(); it != end; ++it ) {
        items->push_back((*it)->eval(env));
    }

    return items;
}

ValuePtr Sequence::first() const
{
    return count() == 0 ? type::nilValue() : (*m_items)[0];
}

ValuePtr Sequence::rest() const
{
    ValueIter start = (count() > 0) ? begin() + 1 : end();
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

ValuePtr Symbol::eval(EnvPtr env)
{
    return env->get(value());
}

// ================================
// LIST
const std::string List::toString(bool readably) const
{
    return '(' + Sequence::toString(readably) + ')';
}

ValuePtr List::eval(EnvPtr env)
{
    if ( count() == 0 ) {
        return ValuePtr(this);
    }

    auto APPLY = [ ] (ValuePtr op, ValueIter begin, ValueIter end)
    {
        const Applicable* handler = dynamic_cast<Applicable*>(op.ptr());

        if ( handler == NULL ) {
            throw "handler is NULL";
        }

        return handler->apply(begin, end);
    };

    std::unique_ptr<ValueVec> items(evalItems(env));
    auto it = items->begin();
    ValuePtr op = *it;
    return APPLY(op, ++it, items->end());
}

ValuePtr List::conj(ValueIter argsBegin, ValueIter argsEnd) const
{
    int oldItemCount = std::distance(begin(), end());
    int newItemCount = std::distance(argsBegin, argsEnd);

    ValueVec* items = new ValueVec(oldItemCount + newItemCount);
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

ValuePtr Vector::eval(EnvPtr env)
{
    return type::vector(evalItems(env));
}

ValuePtr Vector::conj(ValueIter begin_iter, ValueIter end_iter) const
{
    int oldItemCount = std::distance(begin(), end());
    int newItemCount = std::distance(begin_iter, end_iter);

    ValueVec* items = new ValueVec(oldItemCount + newItemCount);
    std::copy(begin(), end(), items->begin());
    std::copy(begin_iter, end_iter, items->begin() + oldItemCount);

    return type::vector(items);
}

// ================================
// HASH
std::string Hash::makeHashKey(ValuePtr key)
{
    if ( const String* skey = dynamic_cast<String*>(key.ptr()) ) {
        return skey->toString(true);
    }
    else if ( const Keyword* kkey = dynamic_cast<Keyword*>(key.ptr()) ) {
        return kkey->toString(true);
    }

    throw std::string("not a string or keyword");
}

Hash::Map Hash::addToMap(Hash::Map& map, ValueIter begin, ValueIter end)
{
    for ( ValueIter it = begin; it != end; ++it ) {
        std::string key = makeHashKey(*it++);
        map[key] = *it;
    }

    return map;
}

Hash::Map Hash::createMap(ValueIter begin, ValueIter end)
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

ValuePtr Hash::assoc(ValueIter begin, ValueIter end) const
{
    if ( std::distance(begin, end) % 2 != 0 ) {
        throw "assoc requires even-sized lists";
    }
    Hash::Map map(m_map);
    return type::hash(addToMap(map, begin, end));
}

ValuePtr Hash::dissoc(ValueIter begin, ValueIter end) const
{
    Hash::Map map(m_map);
    for ( auto it = begin; it != end; ++it ) {
        std::string key = makeHashKey(*it);
        map.erase(key);
    }

    return type::hash(map);
}

bool Hash::contains(ValuePtr key) const
{
    return m_map.find(makeHashKey(key)) != m_map.end();
}

ValuePtr Hash::eval(EnvPtr env)
{
    if ( m_isEval ) {
        return ValuePtr(this);
    }

    Hash::Map map;

    for ( auto it = m_map.begin(), end = m_map.end(); it != end; ++it ) {
        map[it->first] = (it->second)->eval(env);
    }
    return type::hash(map);
}

ValuePtr Hash::get(ValuePtr key) const
{
    auto it = m_map.find(makeHashKey(key));
    return it == m_map.end() ? type::nilValue() : it->second;
}

ValuePtr Hash::keys() const
{
    ValueVec* keys = new ValueVec();
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

ValuePtr Hash::values() const
{
    ValueVec* keys = new ValueVec();
    keys->reserve(m_map.size());
    for ( auto it = m_map.begin(), end = m_map.end(); it != end; ++it ) {
        keys->push_back(it->second);
    }
    return type::list(keys);
}


// ================================
// BUILTIN
ValuePtr BuiltIn::apply(ValueIter argsBegin, ValueIter argsEnd) const
{
    return m_handler(m_name, argsBegin, argsEnd);
}
