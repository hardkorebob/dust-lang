#include <stdio.h>
#include <stdlib.h>

typedef struct Lexer Lexer;
struct Lexer {
    char* source;
    int pos;
};
void test_array_access() {
Lexer* lex = (Lexer*)malloc(sizeof(Lexer));
(lex->source = "test string");
(lex->pos = 4);
char c = lex->source[lex->pos];
printf("Character at position %d: '%c'\n", lex->pos, c);
char c2 = lex->source[0];
printf("First character: '%c'\n", c2);
char arr[5] = { 'a', 'b', 'c', 'd', 'e' };
char c3 = arr[2];
printf("Array element: '%c'\n", c3);
free(lex);
return;
}
int main() {
printf("Testing array access parsing...\n");
test_array_access();
printf("Test completed!\n");
return 0;
}
