#include <stdbool.h>

typedef struct OpInfo OpInfo;
struct OpInfo {
    char* op;
    int precedence;
    int left_assoc;
};
OpInfo operator_table[] = { { "*", 10, true }, { "/", 10, true }, { "%", 10, true }, { "+", 9, true }, { "-", 9, true }, { "<<", 8, true }, { ">>", 8, true }, { "<", 7, true }, { ">", 7, true }, { "<=", 7, true }, { ">=", 7, true }, { "==", 6, true }, { "!=", 6, true }, { "&", 5, true }, { "^", 4, true }, { "|", 3, true }, { "&&", 2, true }, { "||", 1, true }, { "=", 0, false }, { "+=", 0, false }, { "-=", 0, false }, { "*=", 0, false }, { "/=", 0, false }, { "%=", 0, false }, { "&=", 0, false }, { "|=", 0, false }, { "^=", 0, false }, { "<<=", 0, false }, { ">>=", 0, false }, { NULL, 0, false } };
// Forward declarations
void parse_binary_expr_ASTNodep(void p_Parserp, int min_precedence);

void parse_binary_expr_ASTNodep(void p_Parserp, int min_precedence) {
void left_ASTNodep = parse_unary_ASTNodep(p_Parserp);
while (true) {
char* current_op_text = p_Parserp->current->text;
OpInfo* op_info = NULL;
OpInfo* op_iterator = &operator_table[0];
while ((op_iterator->op != NULL)) {
if (((strcmp(current_op_text, op_iterator->op) == 0) && (op_iterator->precedence >= min_precedence))) {
op_info = op_iterator;
break;
}
op_iterator++;
}
if ((op_info == NULL)) {
break;
}
void op_tok_Tokenp = advance_Tokenp(p_Parserp);
int next_min_prec = (op_info->left_assoc ? (op_info->precedence + 1) : op_info->precedence);
void right_ASTNodep = parse_binary_expr_ASTNodep(p_Parserp, next_min_prec);
void node_ASTNodep = create_node_ASTNodep(AST_BINARY_OP, op_tok_Tokenp->text);
add_child(node_ASTNodep, left_ASTNodep);
add_child(node_ASTNodep, right_ASTNodep);
left_ASTNodep = node_ASTNodep;
}
return left_ASTNodep;
}
