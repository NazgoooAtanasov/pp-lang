#ifndef PP_LANG_AST_
#define PP_LANG_AST_

#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

struct _ast_expression;

typedef struct _ast_expression {
  struct _ast_expression* left;
  struct _ast_expression* right;
  char value[256];
  char operation;
} ast_expression;

typedef struct {
    char ident[256];
    ast_expression* value;
} ast_variable_assign;

typedef struct {
    ast_variable_assign* variable_assign;
} ast_blockless_stmt;

typedef struct _ast_stmt {
    struct _ast_stmt* next;
    ast_blockless_stmt* blockless;
    /* ast_blocked_stmt* blocked; */
} ast_stmt;

typedef struct {
    ast_stmt* statements;
} ast_block;

typedef struct {
    ast_block* block;
} ast_fun_def;

ast_expression* build_expression_node(const char* expr_str, size_t sz);
void print_expression_node(ast_expression* node, int offset);

#endif // PP_LANG_AST_
