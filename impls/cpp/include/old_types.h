#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>
#include <memory>


namespace types {
    class Object {
    public:
        virtual ~Object() = default;
        virtual std::string toString() const = 0;
    };

    class Atom : public Object {
    private:
        std::string m_value;

    public:
        Atom(const std::string& value) : m_value(value) { }
        std::string toString() const override { return m_value; }
    };

    class Symbol : public Object {
    private:
        std::string m_value;

    public:
        Symbol(const std::string& value) : m_value(value) { }
        std::string toString() const override { return m_value; }
    };

    class Integer : public Object {
    private:
        uint64_t m_value;

    public:
        Integer(uint64_t value) : m_value(value) { }
        std::string toString() const override { return std::to_string(m_value); }
    };

    class String : public Object {
    private:
        std::string m_value;

    public:
        String(const std::string& value) : m_value(value) { }
        std::string toString() const override { return m_value; }
    };

    class List : public Object {
    private:
        std::vector<std::shared_ptr<Object>> m_list;
    public:
        List() : m_list(0) { }
        List(const List& other) : m_list(other.m_list) { }

        std::string toString() const override
        {
            std::string res = "(";
            if ( m_list.size() == 0 ) {
                res += " )";
                return res;
            }

            for ( auto& elem : m_list ) {
                res += elem->toString();
                res += " ";
            }

            res.back() = ')';

            return res;
        }

        void push_back(std::shared_ptr<Object>&& ptr)
        {
            m_list.push_back(std::move(ptr));
        }
    };

}; // namespace types

#endif // TYPES_H