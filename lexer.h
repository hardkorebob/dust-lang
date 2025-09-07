// ===== lexer.h =====
#ifndef LEXER_H
#define LEXER_H

#include "component_system.h"
#include "type_table.h" // NEW: Include the type table

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
    // For identifiers with a Dust suffix:
    char* base_name;
    SuffixInfo suffix_info; // FIXED: Store the struct directly, not a pointer
    int line;
} Token;

typedef struct {
    const char* source;
    int pos;
    int len;
    int line;
    const TypeTable* type_table; // NEW: Lexer now needs access to the type table
} Lexer;

// NEW: Updated function signature
Lexer* lexer_create(const char* source, const TypeTable* type_table);
Token* lexer_next(Lexer* lex);
void lexer_destroy(Lexer* lex);
void token_free(Token* tok);

#endif
