#ifndef PP_LANG_ASMGEN_
#define PP_LANG_ASMGEN_

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "sb.h"
#include "ast.h"

typedef struct {
    const char* ident;
    size_t offset;
} Variable;

typedef struct {
    Variable items[1024];
    size_t idx;
} Variables;

Variable* asmgen_variables_find(Variables* vars, const char* ident);

typedef struct {
    StringBuilder asm_source;
    Variables vars; // @INFO: this will be capped to 1024 for now.
} AsmGen;

void asmgen_gen_assign(AsmGen* ag, size_t target_offset, size_t source_offset);
void asmgen_gen_assigns(AsmGen* ag, size_t target_offset, const char* source);

void asmgen_gen_add(AsmGen* ag, size_t target_offset, size_t source_offset);
void asmgen_gen_adds(AsmGen* ag, size_t target_offset, const char* source);

void asmgen_gen_sub(AsmGen* ag, size_t target_offset, size_t source_offset);
void asmgen_gen_subs(AsmGen* ag, size_t target_offset, const char* source);

void asmgen_gen_mul(AsmGen* ag, size_t target_offset, size_t source_offset);
void asmgen_gen_muls(AsmGen* ag, size_t target_offset, const char* source);

void asmgen_gen_div(AsmGen* ag, size_t target_offset, size_t source_offset);
void asmgen_gen_divs(AsmGen* ag, size_t target_offset, const char* source);

void asmgen_gen_header(AsmGen* ag);
void asmgen_gen_fun(AsmGen* ag, ast_fun_def* fun);
void asmgen_gen_block(AsmGen* ag, ast_block* block);
void asmgen_gen_var_assign(AsmGen* ag, ast_variable_assign* var_assign);
void asmgen_gen_expr_var(AsmGen* ag, ast_variable_assign* var_assign, ast_expression* expr, char math_op);

void asmgen_create(AsmGen* ag);
void asmgen_gen_asm(AsmGen* ag, ast_fun_def* ast);

#endif // PP_LANG_ASMGEN_
