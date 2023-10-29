#ifndef PP_LANG_AST_
#define PP_LANG_AST_

typedef struct {
    char ident[256];
    void* value;
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

#endif // PP_LANG_AST_
