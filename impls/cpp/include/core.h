/**
 *
 * list -> take params, return as list
 * list? -> true if first param is list, else false
 *
 * empty? -> treat first param as list, true if empty, else false
 *
 * count -> treat first param as list, return nr of elems
 *
 * = -> compara first two params, true if same type and same value
 *      -> if list: compare each elem, if same true
 *
 * < <= >= > -> treat first two params as numbers, and compare
 */

#include "def.h"
#include "environment.h"
#include "types.h"

#include <iterator>
#include <fstream>
#include <chrono>

template <typename T>
class StaticList {
public:
    StaticList() : m_head(nullptr) { }

    class Iterator;
    Iterator begin() { return Iterator(m_head); }
    Iterator end() { return Iterator(nullptr); }

    class Node {
    public:
        Node(StaticList<T>& list, T item)
            : m_item(item), m_next(list.m_head)
        {
            list.m_head = this;
        }

    private:
        friend class Iterator;
        T m_item;
        Node* m_next;
    };

    class Iterator {
    public:
        Iterator& operator++()
        {
            m_node = m_node->m_next;
            return *this;
        }

        T& operator*() { return m_node->m_item; }
        bool operator !=(const Iterator& that)
        {
            return m_node != that.m_node;
        }

    private:
        friend class StaticList<T>;
        Iterator(Node* node) : m_node(node) { }
        Node* m_node;
    };

private:
    friend class Node;
    Node* m_head;
};

int checkArgsIs(const std::string& name, int expected, int got)
{
    if ( got != expected ) {
        throw LISP_ERROR("\"", name, "\" expects ", std::to_string(expected), " args, ",
            std::to_string(got), " supplied");
    }

    return got;
}

int checkArgsBetween(const std::string& name, int min, int max, int got)
{
    if ( got < min || got > max ) {
        throw LISP_ERROR("\"", name, "\" expects between ", std::to_string(min),
         " and ", std::to_string(max), " args, ",
            std::to_string(got), " supplied");
    }

    return got;
}

int checkArgsAtLeast(const std::string& name, int min, int got)
{
    if ( got < min ) {
        throw LISP_ERROR("\"", name, "\" expects at least ", std::to_string(min), " args, ",
            std::to_string(got), " supplied");
    }

    return got;
}

int checkArgsEven(const std::string& name, int got)
{
    if ( got % 2 != 0 ) {
        throw LISP_ERROR("\"", name, "\" expects even number of args");
    }

    return got;
}

#define CHECK_ARGS_IS(expected)         checkArgsIs(name, expected, std::distance(argsBegin, argsEnd));
#define CHECK_ARGS_BETWEEN(min, max)    checkArgsBetween(name, min, max, std::distance(argsBegin, argsEnd));
#define CHECK_ARGS_AT_LEAST(min)        checkArgsAtLeast(name, min, std::distance(argsBegin, argsEnd));

static std::string printValues(AST_iter begin, AST_iter end, const std::string& sep, bool readably);
static StaticList<BuiltIn*> handlers;

#define ARG(type, name) type* name = VALUE_CAST(type, *argsBegin++)

#define FUNCNAME(uniq) builtIn ## uniq
#define HRECNAME(uniq) handler ## uniq

#define BUILTIN_DEF(uniq, symbol) \
    static BuiltIn::ApplyFunc FUNCNAME(uniq); \
    static StaticList<BuiltIn*>::Node HRECNAME(uniq) \
        (handlers, new BuiltIn(symbol, FUNCNAME(uniq))); \
    AST FUNCNAME(uniq)(const std::string& name, \
        AST_iter argsBegin, AST_iter argsEnd)

#define BUILTIN(symbol)  BUILTIN_DEF(__LINE__, symbol)

#define BUILTIN_ISA(symbol, Type) \
    BUILTIN(symbol) { \
        CHECK_ARGS_IS(1); \
        return type::boolean(dynamic_cast<Type*>((*argsBegin).ptr())); \
    }

#define BUILTIN_IS(op, constant) \
    BUILTIN(op) { \
        CHECK_ARGS_IS(1); \
        return type::boolean(*argsBegin == type::constant()); \
    }

#define BUILTIN_INTOP(op, checkDivByZero) \
    BUILTIN(#op) { \
        CHECK_ARGS_IS(2); \
        ARG(Integer, lhs); \
        ARG(Integer, rhs); \
        if constexpr (checkDivByZero) { \
            if (checkDivByZero && rhs->value() == 0) { \
                LISP_ERROR("Division by zero"); \
            } \
        } \
        return type::integer(lhs->value() op rhs->value()); \
    }

BUILTIN_ISA("atom?", Atom);
BUILTIN_ISA("keyword?", Keyword);
BUILTIN_ISA("list?", List);
BUILTIN_ISA("map?", Hash);
BUILTIN_ISA("number?", Integer);
BUILTIN_ISA("sequential?", Sequence);
BUILTIN_ISA("string?", String);
BUILTIN_ISA("symbol?", Symbol);
BUILTIN_ISA("vector?", Vector);

BUILTIN_INTOP(+, false);
BUILTIN_INTOP(/, true);
BUILTIN_INTOP(*, false);
BUILTIN_INTOP(%, true);

BUILTIN_IS("true?", trueValue);
BUILTIN_IS("false?", falseValue);
BUILTIN_IS("nil?", nilValue);

BUILTIN("-")
{
    int argCount = CHECK_ARGS_BETWEEN(1, 2);
    ARG(Integer, lhs);

    if ( argCount == 1 ) {
        return type::integer(-lhs->value());
    }

    ARG(Integer, rhs);
    return type::integer(lhs->value() - rhs->value());
}

BUILTIN("<=")
{
    CHECK_ARGS_IS(2);
    ARG(Integer, lhs);
    ARG(Integer, rhs);

    return type::boolean(lhs->value() <= rhs->value());
}

BUILTIN(">=")
{
    CHECK_ARGS_IS(2);
    ARG(Integer, lhs);
    ARG(Integer, rhs);

    return type::boolean(lhs->value() >= rhs->value());
}

BUILTIN("<")
{
    CHECK_ARGS_IS(2);
    ARG(Integer, lhs);
    ARG(Integer, rhs);

    return type::boolean(lhs->value() < rhs->value());
}

BUILTIN(">")
{
    CHECK_ARGS_IS(2);
    ARG(Integer, lhs);
    ARG(Integer, rhs);

    return type::boolean(lhs->value() > rhs->value());
}

BUILTIN("=")
{
    CHECK_ARGS_IS(2);
    const Expression* lhs = (*argsBegin++).ptr();
    const Expression* rhs = (*argsBegin++).ptr();

    return type::boolean(lhs->isEqualTo(rhs));
}

BUILTIN("prn")
{
    std::cout << printValues(argsBegin, argsEnd, " ", true) << "\n";
    return type::nilValue();
}

BUILTIN("list")
{
    return type::list(argsBegin, argsEnd);
}

BUILTIN("empty?")
{
    CHECK_ARGS_IS(1);
    ARG(Sequence, seq);

    return type::boolean(seq->isEmpty());
}

BUILTIN("count")
{
    CHECK_ARGS_IS(1);
    if ( *argsBegin == type::nilValue() ) {
        return type::integer(0);
    }

    ARG(Sequence, seq);
    return type::integer(seq->count());
}

BUILTIN("str")
{
    return type::string(printValues(argsBegin, argsEnd, "", false));
}

BUILTIN("pr-str")
{
    return type::string(printValues(argsBegin, argsEnd, " ", true));
}

BUILTIN("println")
{
    std::cout << printValues(argsBegin, argsEnd, " ", false) << "\n";
    return type::nilValue();
}

BUILTIN("apply")
{
    CHECK_ARGS_AT_LEAST(2);
    AST op = *argsBegin++;

    // Copy the first N-1 arguments in.
    AST_vec args(argsBegin, argsEnd-1);

    // Then append the argument as a list.
    const Sequence* lastArg = VALUE_CAST(Sequence, *(argsEnd-1));
    for ( int i = 0; i < lastArg->count(); i++ ) {
        args.push_back(lastArg->item(i));
    }

    return APPLY(op, args.begin(), args.end());
}

BUILTIN("assoc")
{
    CHECK_ARGS_AT_LEAST(1);
    ARG(Hash, hash);
    return hash->assoc(argsBegin, argsEnd);
}

BUILTIN("atom")
{
    CHECK_ARGS_IS(1);
    return type::atom(*argsBegin);
}

BUILTIN("eval")
{
    CHECK_ARGS_IS(1);
    return EVAL(*argsBegin, NULL);
}

BUILTIN("fn?")
{
    CHECK_ARGS_IS(1);
    AST arg = *argsBegin++;

    if ( const Lambda* lambda = dynamic_cast<Lambda*>(arg.ptr()) ) {
        return type::boolean(!lambda->isMacro());
    }

    return type::boolean(dynamic_cast<BuiltIn*>(arg.ptr()));
}

BUILTIN("vals")
{
    CHECK_ARGS_IS(1);
    ARG(Hash, hash);
    return hash->values();
}

BUILTIN("vec")
{
    CHECK_ARGS_IS(1);
    ARG(Sequence, s);
    return type::vector(s->begin(), s->end());
}

BUILTIN("vector")
{
    return type::vector(argsBegin, argsEnd);
}

BUILTIN("concat")
{
    int count = 0;
    for ( auto it = argsBegin; it != argsEnd; ++it ) {
        const Sequence* seq = VALUE_CAST(Sequence, *it);
        count += seq->count();
    }

    AST_vec* items = new AST_vec(count);
    int offset = 0;
    for ( auto it = argsBegin; it != argsEnd; ++it ) {
        const Sequence* seq = STATIC_CAST(Sequence, *it);
        std::copy(seq->begin(), seq->end(), items->begin() + offset);
        offset += seq->count();
    }

    return type::list(items);
}

BUILTIN("conj")
{
    CHECK_ARGS_AT_LEAST(1);
    ARG(Sequence, seq);

    return seq->conj(argsBegin, argsEnd);
}

BUILTIN("cons")
{
    CHECK_ARGS_IS(2);
    AST first = *argsBegin++;
    ARG(Sequence, rest);

    AST_vec* items = new AST_vec(1 + rest->count());
    items->at(0) = first;
    std::copy(rest->begin(), rest->end(), items->begin() + 1);

    return type::list(items);
}

BUILTIN("contains?")
{
    CHECK_ARGS_IS(2);
    if ( *argsBegin == type::nilValue() ) {
        return *argsBegin;
    }
    ARG(Hash, hash);
    return type::boolean(hash->contains(*argsBegin));
}

BUILTIN("deref")
{
    CHECK_ARGS_IS(1);
    ARG(Atom, atom);

    return atom->deref();
}

BUILTIN("dissoc")
{
    CHECK_ARGS_AT_LEAST(1);
    ARG(Hash, hash);

    return hash->dissoc(argsBegin, argsEnd);
}

BUILTIN("first")
{
    CHECK_ARGS_IS(1);
    if ( *argsBegin == type::nilValue() ) {
        return type::nilValue();
    }
    ARG(Sequence, seq);
    return seq->first();
}

BUILTIN("get")
{
    CHECK_ARGS_IS(2);
    if ( *argsBegin == type::nilValue() ) {
        return *argsBegin;
    }
    ARG(Hash, hash);
    return hash->get(*argsBegin);
}

BUILTIN("hash-map")
{
    return type::hash(argsBegin, argsEnd, true);
}

BUILTIN("keys")
{
    CHECK_ARGS_IS(1);
    ARG(Hash, hash);
    return hash->keys();
}

BUILTIN("keyword")
{
    CHECK_ARGS_IS(1);
    const AST arg = *argsBegin++;
    if ( Keyword* s = dynamic_cast<Keyword*>(arg.ptr()) ) {
        return s;
    }

    if ( const String* s = dynamic_cast<String*>(arg.ptr()) ) {
        return type::keyword(":" + s->value());
    }

    LISP_ERROR("keyword expects a keyword or string");
    return nullptr; // for the linker
}

BUILTIN("macro?")
{
    CHECK_ARGS_IS(1);

    // Macros are implemented as lambdas, with a special flag.
    const Lambda* lambda = dynamic_cast<Lambda*>((*argsBegin).ptr());
    return type::boolean((lambda != NULL) && lambda->isMacro());
}

BUILTIN("map")
{
    CHECK_ARGS_IS(2);
    AST op = *argsBegin++; // this gets checked in APPLY
    ARG(Sequence, source);

    const int length = source->count();
    AST_vec* items = new AST_vec(length);
    auto it = source->begin();
    for ( int i = 0; i < length; i++ ) {
        items->at(i) = APPLY(op, it+i, it+i+1);
    }

    return  type::list(items);
}

BUILTIN("meta")
{
    CHECK_ARGS_IS(1);
    AST obj = *argsBegin++;

    return obj->meta();
}

BUILTIN("nth")
{
    CHECK_ARGS_IS(2);
    ARG(Sequence, seq);
    ARG(Integer, index);

    int i = index->value();
    if ( i < 0 || i >= seq->count() ) {
        LISP_ERROR("Index out of range");
    }

    return seq->item(i);
}

BUILTIN("rest")
{
    CHECK_ARGS_IS(1);
    if ( *argsBegin == type::nilValue() ) {
        return type::list(new AST_vec(0));
    }
    ARG(Sequence, seq);
    return seq->rest();
}

BUILTIN("swap!")
{
    CHECK_ARGS_AT_LEAST(2);
    ARG(Atom, atom);

    AST op = *argsBegin++; // this gets checked in APPLY

    AST_vec args(1 + argsEnd - argsBegin);
    args[0] = atom->deref();
    std::copy(argsBegin, argsEnd, args.begin() + 1);

    AST value = APPLY(op, args.begin(), args.end());
    return atom->reset(value);
}

BUILTIN("throw")
{
    CHECK_ARGS_IS(1);
    throw* argsBegin;
}

BUILTIN("time-ms")
{
    CHECK_ARGS_IS(0);

    using namespace std::chrono;
    milliseconds ms = duration_cast<milliseconds>(
        high_resolution_clock::now().time_since_epoch()
    );

    return type::integer(ms.count());
}

BUILTIN("with-meta")
{
    CHECK_ARGS_IS(2);
    AST obj = *argsBegin++;
    AST meta = *argsBegin++;
    return obj->withMeta(meta);
}

BUILTIN("symbol")
{
    CHECK_ARGS_IS(1);
    ARG(String, token);
    return type::symbol(token->value());
}

BUILTIN("reset!")
{
    CHECK_ARGS_IS(2);
    ARG(Atom, atom);
    return atom->reset(*argsBegin);
}

BUILTIN("readline")
{
    CHECK_ARGS_IS(1);
    ARG(String, str);

    std::cout << str->value();
    std::string line;

    if ( !std::getline(std::cin, line) ) {
        return type::nilValue();
    }

    return type::string(line);
}

BUILTIN("read-string")
{
    CHECK_ARGS_IS(1);
    ARG(String, str);

    return tokenize_string(str->value());
}

BUILTIN("seq")
{
    CHECK_ARGS_IS(1);
    AST arg = *argsBegin++;

    if ( arg == type::nilValue() ) {
        return type::nilValue();
    }

    if ( const Sequence* seq = DYNAMIC_CAST(Sequence, arg) ) {
        return seq->isEmpty() ? type::nilValue()
            : type::list(seq->begin(), seq->end());
    }

    if ( const String* strVal = DYNAMIC_CAST(String, arg) ) {
        const std::string str = strVal->value();
        int length = str.length();
        if ( length == 0 ) {
            return type::nilValue();
        }

        AST_vec* items = new AST_vec(length);
        for ( int i = 0; i < length; i++ ) {
            (*items)[i] = type::string(str.substr(i, 1));
        }
        return type::list(items);
    }

    LISP_ERROR(arg->toString(true), "is not a string or sequence");
    return nullptr; // for linker
}

BUILTIN("slurp")
{
    CHECK_ARGS_IS(1);
    ARG(String, filename);

    std::ios_base::openmode openmode = std::ios::ate | std::ios::in | std::ios::binary;
    std::ifstream file(filename->value(), openmode);

    if ( file.fail() ) {
        LISP_ERROR("Cannot open ", filename->value());
    }

    std::string data;
    data.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    data.append(std::istreambuf_iterator<char>(file.rdbuf()),
                std::istreambuf_iterator<char>());

    return type::string(data);
}

void installCore(EnvPtr env)
{
    for ( auto it = handlers.begin(), end = handlers.end(); it != end; ++it ) {
        BuiltIn* handler = *it;
        env->set(handler->name(), handler);
    }
}

static std::string printValues(AST_iter begin, AST_iter end,
                          const std::string& sep, bool readably)
{
    std::string out;

    if ( begin != end ) {
        out += (*begin)->toString(readably);
        ++begin;
    }

    for ( ; begin != end; ++begin ) {
        out += sep;
        out += (*begin)->toString(readably);
    }

    return out;
}