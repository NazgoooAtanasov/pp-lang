#include "includes/lex.h"

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
        case TYPE_ANNOT_PREFIX:
            return "TYPE_ANNOT_PREFIX";
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
        } else if (is_type_annot_prefix(lex->source_code[i])) {
            Token t = { .type = TYPE_ANNOT_PREFIX };
            t.str_value[0] = lex->source_code[i];
            lex->tokens[lex->token_size++] = t;
            i++;
        } else {
            fprintf(stderr, "[LEXICAL ERROR] Wrong symbol '%c'.\n", lex->source_code[i]);
            exit(1);
        }
    }
}
