#include <stdbool.h>
#include <stdio.h>

typedef enum TokenType {
  EOFF = 0,
  IDENTIFIER = 1,
  NUMBER = 2,
  KEYWORD = 3,
  OPERATOR = 4
} TokenType;
typedef enum ASTType {
  AST_PROGRAM = 0,
  AST_FUNCTION = 1,
  AST_VAR_DECL = 2,
  AST_BLOCK = 3,
  AST_IF = 4,
  AST_WHILE = 5
} ASTType;
typedef struct Token Token;
struct Token {
  TokenType type;
  char *text;
  int line;
};
// Forward declarations
int main();
char *token_type_name(TokenType type);
bool is_keyword(TokenType type);
TokenType get_eof_token();

TokenType get_eof_token() { return EOFF; }
bool is_keyword(TokenType type) {
  if ((type == KEYWORD)) {
    return 1;
  }
  return 0;
}
char *token_type_name(TokenType type) {
  switch (type) {
  case EOFF:
    return "EOF";
  case IDENTIFIER:
    return "IDENTIFIER";
  case NUMBER:
    return "NUMBER";
  case KEYWORD:
    return "KEYWORD";
  case OPERATOR:
    return "OPERATOR";
  default:
    return "UNKNOWN";
  }
}
int main() {
  printf("Testing Dust enum support with suffix style\n\n");
  TokenType token_type = IDENTIFIER;
  printf("Token type: %d\n", token_type);
  if ((token_type == IDENTIFIER)) {
    printf("It's an identifier!\n");
  }
  TokenType eof = get_eof_token();
  printf("EOF value: %d\n", eof);
  bool is_kw = is_keyword(KEYWORD);
  printf("KEYWORD is keyword: %d\n", is_kw);
  printf("Token names:\n");
  TokenType i = EOF;
  while ((i <= OPERATOR)) {
    char *name = token_type_name(i);
    printf("  %d: %s\n", i, name);
    (i = (i + 1));
  }
  Token tok;
  (tok.type = NUMBER);
  (tok.text = "42");
  (tok.line = 1);
  printf("\nToken struct: type=%d, text=%s, line=%d\n", tok.type, tok.text,
         tok.line);
  ASTType node_type = AST_FUNCTION;
  if ((node_type == AST_FUNCTION)) {
    printf("Node is a function\n");
  }
  return 0;
}
