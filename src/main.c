#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "includes/lex.h"
#include "includes/parser.h"

#define SOURCE_FILE "examples/main.pp"

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
    ast_fun_def* fun = parser_parse_z(&parser);

    for (size_t i = 0; i < lex.token_size; ++i) {
        printf("[TOKEN] %s %s\n", get_token_str(lex.tokens[i].type), lex.tokens[i].str_value);
    }

    return 0;
}
