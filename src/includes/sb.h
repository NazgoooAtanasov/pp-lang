#ifndef SB_H_
#define SB_H_

#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t sz;
    size_t idx;
    char* str;
} StringBuilder;

void sb_initialize(StringBuilder* sb, size_t sz);
void sb_zero(StringBuilder* sb);
void sb_append(StringBuilder* sb, const char* str, size_t len);
void sb_free(StringBuilder* sb);


#ifdef SB_IMPLEMENTATION
void sb_initialize(StringBuilder* sb, size_t sz) {
    sb->sz = sz;
    sb->str = (char*) malloc(sizeof(char) * sz);
    sb_zero(sb);
}

void sb_zero(StringBuilder* sb) {
    sb->idx = 0;
    memset(sb->str, '\0', sb->sz);
}

void sb_append(StringBuilder* sb, const char* str, size_t len) {
    if (sb->idx + len >= sb->sz) {
        sb->str = realloc(sb->str, sizeof(char) * (sb->idx + len) + 10);
        sb->sz = (sb->idx + len) + 10;
    }

    size_t idx = 0;
    while (idx < len) {
        sb->str[sb->idx++] = str[idx++];
    }
}

void sb_free(StringBuilder* sb) {
    free(sb->str);
    sb->str = NULL;
    sb->sz = 0;
    sb->idx = 0;
}
#endif

#endif
