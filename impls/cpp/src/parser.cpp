#include "parser.h"
#include "types.h"

#include <memory>

AST tokenize_string(const std::string& input)
{
    Tokeniser tokenizer(input);

    if ( tokenizer.eof() ) {
        throw EmptyInputException();
    }

    return read_form(tokenizer);
}

AST read_form(Tokeniser& tokenizer)
{
    assert(!tokenizer.eof() && "read_form failed - tokenizer reached EOF\n");

    const auto init_sequence = [&tokenizer] (char closing_bracket) -> AST_vec*
    {
        tokenizer.next();
        std::unique_ptr<AST_vec> items(new AST_vec);
        read_list(tokenizer, items.get(), closing_bracket);
        return items.release();
    };

    switch ( tokenizer.peek().front() ) {
        case '(': {
            return type::list(init_sequence(')'));
        }
        case '[': {
            return type::vector(init_sequence(']'));
        }
        case '{': {
            return type::hash(init_sequence('}'), false);
        }
        default:
            return read_atom(tokenizer);
    }
}

AST read_atom(Tokeniser& tokenizer)
{
    static const std::unordered_map<std::string, AST> constantTable = {
        {"false", type::falseValue()},
        {"nil", type::nilValue()},
        {"true", type::trueValue()}
    };

    static const std::unordered_map<std::string, std::string> macroTable = {
        {"@", "deref"},
        {"`", "quasiquote"},
        {"'", "quote"},
        {"~@", "splice-unquote"},
        {"~", "unquote"}
    };

    std::string token = tokenizer.next();
    if ( token[0] == '"' ) {
        return type::string(unescape(token));
    }

    if ( token[0] == ':' ) {
        return type::keyword(token);
    }

    if ( token == "^" ) {
        AST meta = read_form(tokenizer);
        AST value = read_form(tokenizer);

        return type::list(type::symbol("with-meta"), value, meta);
    }

    if ( auto constIt = constantTable.find(token); constIt != constantTable.end() ) {
        return constIt->second;
    }

    if ( auto macroIt = macroTable.find(token); macroIt != macroTable.end() ) {
        return process_macro(tokenizer, macroIt->second);
    }

    static const std::regex int_regex("^[-+]?\\d+$");
    if ( std::regex_match(token, int_regex) ) {
        return type::integer(token);
    }

    return type::symbol(token);
}

void read_list(Tokeniser& tokenizer, AST_vec* items, char closing_bracket)
{
    while ( !tokenizer.eof() && tokenizer.peek() != std::string(1, closing_bracket) ) {
        items->push_back(read_form(tokenizer));
    }

    if ( tokenizer.eof() ) {
        throw LISP_ERROR("EOF");
    }

    tokenizer.next(); // consume bracket
}

AST process_macro(Tokeniser& tokenizer, const std::string& symbol)
{
    return type::list(type::symbol(symbol), read_form(tokenizer));
}