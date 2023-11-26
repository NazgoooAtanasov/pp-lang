#ifndef PP_LANG_AST_
#define PP_LANG_AST_

#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

struct _ast_block;

typedef struct _ast_expression {
  struct _ast_expression* left;
  struct _ast_expression* right;
  char value[256];
  char operation;
} ast_expression;

typedef enum {
    AST_VARIABLE_INTEGER
} ast_variable_type;

typedef struct {
    char ident[256];
    ast_expression* value;
    ast_variable_type type;
} ast_variable_assign;

typedef struct {
    ast_variable_assign* variable_assign;
} ast_blockless_stmt;

typedef enum {
    NOOP,
    GREATER_EQ,
    GREATER,
    LESS_EQ,
    LESS,
    EQUAL
} ast_boolean_op;

typedef struct {
    ast_expression* left;
    ast_expression* right;
    ast_expression* value;
    ast_boolean_op op;
} ast_boolean;

typedef enum {
    EXPR_NOOP,
    AND_AND,
    OR_OR
} ast_booleanexpr_op;

typedef struct _ast_booleanexpr {
    struct _ast_booleanexpr* left;
    struct _ast_booleanexpr* right;
    ast_booleanexpr_op op;
    ast_boolean* value;
} ast_booleanexpr;

typedef struct {
    struct _ast_block* main_block;
    struct _ast_block* else_block;
    void* boolean_expr;
} ast_if_stmt;

typedef struct {
    void* boolean_expr;
    struct _ast_block* main_block;
} ast_while_stmt;

typedef struct {
    ast_if_stmt* if_stmt;
    ast_while_stmt* while_stmt;
} ast_blocked_stmt;

typedef struct _ast_stmt {
    struct _ast_stmt* next;
    ast_blockless_stmt* blockless;
    ast_blocked_stmt* blocked;
} ast_stmt;

typedef struct _ast_block {
    ast_stmt* statements;
} ast_block;

typedef struct {
    char name[256];
    ast_block* block;
} ast_fun_def;

ast_expression* build_expression_node(const char* expr_str, size_t sz);
ast_booleanexpr* build_boolean_expression_node(void** boolean_items, size_t sz);
void print_expression_node(ast_expression* node, int offset);
void ast_print_tree(ast_fun_def* fun, int offset);

#endif // PP_LANG_AST_
