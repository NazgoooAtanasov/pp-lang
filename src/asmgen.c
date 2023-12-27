#include "includes/asmgen.h"

Variable* asmgen_variables_find(Variables* vars, const char* ident) {
    for (size_t i = 0; i < vars->idx; ++i) {
        if (strcmp(vars->items[i].ident, ident) == 0) {
            return &vars->items[i];
            break;
        }
    }
    return NULL;
}

inline void asmgen_gen_assign(AsmGen* ag, size_t target_offset, size_t source_offset) {
    char instr_buff[1024]; // @INFO: this has been hardcoded because i was lazy to do it otherwise.
    sprintf(
        instr_buff, 
        "    mov rax, [rbp-%zu]\n"
        "    mov [rbp-%zu], eax\n",
        source_offset,
        target_offset
    );
    sb_append(&ag->asm_source, instr_buff, strlen(instr_buff));
}

inline void asmgen_gen_assigns(AsmGen* ag, size_t target_offset, const char* source) {
    char instr_buff[1024]; // @INFO: this has been hardcoded because i was lazy to do it otherwise.
    sprintf(instr_buff, "    mov DWORD [rbp-%zu],%s\n", target_offset, source);
    sb_append(&ag->asm_source, instr_buff, strlen(instr_buff));
}

inline void asmgen_gen_add(AsmGen* ag, size_t target_offset, size_t source_offset) {
    char instr_buff[1024]; // @INFO: this has been hardcoded because i was lazy to do it otherwise.
    sprintf(
        instr_buff,
        "    mov eax, [rbp-%zu]\n"
        "    add eax, [rbp-%zu]\n"
        "    mov [rbp-%zu], eax\n",
        target_offset,
        source_offset,
        target_offset
    );
    sb_append(&ag->asm_source, instr_buff, strlen(instr_buff));
}

inline void asmgen_gen_adds(AsmGen* ag, size_t target_offset, const char* source) {
    char instr_buff[1024]; // @INFO: this has been hardcoded because i was lazy to do it otherwise.
    sprintf(
        instr_buff, 
        "    add DWORD [rbp-%zu],%s\n",
        target_offset,
        source
    );
    sb_append(&ag->asm_source, instr_buff, strlen(instr_buff));
}

inline void asmgen_gen_sub(AsmGen* ag, size_t target_offset, size_t source_offset) {
    char instr_buff[1024]; // @INFO: this has been hardcoded because i was lazy to do it otherwise.
    sprintf(
        instr_buff,
        "    mov eax, [rbp-%zu]\n"
        "    sub eax, [rbp-%zu]\n"
        "    mov [rbp-%zu], eax\n",
        target_offset,
        source_offset,
        target_offset
    );
    sb_append(&ag->asm_source, instr_buff, strlen(instr_buff));
}

inline void asmgen_gen_subs(AsmGen* ag, size_t target_offset, const char* source) {
    char instr_buff[1024]; // @INFO: this has been hardcoded because i was lazy to do it otherwise.
    sprintf(
        instr_buff, 
        "    sub DWORD [rbp-%zu],%s\n", 
        target_offset, 
        source
    );
    sb_append(&ag->asm_source, instr_buff, strlen(instr_buff));
}

inline void asmgen_gen_mul(AsmGen* ag, size_t target_offset, size_t source_offset) {
    char instr_buff[1024]; // @INFO: this has been hardcoded because i was lazy to do it otherwise.
    sprintf(
        instr_buff,
        "    mov eax, [rbp-%zu]\n" 
        "    mul DWORD [rbp-%zu]\n"
        "    mov DWORD [rbp-%zu], eax\n",
        target_offset,
        source_offset,
        target_offset
    );
    sb_append(&ag->asm_source, instr_buff, strlen(instr_buff));
}

inline void asmgen_gen_muls(AsmGen* ag, size_t target_offset, const char* source) {
    char instr_buff[1024]; // @INFO: this has been hardcoded because i was lazy to do it otherwise.
    sprintf(
        instr_buff,
        "    mov eax, [rbp-%zu]\n" 
        "    mov ebx, %s\n" 
        "    mul DWORD ebx\n" 
        "    mov DWORD [rbp-%zu], eax\n",
        target_offset,
        source,
        target_offset
    );
    sb_append(&ag->asm_source, instr_buff, strlen(instr_buff));
}

inline void asmgen_gen_div(AsmGen* ag, size_t target_offset, size_t source_offset) {
    char instr_buff[1024]; // @INFO: this has been hardcoded because i was lazy to do it otherwise.
    sprintf(
        instr_buff,
        "    mov eax, [rbp-%zu]\n" 
        "    div DWORD [rbp-%zu]\n"
        "    mov DWORD [rbp-%zu], eax\n",
        target_offset,
        source_offset,
        target_offset
    );
    sb_append(&ag->asm_source, instr_buff, strlen(instr_buff));
}

inline void asmgen_gen_divs(AsmGen* ag, size_t target_offset, const char* source) {
    char instr_buff[1024]; // @INFO: this has been hardcoded because i was lazy to do it otherwise.
    sprintf(
        instr_buff,
        "    mov eax, [rbp-%zu]\n" 
        "    mov ebx, %s\n" 
        "    div DWORD ebx\n" 
        "    mov DWORD [rbp-%zu], eax\n",
        target_offset,
        source,
        target_offset
    );
    sb_append(&ag->asm_source, instr_buff, strlen(instr_buff));
}

void asmgen_create(AsmGen* ag) {
    sb_initialize(&ag->asm_source, 1024);
}

void asmgen_gen_header(AsmGen* ag) {
    const char* header = 
        "BITS 64\n" \
        "global _start\n" \
        "_start:\n" \
        "  call main\n";
    sb_append(&ag->asm_source, header, strlen(header));
}

void asmgen_gen_fun(AsmGen* ag, ast_fun_def* fun) {
    // gen fun label
    size_t fun_name_len = strlen(fun->name);
    char buff[fun_name_len + 2];
    sprintf(buff, "%s:\n", fun->name);
    sb_append(&ag->asm_source, buff, fun_name_len + 2);

    // gen fun stack frame
    const char* stack_frame_init = 
        "    push rbp\n" \
        "    mov rbp, rsp\n";
    size_t stack_frame_sz = strlen(stack_frame_init);

    const char* stack_frame_vars = "    sub rsp, ";
    size_t stack_frame_vars_sz = strlen(stack_frame_vars);

    size_t stack_frame_vars_bytes = fun->block->var_count * 4;
    size_t stack_frame_vars_bytes_sz = stack_frame_vars_bytes > 9 ? 2 : 1; // @TODO: make possible to have move than 99 bytes allocated per stack frame.

    char stack_frame_buf[stack_frame_sz + stack_frame_vars_sz + stack_frame_vars_bytes_sz + 1]; // +1 for the \n char
    sprintf(stack_frame_buf, "%s%s%zu\n", stack_frame_init, stack_frame_vars, stack_frame_vars_bytes);
    sb_append(&ag->asm_source, stack_frame_buf, strlen(stack_frame_buf));

    asmgen_gen_block(ag, fun->block);

    // @TODO: gen exit call. hardcoded for now
    const char* exit = 
        "    mov rax, 60\n" \
        "    mov rdi, 0\n" \
        "    syscall\n";
    sb_append(&ag->asm_source, exit, strlen(exit));

    // gen fun stack frame end
    const char* stack_frame_end = 
        "    mov rsp, rbp\n" \
        "    pop rbp\n" \
        "    ret\n";
    sb_append(&ag->asm_source, stack_frame_end, strlen(stack_frame_end));
}

void asmgen_gen_block(AsmGen* ag, ast_block* block) {
    ast_stmt* stmt = block->statements;
    while(stmt != NULL) {
        if (stmt->blockless && stmt->blockless->variable_assign) {
            ast_variable_assign* var_assign = stmt->blockless->variable_assign;
            asmgen_gen_var_assign(ag, var_assign);
            ag->vars.items[ag->vars.idx++] = (Variable) {
                .ident = var_assign->ident,
                .offset = var_assign->offset
            };
        }

        stmt = stmt->next;
    }
}

void asmgen_gen_var_assign(AsmGen* ag, ast_variable_assign* var_assign) {
    asmgen_gen_expr_var(ag, var_assign, var_assign->value, '+'); // the '+' is not actually used at all when this function is called
}

void asmgen_gen_expr_var(AsmGen* ag, ast_variable_assign* var_assign, ast_expression* expr, char math_op) {
    if (expr->left == NULL && expr->right == NULL) {
        if (var_assign->assigned == false) {
            if (expr->identifier) {
                // the var should never be NULL here. there will be a semantic checks and type checks in the futuire to guarentee that. 
                Variable* var = asmgen_variables_find(&ag->vars, expr->value);
                asmgen_gen_assign(ag, var_assign->offset * 4, var->offset * 4);
            } else {
                asmgen_gen_assigns(ag, var_assign->offset * 4, expr->value);
            }
            var_assign->assigned = true;
        } else {
            if (math_op == '+') {
                if (expr->identifier) {
                    Variable* var = asmgen_variables_find(&ag->vars, expr->value);
                    asmgen_gen_add(ag, var_assign->offset * 4, var->offset * 4);
                } else {
                    asmgen_gen_adds(ag, var_assign->offset * 4, expr->value);
                }
            } else if (math_op == '-') {
                if (expr->identifier) {
                    Variable* var = asmgen_variables_find(&ag->vars, expr->value);
                    asmgen_gen_sub(ag, var_assign->offset * 4, var->offset * 4);
                } else {
                    asmgen_gen_subs(ag, var_assign->offset * 4, expr->value);
                }
            } else if (math_op == '*') {
                if (expr->identifier) {
                    Variable* var = asmgen_variables_find(&ag->vars, expr->value);
                    asmgen_gen_mul(ag, var_assign->offset * 4, var->offset * 4);
                } else {
                    asmgen_gen_muls(ag, var_assign->offset * 4, expr->value);
                }
            } else if (math_op == '/') {
                if (expr->identifier) {
                    Variable* var = asmgen_variables_find(&ag->vars, expr->value);
                    asmgen_gen_div(ag, var_assign->offset * 4, var->offset * 4);
                } else {
                    asmgen_gen_divs(ag, var_assign->offset * 4, expr->value);
                }
            } else {
                fprintf(stderr, "\"%c\" is not yet implemented for asm generation\n", math_op);
                exit(1);
            }
        }
    } else {
        if (expr->left->high_expr || (!expr->left->high_expr && !expr->right->high_expr)) {
            asmgen_gen_expr_var(ag, var_assign, expr->left, expr->operation);
            asmgen_gen_expr_var(ag, var_assign, expr->right, expr->operation);
        } else if (expr->right->high_expr) {
            asmgen_gen_expr_var(ag, var_assign, expr->right, expr->operation);
            asmgen_gen_expr_var(ag, var_assign, expr->left, expr->operation);
        }

    }
}

void asmgen_gen_asm(AsmGen* ag, ast_fun_def* ast) {
    asmgen_gen_header(ag);
    asmgen_gen_fun(ag, ast);

    FILE* f = fopen("out.asm", "w");
    fprintf(f, "%s", ag->asm_source.str);
    fclose(f);
}
