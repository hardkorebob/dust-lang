#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define TOKEN_EOF 0
#define TOKEN_IDENTIFIER 1
#define TOKEN_NUMBER 2
#define TOKEN_KEYWORD 3
#define TOKEN_OPERATOR 4

typedef struct Token Token;
struct Token {
    int type;
    char* text;
    int line;
    Token* next;
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
void test_arrays();
void test_string_handling();
Token* lexer_next(Lexer* lex);
void skip_whitespace(Lexer* lex);
Lexer* lexer_create(char* source);
void token_free(Token* tok);
Token* make_token(int type, char* text, int line);

Token* make_token(int type, char* text, int line) {
Token* tok = (Token*)malloc(sizeof(Token));
if ((tok == NULL)) {
return NULL;
}
tok->type = type;
tok->line = line;
tok->next = NULL;
if ((text != NULL)) {
int len = strlen(text);
tok->text = (char*)malloc((len + 1));
strcpy(tok->text, text);
} else {
tok->text = NULL;
}
return tok;
}
void token_free(Token* tok) {
if ((tok == NULL)) {
return;
}
if ((tok->text != NULL)) {
free(tok->text);
}
free(tok);
}
Lexer* lexer_create(char* source) {
Lexer* lex = (Lexer*)malloc(sizeof(Lexer));
if ((lex == NULL)) {
return NULL;
}
lex->source = source;
lex->pos = 0;
lex->len = strlen(source);
lex->line = 1;
return lex;
}
void skip_whitespace(Lexer* lex) {
while ((lex->pos < lex->len)) {
char c = lex->source[lex->pos];
if (isspace(c)) {
if ((c == '\n')) {
lex->line = (lex->line + 1);
}
lex->pos = (lex->pos + 1);
} else {
break;
}
}
}
Token* lexer_next(Lexer* lex) {
skip_whitespace(lex);
if ((lex->pos >= lex->len)) {
return make_token(TOKEN_EOF, "", lex->line);
}
int start = lex->pos;
char c = lex->source[lex->pos];
if ((isalpha(c) || (c == '_'))) {
while ((lex->pos < lex->len)) {
char ch = lex->source[lex->pos];
if ((isalnum(ch) || (ch == '_'))) {
lex->pos = (lex->pos + 1);
} else {
break;
}
}
int len = (lex->pos - start);
char* word = (char*)malloc((len + 1));
memcpy(word, (lex->source + start), len);
word[len] = '\0';
int type = TOKEN_IDENTIFIER;
if (((strcmp(word, "if") == 0) || (strcmp(word, "while") == 0))) {
type = TOKEN_KEYWORD;
}
Token* tok = make_token(type, word, lex->line);
free(word);
return tok;
}
if (isdigit(c)) {
while (((lex->pos < lex->len) && isdigit(lex->source[lex->pos]))) {
lex->pos = (lex->pos + 1);
}
int len = (lex->pos - start);
char* num = (char*)malloc((len + 1));
memcpy(num, (lex->source + start), len);
num[len] = '\0';
Token* tok = make_token(TOKEN_NUMBER, num, lex->line);
free(num);
return tok;
}
lex->pos = (lex->pos + 1);
char op[2];
op[0] = c;
op[1] = '\0';
return make_token(TOKEN_OPERATOR, op, lex->line);
}
void test_string_handling() {
printf("Testing string operations:\n");
char* test = "Hello, Dust!";
char* copy = (char*)malloc((strlen(test) + 1));
strcpy(copy, test);
printf("  Original: %s\n", test);
printf("  Copy: %s\n", copy);
copy[0] = 'J';
printf("  Modified: %s\n", copy);
free(copy);
}
void test_arrays() {
printf("\nTesting array operations:\n");
int nums[10];
int i = 0;
while ((i < 10)) {
nums[i] = (i * i);
i = (i + 1);
}
printf("  nums[0] = %d\n", nums[0]);
printf("  nums[5] = %d\n", nums[5]);
printf("  nums[9] = %d\n", nums[9]);
}
int main() {
printf("=== Dust Self-Hosting Capability Test ===\n\n");
test_string_handling();
test_arrays();
printf("\nTesting lexer:\n");
char* source = "if x 123 + while";
Lexer* lex = lexer_create(source);
if ((lex == NULL)) {
printf("  Failed to create lexer\n");
return 1;
}
printf("  Tokenizing: '%s'\n", source);
int token_count = 0;
while (1) {
Token* tok = lexer_next(lex);
if ((tok->type == TOKEN_EOF)) {
token_free(tok);
break;
}
printf("    Token %d: type=%d, text='%s', line=%d\n", token_count, tok->type, tok->text, tok->line);
token_count = (token_count + 1);
token_free(tok);
}
printf("  Total tokens: %d\n", token_count);
free(lex);
printf("\n=== All tests passed! ===\n");
return 0;
}
