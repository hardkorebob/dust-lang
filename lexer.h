#ifndef LEXER_H
#define LEXER_H

#include "component_system.h"
#include "type_table.h" 
typedef enum {
    TOKEN_EOF,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_KEYWORD,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATION,
    TOKEN_CHARACTER,
    TOKEN_DIRECTIVE,
    TOKEN_ARROW,
} TokenType;

typedef struct {
    TokenType type;
    char* text;
    char* base_name;
    SuffixInfo suffix_info; 
    int line;
} Token;

typedef struct {
    const char* source;
    int pos;
    int len;
    int line;
    const TypeTable* type_table;
} Lexer;

Lexer* lexer_create(const char* source, const TypeTable* type_table);
Token* lexer_next(Lexer* lex);
void lexer_destroy(Lexer* lex);
void token_free(Token* tok);

#endif
