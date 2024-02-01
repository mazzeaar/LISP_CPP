#ifndef PARSER_H
#define PARSER_H

#include <string>

#include "def.h"
#include "tokenizer.h"

AST read_atom(Tokeniser& tokenizer);
AST read_form(Tokeniser& tokenizer);
void read_list(Tokeniser& tokenizer, AST_vec* items, char closing_bracket);
AST process_macro(Tokeniser& tokenizer, const std::string& symbol);
AST tokenize_string(const std::string& input);

#endif // PARSER_H