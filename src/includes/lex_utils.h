#ifndef PP_LANG_LEX_UTILS_
#define PP_LANG_LEX_UTILS_

#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define KEYWORD_COUNT 10
static const char* KEYWORDS[] = {"def", "main", "do", "end", "if", "else", "while", "puts", "reads", "integer"};
static inline bool is_keyword(const char* word) {
    assert(KEYWORD_COUNT == (sizeof(KEYWORDS) / sizeof(char*)) && "[COMPILER_ERROR]: Excessive number of keywords");

    for (int i = 0; i < KEYWORD_COUNT; ++i) {
        if (strcmp(word, KEYWORDS[i]) == 0) {
            return true;
        }
    }

    return false;
}

static inline bool is_numeric(const char c) {
    return c >= 48 && c <= 57;
}

static inline bool is_alpha(const char c) {
    return c >= 65 && c <= 122;
}

static inline bool is_identifier(const char* word) {
    if (is_numeric(word[0])) {
        return false;
    }

    size_t len = strlen(word);
    for (size_t i = 0; i < len; ++i) {
        if (!is_alpha(word[i])) {
            return false;
        }
    }
    return true;
}

static inline bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

static inline bool is_math_add_or_subt(char c) {
    return c == '+' || c == '-';
}

static inline bool is_math_mult_or_div(char c) {
    return c == '*' || c == '/';
}

static inline bool is_assign_op(char c) {
    return c == '=';
}

static inline bool is_semi_colon(char c) {
    return c == ';';
}

static inline bool is_open_paren(char c) {
    return c == '(';
}

static inline bool is_close_paren(char c) {
    return c == ')';
}

static inline bool is_logic_op_start(char c) {
    return c == '<' || c == '>';
}

static inline bool is_logic_gate_start(char c) {
    return c == '&' || c == '|';
}

static inline bool is_logic_gate(const char op[2]) {
    return strcmp(op, "&&") == 0 || strcmp(op, "||") == 0;
}

static inline bool is_logic_op(const char op[2]) {
    return strcmp(op, ">")  == 0   || 
           strcmp(op, "<")  == 0   || 
           strcmp(op, "<=") == 0   || 
           strcmp(op, ">=") == 0   || 
           strcmp(op, "==") == 0;
} 

static inline bool is_type_annot_prefix(const char c) {
    return c == ':';
}

#endif // PP_LANG_LEX_UTILS_
