#include "../include/types.h"

namespace type {
    ValuePtr boolean(bool value);
    // ValuePtr builtin(const String& name, BuiltIn::ApplyFunc handler);
    ValuePtr falseValue();

    ValuePtr integer(int64_t value);
    ValuePtr integer(const String& token);
    ValuePtr keyword(const String& token);
    // ValuePtr lambda(const std::vector<std::string>&, ValuePtr, malEnvPtr);
    ValuePtr macro(const Lambda& lambda);
    ValuePtr nilValue();
    ValuePtr symbol(const std::string& token);
    ValuePtr trueValue();

    ValuePtr hash(ValueIter begin, ValueIter end, bool isEvaluated)
    {
        return ValuePtr(new Hash(begin, end, isEvaluated));
    }

    ValuePtr hash(const Hash::Map& map)
    {
        return ValuePtr(new Hash(map));
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

    ValuePtr error(ParseException& error)
    {
        return string(error.what());
    }
} // namespace type

bool Expression::isEqualTo(const Expression* rhs) const
{
    bool types_match = (typeid(*this) == typeid(*rhs))
        || (dynamic_cast<const Sequence*>(this) && dynamic_cast<const Sequence*>(rhs));

    return types_match && (this == rhs);
}