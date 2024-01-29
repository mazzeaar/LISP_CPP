/*
s_expression ::= atomic_symbol | "(" s_expression "." s_expression ")" | list
list ::= "(" s_expression* ")"
atomic_symbol ::= letter atom_part
atom_part ::= empty | letter atom_part | number atom_part
letter ::= "a" | "b" | " ..." | "z"
number ::= "1" | "2" | " ..." | "9"
*/
#ifndef TYPES_H
#define TYPES_H

#include "../include/type_base.h"
#include <vector>
#include <iostream>

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

namespace types {
    class Expression;
    typedef RefCountedPtr<Expression>   ValuePtr;
    typedef std::vector<ValuePtr>       ValueVec;
    typedef ValueVec::iterator          ValueIter;

    class Expression : public ReferenceCounter {
    public:
        Expression() { }
        Expression(ValuePtr ptr) : m_ptr(ptr) { }
        virtual ~Expression() = default;

        virtual const std::string toString() const = 0;

        friend std::ostream& operator<<(std::ostream& os, const Expression& ex)
        {
            os << ex.toString();
            return os;
        }

    private:
        ValuePtr m_ptr;
    };

    class Atom : public Expression {
    public:
        Atom(const std::string& atom) : m_atom(atom) { }

        virtual const std::string toString() const override
        {
            return m_atom;
        }

    private:
        std::string m_atom;
    };

    class List : public Expression {
    public:
        List() : m_list(nullptr) { }
        List(ValueVec* list) : m_list(list) { }
        List(ValueVec& list) : m_list(&list) { }

        void push_back(ValuePtr ptr)
        {
            m_list->push_back(ptr);
        }

        virtual const std::string toString() const override
        {
            std::string res = "( ";
            for ( const auto& elem : *m_list ) {
                res += elem->toString();
                res += " ";
            }

            res.back() = ')';
            return res;
        }

    private:
        ValueVec* const m_list;
    };

} // namespace types

#endif // TYPES_H
