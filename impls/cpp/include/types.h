#ifndef TYPES_H
#define TYPES_H

#include "../include/type_base.h"
#include <vector>
#include <iostream>
#include <map>

class Expression;
typedef RefCountedPtr<Expression>   ValuePtr;
typedef std::vector<ValuePtr>       ValueVec;
typedef ValueVec::iterator          ValueIter;

class ParseException : public std::exception {
public:
    ParseException(const std::string& msg, const std::string& detail = "") : m_what(msg), m_detail(detail) { }

    virtual const char* what() const throw()
    {
        return m_what.c_str();
    }

private:
    std::string m_what;
    std::string m_detail;
};

class Expression : public ReferenceCounter {
public:
    Expression() { /* add logging */ }
    Expression(ValuePtr ptr) : m_meta(ptr) { /* add logging */ }
    virtual ~Expression() { /* add logging */ }

    virtual const std::string toString(bool readably) const = 0;

    bool isEqualTo(const Expression* rhs) const;
    friend std::ostream& operator<<(std::ostream& os, const Expression& ex)
    {
        os << ex.toString(true);
        return os;
    }

protected:
    virtual bool operator==(const Expression* rhs) const = 0;
    ValuePtr m_meta;
};

class Atom : public Expression {
public:
    Atom(ValuePtr value) : m_atom(value) { }
    Atom(const Atom& that, ValuePtr meta)
        : Expression(meta), m_atom(that.m_atom)
    { }

    virtual const std::string toString(bool readably) const override
    {
        return "(atom " + m_atom->toString(readably) + ")";
    }

    virtual bool operator==(const Expression* rhs) const override
    {
        return this->m_atom->isEqualTo(rhs);
    }

private:
    ValuePtr m_atom;
};

class Constant : public Expression {
public:
    Constant(const std::string& name) : m_name(name) { }
    Constant(const Constant& that, ValuePtr meta)
        : Expression(meta), m_name(that.m_name)
    { }

    virtual const std::string toString(bool readably) const override
    {
        return m_name;
    }

    virtual bool operator==(const Expression* rhs) const override
    {
        return this == rhs;
    }

private:
    const std::string m_name;
};

class Integer : public Expression {
public:
    Integer(int64_t value) : m_val(value) { }
    Integer(const Integer& that, ValuePtr meta)
        : Expression(meta), m_val(that.m_val)
    { }

    virtual const std::string toString(bool readably) const override
    {
        return std::to_string(m_val);
    }

    int64_t value() const { return m_val; }

    virtual bool operator==(const Expression* rhs) const override
    {
        return m_val == static_cast<const Integer*>(rhs)->m_val;
    }

private:
    const int64_t m_val;
};

class StringBase : public Expression {
public:
    StringBase(const std::string& token) : m_string(token) { }
    StringBase(const StringBase& that, ValuePtr meta)
        : Expression(meta), m_string(that.value())
    { }

    virtual const std::string toString(bool readably) const override { return m_string; }
    std::string value() const { return m_string; }
private:
    const std::string m_string;
};

class String : public StringBase {
public:
    String(const std::string& token) : StringBase(token) { }
    String(const String& that, ValuePtr meta)
        : StringBase(that, meta)
    { }

    virtual const std::string toString(bool readably) const
    {
        return value();
    }

    virtual bool operator==(const Expression* rhs) const
    {
        return value() == static_cast<const String*>(rhs)->value();
    }
};

class Keyword : public StringBase {
public:
    Keyword(const std::string& token) : StringBase(token) { }
    Keyword(const Keyword& that, ValuePtr meta) : StringBase(that, meta) { }

    virtual bool operator==(const Expression* rhs) const
    {
        return value() == static_cast<const Keyword*>(rhs)->value();
    }
};
class Symbol : public StringBase {
public:
    Symbol(const std::string& token) : StringBase(token) { }
    Symbol(const Symbol& that, ValuePtr meta) : StringBase(that, meta) { }

    virtual bool operator==(const Expression* rhs) const
    {
        return value() == static_cast<const Symbol*>(rhs)->value();
    }
};

class Sequence : public Expression {
public:
    Sequence(ValueVec* items) : m_items(items) { }
    Sequence(ValueIter begin, ValueIter end) : m_items(new ValueVec(begin, end)) { }
    Sequence(const Sequence& that, ValuePtr meta)
        : Expression(meta), m_items(new ValueVec(*(that.m_items)))
    { }

    virtual ~Sequence() { delete m_items; }

    size_t count() const { return m_items->size(); }
    bool isEmpty() const { return m_items->empty(); }
    ValuePtr operator[](int index) const { return (*m_items)[index]; }

    ValueIter begin() const { return m_items->begin(); }
    ValueIter end() const { return m_items->end(); }

    virtual const std::string toString(bool readably) const override
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

    virtual bool operator==(const Expression* rhs) const override
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

private:
    ValueVec* const m_items;
};

class List : public Sequence {
public:
    List(ValueVec* items) : Sequence(items) { }
    List(ValueIter begin, ValueIter end) : Sequence(begin, end) { }
    List(const List& that, ValuePtr meta) : Sequence(that, meta) { }

    virtual const std::string toString(bool readably) const override
    {
        return '(' + Sequence::toString(readably) + ')';
    }
};

class Vector : public Sequence {
public:
    Vector(ValueVec* items) : Sequence(items) { }
    Vector(ValueIter begin, ValueIter end) : Sequence(begin, end) { }
    Vector(const List& that, ValuePtr meta) : Sequence(that, meta) { }

    virtual const std::string toString(bool readably) const override
    {
        return '[' + Sequence::toString(readably) + ']';
    }
};

class Hash : public Expression {
public:
    typedef std::map<std::string, ValuePtr> Map;

    Hash(ValueIter begin, ValueIter end, bool isEvaluated)
        : m_map(createMap(begin, end)), m_isEval(isEvaluated)
    { }
    Hash(const Hash::Map& map)
        : m_map(map), m_isEval(true)
    { }
    Hash(const Hash& that, ValuePtr meta)
        : Expression(meta), m_map(that.m_map), m_isEval(that.m_isEval)
    { }

    static std::string makeHashKey(ValuePtr key)
    {
        // TODO
        return key->toString(true);
    }

    static Hash::Map addToMap(Hash::Map& map, ValueIter begin, ValueIter end)
    {
        for ( ValueIter it = begin; it != end; ++it ) {
            std::string key = makeHashKey(*it++);
            map[key] = *it;
        }

        return map;
    }

    static Hash::Map createMap(ValueIter begin, ValueIter end)
    {
        Hash::Map map;
        return addToMap(map, begin, end);
    }

    virtual const std::string toString(bool readably) const override
    {
        std::string res = "{";
        auto it = m_map.begin(),
            end = m_map.end();

        if ( it != end ) {
            res += it->first + " " + it->second->toString(readably);
        }

        for ( ; it != end; ++it ) {
            res += " " + it->first + " " + it->second->toString(readably);
        }

        return res + "}";
    }

    virtual bool operator==(const Expression* rhs)
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

private:
    const Map m_map;
    const bool m_isEval;
};

class Applicable : public Expression { };
class BuiltIn : public Applicable { };
class Lambda : public Applicable { };

namespace type {
    ValuePtr error(ParseException& error);
    ValuePtr atom(ValuePtr value);
    ValuePtr boolean(bool value);
    // ValuePtr builtin(const String& name, BuiltIn::ApplyFunc handler);
    ValuePtr falseValue();
    ValuePtr hash(ValueIter argsBegin, ValueIter argsEnd,
                        bool isEvaluated);
    // ValuePtr hash(const malHash::Map& map);
    // ValuePtr hash(const std::map<std::String, ValuePtr>& map);
    ValuePtr integer(int64_t value);
    ValuePtr integer(const String& token);
    ValuePtr keyword(const String& token);
    // ValuePtr lambda(const std::vector<std::string>&, ValuePtr, malEnvPtr);
    ValuePtr list(ValueVec* items);
    ValuePtr list(ValueIter begin, ValueIter end);
    ValuePtr list(ValuePtr a);
    ValuePtr list(ValuePtr a, ValuePtr b);
    ValuePtr list(ValuePtr a, ValuePtr b, ValuePtr c);
    ValuePtr macro(const Lambda& lambda);
    ValuePtr nilValue();
    ValuePtr string(const std::string& token);
    ValuePtr symbol(const std::string& token);
    ValuePtr trueValue();
    ValuePtr vector(ValueVec* items);
    ValuePtr vector(ValueIter begin, ValueIter end);
} // namespace type

#endif // TYPES_H
