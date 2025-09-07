#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"

// --- Forward Declarations ---
static ASTNode* parse_statement(Parser* p);
static ASTNode* parse_expression(Parser* p);
static ASTNode* parse_block(Parser* p);
static ASTNode* parse_if_statement(Parser* p);
static ASTNode* parse_struct_definition(Parser* p);
static ASTNode* parse_member_access(Parser* p);
static ASTNode* parse_subscript(Parser* p);
static ASTNode* parse_while_statement(Parser* p);
static ASTNode* parse_do_statement(Parser* p);
static ASTNode* parse_for_statement(Parser* p);
static ASTNode* parse_initializer_list(Parser *p);
static ASTNode* parse_switch_statement(Parser* p);
static ASTNode* parse_ternary(Parser* p); 
static ASTNode* parse_logical_or(Parser* p); 

// --- AST Node Management ---
static ASTNode* create_node(ASTType type, const char* value) {
    ASTNode* node = calloc(1, sizeof(ASTNode));
    node->type = type;
    node->value = value ? clone_string(value) : NULL;
    node->child_cap = 2;
    node->children = calloc(node->child_cap, sizeof(ASTNode*));
    return node;
}

static void add_child(ASTNode* parent, ASTNode* child) {
    if (!parent || !child) return;
    if (parent->child_count >= parent->child_cap) {
        parent->child_cap *= 2;
        parent->children = realloc(parent->children, parent->child_cap * sizeof(ASTNode*));
    }
    parent->children[parent->child_count++] = child;
}

// --- Parser Helper Functions ---
static Token* advance(Parser* p) {
    Token* previous = p->current;
    p->current = lexer_next(p->lexer);
    return previous;
}

static bool check(Parser* p, TokenType type) {
    return p->current->type == type;
}

static void parser_error(Parser* p, const char* message) {
    if (!p->had_error) {
        fprintf(stderr, "Parse Error on line %d near '%s': %s\n", p->current->line, p->current->text, message);
        p->had_error = true;
    }
}

static bool match_and_consume(Parser* p, TokenType type, const char* text) {
    if (p->current->type == type && (!text || strcmp(p->current->text, text) == 0)) {
        token_free(advance(p));
        return true;
    }
    return false;
}

static void expect(Parser* p, TokenType type, const char* text, const char* error_message) {
    if (p->current->type == type && (!text || strcmp(p->current->text, text) == 0)) {
        token_free(advance(p));
    } else {
        parser_error(p, error_message);
    }
}

// --- Full Expression Parsing Logic ---
static ASTNode* parse_primary(Parser* p) {
    // FIX: Allow initializer lists as a primary expression for struct assignments.
    if (check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, "{") == 0) {
        return parse_initializer_list(p);
    }
    if (check(p, TOKEN_NUMBER)) {
        Token* tok = advance(p); ASTNode* node = create_node(AST_NUMBER, tok->text); token_free(tok);
        return node;
    }
    if (check(p, TOKEN_STRING)) {
        Token* tok = advance(p); ASTNode* node = create_node(AST_STRING, tok->text); token_free(tok);
        return node;
    }
    if (check(p, TOKEN_CHARACTER)) {
        Token* tok = advance(p); ASTNode* node = create_node(AST_CHARACTER, tok->text); token_free(tok);
        return node;
    }
    if (match_and_consume(p, TOKEN_KEYWORD, "sizeof")) {
        ASTNode* node = create_node(AST_SIZEOF, "sizeof");
        expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' after 'sizeof'.");
        if (check(p, TOKEN_IDENTIFIER)) {
            Token* type_tok = advance(p);
            // FIX: If sizeof has a type suffix (e.g., let_i), store the type info directly.
            // Otherwise, store the identifier as a child (e.g., sizeof(Point)).
            if (type_tok->base_name) {
                node->suffix_info = type_tok->suffix_info;
            } else {
                add_child(node, create_node(AST_IDENTIFIER, type_tok->text));
            }
            token_free(type_tok);
        }
        expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after sizeof argument.");
        return node;
    }
    if (check(p, TOKEN_IDENTIFIER)) {
        Token* tok = advance(p);
        if (check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, "(") == 0) {
            advance(p);
            ASTNode* call = create_node(AST_CALL, tok->base_name ? tok->base_name : tok->text);
            if (tok->base_name) call->suffix_info = tok->suffix_info;
            if (! (check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, ")") == 0) ) {
                do { add_child(call, parse_expression(p)); } while (match_and_consume(p, TOKEN_PUNCTUATION, ","));
            }
            expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after function arguments.");
            token_free(tok);
            return call;
        }
        ASTNode* node = create_node(AST_IDENTIFIER, tok->base_name ? tok->base_name : tok->text);
        if (tok->base_name) node->suffix_info = tok->suffix_info;
        token_free(tok);
        return node;
    }
    if (match_and_consume(p, TOKEN_PUNCTUATION, "(")) {
        ASTNode* expr = parse_expression(p);
        expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after expression.");
        return expr;
    }
    parser_error(p, "Expected expression.");
    return NULL;
}

static ASTNode* parse_ternary(Parser* p) {
    ASTNode* condition = parse_logical_or(p); // Parse the condition part

    if (match_and_consume(p, TOKEN_OPERATOR, "?")) {
        ASTNode* ternary_node = create_node(AST_TERNARY_OP, "?");
        add_child(ternary_node, condition); // Child 0: condition

        // Child 1: The 'true' expression
        add_child(ternary_node, parse_expression(p));

        expect(p, TOKEN_PUNCTUATION, ":", "Expected ':' for ternary operator.");

        // Child 2: The 'false' expression
        add_child(ternary_node, parse_ternary(p)); // Allows for chained ternaries

        return ternary_node;
    }

    return condition; // Not a ternary, just return the expression
}

static ASTNode* parse_switch_statement(Parser* p) {
    ASTNode* node = create_node(AST_SWITCH, "switch");
    expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' after 'switch'.");
    add_child(node, parse_expression(p)); // Child 0: the value to switch on
    expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after switch expression.");
    expect(p, TOKEN_PUNCTUATION, "{", "Expected '{' to begin switch body.");

    // Parse the case and default blocks
    while (!match_and_consume(p, TOKEN_PUNCTUATION, "}")) {
        if (check(p, TOKEN_EOF)) {
            parser_error(p, "Unterminated switch statement.");
            break;
        }

        if (match_and_consume(p, TOKEN_KEYWORD, "case")) {
            ASTNode* case_node = create_node(AST_CASE, "case");
            add_child(case_node, parse_expression(p));
            expect(p, TOKEN_PUNCTUATION, ":", "Expected ':' after case value.");
            add_child(node, case_node); // Add the case node to the switch

            // SIMPLIFIED LOOP: Keep parsing statements until we hit the next label or the end
            while (true) {
                if (check(p, TOKEN_EOF) ||
                    (check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, "}") == 0) ||
                    (check(p, TOKEN_KEYWORD) && strcmp(p->current->text, "case") == 0) ||
                    (check(p, TOKEN_KEYWORD) && strcmp(p->current->text, "default") == 0)) {
                    break; 
                }
                add_child(case_node, parse_statement(p));
            }
        } else if (match_and_consume(p, TOKEN_KEYWORD, "default")) {
            ASTNode* default_node = create_node(AST_DEFAULT, "default");
            expect(p, TOKEN_PUNCTUATION, ":", "Expected ':' after 'default'.");
            add_child(node, default_node); // Add the default node to the switch

            // SIMPLIFIED LOOP for default
            while (true) {
                if (check(p, TOKEN_EOF) ||
                    (check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, "}") == 0) ||
                    (check(p, TOKEN_KEYWORD) && strcmp(p->current->text, "case") == 0)) { // Stop at next case or end
                    break;
                }
                add_child(default_node, parse_statement(p));
            }
        } else {
            parser_error(p, "Expected 'case' or 'default' inside switch body.");
            token_free(advance(p));
        }
    }

    return node;
}

static ASTNode* parse_subscript(Parser* p) {
    ASTNode* expr = parse_primary(p);
    while (match_and_consume(p, TOKEN_PUNCTUATION, "[")) {
        ASTNode* node = create_node(AST_SUBSCRIPT, NULL);
        add_child(node, expr);
        add_child(node, parse_expression(p));
        expect(p, TOKEN_PUNCTUATION, "]", "Expected ']' after subscript index.");
        expr = node;
    }
    return expr;
}

// ===== In parser.c, replace the existing function with this one =====

static ASTNode* parse_member_access(Parser* p) {
    ASTNode* left = parse_subscript(p); // Start with the highest precedence part

    while (true) {
        if (match_and_consume(p, TOKEN_PUNCTUATION, ".")) {
            ASTNode* node = create_node(AST_MEMBER_ACCESS, ".");
            add_child(node, left);

            Token* member = advance(p);
            if (member->type != TOKEN_IDENTIFIER) {
                parser_error(p, "Expected member name after '.'.");
            }
            add_child(node, create_node(AST_IDENTIFIER, member->base_name ? member->base_name : member->text));
            token_free(member);
            left = node;
        } else if (match_and_consume(p, TOKEN_ARROW, "->")) {
            ASTNode* node = create_node(AST_MEMBER_ACCESS, "->");
            add_child(node, left);

            Token* member = advance(p);
            if (member->type != TOKEN_IDENTIFIER) {
                parser_error(p, "Expected member name after '->'.");
            }
            // The member token has the suffix info (e.g., secret_number_i)
            ASTNode* member_node = create_node(AST_IDENTIFIER, member->base_name ? member->base_name : member->text);
            if (member->base_name) {
                // CRITICAL FIX: The SuffixInfo belongs to the final expression.
                node->suffix_info = member->suffix_info;
            }
            add_child(node, member_node);
            token_free(member);
            left = node;
        } else {
            break;
        }
    }
    return left;
}

static ASTNode* parse_unary(Parser* p) {
    if (check(p, TOKEN_OPERATOR) && (strcmp(p->current->text, "-") == 0 || strcmp(p->current->text, "!") == 0 ||
        strcmp(p->current->text, "&") == 0 || strcmp(p->current->text, "*") == 0)) {
        Token* op_tok = advance(p); ASTNode* node = create_node(AST_UNARY_OP, op_tok->text);
        add_child(node, parse_unary(p)); token_free(op_tok); 
        return node;
    }
    return parse_member_access(p);
}

static ASTNode* parse_multiplicative(Parser* p) {
    ASTNode* left = parse_unary(p);
    // ADDED '%' to this check
    while (check(p, TOKEN_OPERATOR) && (strcmp(p->current->text, "*") == 0 || strcmp(p->current->text, "/") == 0 || strcmp(p->current->text, "%") == 0)) {
        Token* op_tok = advance(p); ASTNode* op_node = create_node(AST_BINARY_OP, op_tok->text);
        add_child(op_node, left); add_child(op_node, parse_unary(p));
        left = op_node; token_free(op_tok);
    }
    return left;
}

static ASTNode* parse_additive(Parser* p) {
    ASTNode* left = parse_multiplicative(p);
    while (check(p, TOKEN_OPERATOR) && (strcmp(p->current->text, "+") == 0 || strcmp(p->current->text, "-") == 0)) {
        Token* op_tok = advance(p); ASTNode* op_node = create_node(AST_BINARY_OP, op_tok->text);
        add_child(op_node, left); add_child(op_node, parse_multiplicative(p));
        left = op_node; token_free(op_tok);
    }
    return left;
}

static ASTNode* parse_relational(Parser* p) {
    ASTNode* left = parse_additive(p);
    while (check(p, TOKEN_OPERATOR) && 
           (strcmp(p->current->text, "<") == 0 || strcmp(p->current->text, ">") == 0 ||
            strcmp(p->current->text, "<=") == 0 || strcmp(p->current->text, ">=") == 0)) {
        Token* op_tok = advance(p); 
        ASTNode* op_node = create_node(AST_BINARY_OP, op_tok->text);
        add_child(op_node, left); 
        add_child(op_node, parse_additive(p));
        left = op_node; 
        token_free(op_tok);
    }
    return left;
}

static ASTNode* parse_equality(Parser* p) {
    ASTNode* left = parse_relational(p);
    while (check(p, TOKEN_OPERATOR) && (strcmp(p->current->text, "==") == 0 || strcmp(p->current->text, "!=") == 0)) {
        Token* op_tok = advance(p); ASTNode* op_node = create_node(AST_BINARY_OP, op_tok->text);
        add_child(op_node, left); add_child(op_node, parse_relational(p));
        left = op_node; token_free(op_tok);
    }
    return left;
}

static ASTNode* parse_logical_and(Parser* p) {
    ASTNode* left = parse_equality(p);
    while (check(p, TOKEN_OPERATOR) && strcmp(p->current->text, "&&") == 0) {
        Token* op_tok = advance(p);
        ASTNode* op_node = create_node(AST_BINARY_OP, op_tok->text);
        add_child(op_node, left);
        add_child(op_node, parse_equality(p));
        left = op_node;
        token_free(op_tok);
    }
    return left;
}

static ASTNode* parse_logical_or(Parser* p) {
    ASTNode* left = parse_logical_and(p);
    while (check(p, TOKEN_OPERATOR) && strcmp(p->current->text, "||") == 0) {
        Token* op_tok = advance(p);
        ASTNode* op_node = create_node(AST_BINARY_OP, op_tok->text);
        add_child(op_node, left);
        add_child(op_node, parse_logical_and(p));
        left = op_node;
        token_free(op_tok);
    }
    return left;
}

static ASTNode* parse_assignment(Parser* p) {
    ASTNode* left = parse_ternary(p);
    if(match_and_consume(p, TOKEN_OPERATOR, "=")) {
        ASTNode* op_node = create_node(AST_BINARY_OP, "=");
        add_child(op_node, left); add_child(op_node, parse_assignment(p)); 
        return op_node;
    }
    return left;
}

static ASTNode* parse_expression(Parser* p) {
    return parse_assignment(p);
}

// --- Statement & Declaration Parsing ---
static ASTNode* parse_var_decl(Parser* p) {
    Token* name = advance(p);
    if (name->type != TOKEN_IDENTIFIER) { parser_error(p, "Expected variable name."); token_free(name); return NULL; }
    ASTNode* node = create_node(AST_VAR_DECL, name->base_name ? name->base_name : name->text);
    if (name->base_name) { node->suffix_info = name->suffix_info; }
    
    // Check for array declaration [size]
    if (match_and_consume(p, TOKEN_PUNCTUATION, "[")) {
        add_child(node, parse_expression(p)); // Child 0: size
        expect(p, TOKEN_PUNCTUATION, "]", "Expected ']' after array size.");
        
        // Check for an optional initializer list
        if (match_and_consume(p, TOKEN_OPERATOR, "=")) {
            add_child(node, parse_initializer_list(p)); // Child 1: initializer
        }
    } else if (match_and_consume(p, TOKEN_OPERATOR, "=")) {
        // Regular variable initialization
        add_child(node, parse_expression(p)); // Child 0: initializer
    }
    
    token_free(name);
    return node;
}

static ASTNode* parse_if_statement(Parser* p) {
    ASTNode* node = create_node(AST_IF, "if");
    expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' after 'if'.");
    add_child(node, parse_expression(p));
    expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after if condition.");
    add_child(node, parse_block(p));

    if (match_and_consume(p, TOKEN_KEYWORD, "else")) {
        // Check for an "else if"
        if (check(p, TOKEN_KEYWORD) && strcmp(p->current->text, "if") == 0) {
            advance(p); // Consume the 'if'
            add_child(node, parse_if_statement(p)); // Recursively call to parse the rest of the chain
        } else {
            // It's a simple "else"
            add_child(node, parse_block(p));
        }
    }
    return node;
}

static ASTNode* parse_initializer_list(Parser *p) {
    ASTNode* list = create_node(AST_INITIALIZER_LIST, NULL);
    expect(p, TOKEN_PUNCTUATION, "{", "Expected '{' to begin initializer list.");

    if (!check(p, TOKEN_PUNCTUATION) || strcmp(p->current->text, "}") != 0) {
        do {
            add_child(list, parse_expression(p));
        } while (match_and_consume(p, TOKEN_PUNCTUATION, ","));
    }

    expect(p, TOKEN_PUNCTUATION, "}", "Expected '}' to end initializer list.");
    return list;
}

static ASTNode* parse_for_statement(Parser* p) {
    ASTNode* node = create_node(AST_FOR, "for");
    
    expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' after 'for'.");
    
    // 1. Parse the initializer
    if (match_and_consume(p, TOKEN_PUNCTUATION, ";")) {
        // No initializer
        add_child(node, NULL); 
    } else {
        // Allow either 'let' or a simple expression
        if (check(p, TOKEN_KEYWORD) && strcmp(p->current->text, "let") == 0) {
             token_free(advance(p));
             add_child(node, parse_var_decl(p));
        } else {
             add_child(node, parse_expression(p));
        }
        expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after for loop initializer.");
    }
    
    // 2. Parse the condition
    if (match_and_consume(p, TOKEN_PUNCTUATION, ";")) {
        // No condition (infinite loop)
        add_child(node, NULL);
    } else {
        add_child(node, parse_expression(p));
        expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after for loop condition.");
    }
    
    // 3. Parse the increment
    if (check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, ")") == 0) {
        // No increment
        add_child(node, NULL);
    } else {
        add_child(node, parse_expression(p));
    }
    expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after for loop clauses.");

    // 4. Parse the body
    add_child(node, parse_block(p));
    
    return node;
}

static ASTNode* parse_while_statement(Parser* p) {
    ASTNode* node = create_node(AST_WHILE, "while");
    expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' after 'while'.");
    add_child(node, parse_expression(p));
    expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after while condition.");
    add_child(node, parse_block(p));
    return node;
}

static ASTNode* parse_do_statement(Parser* p) {
    ASTNode* node = create_node(AST_DO, "do");
    add_child(node, parse_block(p));
    expect(p, TOKEN_KEYWORD, "while", "Expected 'while' after do-block.");
    expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' after 'while'.");
    add_child(node, parse_expression(p));
    expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after do-while condition.");
    expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after do-while statement.");
    return node;
}

static ASTNode* parse_statement(Parser* p) {
    if (check(p, TOKEN_KEYWORD)) {
        if (strcmp(p->current->text, "let") == 0) {
            advance(p); ASTNode* decl = parse_var_decl(p);
            expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after variable declaration.");
            return decl;
        }
        if (strcmp(p->current->text, "if") == 0) {
            advance(p); 
            return parse_if_statement(p);
        }
        if (strcmp(p->current->text, "while") == 0) { 
            advance(p); 
            return parse_while_statement(p); 
        }
        if (strcmp(p->current->text, "do") == 0) { 
            advance(p);
            return parse_do_statement(p); 
        }
        if (strcmp(p->current->text, "for") == 0) {
            advance(p); 
            return parse_for_statement(p); 
        }
        // Add logic for break
        if (strcmp(p->current->text, "break") == 0) {
            advance(p);
            expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after 'break'.");
            return create_node(AST_BREAK, "break");
        }
        if (strcmp(p->current->text, "switch") == 0) {
            advance(p);
            return parse_switch_statement(p);
        }
        // Add logic for continue
        if (strcmp(p->current->text, "continue") == 0) {
            advance(p);
            expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after 'continue'.");
            return create_node(AST_CONTINUE, "continue");
        }
        if (strcmp(p->current->text, "return") == 0) {
            advance(p); ASTNode* node = create_node(AST_RETURN, "return");
            if (! (check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, ";") == 0)) { 
                add_child(node, parse_expression(p)); 
            }
            expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after return value."); 
            return node;
        }
    }
    ASTNode* expr = parse_expression(p);
    expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after expression.");
    return expr;
}

static ASTNode* parse_block(Parser* p) {
    expect(p, TOKEN_PUNCTUATION, "{", "Expected '{' to begin a block.");
    ASTNode* block = create_node(AST_BLOCK, NULL);
    while (! (check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, "}") == 0) && !check(p, TOKEN_EOF)) {
        add_child(block, parse_statement(p));
    }
    expect(p, TOKEN_PUNCTUATION, "}", "Expected '}' to end a block.");
    return block;
}

static ASTNode* parse_struct_definition(Parser* p) {
    Token* name_tok = advance(p);
    if (name_tok->type != TOKEN_IDENTIFIER) {
        parser_error(p, "Expected struct name.");
        token_free(name_tok);
        return NULL;
    }

    // CRITICAL FIX: Add the struct name to the type table!
    type_table_add((TypeTable*)p->type_table, name_tok->text);

    ASTNode* struct_node = create_node(AST_STRUCT_DEF, name_tok->text);
    token_free(name_tok);

    expect(p, TOKEN_PUNCTUATION, "{", "Expected '{' after struct name.");

    while (!check(p, TOKEN_PUNCTUATION) || strcmp(p->current->text, "}") != 0) {
        if (check(p, TOKEN_EOF)) {
            parser_error(p, "Unterminated struct definition.");
            ast_destroy(struct_node);
            return NULL;
        }

        if (check(p, TOKEN_IDENTIFIER)) {
            Token* member_tok = advance(p);
            ASTNode* member_node = create_node(AST_VAR_DECL, member_tok->base_name ? member_tok->base_name : member_tok->text);
            if (member_tok->base_name) {
                member_node->suffix_info = member_tok->suffix_info;
            }
            add_child(struct_node, member_node);
            token_free(member_tok);
            expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after struct member.");
        } else {
            parser_error(p, "Expected member declaration inside struct.");
            token_free(advance(p));
        }
    }

    expect(p, TOKEN_PUNCTUATION, "}", "Expected '}' to close struct definition.");
    expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after struct definition.");

    return struct_node;
}
static ASTNode* parse_function(Parser* p) {
    Token* name = advance(p);
    if (name->type != TOKEN_IDENTIFIER) {
        parser_error(p, "Expected function name."); token_free(name); return NULL;
    }
    ASTNode* func_node = create_node(AST_FUNCTION, name->base_name ? name->base_name : name->text);
    if (name->base_name) { func_node->suffix_info = name->suffix_info; }
    expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' after function name.");
    ASTNode* params_node = create_node(AST_VAR_DECL, "params");
    add_child(func_node, params_node);
    if (! (check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, ")") == 0)) {
        do {
            Token* param_tok = advance(p);
            if (param_tok->type != TOKEN_IDENTIFIER) {
                parser_error(p, "Expected parameter name."); token_free(param_tok); break;
            }
            ASTNode* param_node = create_node(AST_VAR_DECL, param_tok->base_name ? param_tok->base_name : param_tok->text);
            if (param_tok->base_name) { param_node->suffix_info = param_tok->suffix_info; }
            add_child(params_node, param_node); token_free(param_tok);
        } while (match_and_consume(p, TOKEN_PUNCTUATION, ","));
    }
    expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after parameters.");
    add_child(func_node, parse_block(p));
    token_free(name);
    return func_node;
}

// --- Public API ---
Parser* parser_create(const char* source, const TypeTable* type_table) {
    Parser* p = calloc(1, sizeof(Parser));
    p->type_table = (TypeTable*)type_table; 
    p->lexer = lexer_create(source, p->type_table);
    p->current = lexer_next(p->lexer);
    return p;
}

ASTNode* parser_parse(Parser* p) {
    ASTNode* program = create_node(AST_PROGRAM, NULL);
    
    // First, collect all preprocessor directives
    while (check(p, TOKEN_DIRECTIVE)) {
        Token* dir_tok = advance(p);
        add_child(program, create_node(AST_DIRECTIVE, dir_tok->text));
        token_free(dir_tok);
    }
    
    // Then parse the rest of the program
    while (!check(p, TOKEN_EOF)) {
        if (check(p, TOKEN_DIRECTIVE)) {
            // Handle any directives that appear later (though this is unusual in C)
            Token* dir_tok = advance(p);
            add_child(program, create_node(AST_DIRECTIVE, dir_tok->text));
            token_free(dir_tok);
        } else if (check(p, TOKEN_KEYWORD)) {
            if (strcmp(p->current->text, "func") == 0) {
                advance(p); 
                add_child(program, parse_function(p));
            } else if (strcmp(p->current->text, "struct") == 0) {
                advance(p);
                add_child(program, parse_struct_definition(p));
            } else {
                parser_error(p, "Only function, struct, or preprocessor directives are allowed at the top level."); 
                token_free(advance(p));
            }
        } else {
            parser_error(p, "Unexpected token at top level."); 
            token_free(advance(p));
        }
    }
    return program;
}

void parser_destroy(Parser* p) {
    if (p) { token_free(p->current); lexer_destroy(p->lexer); free(p); }
}

void ast_destroy(ASTNode* node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; i++) { ast_destroy(node->children[i]); }
    free(node->value);
    free(node->children);
    free(node);
}


