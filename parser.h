// ===== parser.h =====
#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum {
    AST_PROGRAM,
    AST_FUNCTION,
    AST_VAR_DECL,
    AST_BLOCK,
    AST_INITIALIZER_LIST,
    AST_IF,
    AST_WHILE,
    AST_DO,
    AST_FOR,
    AST_CASE,
    AST_SWITCH,
    AST_BREAK,
    AST_DEFAULT,
    AST_CONTINUE,
    AST_RETURN,
    AST_EXPRESSION,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_CALL,
    AST_SUBSCRIPT,
    AST_IDENTIFIER,
    AST_NUMBER,
    AST_STRING,
    AST_CHARACTER,
    AST_SIZEOF,
    AST_STRUCT_DEF,
    AST_MEMBER_DECL,
    AST_DIRECTIVE,
    AST_MEMBER_ACCESS,
    AST_TERNARY_OP,
} ASTType;

typedef struct ASTNode {
    ASTType type;
    char* value;
    SuffixInfo suffix_info;
    struct ASTNode** children;
    int child_count;
    int child_cap;
} ASTNode;

typedef struct {
    Lexer* lexer;
    Token* current;
    TypeTable* type_table; // NEW: Parser needs access to the type table
    bool had_error;              // NEW: Flag for error reporting
} Parser;

// FIXED: Updated function signature to accept the type table
Parser* parser_create(const char* source, const TypeTable* type_table);
ASTNode* parser_parse(Parser* parser);
void parser_destroy(Parser* parser);
void ast_destroy(ASTNode* node);

#endif
