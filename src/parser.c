#include "includes/parser.h"
#include "ast.h"
#include <stdlib.h>
#include <string.h>

void parser_create(Parser* parser, Token* tokens, size_t size) {
    parser->tokens = tokens;
    parser->token_count = size;
    parser->idx = 0;
}

Token parser_get_next_token(Parser* parser) {
    if (parser->idx + 1 > parser->token_count) {
        fprintf(stderr, "[INTERNAL_ERROR] Tried to access token at index that is bigger than the max token count .\n");
        exit(1);
    }
    return parser->tokens[parser->idx++];
}

Token parser_peek_next_token(Parser* parser) {
    if (parser->idx + 1 > parser->token_count) {
        fprintf(stderr, "[INTERNAL_ERROR] Tried to access token at index that is bigger than the max token count .\n");
        exit(1);
    }
    return parser->tokens[parser->idx];
}

ast_boolean* parser_parse_boolean(Parser* parser) {
    ast_boolean* boolean = malloc(sizeof(ast_boolean));
    ast_expression* first_exrp = parser_parse_expression(parser);

    Token t = parser_peek_next_token(parser);
    if (t.type == GTE_OPERATOR || t.type == GT_OPERATOR || t.type == LTE_OPERATOR || t.type == LT_OPERATOR || t.type == EQ_OPERATOR) {
        boolean->left = first_exrp;

        t = parser_get_next_token(parser);
        if (t.type == GTE_OPERATOR) {
            boolean->op = GREATER_EQ;
        } else if (t.type == GT_OPERATOR) {
            boolean->op = GREATER;
        } else if (t.type == LTE_OPERATOR) {
            boolean->op = LESS_EQ;
        } else if (t.type == LT_OPERATOR) {
            boolean->op = LESS;
        } else if (t.type == EQ_OPERATOR) {
            boolean->op = EQUAL;
        }
        boolean->right = parser_parse_expression(parser);
    } else {
        boolean->value = first_exrp;
    }

    return boolean;
}

ast_booleanexpr* parser_parse_booleanexpr(Parser* parser) {
    void** bag = malloc(sizeof(void*) * 40);
    size_t idx = 0;
    ast_boolean* boolean = parser_parse_boolean(parser);
    bag[idx++] = boolean;

    Token t = parser_peek_next_token(parser);
    while (t.type == LOGIC_OR || t.type == LOGIC_AND) {
        t = parser_get_next_token(parser);
        Token* interim = malloc(sizeof(Token));
        memcpy(interim, &t, sizeof(Token));
        bag[idx++] = interim;
        boolean = parser_parse_boolean(parser);
        bag[idx++] = boolean;
        t = parser_peek_next_token(parser);
    }

    ast_booleanexpr* tree = build_boolean_expression_node(bag, idx);
    for (size_t i = 0; i < idx; ++i) {
        if (i % 2 != 0) {
            free(bag[i]);
        }
    }
    free(bag);
    return tree;
}

void parser_parse_operand(Parser* parser, StringBuilder* sb) {
    Token t = parser_get_next_token(parser);
    if (t.type != IDENT && t.type != CONST && t.type != OPEN_PAREN) {
        fprintf(stderr, "[SYNTAX_ERROR] Expected identifier or constant in computation, but got '%s'.\n", t.str_value);
        exit(1);
    } else {
        size_t len = strlen(t.str_value);
        sb_append(sb, t.str_value, len);
    }

    if (t.type == OPEN_PAREN) {
        parser_parse_expression(parser);

        t = parser_get_next_token(parser);
        if (t.type != CLOSING_PAREN) {
            fprintf(stderr, "[SYNTAX_ERROR] Expected closing parenthesis, but got '%s'.\n", t.str_value);
            exit(1);
        }
    }
}

ast_expression* parser_parse_expression(Parser* parser) {
    StringBuilder sb;
    sb_initialize(&sb, 10);
    parser_parse_operand(parser, &sb);

    Token t = parser_peek_next_token(parser);
    while(t.type == MULT_OPERATOR || t.type == DIV_OPERATOR || t.type == ADD_OPERATOR || t.type == SUBT_OPERATOR) {
        t = parser_get_next_token(parser);
        size_t len = strlen(t.str_value);
        sb_append(&sb, t.str_value, len);

        parser_parse_operand(parser, &sb);
        t = parser_peek_next_token(parser);
    }

    ast_expression* expr = build_expression_node(sb.str, sb.idx);
    sb_free(&sb);
    return expr;
}

ast_if_stmt* parser_parse_if_stmt(Parser* parser) {
    ast_if_stmt* if_stmt = malloc(sizeof(ast_if_stmt));

    if_stmt->boolean_expr = parser_parse_booleanexpr(parser);
    if_stmt->main_block = parser_parse_block(parser);

    Token t = parser_peek_next_token(parser);
    if (t.type == KEYWORD && strcmp(t.str_value, "else") == 0) {
        parser_get_next_token(parser); // consume the else keyword
        if_stmt->else_block = parser_parse_block(parser);
    }

    return if_stmt;
}

ast_while_stmt* parser_parse_while_stmt(Parser* parser) {
    ast_while_stmt* while_stmt = malloc(sizeof(ast_while_stmt));
    while_stmt->boolean_expr = parser_parse_booleanexpr(parser);
    while_stmt->main_block = parser_parse_block(parser);

    return while_stmt;
}

ast_blocked_stmt* parser_parse_blocked_stmt(Parser* parser, const char* token_str_value) {
    ast_blocked_stmt* blocked = malloc(sizeof(ast_blocked_stmt));
    if (strcmp(token_str_value, "if") == 0) {
        blocked->if_stmt = parser_parse_if_stmt(parser);
    } else if (strcmp(token_str_value, "while") == 0) {
        blocked->while_stmt = parser_parse_while_stmt(parser);
    }

    return blocked;
}

ast_variable_assign* parser_parse_variable_assign(Parser* parser, const Token* current_token) {
    ast_variable_assign* variable_assign = malloc(sizeof(ast_variable_assign));
    Token t = parser_get_next_token(parser);

    if (t.type == TYPE_ANNOT_PREFIX) {
        t = parser_get_next_token(parser);
        if (t.type != KEYWORD && strcmp(t.str_value, "integer") != 0) {
            fprintf(stderr, "[SYNTAX_ERROR] Expected type after type anotation prefix (:), but got '%s'.\n", t.str_value);
            exit(1);
        }

        if (t.type == KEYWORD && strcmp(t.str_value, "integer") == 0) {
            variable_assign->type = AST_VARIABLE_INTEGER;
        }

        t = parser_get_next_token(parser);
    }

    if (t.type != ASSIGN_OPERATOR) {
        fprintf(stderr, "[SYNTAX_ERROR] Expected assign operator (=), but got '%s'.\n", t.str_value);
        exit(1);
    }

    strcpy(variable_assign->ident, current_token->str_value);
    variable_assign->value = parser_parse_expression(parser);
    return variable_assign;
}

ast_blockless_stmt* parser_parse_blockless_stmt(Parser* parser, const Token* current_token) {
    ast_blockless_stmt* blockless_stmt = malloc(sizeof(ast_blockless_stmt));

    if (current_token->type == IDENT) {
        ast_variable_assign* variable_assign = parser_parse_variable_assign(parser, current_token);
        blockless_stmt->variable_assign = variable_assign;
    } else if (current_token->type == KEYWORD && strcmp(current_token->str_value, "puts") == 0) {
        parser_parse_expression(parser);
    } else if (current_token->type == KEYWORD && strcmp(current_token->str_value, "reads") == 0) {
        Token t = parser_get_next_token(parser);
        if (t.type != IDENT) {
            fprintf(stderr, "[SYNTAX_ERROR] expected identifier to follow a 'reads' call, but found %s.\n", t.str_value);
            exit(1);
        }
    }

    Token t = parser_peek_next_token(parser);
    if (t.type == SEMI) {
        parser_get_next_token(parser); // consume the semi token
    }

    return blockless_stmt;
}

ast_stmt* parser_parse_stmt(Parser* parser) {
    Token t = parser_peek_next_token(parser);

    if (t.type != IDENT && (t.type != KEYWORD && strcmp(t.str_value, "if") != 0) && strcmp(t.str_value, "while") != 0) {
        fprintf(stderr, "[SYNTAX_ERROR] Unexpected syntax for statement. Expected 'assignement, if, while, ..., but got '%s'.\n", t.str_value);
        exit(1);
    }

    ast_stmt* stmt = malloc(sizeof(ast_stmt));
    stmt->next = NULL;

    // move this check to something like `is_blockless_stmt_keyword`
    if (t.type == IDENT || (t.type == KEYWORD && strcmp(t.str_value, "puts") == 0) || (t.type == KEYWORD && strcmp(t.str_value, "reads") == 0)) {
        parser_get_next_token(parser); // the token needs to be consumed only if it is a valid stmt token
        ast_blockless_stmt* blockless_stmt = parser_parse_blockless_stmt(parser, &t);
        stmt->blockless = blockless_stmt;
        return stmt;
    } else if (t.type == KEYWORD && (strcmp(t.str_value, "if") == 0 || strcmp(t.str_value, "while") == 0)) { // move this check to something like `is_blocked_stmt_keyword`
        parser_get_next_token(parser); // the token needs to be consumed only if it is a valid stmt token
        stmt->blocked = parser_parse_blocked_stmt(parser, t.str_value);
        return stmt;
    }

    return NULL;
}

ast_block* parser_parse_block(Parser* parser) {
    Token t = parser_get_next_token(parser);
    if (t.type != KEYWORD || strcmp(t.str_value, "do") != 0) {
        fprintf(stderr, "[SYNTAX_ERROR] Expected keyword 'do' to define the start of the function, but got '%s'.\n", t.str_value);
        exit(1);
    }

    ast_block* block = malloc(sizeof(ast_block));
    block->statements = NULL;
    ast_stmt* parsed_stmt;
    do {
        parsed_stmt = parser_parse_stmt(parser);

        if (block->statements == NULL) {
            block->statements = parsed_stmt;
        } else {
            ast_stmt* s = block->statements;
            while (s->next != NULL) {
                s = s->next;
            }
            s->next = parsed_stmt;
        }
    } while (parsed_stmt != NULL);

    t = parser_get_next_token(parser);
    if (t.type != KEYWORD || strcmp(t.str_value, "end") != 0) {
        fprintf(stderr, "[SYNTAX_ERROR] Expected keyword 'end' to define the end of the function, but got '%s'.\n", t.str_value);
        exit(1);
    }

    return block;
}

ast_fun_def* parser_parse_z(Parser* parser) {
    ast_fun_def* fun_def = malloc(sizeof(ast_fun_def));
    Token t = parser_get_next_token(parser);
    if (t.type != KEYWORD || strcmp(t.str_value, "def") != 0) {
        fprintf(stderr, "[SYNTAX_ERROR] Expected keyword 'def' for function definition, but got '%s'.\n", t.str_value);
        exit(1);
    }

    t = parser_get_next_token(parser);
    if (t.type != KEYWORD || strcmp(t.str_value, "main") != 0) {
        fprintf(stderr, "[SYNTAX_ERROR] Expected identifier 'main' for main function definition, but got '%s'.\n", t.str_value);
        exit(1);
    }

    strcpy(fun_def->name, t.str_value);
    ast_block* block = parser_parse_block(parser);
    fun_def->block = block;
    return fun_def;
}
