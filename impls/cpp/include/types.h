#ifndef TYPES_H
#define TYPES_H

#include "type_base.h"
#include "utils.h"

#include <vector>
#include <iostream>
#include <map>

class Expression;
typedef RefCountedPtr<Expression>   ValuePtr;
typedef std::vector<ValuePtr>       ValueVec;
typedef ValueVec::iterator          ValueIter;

class EmptyInputException : public std::exception { };
class ParseException : public std::exception {
public:
    ParseException(const std::string& msg, const std::string& detail = "")
        : m_msg(msg), m_detail(detail)
    { }

    virtual const char* what() const throw()
    {
        return m_msg.c_str();
    }

private:
    std::string m_msg;
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
        return os << ex.toString(true);
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

    const std::string toString(bool readably) const;
    bool operator==(const Expression* rhs) const;

private:
    ValuePtr m_atom;
};

class Constant : public Expression {
public:
    Constant(const std::string& name) : m_name(name) { }
    Constant(const Constant& that, ValuePtr meta)
        : Expression(meta), m_name(that.m_name)
    { }

    const std::string toString(bool readably) const;
    bool operator==(const Expression* rhs) const;

private:
    const std::string m_name;
};

class Integer : public Expression {
public:
    Integer(int64_t value) : m_val(value) { }
    Integer(const Integer& that, ValuePtr meta)
        : Expression(meta), m_val(that.m_val)
    { }

    int64_t value() const { return m_val; }

    const std::string toString(bool readably) const;
    bool operator==(const Expression* rhs) const;

private:
    const int64_t m_val;
};

class StringBase : public Expression {
public:
    StringBase(const std::string& token) : m_string(token) { }
    StringBase(const StringBase& that, ValuePtr meta)
        : Expression(meta), m_string(that.value())
    { }

    virtual const std::string toString(bool readably) const { return m_string; }
    std::string value() const { return m_string; }
private:
    const std::string m_string;
};

class String : public StringBase {
public:
    String(const std::string& token) : StringBase(token) { }
    String(const String& that, ValuePtr meta) : StringBase(that, meta) { }

    const std::string toString(bool readably) const override;
    bool operator==(const Expression* rhs) const override;
};

class Keyword : public StringBase {
public:
    Keyword(const std::string& token) : StringBase(token) { }
    Keyword(const Keyword& that, ValuePtr meta) : StringBase(that, meta) { }

    bool operator==(const Expression* rhs) const override;
};
class Symbol : public StringBase {
public:
    Symbol(const std::string& token) : StringBase(token) { }
    Symbol(const Symbol& that, ValuePtr meta) : StringBase(that, meta) { }

    bool operator==(const Expression* rhs) const override;
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

    virtual const std::string toString(bool readably) const override;
    bool operator==(const Expression* rhs) const override;

private:
    ValueVec* const m_items;
};

class List : public Sequence {
public:
    List(ValueVec* items) : Sequence(items) { }
    List(ValueIter begin, ValueIter end) : Sequence(begin, end) { }
    List(const List& that, ValuePtr meta) : Sequence(that, meta) { }

    const std::string toString(bool readably) const override;
};

class Vector : public Sequence {
public:
    Vector(ValueVec* items) : Sequence(items) { }
    Vector(ValueIter begin, ValueIter end) : Sequence(begin, end) { }
    Vector(const List& that, ValuePtr meta) : Sequence(that, meta) { }

    const std::string toString(bool readably) const override;
};

class Hash : public Expression {
public:
    typedef std::map<std::string, ValuePtr> Map;

    Hash(const Hash::Map& map) : m_map(map), m_isEval(true) { }
    Hash(ValueIter begin, ValueIter end, bool isEvaluated)
        : m_map(createMap(begin, end)), m_isEval(isEvaluated)
    { }
    Hash(const Hash& that, ValuePtr meta)
        : Expression(meta), m_map(that.m_map), m_isEval(that.m_isEval)
    { }

    static std::string makeHashKey(ValuePtr key);
    static Hash::Map addToMap(Hash::Map& map, ValueIter begin, ValueIter end);
    static Hash::Map createMap(ValueIter begin, ValueIter end);

    const std::string toString(bool readably) const override;
    bool operator==(const Expression* rhs) const override;

private:
    const Map m_map;
    const bool m_isEval;
};

class Applicable : public Expression { };
class BuiltIn : public Applicable { };
class Lambda : public Applicable { };

namespace type {
    // ValuePtr builtin(const String& name, BuiltIn::ApplyFunc handler);
    // ValuePtr lambda(const std::vector<std::string>&, ValuePtr, malEnvPtr);

    ValuePtr macro(const Lambda& lambda);
    ValuePtr atom(ValuePtr value);

    ValuePtr symbol(const std::string& token);
    ValuePtr keyword(const std::string& token);

    ValuePtr falseValue();
    ValuePtr nilValue();
    ValuePtr trueValue();

    ValuePtr boolean(bool value);
    ValuePtr string(const std::string& token);
    ValuePtr integer(const std::string& token);
    ValuePtr integer(int64_t value);

    ValuePtr hash(ValueIter argsBegin, ValueIter argsEnd, bool isEvaluated);
    ValuePtr hash(const Hash::Map& map);
    ValuePtr hash(ValueVec* items, bool isEvaluated);

    ValuePtr list(ValueVec* items);
    ValuePtr list(ValueIter begin, ValueIter end);
    ValuePtr list(ValuePtr a);
    ValuePtr list(ValuePtr a, ValuePtr b);
    ValuePtr list(ValuePtr a, ValuePtr b, ValuePtr c);

    ValuePtr vector(ValueVec* items);
    ValuePtr vector(ValueIter begin, ValueIter end);
} // namespace type

#endif // TYPES_H
