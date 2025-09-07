#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "utils.h" 

static const char* KEYWORDS[] = {
    "if", "else", "while", "do", "for", "return", "break", "continue",
    "func", "let", "struct", "sizeof", "switch", "case", "default", NULL
};

static bool is_keyword(const char* word) {
    for (int i = 0; KEYWORDS[i]; i++) {
        if (strcmp(word, KEYWORDS[i]) == 0) return true;
    }
    return false;
}

Lexer* lexer_create(const char* source, const TypeTable* type_table) {
    Lexer* lex = malloc(sizeof(Lexer));
    lex->source = source;
    lex->len = strlen(source);
    lex->pos = 0;
    lex->line = 1;
    lex->type_table = type_table;
    return lex;
}

void lexer_destroy(Lexer* lex) {
    free(lex);
}

void token_free(Token* tok) {
    if (tok) {
        free(tok->text);
        free(tok->base_name);
        free(tok);
    }
}

static void skip_whitespace(Lexer* lex) {
    while (lex->pos < lex->len && isspace(lex->source[lex->pos])) {
        if (lex->source[lex->pos] == '\n') lex->line++;
        lex->pos++;
    }
}

static Token* make_token(TokenType type, const char* text, int line) {
    Token* tok = calloc(1, sizeof(Token));
    tok->type = type;
    tok->line = line;
    tok->text = clone_string(text);
    return tok;
}

Token* lexer_next(Lexer* lex) {
    skip_whitespace(lex);
    if (lex->pos >= lex->len) return make_token(TOKEN_EOF, "", lex->line);
    
    int start = lex->pos;
    char c = lex->source[lex->pos];
    
    if (c == '#') {
        lex->pos++;
        start = lex->pos;  
        while (lex->pos < lex->len && lex->source[lex->pos] != '\n') {
            lex->pos++;
        }
        
        int len = lex->pos - start;
        char* directive_line = malloc(len + 2);
        directive_line[0] = '#';  
        memcpy(directive_line + 1, lex->source + start, len);
        directive_line[len + 1] = '\0';
        
        Token* tok = make_token(TOKEN_DIRECTIVE, directive_line, lex->line);
        free(directive_line);
        return tok;
}
    
    if (c == '/' && lex->pos + 1 < lex->len && lex->source[lex->pos + 1] == '/') {
        while (lex->pos < lex->len && lex->source[lex->pos] != '\n') lex->pos++;
        return lexer_next(lex);
    }
    
    if (isalpha(c) || c == '_') {
        while (lex->pos < lex->len && (isalnum(lex->source[lex->pos]) || lex->source[lex->pos] == '_')) {
            lex->pos++;
        }
        int len = lex->pos - start;
        char* word = malloc(len + 1);
        memcpy(word, lex->source + start, len);
        word[len] = '\0';
        
        Token* tok;
        if (is_keyword(word)) {
            tok = make_token(TOKEN_KEYWORD, word, lex->line);
        } else {
            tok = make_token(TOKEN_IDENTIFIER, word, lex->line);
            
            SuffixInfo info;
            if (suffix_parse(word, lex->type_table, &info)) {
                const char* separator = find_suffix_separator(word);
                if (separator) {
                    size_t base_len = separator - word;
                    tok->base_name = malloc(base_len + 1);
                    memcpy(tok->base_name, word, base_len);
                    tok->base_name[base_len] = '\0';
                    tok->suffix_info = info;
                } else {
                    tok->suffix_info = info;
                }
            } else {
                tok->suffix_info = (SuffixInfo){TYPE_VOID, ROLE_NONE, false, false, NULL, false, TYPE_VOID, NULL};
            }
        }
        free(word);
        return tok;
    }
    if (isdigit(c)) {
        while (lex->pos < lex->len && isdigit(lex->source[lex->pos])) lex->pos++;
        if (lex->pos < lex->len && lex->source[lex->pos] == '.') {
            lex->pos++;
            while (lex->pos < lex->len && isdigit(lex->source[lex->pos])) lex->pos++;
        }
        int len = lex->pos - start;
        char* num = malloc(len + 1);
        memcpy(num, lex->source + start, len);
        num[len] = '\0';
        Token* tok = make_token(TOKEN_NUMBER, num, lex->line);
        free(num);
        return tok;
    }
    
    if (c == '"') {
        lex->pos++;
        start = lex->pos;
        while (lex->pos < lex->len && lex->source[lex->pos] != '"') {
            if (lex->source[lex->pos] == '\\' && lex->pos + 1 < lex->len) lex->pos++;
            lex->pos++;
        }
        int len = lex->pos - start;
        char* str = malloc(len + 1);
        memcpy(str, lex->source + start, len);
        str[len] = '\0';
        if (lex->pos < lex->len) lex->pos++;
        Token* tok = make_token(TOKEN_STRING, str, lex->line);
        free(str);
        return tok;
    }

    if (c == '\'') {
        lex->pos++;
        start = lex->pos;
        if (lex->source[lex->pos] == '\\' && lex->pos + 1 < lex->len) {
            lex->pos += 2;
        } else {
            lex->pos++;
        }
        int len = lex->pos - start;
        char* char_val = malloc(len + 1);
        memcpy(char_val, lex->source + start, len);
        char_val[len] = '\0';
        if (lex->pos < lex->len && lex->source[lex->pos] == '\'') {
            lex->pos++;
        }
        Token* tok = make_token(TOKEN_CHARACTER, char_val, lex->line);
        free(char_val);
        return tok;
    }
    
    if (c == '-' && lex->pos + 1 < lex->len && lex->source[lex->pos+1] == '>') {
        lex->pos += 2;
        return make_token(TOKEN_ARROW, "->", lex->line);
    }

    char op_text[3] = {c, '\0', '\0'};
    lex->pos++;
    if (lex->pos < lex->len) {
        char next_c = lex->source[lex->pos];
        if ((c == '=' && next_c == '=') || (c == '!' && next_c == '=') ||
            (c == '<' && next_c == '=') || (c == '>' && next_c == '=') ||
            (c == '&' && next_c == '&') || (c == '|' && next_c == '|')) {
            op_text[1] = next_c;
            lex->pos++;
        }
    }
    
    TokenType type = strchr("{}[]();,.:", c) ? TOKEN_PUNCTUATION : TOKEN_OPERATOR;
    return make_token(type, op_text, lex->line);
}
