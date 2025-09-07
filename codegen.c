#include "codegen.h"
#include <string.h>

static FILE* output_file;
static const TypeTable* type_table;

static void emit_node(ASTNode* node);

static void emit_statement(ASTNode* node) {
    if (node->type == AST_BLOCK || node->type == AST_IF || node->type == AST_WHILE ||
        node->type == AST_FOR || node->type == AST_SWITCH) {
        emit_node(node);
        fprintf(output_file, "\n");
    } else if (node->type == AST_VAR_DECL) {
        emit_node(node);
        fprintf(output_file, ";\n");
    } else if (node->type == AST_EXPRESSION) {
        if (node->child_count > 0) {
            emit_node(node->children[0]);
        }
        fprintf(output_file, ";\n");
    } else {
        emit_node(node);
        fprintf(output_file, ";\n");
    }
}

static void emit_function(ASTNode* node) {
    const char* return_type = get_c_type(&node->suffix_info);
    fprintf(output_file, "%s %s(", return_type, node->value);

    if (node->child_count > 0) {
        ASTNode* params_node = node->children[0];
        for (int i = 0; i < params_node->child_count; i++) {
            if (i > 0) fprintf(output_file, ", ");
            ASTNode* param = params_node->children[i];
            const char* param_type = get_c_type(&param->suffix_info);
            fprintf(output_file, "%s %s", param_type, param->value);
        }
    }
    fprintf(output_file, ") ");
    if (node->child_count > 1) {
        emit_node(node->children[1]);
    } else {
        fprintf(output_file, "{}\n");
    }
}


static void emit_var_decl(ASTNode* node) {
    const char* c_type = get_c_type(&node->suffix_info);
    fprintf(output_file, "%s %s", c_type, node->value);
    if (node->suffix_info.type == TYPE_ARRAY) {
        fprintf(output_file, "[");
        if (node->child_count > 0 && node->children[0]) {
            emit_node(node->children[0]);
        }
        fprintf(output_file, "]");
    }
    int initializer_index = (node->suffix_info.type == TYPE_ARRAY) ? 1 : 0;
    if (node->child_count > initializer_index) {
        fprintf(output_file, " = ");
        emit_node(node->children[initializer_index]);
    }
}


static void emit_node(ASTNode* node) {
    if (!node) return;
    switch (node->type) {
        case AST_PROGRAM:
            for (int i = 0; i < node->child_count; i++) {
                if (node->children[i]->type == AST_DIRECTIVE) {
                    emit_node(node->children[i]);
                }
            }
            fprintf(output_file, "\n");
            for (int i = 0; i < node->child_count; i++) {
                if (node->children[i]->type != AST_DIRECTIVE) {
                    emit_node(node->children[i]);
                    fprintf(output_file, "\n");
                }
            }
            break;
        case AST_DIRECTIVE:
            fprintf(output_file, "%s\n", node->value);
            break;
        case AST_FUNCTION:
            emit_function(node);
            break;
        case AST_BLOCK:
            fprintf(output_file, "{\n");
            for (int i = 0; i < node->child_count; i++) {
                emit_statement(node->children[i]);
            }
            fprintf(output_file, "}");
            break;
        case AST_VAR_DECL:
            emit_var_decl(node);
            break;
        case AST_IF:
            fprintf(output_file, "if (");
            emit_node(node->children[0]);
            fprintf(output_file, ") ");
            emit_node(node->children[1]);
            if (node->child_count > 2) {
                fprintf(output_file, " else ");
                emit_node(node->children[2]);
            }
            break;
        case AST_RETURN:
            fprintf(output_file, "return");
            if (node->child_count > 0) {
                fprintf(output_file, " ");
                emit_node(node->children[0]);
            }
            break;
        case AST_BINARY_OP:
            fprintf(output_file, "(");
            emit_node(node->children[0]);
            fprintf(output_file, " %s ", node->value);
            emit_node(node->children[1]);
            fprintf(output_file, ")");
            break;
        case AST_UNARY_OP:
            fprintf(output_file, "%s(", node->value);
            emit_node(node->children[0]);
            fprintf(output_file, ")");
            break;
        case AST_TERNARY_OP:
            fprintf(output_file, "(");
            emit_node(node->children[0]);
            fprintf(output_file, " ? ");
            emit_node(node->children[1]); 
            fprintf(output_file, " : ");
            emit_node(node->children[2]); 
            fprintf(output_file, ")");
            break;
        case AST_CALL:
            fprintf(output_file, "%s(", node->value);
            for(int i = 0; i < node->child_count; i++) {
                if(i > 0) fprintf(output_file, ", ");
                emit_node(node->children[i]);
            }
            fprintf(output_file, ")");
            break;
        case AST_IDENTIFIER:
            fprintf(output_file, "%s", node->value);
            break;
        case AST_NUMBER:
            fprintf(output_file, "%s", node->value);
            break;
        case AST_STRING:
            fprintf(output_file, "\"%s\"", node->value);
            break;
        case AST_CHARACTER:
            fprintf(output_file, "'%s'", node->value);
            break;
        case AST_SIZEOF:
            fprintf(output_file, "sizeof(");
            if (node->suffix_info.type != TYPE_VOID) {
                fprintf(output_file, "%s", get_c_type(&node->suffix_info));
            } else if (node->child_count > 0) {
                ASTNode* child = node->children[0];
                const char* type_name = type_table_lookup(type_table, child->value);
                fprintf(output_file, "%s", type_name ? type_name : child->value);
            }
            fprintf(output_file, ")");
            break;
        case AST_STRUCT_DEF:
            fprintf(output_file, "typedef struct %s %s;\n", node->value, node->value);
            fprintf(output_file, "struct %s {\n", node->value);
            for (int i = 0; i < node->child_count; i++) {
                fprintf(output_file, "    ");
                emit_var_decl(node->children[i]);
                fprintf(output_file, ";\n");
            }
            fprintf(output_file, "};");
            break;
        case AST_WHILE:
            fprintf(output_file, "while (");
            emit_node(node->children[0]);
            fprintf(output_file, ") ");
            emit_node(node->children[1]);
            break;
        case AST_DO:
            fprintf(output_file, "do ");
            emit_node(node->children[0]);
            fprintf(output_file, " while (");
            emit_node(node->children[1]);
            fprintf(output_file, ")");
            fprintf(output_file, ";");
            break; 
        case AST_SUBSCRIPT:
            emit_node(node->children[0]);
            fprintf(output_file, "[");
            emit_node(node->children[1]);
            fprintf(output_file, "]");
            break;
        case AST_MEMBER_ACCESS:
            emit_node(node->children[0]); 
            fprintf(output_file, "%s", node->value); 
            emit_node(node->children[1]); 
            break;
        case AST_FOR:
            fprintf(output_file, "for (");
            if (node->children[0]) emit_node(node->children[0]);
            fprintf(output_file, "; ");
            if (node->children[1]) emit_node(node->children[1]);
            fprintf(output_file, "; ");
            if (node->children[2]) emit_node(node->children[2]);
            fprintf(output_file, ") ");
            if (node->children[3]) emit_node(node->children[3]);
            break;
        case AST_BREAK:
            fprintf(output_file, "break");
            break;
        case AST_CONTINUE:
            fprintf(output_file, "continue");
            break;
        case AST_INITIALIZER_LIST:
            fprintf(output_file, "{ ");
            for (int i = 0; i < node->child_count; i++) {
                emit_node(node->children[i]);
                if (i < node->child_count - 1) {
                    fprintf(output_file, ", ");
                }
            }
            fprintf(output_file, " }");
            break;
        case AST_SWITCH:
            fprintf(output_file, "switch (");
            emit_node(node->children[0]);
            fprintf(output_file, ") {\n"); 
            for (int i = 1; i < node->child_count; i++) {
                emit_node(node->children[i]);
            }
            fprintf(output_file, "}\n");
            break;
        case AST_CASE:
            fprintf(output_file, "case ");
            emit_node(node->children[0]); 
            fprintf(output_file, ":\n");
            for (int i = 1; i < node->child_count; i++) {
                emit_statement(node->children[i]);
            }
            break;
        case AST_DEFAULT:
            fprintf(output_file, "default:\n");
            for (int i = 0; i < node->child_count; i++) {
                emit_statement(node->children[i]);
            }
            break;
        case AST_EXPRESSION:
             if (node->child_count > 0) emit_node(node->children[0]);
             break;
        default:
            if (node->child_count > 0) {
                if (node->type == AST_EXPRESSION) {
                     emit_node(node->children[0]);
                }
            }
            break;
    }
}

void codegen(ASTNode* ast, const TypeTable* table, FILE* out) {
    output_file = out;
    type_table = table;
    emit_node(ast);
}

