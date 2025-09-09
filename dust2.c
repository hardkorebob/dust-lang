#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ====================
// ARENA ALLOCATOR
// ====================
typedef struct Arena {
    char *data;
    size_t size;
    size_t used;
} Arena;

static Arena g_arena = {0};

/* Initialize arena with given size in bytes */
void arena_init(size_t size) {
    g_arena.data = malloc(size);
    if (!g_arena.data) {
        fprintf(stderr, "Failed to allocate arena\n");
        exit(1);
    }
    g_arena.size = size;
    g_arena.used = 0;
}

/* Allocate memory from arena (8-byte aligned) */
void *arena_alloc(size_t size) {
    size = (size + 7) & ~7;  // Align to 8 bytes
    
    if (g_arena.used + size > g_arena.size) {
        fprintf(stderr, "Arena out of memory (used: %zu, requested: %zu, total: %zu)\n", 
                g_arena.used, size, g_arena.size);
        exit(1);
    }
    
    void *ptr = g_arena.data + g_arena.used;
    g_arena.used += size;
    memset(ptr, 0, size);  // Zero-initialize
    return ptr;
}

/* Duplicate string in arena */
char *arena_strdup(const char *str) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char *new_str = arena_alloc(len);
    memcpy(new_str, str, len);
    return new_str;
}

/* Free entire arena */
void arena_free_all(void) {
    free(g_arena.data);
    g_arena.data = NULL;
    g_arena.size = 0;
    g_arena.used = 0;
}

// Replace clone_string with arena version
#define clone_string arena_strdup

// [Rest of type definitions remain the same through line 267]

/* Create new type table */
TypeTable *type_table_create(void) {
  TypeTable *table = arena_alloc(sizeof(TypeTable));
  table->struct_capacity = 8;
  table->struct_count = 0;
  table->struct_names = arena_alloc(sizeof(char *) * table->struct_capacity);

  table->typedef_capacity = 8;
  table->typedef_count = 0;
  table->typedefs = arena_alloc(sizeof(TypedefInfo) * table->typedef_capacity);
  return table;
}

/* Type table destructor - now empty since arena handles cleanup */
void type_table_destroy(TypeTable *table) {
  // No-op: arena handles all cleanup
}

bool type_table_add(TypeTable *table, const char *type_name) {
  for (size_t i = 0; i < table->struct_count; i++) {
    if (strcmp(table->struct_names[i], type_name) == 0) {
      return true;
    }
  }

  if (table->struct_count >= table->struct_capacity) {
    table->struct_capacity *= 2;
    // Allocate new array and copy old data
    char **new_names = arena_alloc(sizeof(char *) * table->struct_capacity);
    memcpy(new_names, table->struct_names, sizeof(char *) * table->struct_count);
    table->struct_names = new_names;
  }
  table->struct_names[table->struct_count++] = clone_string(type_name);
  return true;
}

// [Continue with similar pattern for other type_table functions...]

/* Create new lexer */
Lexer *lexer_create(const char *source, const TypeTable *type_table) {
  Lexer *lex = arena_alloc(sizeof(Lexer));
  lex->source = source;
  lex->len = strlen(source);
  lex->pos = 0;
  lex->line = 1;
  lex->type_table = type_table;
  return lex;
}

/* Lexer destructor - now empty */
void lexer_destroy(Lexer *lex) { 
    // No-op: arena handles cleanup
}

/* Token destructor - now empty */
void token_free(Token *tok) {
  // No-op: arena handles cleanup
}

/* Create token */
static Token *make_token(TokenType type, const char *text, int line) {
  Token *tok = arena_alloc(sizeof(Token));
  tok->type = type;
  tok->line = line;
  tok->text = clone_string(text);
  return tok;
}

// [In lexer_next, replace malloc calls with arena_alloc, e.g. line 647:]
char *directive_line = arena_alloc(len + 2);
// [And line 674:]
char *passthrough_code = arena_alloc(len + 1);
// [And line 705:]
char *word = arena_alloc(len + 1);
// [Continue pattern for all malloc calls...]

/* Create AST node */
static ASTNode *create_node(ASTType type, const char *value) {
  ASTNode *node = arena_alloc(sizeof(ASTNode));
  node->type = type;
  node->value = value ? clone_string(value) : NULL;
  node->child_cap = 2;
  node->children = arena_alloc(node->child_cap * sizeof(ASTNode *));
  return node;
}

/* Add child to AST node */
static void add_child(ASTNode *parent, ASTNode *child) {
  if (!parent || !child)
    return;
  if (parent->child_count >= parent->child_cap) {
    parent->child_cap *= 2;
    // Allocate new array and copy
    ASTNode **new_children = arena_alloc(parent->child_cap * sizeof(ASTNode *));
    memcpy(new_children, parent->children, parent->child_count * sizeof(ASTNode *));
    parent->children = new_children;
  }
  parent->children[parent->child_count++] = child;
}

/* AST destructor - now empty */
void ast_destroy(ASTNode *node) {
  // No-op: arena handles cleanup
}

// [Parser creation remains similar:]
Parser *parser_create(const char *source, const TypeTable *type_table) {
  Parser *p = arena_alloc(sizeof(Parser));
  p->type_table = (TypeTable *)type_table;
  p->lexer = lexer_create(source, p->type_table);
  p->current = lexer_next(p->lexer);
  return p;
}

/* Parser destructor - now empty */
void parser_destroy(Parser *p) {
  // No-op: arena handles cleanup
}

// [In collect_functions around line 2980:]
FuncDecl *collect_functions(ASTNode *node, FuncDecl *list) {
  if (!node) return list;
  
  if (node->type == AST_FUNCTION) {
    FuncDecl *decl = arena_alloc(sizeof(FuncDecl));
    decl->name = clone_string(node->value);
    decl->return_type = node->suffix_info;
    decl->params = node->child_count > 0 ? node->children[0] : NULL;
    decl->next = list;
    return decl;
  }
  
  for (int i = 0; i < node->child_count; i++) {
    list = collect_functions(node->children[i], list);
  }
  return list;
}

/* Free function declarations - now empty */
void free_func_decls(FuncDecl *decls) {
  // No-op: arena handles cleanup
}

/* Read file into memory */
static char *read_file(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f)
    return NULL;

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *buffer = arena_alloc(size + 1);
  fread(buffer, 1, size, f);
  buffer[size] = '\0';

  fclose(f);
  return buffer;
}

int main(int argc, char **argv) {
    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || 
                      strcmp(argv[1], "-h") == 0 ||
                      strcmp(argv[1], "--suffixes") == 0)) {
        print_suffix_help();
        return 0;
    }
    
    if (argc != 2) {
        fprintf(stderr, "Usage: dustc <file.dust>\n");
        fprintf(stderr, "       dustc --help     (show suffix reference)\n");
        return 1;
    }

    // Initialize arena with 10MB (adjust if needed for larger files)
    arena_init(10 * 1024 * 1024);

    char *source = read_file(argv[1]);
    if (!source) {
        fprintf(stderr, "Error: Cannot read file '%s'\n", argv[1]);
        arena_free_all();
        return 1;
    }

    TypeTable *type_table = type_table_create();
    pre_scan_for_types(source, type_table);

    Parser *parser = parser_create(source, type_table);
    ASTNode *ast = parser_parse(parser);

    if (parser->had_error) {
        fprintf(stderr, "Compilation failed.\n");
        arena_free_all();
        return 1;
    }

    char outname[256];
    strncpy(outname, argv[1], sizeof(outname) - 3);
    outname[sizeof(outname) - 3] = '\0';
    char *dot = strrchr(outname, '.');
    if (dot) {
        strcpy(dot, ".c");
    } else {
        strcat(outname, ".c");
    }

    FILE *out = fopen(outname, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file '%s'\n", outname);
        arena_free_all();
        return 1;
    }

    codegen(ast, type_table, out);
    fclose(out);

    printf("Successfully compiled '%s' to '%s'\n", argv[1], outname);

    // Single cleanup point - frees everything
    arena_free_all();
    return 0;
}
