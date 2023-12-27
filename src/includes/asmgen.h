#ifndef PP_LANG_ASMGEN_
#define PP_LANG_ASMGEN_

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "sb.h"
#include "ast.h"

typedef struct {
    StringBuilder asm_source;
} AsmGen;

void asmgen_gen_header(AsmGen* ag);
void asmgen_gen_fun(AsmGen* ag, ast_fun_def* fun);
void asmgen_gen_block(AsmGen* ag, ast_block* block);
void asmgen_gen_var_assign(AsmGen* ag, ast_variable_assign* var_assign);
void asmgen_gen_expr_var(AsmGen* ag, ast_variable_assign* var_assign, ast_expression* expr, char math_op);

void asmgen_create(AsmGen* ag);
void asmgen_gen_asm(AsmGen* ag, ast_fun_def* ast);

#endif // PP_LANG_ASMGEN_
