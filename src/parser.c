#include "includes/parser.h"
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

void parser_parse_boolean(Parser* parser) {
    parser_parse_expression(parser);

    Token t = parser_peek_next_token(parser);
    if (t.type == GTE_OPERATOR || t.type == GT_OPERATOR || t.type == LTE_OPERATOR || t.type == LT_OPERATOR || t.type == EQ_OPERATOR) {
        parser_get_next_token(parser); // consume the logical operator
        parser_parse_expression(parser);
    }
}


void parser_parse_booleanexpr(Parser* parser) {
    parser_parse_boolean(parser);

    Token t = parser_peek_next_token(parser);
    while (t.type == LOGIC_OR || t.type == LOGIC_AND) {
        parser_get_next_token(parser);
        parser_parse_boolean(parser);
        t = parser_peek_next_token(parser);
    }
}

void parser_parse_operand(Parser* parser) {
    Token t = parser_get_next_token(parser);
    if (t.type != IDENT && t.type != CONST && t.type != OPEN_PAREN) {
        fprintf(stderr, "[SYNTAX_ERROR] Expected identifier or constant in computation, but got '%s'.\n", t.str_value);
        exit(1);
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

void parser_parse_computation(Parser* parser) {
    parser_parse_operand(parser);

    Token t = parser_peek_next_token(parser);
    while(t.type == ADD_OPERATOR || t.type == SUBT_OPERATOR) {
        parser_get_next_token(parser); // consume the math operator
        parser_parse_operand(parser);

        t = parser_peek_next_token(parser);
    }
}

void parser_parse_expression(Parser* parser) {
    parser_parse_computation(parser);

    Token t = parser_peek_next_token(parser);
    while(t.type == MULT_OPERATOR || t.type == DIV_OPERATOR) {
        parser_get_next_token(parser); // consume the math operator
        parser_parse_computation(parser);

        t = parser_peek_next_token(parser);
    }
}

void parser_parse_if_stmt(Parser* parser) {
    parser_parse_booleanexpr(parser);
    parser_parse_block(parser);

    Token t = parser_peek_next_token(parser);
    if (t.type == KEYWORD && strcmp(t.str_value, "else") == 0) {
        parser_get_next_token(parser); // consume the else keyword
        parser_parse_block(parser);
    }
}

void parser_parse_while_stmt(Parser* parser) {
    parser_parse_booleanexpr(parser);
    parser_parse_block(parser);
}

void parser_parse_blocked_stmt(Parser* parser, const char* token_str_value) {
    if (strcmp(token_str_value, "if") == 0) {
        parser_parse_if_stmt(parser);
    } else if (strcmp(token_str_value, "while") == 0) {
        parser_parse_while_stmt(parser);
    }
}

ast_variable_assign* parser_parse_variable_assign(Parser* parser, const Token* current_token) {
    Token t = parser_get_next_token(parser);

    if (t.type == TYPE_ANNOT_PREFIX) {
        t = parser_get_next_token(parser);
        if (t.type != KEYWORD && strcmp(t.str_value, "integer") != 0) {
            fprintf(stderr, "[SYNTAX_ERROR] Expected type after type anotation prefix (:), but got '%s'.\n", t.str_value);
            exit(1);
        }
        t = parser_get_next_token(parser);
    }

    if (t.type != ASSIGN_OPERATOR) {
        fprintf(stderr, "[SYNTAX_ERROR] Expected assign operator (=), but got '%s'.\n", t.str_value);
        exit(1);
    }

    ast_variable_assign* variable_assign = malloc(sizeof(ast_variable_assign));
    strcpy(variable_assign->ident, current_token->str_value);
    parser_parse_expression(parser);
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
        parser_parse_blocked_stmt(parser, t.str_value);
        assert(true && "Attaching blocked stmts is not implemented yet.");
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

    ast_fun_def* fun_def = malloc(sizeof(ast_fun_def));
    ast_block* block = parser_parse_block(parser);
    fun_def->block = block;
    return fun_def;
}
