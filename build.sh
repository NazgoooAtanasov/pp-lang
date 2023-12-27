set -xe

CFLAGS="-Wextra -Wall -pedantic -g -Isrc/includes"

gcc $CFLAGS src/main.c src/lex.c src/parser.c src/ast.c src/asmgen.c -o pp
