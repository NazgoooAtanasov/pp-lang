#ifndef PP_LANG_PARSER_
#define PP_LANG_PARSER_

#include "lex.h"
#include "ast.h"
#include "sb.h"

typedef enum {
    STMT_BLOCKLESS,
    STMT_BLOCKED,
    NOT_A_STMT
} STMT_TYPE;

typedef struct {
    Token* tokens;
    size_t token_count;
    size_t idx;
} Parser;
void parser_create(Parser* parser, Token* tokens, size_t size);
Token parser_get_next_token(Parser* parser);
Token parser_peek_next_token(Parser* parser);

void parser_parse_boolean(Parser* parser);
void parser_parse_booleanexpr(Parser* parser);
void parser_parse_operand(Parser* parser, StringBuilder* sb);
ast_expression* parser_parse_expression(Parser* parser);
ast_variable_assign* parser_parse_variable_assign(Parser* parser, const Token* current_token);
void parser_parse_if_stmt(Parser* parser);
void parser_parse_while_stmt(Parser* parser);
void parser_parse_blocked_stmt(Parser* parser, const char* token_str_value);
ast_blockless_stmt* parser_parse_blockless_stmt(Parser* parser, const Token* current_token);
ast_stmt* parser_parse_stmt(Parser* parser);
ast_block* parser_parse_block(Parser* parser);
ast_fun_def* parser_parse_z(Parser* parser);

#endif // PP_LANG_PARSER_
