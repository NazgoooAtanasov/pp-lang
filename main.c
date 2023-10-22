#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define SOURCE_CODE_MAX_CAP 1024
#define SOURCE_FILE "examples/main.pp"

#define KEYWORD_COUNT 9
static const char* KEYWORDS[] = {"def", "main", "do", "end", "if", "else", "while", "puts", "reads"};
bool is_keyword(const char* word) {
    assert(KEYWORD_COUNT == (sizeof(KEYWORDS) / sizeof(char*)) && "[COMPILER_ERROR]: Excessive number of keywords");

    for (int i = 0; i < KEYWORD_COUNT; ++i) {
        if (strcmp(word, KEYWORDS[i]) == 0) {
            return true;
        }
    }

    return false;
}

bool is_numeric(const char c) {
    if (c >= 48 && c <= 57) {
        return true;
    }
    return false;
}

bool is_alpha(const char c) {
    if (c >= 65 && c <= 122) {
        return true;
    }
    return false;
}

bool is_identifier(const char* word) {
    if (is_numeric(word[0])) {
        return false;
    }

    size_t len = strlen(word);
    for (size_t i = 0; i < len; ++i) {
        if (!is_alpha(word[i])) {
            return false;
        }
    }
    return true;
}

bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

bool is_math_add_or_subt(char c) {
    return c == '+' || c == '-';
}

bool is_math_mult_or_div(char c) {
    return c == '*' || c == '/';
}

bool is_assign_op(char c) {
    return c == '=';
}

bool is_semi_colon(char c) {
    return c == ';';
}

bool is_open_paren(char c) {
    return c == '(';
}

bool is_close_paren(char c) {
    return c == ')';
}

bool is_logic_op_start(char c) {
    return c == '<' || c == '>';
}

bool is_logic_gate_start(char c) {
    return c == '&' || c == '|';
}

bool is_logic_gate(const char op[2]) {
    return strcmp(op, "&&") == 0 || strcmp(op, "||") == 0;
}

bool is_logic_op(const char op[2]) {
    return strcmp(op, ">")  == 0   || 
           strcmp(op, "<")  == 0   || 
           strcmp(op, "<=") == 0   || 
           strcmp(op, ">=") == 0   || 
           strcmp(op, "==") == 0;
} 

typedef enum {
    UNDEFINED,
    CONST,
    IDENT,
    KEYWORD,
    ADD_OPERATOR,
    SUBT_OPERATOR,
    MULT_OPERATOR,
    DIV_OPERATOR,
    ASSIGN_OPERATOR,
    SEMI,
    OPEN_PAREN,
    CLOSING_PAREN,
    GT_OPERATOR,
    LT_OPERATOR,
    GTE_OPERATOR,
    LTE_OPERATOR,
    EQ_OPERATOR,
    LOGIC_AND,
    LOGIC_OR,
} TokenType;
const char* get_token_str(TokenType t) {
    switch(t) {
        case UNDEFINED:
            return "UNDEFINED";
        case CONST:
            return "CONST";
        case IDENT:
            return "IDENT";
        case KEYWORD:
            return "KEYWORD";
        case ADD_OPERATOR:
            return "ADD_OPERATOR";
        case SUBT_OPERATOR:
            return "SUBT_OPERATOR";
        case MULT_OPERATOR:
            return "MULT_OPERATOR";
        case DIV_OPERATOR:
            return "DIV_OPERATOR";
        case ASSIGN_OPERATOR:
            return "ASSIGN_OPERATOR";
        case SEMI:
            return "SEMI";
        case OPEN_PAREN:
            return "OPEN_PAREN";
        case CLOSING_PAREN:
            return "CLOSING_PAREN";
        case GT_OPERATOR:
            return "GT_OPERATOR";
        case LT_OPERATOR:
            return "LT_OPERATOR";
        case GTE_OPERATOR:
            return "GTE_OPERATOR";
        case LTE_OPERATOR:
            return "LTE_OPERATOR";
        case EQ_OPERATOR:
            return "EQ_OPERATOR";
        case LOGIC_AND:
            return "LOGIC_AND";
        case LOGIC_OR:
            return "LOGIC_OR";

        default:
            return "???";
    }
}

TokenType get_logic_op_type(const char op[2]) {
    if (strcmp(op, ">") == 0) return GT_OPERATOR;

    if (strcmp(op, "<")  == 0) return LT_OPERATOR;

    if (strcmp(op, "<=") == 0) return LTE_OPERATOR;

    if (strcmp(op, ">=") == 0) return GTE_OPERATOR;

    return EQ_OPERATOR;
}

TokenType get_logic_gate_type(const char op[2]) {
    if (strcmp(op, "&&") == 0) return LOGIC_AND;
    return LOGIC_OR;
}

typedef struct {
    TokenType type;
    char str_value[256];
} Token;

typedef struct {
    const char* source_code;
    size_t source_size;
    Token tokens[SOURCE_CODE_MAX_CAP];
    size_t token_size;
} Lexer;
void create_lexer(Lexer* lex, const char* source_code) {
    lex->source_code = source_code;
    lex->source_size = strlen(source_code);
    lex->token_size = 0;
}

void gen_tokens(Lexer* lex) {
    size_t i = 0;
    while(i < lex->source_size) {
        char buff[256] = {0};
        size_t idx = 0;

        if (is_whitespace(lex->source_code[i])) {
            i++;
            continue;
        }

        if (is_alpha(lex->source_code[i])) {
            while (is_alpha(lex->source_code[i]) || is_numeric(lex->source_code[i])) {
                buff[idx] = lex->source_code[i];
                i++; idx++;
            }

            if (is_keyword(buff)) {
                Token t = { .type = KEYWORD };
                strcpy(t.str_value, buff);
                lex->tokens[lex->token_size++] = t;
            } else {
                Token t = { .type = IDENT };
                strcpy(t.str_value, buff);
                lex->tokens[lex->token_size++] = t;
            }
        } else if (is_numeric(lex->source_code[i])) {
            while (is_numeric(lex->source_code[i])) {
                buff[idx] = lex->source_code[i];
                i++; idx++;
            }

            Token t = { .type = CONST };
            strcpy(t.str_value, buff);
            lex->tokens[lex->token_size++] = t;
        } else if (is_math_add_or_subt(lex->source_code[i])) {
            Token t = { .type = lex->source_code[i] == '+' ? ADD_OPERATOR : SUBT_OPERATOR };
            t.str_value[0] = lex->source_code[i];
            lex->tokens[lex->token_size++] = t;
            i++;
        } else if (is_math_mult_or_div(lex->source_code[i])) {
            Token t = { .type = lex->source_code[i] == '*' ? MULT_OPERATOR : DIV_OPERATOR };
            t.str_value[0] = lex->source_code[i];
            lex->tokens[lex->token_size++] = t;
            i++;
        } else if (is_assign_op(lex->source_code[i])) {
            if (lex->source_code[i + 1] == '=') { // parsing the '=' as logical operator. it will turn into '=='.
                char logical_eq_op[2];
                logical_eq_op[0] = lex->source_code[i++];
                logical_eq_op[1] = lex->source_code[i];

                Token t = { .type = EQ_OPERATOR };
                strcpy(t.str_value, logical_eq_op);
                lex->tokens[lex->token_size++] = t;
            } else { // parsing the '=' as assignement
                Token t = { .type = ASSIGN_OPERATOR };
                t.str_value[0] = lex->source_code[i];
                lex->tokens[lex->token_size++] = t;
            }
            i++;
        } else if (is_semi_colon(lex->source_code[i])) {
            Token t = { .type = SEMI };
            t.str_value[0] = lex->source_code[i];
            lex->tokens[lex->token_size++] = t;
            i++;
        } else if (is_open_paren(lex->source_code[i]) || is_close_paren(lex->source_code[i])) {
            Token t = { .type = is_open_paren(lex->source_code[i]) ? OPEN_PAREN : CLOSING_PAREN };
            t.str_value[0] = lex->source_code[i];
            lex->tokens[lex->token_size++] = t;
            i++;
        } else if (is_logic_op_start(lex->source_code[i])) {
            char logic_op[2];
            logic_op[0] = lex->source_code[i++];
            logic_op[1] = lex->source_code[i] == '=' ? '=' : '\0';

            if (!is_logic_op(logic_op)) {
                fprintf(stderr, "[LEXICAL ERROR] Unknown logical operator %s.\n", logic_op);
                exit(1);
            }

            Token t = { .type = get_logic_op_type(logic_op) };
            strcpy(t.str_value, logic_op);
            lex->tokens[lex->token_size++] = t;
            i++;
        } else if (is_logic_gate_start(lex->source_code[i])) {
            char logic_gate_op[2];
            logic_gate_op[0] = lex->source_code[i++];
            logic_gate_op[1] = lex->source_code[i];

            if (!is_logic_gate(logic_gate_op)) {
                fprintf(stderr, "[LEXICAL ERROR] Unknown logical gate %s.\n", logic_gate_op);
            }

            Token t = { .type = get_logic_gate_type(logic_gate_op) };
            strcpy(t.str_value, logic_gate_op);
            lex->tokens[lex->token_size++] = t;
            i++;
        } else {
            fprintf(stderr, "[LEXICAL ERROR] Wrong symbol.\n");
            exit(1);
        }
    }
}

typedef struct {
    Token* tokens;
    size_t token_count;
    size_t idx;
} Parser;

typedef enum {
    STMT_BLOCKLESS,
    STMT_BLOCKED,
    NOT_A_STMT
} STMT_TYPE;

void parser_parse_boolean(Parser* parser);
void parser_parse_booleanexpr(Parser* parser);
void parser_parse_operand(Parser* parser);
void parser_parse_computation(Parser* parser);
void parser_parse_expression(Parser* parser);
void parser_parse_variable_assign(Parser* parser);
void parser_parse_if_stmt(Parser* parser);
void parser_parse_while_stmt(Parser* parser);
void parser_parse_blocked_stmt(Parser* parser, const char* token_str_value);
void parser_parse_blockless_stmt(Parser* parser, const Token* current_token);
STMT_TYPE parser_parse_stmt(Parser* parser);
void parser_parse_block(Parser* parser);
void parser_parse_z(Parser* parser);

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

void parser_parse_variable_assign(Parser* parser) {
    Token t = parser_get_next_token(parser);
    if (t.type != ASSIGN_OPERATOR) {
        fprintf(stderr, "[SYNTAX_ERROR] Expected assign operator (=), but got '%s'.\n", t.str_value);
        exit(1);
    }
    parser_parse_expression(parser);
}

void parser_parse_blockless_stmt(Parser* parser, const Token* current_token) {
    if (current_token->type == IDENT) {
        parser_parse_variable_assign(parser);
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
}

STMT_TYPE parser_parse_stmt(Parser* parser) {
    Token t = parser_peek_next_token(parser);

    if (t.type != IDENT && (t.type != KEYWORD && strcmp(t.str_value, "if") != 0) && strcmp(t.str_value, "while") != 0) {
        fprintf(stderr, "[SYNTAX_ERROR] Unexpected syntax for statement. Expected 'assignement, if, while, ..., but got '%s'.\n", t.str_value);
        exit(1);
    }

    // move this check to something like `is_blockless_stmt_keyword`
    if (t.type == IDENT || (t.type == KEYWORD && strcmp(t.str_value, "puts") == 0) || (t.type == KEYWORD && strcmp(t.str_value, "reads") == 0)) {
        parser_get_next_token(parser); // the token needs to be consumed only if it is a valid stmt token
        parser_parse_blockless_stmt(parser, &t);
        return STMT_BLOCKLESS;
    } else if (t.type == KEYWORD && (strcmp(t.str_value, "if") == 0 || strcmp(t.str_value, "while") == 0)) { // move this check to something like `is_blocked_stmt_keyword`
        parser_get_next_token(parser); // the token needs to be consumed only if it is a valid stmt token
        parser_parse_blocked_stmt(parser, t.str_value);
        return STMT_BLOCKED;
    }

    return NOT_A_STMT;
}

void parser_parse_block(Parser* parser) {
    Token t = parser_get_next_token(parser);
    if (t.type != KEYWORD || strcmp(t.str_value, "do") != 0) {
        fprintf(stderr, "[SYNTAX_ERROR] Expected keyword 'do' to define the start of the function, but got '%s'.\n", t.str_value);
        exit(1);
    }

    STMT_TYPE stmt_parsed_type;
    do {
        stmt_parsed_type = parser_parse_stmt(parser);
    } while (stmt_parsed_type != NOT_A_STMT);

    /* parser_parse_stmt(parser); */
    /* t = parser_peek_next_token(parser); */
    /* while (t.type == SEMI) { */
    /*     parser_get_next_token(parser); // consume the semi */
    /*     parser_parse_stmt(parser); */
    /*     t = parser_peek_next_token(parser); */
    /* } */

    t = parser_get_next_token(parser);
    if (t.type != KEYWORD || strcmp(t.str_value, "end") != 0) {
        fprintf(stderr, "[SYNTAX_ERROR] Expected keyword 'end' to define the end of the function, but got '%s'.\n", t.str_value);
        exit(1);
    }
}

void parser_parse_z(Parser* parser) {
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

    parser_parse_block(parser);
}

int main(void) {
    FILE* source_file = fopen("examples/main.pp", "r");

    char source_code[SOURCE_CODE_MAX_CAP] = {0};
    if (fseek(source_file, 0, SEEK_END) != 0) {
        fprintf(stderr, "There was a problem opening your source file %s:%s\n", SOURCE_FILE, strerror(errno));
        exit(1);
    }

    int pos = ftell(source_file);

    if (fseek(source_file, 0, SEEK_SET) != 0) {
        fprintf(stderr, "There was a problem opening your source file %s:%s\n", SOURCE_FILE, strerror(errno));
        exit(1);
    }

    fread(source_code, sizeof(char), pos, source_file);
    fclose(source_file);

    Lexer lex;
    create_lexer(&lex, source_code);
    gen_tokens(&lex);

    Parser parser;
    parser_create(&parser, lex.tokens, lex.token_size);
    parser_parse_z(&parser);

    for (size_t i = 0; i < lex.token_size; ++i) {
        printf("[TOKEN] %s %s\n", get_token_str(lex.tokens[i].type), lex.tokens[i].str_value);
    }

    return 0;
}
