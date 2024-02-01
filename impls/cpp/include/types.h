#ifndef TYPES_H
#define TYPES_H

#include "type_base.h"
#include "utils.h"
#include "def.h"
#include "environment.h"
#include "lisp_error.h"

#include <vector>
#include <iostream>
#include <map>

class EmptyInputException : public std::exception { };

class Expression : public ReferenceCounter {
public:
    Expression() { /* add logging */ }
    Expression(AST ptr) : m_meta(ptr) { /* add logging */ }
    virtual ~Expression() { /* add logging */ }

    bool isEqualTo(const Expression* rhs) const;
    bool isTrue() const;

    virtual AST eval(EnvPtr env);

    virtual const std::string toString(bool readably) const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Expression& ex)
    {
        return os << ex.toString(true);
    }

protected:
    virtual bool operator==(const Expression* rhs) const = 0;
    AST m_meta;
};

template<class T>
T* value_cast(AST obj, const char* typeName)
{
    T* dest = dynamic_cast<T*>(obj.ptr());
    if ( dest == NULL ) {
        throw LISP_ERROR(obj->toString(true), " != ", std::string(typeName));
    }
    return dest;
}

#define VALUE_CAST(Type, Value) value_cast<Type>(Value, #Type)

class Atom : public Expression {
public:
    Atom(AST value) : m_atom(value) { }
    Atom(const Atom& that, AST meta)
        : Expression(meta), m_atom(that.m_atom)
    { }

    const std::string toString(bool readably) const;
    bool operator==(const Expression* rhs) const;

private:
    AST m_atom;
};

class Constant : public Expression {
public:
    Constant(const std::string& name) : m_name(name) { }
    Constant(const Constant& that, AST meta)
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
    Integer(const Integer& that, AST meta)
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
    StringBase(const StringBase& that, AST meta)
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
    String(const String& that, AST meta) : StringBase(that, meta) { }

    const std::string toString(bool readably) const override;
    bool operator==(const Expression* rhs) const override;
};

class Keyword : public StringBase {
public:
    Keyword(const std::string& token) : StringBase(token) { }
    Keyword(const Keyword& that, AST meta) : StringBase(that, meta) { }

    bool operator==(const Expression* rhs) const override;
};
class Symbol : public StringBase {
public:
    Symbol(const std::string& token) : StringBase(token) { }
    Symbol(const Symbol& that, AST meta) : StringBase(that, meta) { }

    AST eval(EnvPtr env) override;
    bool operator==(const Expression* rhs) const override;
};

class Sequence : public Expression {
public:
    Sequence(AST_vec* items) : m_items(items) { }
    Sequence(AST_iter begin, AST_iter end) : m_items(new AST_vec(begin, end)) { }
    Sequence(const Sequence& that, AST meta)
        : Expression(meta), m_items(new AST_vec(*(that.m_items)))
    { }

    virtual ~Sequence() { delete m_items; }

    size_t count() const { return m_items->size(); }
    bool isEmpty() const { return m_items->empty(); }
    AST operator[](int index) const { return (*m_items)[index]; }

    virtual AST_vec* evalItems(EnvPtr env) const;

    AST_iter begin() const { return m_items->begin(); }
    AST_iter end() const { return m_items->end(); }

    virtual const std::string toString(bool readably) const override;
    bool operator==(const Expression* rhs) const override;

    virtual AST conj(AST_iter argsBegin, AST_iter argsEnd) const = 0;

    AST first() const;
    virtual AST rest() const;

private:
    AST_vec* const m_items;
};

class List : public Sequence {
public:
    List(AST_vec* items) : Sequence(items) { }
    List(AST_iter begin, AST_iter end) : Sequence(begin, end) { }
    List(const List& that, AST meta) : Sequence(that, meta) { }

    virtual AST eval(EnvPtr env) override;
    virtual AST conj(AST_iter argsBegin, AST_iter argsEnd) const override;
    const std::string toString(bool readably) const override;
};

class Vector : public Sequence {
public:
    Vector(AST_vec* items) : Sequence(items) { }
    Vector(AST_iter begin, AST_iter end) : Sequence(begin, end) { }
    Vector(const List& that, AST meta) : Sequence(that, meta) { }

    AST eval(EnvPtr env) override;
    virtual AST conj(AST_iter argsBegin, AST_iter argsEnd) const override;
    const std::string toString(bool readably) const override;
};

class Hash : public Expression {
public:
    typedef std::map<std::string, AST> Map;

    Hash(const Hash::Map& map) : m_map(map), m_isEval(true) { }
    Hash(AST_iter begin, AST_iter end, bool isEvaluated)
        : m_map(createMap(begin, end)), m_isEval(isEvaluated)
    { }
    Hash(const Hash& that, AST meta)
        : Expression(meta), m_map(that.m_map), m_isEval(that.m_isEval)
    { }

    AST assoc(AST_iter argsBegin, AST_iter argsEnd) const;
    AST dissoc(AST_iter argsBegin, AST_iter argsEnd) const;
    bool contains(AST key) const;

    AST eval(EnvPtr env) override;
    AST get(AST key) const;
    AST keys() const;
    AST values() const;

    static std::string makeHashKey(AST key);
    static Hash::Map addToMap(Hash::Map& map, AST_iter begin, AST_iter end);
    static Hash::Map createMap(AST_iter begin, AST_iter end);

    const std::string toString(bool readably) const override;
    bool operator==(const Expression* rhs) const override;

private:
    const Map m_map;
    const bool m_isEval;
};

class Applicable : public Expression {
public:
    Applicable() { }
    Applicable(AST meta) : Expression(meta) { }

    virtual AST apply(AST_iter begin, AST_iter end) const = 0;
};

class BuiltIn : public Applicable {
public:
    typedef AST(ApplyFunc)(const std::string& name,
                        AST_iter argsBegin, AST_iter argsEnd);

    BuiltIn(const std::string& name, ApplyFunc* handler)
        : m_name(name), m_handler(handler)
    { }

    BuiltIn(const BuiltIn& that, AST meta)
        : Applicable(meta), m_name(that.m_name), m_handler(that.m_handler)
    { }

    virtual AST apply(AST_iter argsBegin, AST_iter argsEnd) const;

    const std::string toString(bool readably) const
    {
        return "#builtin-function(" + m_name + ")";
    }

    bool operator==(const Expression* rhs) const
    {
        return this == rhs;
    }

private:
    const std::string m_name;
    ApplyFunc* m_handler;
};

class Lambda : public Applicable { };

namespace type {
    // AST lambda(const std::vector<std::string>&, AST, malEnvPtr);

    AST builtin(const std::string& name, BuiltIn::ApplyFunc handler);
    AST macro(const Lambda& lambda);
    AST atom(AST value);

    AST symbol(const std::string& token);
    AST keyword(const std::string& token);

    AST falseValue();
    AST nilValue();
    AST trueValue();

    AST boolean(bool value);
    AST string(const std::string& token);
    AST integer(const std::string& token);
    AST integer(int64_t value);

    AST hash(AST_iter argsBegin, AST_iter argsEnd, bool isEvaluated);
    AST hash(const Hash::Map& map);
    AST hash(AST_vec* items, bool isEvaluated);

    AST list(AST_vec* items);
    AST list(AST_iter begin, AST_iter end);
    AST list(AST a);
    AST list(AST a, AST b);
    AST list(AST a, AST b, AST c);

    AST vector(AST_vec* items);
    AST vector(AST_iter begin, AST_iter end);
} // namespace type

#endif // TYPES_H
