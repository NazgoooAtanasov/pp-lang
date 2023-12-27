#include "includes/ast.h"
#include "includes/lex.h"
#include "includes/lex_utils.h"

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

        if (new_expr->operation == '*' || new_expr->operation == '/') {
            new_expr->high_expr = true;
        }
    } else {
        new_expr->identifier = is_identifier(expr_str);
        strcpy(new_expr->value, expr_str);
    }

    return new_expr;
}

ast_booleanexpr* build_boolean_expression_node(void** boolean_items, size_t sz) {
    ast_booleanexpr* node = malloc(sizeof(ast_booleanexpr));

    if (sz <= 1) {
        node->value = (ast_boolean*) boolean_items[0];
        return node;
    }

    size_t last_and_and_boolean_expr = 0;
    size_t last_or_or_boolean_expr = 0;

    for (size_t i = 0; i < sz; ++i) {
        if (i % 2 != 0) {
            Token* t = (Token*) boolean_items[i];
            if (t->type == LOGIC_OR) {
                last_or_or_boolean_expr = i;
            } else if(t->type == LOGIC_AND) {
                last_and_and_boolean_expr = i;
            }
        }
    }

    size_t idx = last_or_or_boolean_expr > 0 ? last_or_or_boolean_expr : last_and_and_boolean_expr;

    Token* t = (Token*) boolean_items[idx];
    node->op = t->type == LOGIC_OR ? OR_OR : AND_AND;

    node->left = build_boolean_expression_node(boolean_items, idx - 1);
    node->right = build_boolean_expression_node(&boolean_items[idx + 1], sz - idx - 1);

    return node;
}

const char* ast_get_variable_type(ast_variable_type t) {
    switch (t) {
        case AST_VARIABLE_INTEGER:
            return "INTEGER";
        default:
            return "N/A";
    }
}

const char* ast_get_boolean_op_str(ast_boolean_op op) {
    switch (op) {
        case GREATER_EQ: 
            return ">=";
        case GREATER:
            return ">";
        case LESS_EQ: 
            return "<=";
        case LESS:
            return "<";
        case EQUAL:
            return "==";
        case NOOP:
        default: {
            return "NOOP";
        }
    }
}

const char* ast_get_booleanexpr_op_str(ast_booleanexpr_op op) {
    switch (op) {
        case AND_AND:
            return "&&";
        case OR_OR:
            return "||";
        case EXPR_NOOP:
        default: 
            return "NOOP";
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

void print_boolean_node(ast_boolean* node, int offset) {
    if (node->left && node->right) {
        printf("%*c LEFT:\n", offset, ' ');
        print_expression_node(node->left, offset + 2);
        printf("%*c BOOLEAN_OP %s\n", offset, ' ', ast_get_boolean_op_str(node->op));
        printf("%*c RIGHT:\n", offset, ' ');
        print_expression_node(node->right, offset + 2);
    } else {
        print_expression_node(node->value, offset + 2);
    }
}

void print_booleanexpr_node(ast_booleanexpr* node, int offset) {
    printf("%*cBOOLEAN_EXPR:\n", offset, ' ');
    if (node->left && node->right) {
        printf("%*c LEFT:\n", offset, ' ');
        print_booleanexpr_node(node->left, offset + 2);
        printf("%*c BOOLEAN_EXPR %s\n", offset, ' ', ast_get_booleanexpr_op_str(node->op));
        printf("%*c RIGHT:\n", offset, ' ');
        print_booleanexpr_node(node->right, offset + 2);
    } else {
        printf("%*c EXPRESSION:\n", offset, ' ');
        print_boolean_node(node->value, offset + 2);
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
                print_booleanexpr_node(stmt->blocked->if_stmt->boolean_expr, offset + 2);
                printf("%*cIF_STMT_MAIN_BLOCK:\n", offset + 2, ' ');
                ast_print_block(stmt->blocked->if_stmt->main_block, offset + 2);

                if (stmt->blocked->if_stmt->else_block) {
                    printf("%*cIF_STMT_ELSE_BLOCK:\n", offset, ' ');
                    ast_print_block(stmt->blocked->if_stmt->else_block, offset + 2);
                }
            }

            if (stmt->blocked->while_stmt) {
                printf("%*cWHILE_STMT:\n", offset, ' ');
                print_booleanexpr_node(stmt->blocked->while_stmt->boolean_expr, offset + 2);
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

