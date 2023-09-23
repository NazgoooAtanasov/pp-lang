set -xe

CFLAGS="-Wextra -Wall -pedantic -g"

gcc $CFLAGS main.c -o pp
