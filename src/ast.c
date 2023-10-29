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

/* For debugging purposes */
/* void print_expression_node(ast_expression* node, int offset) { */
/*     if (node->left && node->right) { */
/*         printf("%*cEXPRESSION_NODE:\n", offset, ' '); */
/*         print_expression_node(node->left, offset * 2); */
/*         printf("%*cOPERATION: \"%c\"\n", offset * 2, ' ', node->operation); */
/*         print_expression_node(node->right, offset * 2); */
/*     } else { */
/*         printf("%*cVALUE:\n", offset, ' '); */
/*         printf("%*c-> %s\n", offset, ' ', node->value); */
/*     } */
/* } */
