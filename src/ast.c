#include "includes/ast.h"
#include <stdio.h>

ast_expression* build_expression_node(const char* expr_str, size_t sz) {
    ast_expression* new_expr = malloc(sizeof(ast_expression));
    new_expr->right = NULL;
    new_expr->left = NULL;

    int last_low_order_sing_idx = -1;
    int last_high_order_sing_idx = -1;

    for (size_t i = 0; i < sz; ++i) {
        if (expr_str[i] == '+' || expr_str[i] == '-') { last_low_order_sing_idx = i; }
        if (expr_str[i] == '*' || expr_str[i] == '/') { last_high_order_sing_idx = i; }
    }

    int sign_idx = last_low_order_sing_idx > -1 ? last_low_order_sing_idx : last_high_order_sing_idx;

    if (sign_idx > -1) {
        char sub_expr[256];

        memset(sub_expr, '\0', 256);
        strncpy(sub_expr, expr_str, sign_idx);
        size_t len = strlen(sub_expr);
        new_expr->left = build_expression_node(sub_expr, len);

        memset(sub_expr, '\0', 256);
        strncpy(sub_expr, &expr_str[sign_idx+1], sz - sign_idx);
        len = strlen(sub_expr);
        new_expr->right = build_expression_node(sub_expr, len);

        new_expr->operation = expr_str[sign_idx];
    } else {
        strcpy(new_expr->value, expr_str);
    }

    return new_expr;
}

const char* ast_get_variable_type(ast_variable_type t) {
    switch (t) {
        case AST_VARIABLE_INTEGER:
            return "INTEGER";
        default:
            return "N/A";
    }
}

void print_expression_node(ast_expression* node, int offset) {
    if (node->left && node->right) {
        printf("%*cEXPRESSION_NODE:\n", offset, ' ');
        print_expression_node(node->left, offset + 2);
        printf("%*cOPERATION: \"%c\"\n", offset, ' ', node->operation);
        print_expression_node(node->right, offset + 2);
    } else {
        printf("%*cVALUE:\n", offset, ' ');
        printf("%*c-> %s\n", offset, ' ', node->value);
    }
}

void ast_print_block(ast_block* block, int offset) {
    ast_stmt* stmt = block->statements;
    while (stmt) {
        if (stmt->blockless) {
            printf("%*cVARIABLE_ASSIGN \"NAME\": %s, \"TYPE\": %s\n", offset, ' ', stmt->blockless->variable_assign->ident, ast_get_variable_type(stmt->blockless->variable_assign->type));
            print_expression_node(stmt->blockless->variable_assign->value, offset);
        }

        if (stmt->blocked) {
            if (stmt->blocked->if_stmt) {
                printf("%*cIF_STMT:\n", offset, ' ');
                printf("%*cBOOLEAN_EXPR: NULL\n", offset, ' ');
                printf("%*cIF_STMT_MAIN_BLOCK:\n", offset, ' ');
                ast_print_block(stmt->blocked->if_stmt->main_block, offset + 2);

                if (stmt->blocked->if_stmt->else_block) {
                    printf("%*cIF_STMT_ELSE_BLOCK:\n", offset, ' ');
                    ast_print_block(stmt->blocked->if_stmt->else_block, offset + 2);
                }
            }

            if (stmt->blocked->while_stmt) {
                printf("%*cWHILE_STMT:\n", offset, ' ');
                printf("%*cBOOLEAN_EXPR: NULL\n", offset, ' ');
                printf("%*cWHILE_STMT_BLOCK:\n", offset, ' ');
                ast_print_block(stmt->blocked->while_stmt->main_block, offset + 2);
            }
        }

        stmt = stmt->next;
    }
}

void ast_print_tree(ast_fun_def* fun, int offset) {
    printf("%*cFUN_DEF \"NAME\": %s\n", offset, ' ', fun->name);
    printf("%*cFUN_BLOCK:\n", offset, ' ');
    ast_print_block(fun->block, offset + 2);
}

