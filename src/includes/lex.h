#ifndef PP_LANG_LEX_
#define PP_LANG_LEX_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lex_utils.h"

#define SOURCE_CODE_MAX_CAP 1024

typedef enum {
    UNDEFINED,
    CONST,
    IDENT,
    KEYWORD,
    ADD_OPERATOR,
    SUBT_OPERATOR,
    MULT_OPERATOR,
    DIV_OPERATOR,
    ASSIGN_OPERATOR,
    SEMI,
    TYPE_ANNOT_PREFIX,
    OPEN_PAREN,
    CLOSING_PAREN,
    GT_OPERATOR,
    LT_OPERATOR,
    GTE_OPERATOR,
    LTE_OPERATOR,
    EQ_OPERATOR,
    LOGIC_AND,
    LOGIC_OR,
} TokenType;

typedef struct {
    TokenType type;
    char str_value[256];
} Token;

typedef struct {
    const char* source_code;
    size_t source_size;
    Token tokens[SOURCE_CODE_MAX_CAP];
    size_t token_size;
} Lexer;

const char* get_token_str(TokenType t);
TokenType get_logic_op_type(const char op[2]);
TokenType get_logic_gate_type(const char op[2]);

void create_lexer(Lexer* lex, const char* source_code);
void gen_tokens(Lexer* lex);

#endif // PP_LANG_LEX_
