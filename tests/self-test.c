#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Arena Arena;
struct Arena {
    char* data;
    void size_st;
    void used_st;
};
typedef struct Lexer Lexer;
struct Lexer {
    char* source;
    int pos;
    int len;
    int line;
};
// Forward declarations
int main();
int compile(char* source);
void emit_number(void* node);
void emit_identifier(void* node);
int str_cmp(char* a, char* b);
void* arena_alloc(void size_st);
void arena_init(void size_st);

Arena g_arena;
void (*emit_table[])(void*) = { emit_identifier, emit_number, NULL, NULL };
void arena_init(void size_st) {
g_arena.data = malloc(size_st);
g_arena.size_st = size_st;
g_arena.used_st = 0;
}
void* arena_alloc(void size_st) {
size_st = ((size_st + 7) & ~7);
if (((g_arena.used_st + size_st) > g_arena.size_st)) {
fprintf(stderr, "Arena out of memory\n");
exit(1);
}
void* ptr = (g_arena.data + g_arena.used_st);
g_arena.used_st = (g_arena.used_st + size_st);
memset(ptr, 0, size_st);
return ptr;
}
int str_cmp(char* a, char* b) {
void i_st = 0;
while (((a[i_st] != '\0') && (b[i_st] != '\0'))) {
if ((a[i_st] != b[i_st])) {
return (a[i_st] - b[i_st]);
}
i_st = (i_st + 1);
}
return (a[i_st] - b[i_st]);
}
void emit_identifier(void* node) {
fprintf(stdout, "%s", (char*)node);
}
void emit_number(void* node) {
fprintf(stdout, "%s", (char*)node);
}
int compile(char* source) {
Lexer* lex = arena_alloc(sizeof(Lexer));
lex->source = source;
lex->pos = 0;
lex->len = strlen(source);
lex->line = 1;
int token_count = 0;
while ((lex->pos < lex->len)) {
char c = lex->source[lex->pos];
if (isalpha(c)) {
while (isalnum(lex->source[lex->pos])) {
lex->pos = (lex->pos + 1);
}
token_count = (token_count + 1);
} else if (isdigit(c)) {
while (isdigit(lex->source[lex->pos])) {
lex->pos = (lex->pos + 1);
}
token_count = (token_count + 1);
} else {
lex->pos = (lex->pos + 1);
}
}
return token_count;
}
int main() {
arena_init((1024 * 1024));
printf("Testing Dust compiler...\n");
char* test = "func test_i() { return 42 }";
int tokens = compile(test);
printf("Tokenized %d tokens\n", tokens);
if ((emit_table[0] != NULL)) {
printf("Dispatch table initialized\n");
}
if ((str_cmp("dust", "dust") == 0)) {
printf("String comparison works\n");
}
return 0;
}
