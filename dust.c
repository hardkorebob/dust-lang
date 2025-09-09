#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ====================
// UTILITY FUNCTIONS
// ====================

char *clone_string(const char *str) {
  if (!str)
    return NULL;
  size_t len = strlen(str);
  char *new_str = malloc(len + 1);
  if (!new_str)
    return NULL;
  memcpy(new_str, str, len);
  new_str[len] = '\0';
  return new_str;
}

// =============
// TYPE TABLE - 
// =============
typedef enum {
    TYPE_VOID,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_POINTER,
    TYPE_ARRAY,
    TYPE_USER,
    TYPE_FUNC_POINTER,
    TYPE_SIZE_T,
    
    // Fixed-width integers
    TYPE_UINT8,
    TYPE_UINT16,
    TYPE_UINT32,
    TYPE_UINT64,
    TYPE_INT8,
    TYPE_INT16,
    TYPE_INT32,
    TYPE_INT64,
    
    // Architecture types
    TYPE_UINTPTR,
    TYPE_INTPTR,
    TYPE_SIZE,
    TYPE_SSIZE,
    TYPE_OFF,
    
    // OS development
    TYPE_PHYS_ADDR,
    TYPE_VIRT_ADDR,
    TYPE_PTE,
    TYPE_PDE,
    TYPE_PFN,
    TYPE_PORT,
    TYPE_MMIO,
    TYPE_VOLATILE,
    TYPE_IRQ,
    TYPE_VECTOR,
    TYPE_ISR_PTR,
    
    // Atomic types
    TYPE_ATOMIC_U32,
    TYPE_ATOMIC_U64,
    TYPE_ATOMIC_PTR,
} DataType;

typedef enum {
  ROLE_OWNED,
  ROLE_BORROWED,
  ROLE_REFERENCE,
  ROLE_RESTRICT,
  ROLE_NONE
} SemanticRole;

typedef struct {
  DataType type;
  SemanticRole role;
  bool is_pointer;
  bool is_const;
  const char *user_type_name;
  bool is_pointer_to_pointer;
  DataType array_base_type;
  const char *array_user_type_name;
} SuffixInfo;

typedef struct {
  char *name;
  SuffixInfo type_info;
} TypedefInfo;

typedef struct {
  char **struct_names;
  size_t struct_count;
  size_t struct_capacity;
  TypedefInfo *typedefs;
  size_t typedef_count;
  size_t typedef_capacity;
} TypeTable;

typedef struct {
    const char *suffix;
    DataType type;
    SemanticRole role;
    bool is_pointer;
    bool is_const;
} SuffixMapping;

typedef struct {
    DataType type;
    const char *c_type;
} TypeMapping;


static const TypeMapping type_map[] = {
    // Basic types
    {TYPE_VOID,       "void"},
    {TYPE_INT,        "int"},
    {TYPE_FLOAT,      "float"},
    {TYPE_CHAR,       "char"},
    {TYPE_STRING,     "char*"},
    {TYPE_SIZE_T,     "size_t"},
    
    // Fixed-width integers
    {TYPE_UINT8,      "uint8_t"},
    {TYPE_UINT16,     "uint16_t"},
    {TYPE_UINT32,     "uint32_t"},
    {TYPE_UINT64,     "uint64_t"},
    {TYPE_INT8,       "int8_t"},
    {TYPE_INT16,      "int16_t"},
    {TYPE_INT32,      "int32_t"},
    {TYPE_INT64,      "int64_t"},
    
    // Architecture types
    {TYPE_UINTPTR,    "uintptr_t"},
    {TYPE_INTPTR,     "intptr_t"},
    {TYPE_SIZE,       "size_t"},
    {TYPE_SSIZE,      "ssize_t"},
    {TYPE_OFF,        "off_t"},
    
    // OS development types
    {TYPE_PHYS_ADDR,  "phys_addr_t"},
    {TYPE_VIRT_ADDR,  "virt_addr_t"},
    {TYPE_PTE,        "pte_t"},
    {TYPE_PDE,        "pde_t"},
    {TYPE_PFN,        "pfn_t"},
    {TYPE_PORT,       "port_t"},
    {TYPE_MMIO,       "void*"},        // MMIO is typically void*
    {TYPE_VOLATILE,   "volatile void*"},
    {TYPE_IRQ,        "irq_t"},
    {TYPE_VECTOR,     "vector_t"},
    {TYPE_ISR_PTR,    "isr_t"},
    
    // Atomic types
    {TYPE_ATOMIC_U32, "_Atomic uint32_t"},
    {TYPE_ATOMIC_U64, "_Atomic uint64_t"},
    {TYPE_ATOMIC_PTR, "_Atomic void*"},
    
    // Sentinel
    {TYPE_VOID,       NULL}
};

static const SuffixMapping suffix_table[] = {
    // Primitive types
    {"i",   TYPE_INT,    ROLE_NONE,   false, false},
    {"bl",  TYPE_INT,    ROLE_NONE,   false, false}, // bool as int
    {"st",  TYPE_SIZE_T, ROLE_NONE,   false, false},
    {"f",   TYPE_FLOAT,  ROLE_NONE,   false, false},
    {"c",   TYPE_CHAR,   ROLE_NONE,   false, false},
    {"s",   TYPE_STRING, ROLE_NONE,   true,  false},
    {"v",   TYPE_VOID,   ROLE_NONE,   false, false},
    
    // Primitive pointers with ownership
    {"ip",  TYPE_INT,    ROLE_OWNED,     true,  false},
    {"ib",  TYPE_INT,    ROLE_BORROWED,  true,  true},
    {"ir",  TYPE_INT,    ROLE_REFERENCE, true,  true},
    {"cp",  TYPE_CHAR,   ROLE_OWNED,     true,  false},
    {"cb",  TYPE_CHAR,   ROLE_BORROWED,  true,  true},
    {"cr",  TYPE_CHAR,   ROLE_REFERENCE, true,  true},
    {"fp",  TYPE_FUNC_POINTER, ROLE_OWNED, true, false},
    
    // Generic borrowed pointer
    {"b",   TYPE_POINTER, ROLE_BORROWED, true,  true},
    // Fixed-width integer types (essential for OS dev)
    {"u8",   TYPE_UINT8,   ROLE_NONE,   false, false},  // uint8_t
    {"u16",  TYPE_UINT16,  ROLE_NONE,   false, false},  // uint16_t  
    {"u32",  TYPE_UINT32,  ROLE_NONE,   false, false},  // uint32_t
    {"u64",  TYPE_UINT64,  ROLE_NONE,   false, false},  // uint64_t
    {"i8",   TYPE_INT8,    ROLE_NONE,   false, false},  // int8_t
    {"i16",  TYPE_INT16,   ROLE_NONE,   false, false},  // int16_t
    {"i32",  TYPE_INT32,   ROLE_NONE,   false, false},  // int32_t
    {"i64",  TYPE_INT64,   ROLE_NONE,   false, false},  // int64_t

    // Architecture-specific types
    {"ux",   TYPE_UINTPTR, ROLE_NONE,   false, false},  // uintptr_t (native word)
    {"ix",   TYPE_INTPTR,  ROLE_NONE,   false, false},  // intptr_t  
    {"sz",   TYPE_SIZE,    ROLE_NONE,   false, false},  // size_t
    {"ssz",  TYPE_SSIZE,   ROLE_NONE,   false, false},  // ssize_t
    {"off",  TYPE_OFF,     ROLE_NONE,   false, false},  // off_t

    // Special pointer qualifiers (combine with base types)
    {"vp",   TYPE_VOID,    ROLE_OWNED,  true,  false},  // void pointer
    {"cvp",  TYPE_VOID,    ROLE_BORROWED, true, true},  // const void pointer
    {"rp",   TYPE_VOID,    ROLE_RESTRICT, true, false}, // restrict pointer    
    {NULL, TYPE_VOID, ROLE_NONE, false, false} // Sentinel
};

// Add after the suffix_table definition, around line 140
static void print_suffix_help(void) {
    printf("Dust Language Suffix Reference\n");
    printf("==============================\n\n");
    
    printf("PRIMITIVE TYPES:\n");
    printf("  i     - int\n");
    printf("  bl    - bool (as int)\n");
    printf("  f     - float\n");
    printf("  c     - char\n");
    printf("  s     - string (char*)\n");
    printf("  v     - void\n");
    printf("  st    - size_t\n");
    
    printf("\nFIXED-WIDTH INTEGERS:\n");
    printf("  u8    - uint8_t\n");
    printf("  u16   - uint16_t\n");
    printf("  u32   - uint32_t\n");
    printf("  u64   - uint64_t\n");
    printf("  i8    - int8_t\n");
    printf("  i16   - int16_t\n");
    printf("  i32   - int32_t\n");
    printf("  i64   - int64_t\n");
    
    printf("\nARCHITECTURE TYPES:\n");
    printf("  ux    - uintptr_t (native word)\n");
    printf("  ix    - intptr_t\n");
    printf("  sz    - size_t\n");
    printf("  ssz   - ssize_t\n");
    printf("  off   - off_t\n");
    
    printf("\nPOINTER SUFFIXES:\n");
    printf("  _p    - owned pointer (suffix: ip, cp, etc.)\n");
    printf("  _b    - borrowed pointer (const)\n");
    printf("  _r    - reference pointer (const)\n");
    printf("  vp    - void pointer\n");
    printf("  fp    - function pointer\n");
    
    printf("\nARRAY SUFFIX:\n");
    printf("  _a    - array (suffix: ia, ca, u8a, etc.)\n");
    printf("         Example: buffer_u8a for uint8_t array\n");
    
    printf("\nUSER-DEFINED TYPES:\n");
    printf("  After 'struct Foo', you can use:\n");
    printf("    _Foo  - Foo instance\n");
    printf("    _Foop - Foo* (owned pointer)\n");
    printf("    _Foob - const Foo* (borrowed)\n");
    printf("    _Foor - const Foo* (reference)\n");
    printf("    _Fooa - Foo array\n");
    
    printf("\nEXAMPLES:\n");
    printf("  let count_i = 42;           // int\n");
    printf("  let name_s = \"Dust\";        // string\n");
    printf("  let buffer_u8a[256];        // uint8_t array\n");
    printf("  let player_Playerp;         // Player* (owned)\n");
    printf("  let callback_fp;            // function pointer\n");
    printf("  let gdt_base_pa = 0x1000;   // physical address\n");
    
    printf("\nSPECIAL KEYWORDS:\n");
    printf("  func name_<suffix>()  - function with return type\n");
    printf("  let name_<suffix>     - variable declaration\n");
    printf("  cast_<suffix>(expr)   - type cast\n");
    printf("  null                  - NULL constant\n");
    printf("  @c(...)              - inline C code escape hatch\n");
}

TypeTable *type_table_create(void);
void type_table_destroy(TypeTable *table);
bool type_table_add(TypeTable *table, const char *type_name);
const char *type_table_lookup(const TypeTable *table, const char *type_name);
bool type_table_add_typedef(TypeTable *table, const char *name, const SuffixInfo *type_info);
const TypedefInfo *type_table_lookup_typedef(const TypeTable *table, const char *name);
bool type_table_add_enum(TypeTable *table, const char *enum_name);


TypeTable *type_table_create(void) {
  TypeTable *table = malloc(sizeof(TypeTable));
  table->struct_capacity = 8;
  table->struct_count = 0;
  table->struct_names = malloc(sizeof(char *) * table->struct_capacity);

  table->typedef_capacity = 8;
  table->typedef_count = 0;
  table->typedefs = malloc(sizeof(TypedefInfo) * table->typedef_capacity);
  return table;
}

void type_table_destroy(TypeTable *table) {
  for (size_t i = 0; i < table->struct_count; i++) {
    free(table->struct_names[i]);
  }
  free(table->struct_names);

  for (size_t i = 0; i < table->typedef_count; i++) {
    free(table->typedefs[i].name);
  }
  free(table->typedefs);
  free(table);
}

bool type_table_add(TypeTable *table, const char *type_name) {
  for (size_t i = 0; i < table->struct_count; i++) {
    if (strcmp(table->struct_names[i], type_name) == 0) {
      return true;
    }
  }

  if (table->struct_count >= table->struct_capacity) {
    table->struct_capacity *= 2;
    table->struct_names = realloc(table->struct_names, sizeof(char *) * table->struct_capacity);
  }
  table->struct_names[table->struct_count++] = clone_string(type_name);
  return true;
}

bool type_table_add_enum(TypeTable *table, const char *enum_name) {
  // For now, enums are tracked the same as structs
  // Later you might want separate tracking
  return type_table_add(table, enum_name);
}

const char *type_table_lookup(const TypeTable *table, const char *type_name) {
  for (size_t i = 0; i < table->struct_count; i++) {
    if (strcmp(table->struct_names[i], type_name) == 0) {
      return table->struct_names[i];
    }
  }
  return NULL;
}

bool type_table_add_typedef(TypeTable *table, const char *name, const SuffixInfo *type_info) {

  for (size_t i = 0; i < table->typedef_count; i++) {
    if (strcmp(table->typedefs[i].name, name) == 0) {
      return false;
    }
  }

  if (table->typedef_count >= table->typedef_capacity) {
    table->typedef_capacity *= 2;
    table->typedefs = realloc(table->typedefs, sizeof(TypedefInfo) * table->typedef_capacity);
  }
  table->typedefs[table->typedef_count].name = clone_string(name);
  table->typedefs[table->typedef_count].type_info = *type_info;
  table->typedef_count++;
  return true;
}

const TypedefInfo *type_table_lookup_typedef(const TypeTable *table, const char *name) {
  for (size_t i = 0; i < table->typedef_count; i++) {
    if (strcmp(table->typedefs[i].name, name) == 0) {
      return &table->typedefs[i];
    }
  }
  return NULL;
}

// ==================
// COMPONENT SYSTEM 
// ==================

const char *find_suffix_separator(const char *name) {
  return strrchr(name, '_');
}

bool suffix_parse(const char *full_variable_name, const TypeTable *type_table, SuffixInfo *result_info) {
    *result_info = (SuffixInfo){TYPE_VOID, ROLE_NONE, false, false, NULL, false, TYPE_VOID, NULL};
    
    const char *separator = find_suffix_separator(full_variable_name);
    if (!separator) return false;
    
    const char *suffix_str = separator + 1;
    size_t suffix_len = strlen(suffix_str);
    if (suffix_len == 0) return false;

    // --- NEW, ROBUST LOGIC ---

    // 1. Check for arrays FIRST. This handles both primitive (_ia) and user-defined (_PCIDevicea) arrays.
    if (suffix_len > 1 && suffix_str[suffix_len - 1] == 'a') {
        char base_suffix[128];
        strncpy(base_suffix, suffix_str, suffix_len - 1);
        base_suffix[suffix_len - 1] = '\0';

        // Check for primitive array base (e.g., 'i' in 'ia')
        for (const SuffixMapping *m = suffix_table; m->suffix; m++) {
            if (strcmp(base_suffix, m->suffix) == 0) {
                result_info->type = TYPE_ARRAY;
                result_info->array_base_type = m->type;
                return true;
            }
        }
        
        // Check for user-defined array base (e.g., 'PCIDevice' in 'PCIDevicea')
        const char *user_type = type_table_lookup(type_table, base_suffix);
        if (user_type) {
            result_info->type = TYPE_ARRAY;
            result_info->array_base_type = TYPE_USER;
            result_info->array_user_type_name = user_type;
            return true;
        }
    }

    // 2. Check for user-defined types with pointer modifiers (e.g., _PCIDevicep)
    // This logic MUST come before the generic primitive check.
    for (size_t i = 0; i < type_table->struct_count; i++) {
        const char *user_type_name = type_table->struct_names[i];
        size_t user_type_len = strlen(user_type_name);

        if (strncmp(suffix_str, user_type_name, user_type_len) == 0) {
            const char *modifier = suffix_str + user_type_len;
            if (strcmp(modifier, "p") == 0 || strcmp(modifier, "b") == 0 || strcmp(modifier, "r") == 0) {
                result_info->type = TYPE_USER;
                result_info->user_type_name = user_type_name;
                result_info->is_pointer = true;
                if (modifier[0] == 'p') { result_info->role = ROLE_OWNED; }
                if (modifier[0] == 'b') { result_info->is_const = true; result_info->role = ROLE_BORROWED; }
                if (modifier[0] == 'r') { result_info->is_const = true; result_info->role = ROLE_REFERENCE; }
                return true;
            }
        }
    }

    // 3. Handle exact matches for ALL other suffixes (primitives like _i, and plain user types like _PCIDevice)
    // This uses your clean, data-driven table.
    for (const SuffixMapping *m = suffix_table; m->suffix; m++) {
        if (strcmp(suffix_str, m->suffix) == 0) {
            result_info->type = m->type; result_info->role = m->role;
            result_info->is_pointer = m->is_pointer; result_info->is_const = m->is_const;
            return true;
        }
    }
    const char *user_type = type_table_lookup(type_table, suffix_str);
    if (user_type) {
        result_info->type = TYPE_USER; result_info->user_type_name = user_type;
        return true;
    }

    // 4. Fallback for typedefs
    const TypedefInfo *typedef_info = type_table_lookup_typedef(type_table, suffix_str);
    if (typedef_info) {
        *result_info = typedef_info->type_info;
        return true;
    }
    
    return false;
}
const char *get_c_type(const SuffixInfo *info) {
    static char type_buffer[256];
    
    // Handle special cases first
    if (info->type == TYPE_USER) {
        if (info->user_type_name) {
            snprintf(type_buffer, sizeof(type_buffer), "%s%s%s",
                     info->is_const ? "const " : "",
                     info->user_type_name,
                     info->is_pointer ? "*" : "");
            return type_buffer;
        }
    }
    
    // Determine base type
    DataType lookup_type = (info->type == TYPE_ARRAY) ? 
                           info->array_base_type : info->type;
    
    // Handle array with user type
    if (info->type == TYPE_ARRAY && info->array_base_type == TYPE_USER) {
        strcpy(type_buffer, info->array_user_type_name ? 
               info->array_user_type_name : "void");
        return type_buffer;
    }
    
    // Look up in table
    const char *base_type = "void";
    for (const TypeMapping *m = type_map; m->c_type; m++) {
        if (m->type == lookup_type) {
            base_type = m->c_type;
            break;
        }
    }
    
    // Build final type string
    if (info->type == TYPE_ARRAY || info->type == TYPE_STRING) {
        strcpy(type_buffer, base_type);
    } else if (info->type == TYPE_FUNC_POINTER) {
        strcpy(type_buffer, "void*");  // Generic function pointer
    } else {
        snprintf(type_buffer, sizeof(type_buffer), "%s%s%s",
                 info->is_const ? "const " : "",
                 base_type,
                 info->is_pointer ? "*" : "");
    }
    
    return type_buffer;
}

// ======
// LEXER 
// ======

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
  TOKEN_PASSTHROUGH,
} TokenType;

typedef struct {
  TokenType type;
  char *text;
  char *base_name;
  SuffixInfo suffix_info;
  int line;
} Token;

typedef struct {
  const char *source;
  int pos;
  int len;
  int line;
  const TypeTable *type_table;
} Lexer;

typedef struct {
    char first;
    char second;
    char third;  // For three-char ops like <<=
    const char *token;
} MultiCharOp;

static const char *KEYWORDS[] = {
    "if",
    "else",
    "while",
    "do",
    "for",
    "return",
    "break",
    "continue",
    "func",
    "let",
    "struct",
    "sizeof",
    "switch",
    "case",
    "default",
    "typedef",
    "cast",
    "null",
    "enum",
    "static",
    "const",
    "extern",
    "union",
     NULL
};

static const MultiCharOp multi_char_ops[] = {
    // Three-character operators
    {'<', '<', '=', "<<="},
    {'>', '>', '=', ">>="},
    
    // Two-character operators
    {'=', '=', '\0', "=="},
    {'!', '=', '\0', "!="},
    {'<', '=', '\0', "<="},
    {'>', '=', '\0', ">="},
    {'&', '&', '\0', "&&"},
    {'|', '|', '\0', "||"},
    {'<', '<', '\0', "<<"},
    {'>', '>', '\0', ">>"},
    {'+', '+', '\0', "++"},
    {'-', '-', '\0', "--"},
    {'+', '=', '\0', "+="},
    {'-', '=', '\0', "-="},
    {'*', '=', '\0', "*="},
    {'/', '=', '\0', "/="},
    {'%', '=', '\0', "%="},
    {'&', '=', '\0', "&="},
    {'|', '=', '\0', "|="},
    {'^', '=', '\0', "^="},
    {'-', '>', '\0', "->"},  // For completeness
    
    {'\0', '\0', '\0', NULL}  // Sentinel
};

static bool is_keyword(const char *word) {
  for (int i = 0; KEYWORDS[i]; i++) {
    if (strcmp(word, KEYWORDS[i]) == 0)
      return true;
  }
  return false;
}

Lexer *lexer_create(const char *source, const TypeTable *type_table) {
  Lexer *lex = malloc(sizeof(Lexer));
  lex->source = source;
  lex->len = strlen(source);
  lex->pos = 0;
  lex->line = 1;
  lex->type_table = type_table;
  return lex;
}

void lexer_destroy(Lexer *lex) { 
    free(lex); 
}

void token_free(Token *tok) {
  if (tok) {
    free(tok->text);
    free(tok->base_name);
    free(tok);
  }
}

static void skip_whitespace(Lexer *lex) {
  while (lex->pos < lex->len && isspace(lex->source[lex->pos])) {
    if (lex->source[lex->pos] == '\n')
      lex->line++;
    lex->pos++;
  }
}

static Token *make_token(TokenType type, const char *text, int line) {
  Token *tok = calloc(1, sizeof(Token));
  tok->type = type;
  tok->line = line;
  tok->text = clone_string(text);
  return tok;
}

Token *lexer_next(Lexer *lex) {
  skip_whitespace(lex);
  if (lex->pos >= lex->len)
    return make_token(TOKEN_EOF, "", lex->line);

  int start = lex->pos;
  char c = lex->source[lex->pos];

  // Preprocessor directives
  if (c == '#') {
    lex->pos++;
    start = lex->pos;
    while (lex->pos < lex->len && lex->source[lex->pos] != '\n') {
      lex->pos++;
    }
    int len = lex->pos - start;
    char *directive_line = malloc(len + 2);
    directive_line[0] = '#';
    memcpy(directive_line + 1, lex->source + start, len);
    directive_line[len + 1] = '\0';
    Token *tok = make_token(TOKEN_DIRECTIVE, directive_line, lex->line);
    free(directive_line);
    return tok;
  }
  // Escape Hatch
  if (c == '@' && lex->pos + 2 < lex->len && lex->source[lex->pos + 1] == 'c' &&
      lex->source[lex->pos + 2] == '(') {
    lex->pos += 3; // Skip @c(
    start = lex->pos;

    // Find the closing )
    int paren_count = 1;
    while (lex->pos < lex->len && paren_count > 0) {
      if (lex->source[lex->pos] == '(')
        paren_count++;
      else if (lex->source[lex->pos] == ')')
        paren_count--;
      if (paren_count > 0)
        lex->pos++;
    }

    int len = lex->pos - start;
    char *passthrough_code = malloc(len + 1);
    memcpy(passthrough_code, lex->source + start, len);
    passthrough_code[len] = '\0';

    if (lex->pos < lex->len)
      lex->pos++; // Skip closing )

    Token *tok = make_token(TOKEN_PASSTHROUGH, passthrough_code, lex->line);
    free(passthrough_code);
    return tok;
  }
  // Comments
  if (c == '/' && lex->pos + 1 < lex->len && lex->source[lex->pos + 1] == '/') {
    while (lex->pos < lex->len && lex->source[lex->pos] != '\n')
      lex->pos++;
    return lexer_next(lex);
  }

  // Identifiers and keywords
  if (isalpha(c) || c == '_') {
    while (lex->pos < lex->len && (isalnum(lex->source[lex->pos]) || lex->source[lex->pos] == '_')) {
      lex->pos++;
    }
    int len = lex->pos - start;
    char *word = malloc(len + 1);
    memcpy(word, lex->source + start, len);
    word[len] = '\0';

    Token *tok;
    if (is_keyword(word)) {
      tok = make_token(TOKEN_KEYWORD, word, lex->line);
    } else {
      tok = make_token(TOKEN_IDENTIFIER, word, lex->line);

      // Parse suffix
      SuffixInfo info;
      if (suffix_parse(word, lex->type_table, &info)) {
        const char *separator = find_suffix_separator(word);
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

  // Numbers & 0xfu
  if (isdigit(c)) {
    if (c == '0' && lex->pos + 1 < lex->len && (lex->source[lex->pos + 1] == 'x' || lex->source[lex->pos + 1] == 'X')) {
        lex->pos += 2; // Skip '0x'
        start = lex->pos;
        while (lex->pos < lex->len && isxdigit(lex->source[lex->pos])) {
            lex->pos++;
        }
        int len = lex->pos - start;
        char *hex_num = malloc(len + 3); // for 0x prefix and null terminator
        strcpy(hex_num, "0x");
        memcpy(hex_num + 2, lex->source + start, len);
        hex_num[len + 2] = '\0';
        Token *tok = make_token(TOKEN_NUMBER, hex_num, lex->line);
        free(hex_num);
        return tok;
    }
    while (lex->pos < lex->len && isdigit(lex->source[lex->pos]))
      lex->pos++;
    if (lex->pos < lex->len && lex->source[lex->pos] == '.') {
      lex->pos++;
      while (lex->pos < lex->len && isdigit(lex->source[lex->pos]))
        lex->pos++;
    }
    int len = lex->pos - start;
    char *num = malloc(len + 1);
    memcpy(num, lex->source + start, len);
    num[len] = '\0';
    Token *tok = make_token(TOKEN_NUMBER, num, lex->line);
    free(num);
    return tok;
  }

  // Strings
  if (c == '"') {
    lex->pos++;
    start = lex->pos;
    while (lex->pos < lex->len && lex->source[lex->pos] != '"') {
      if (lex->source[lex->pos] == '\\' && lex->pos + 1 < lex->len)
        lex->pos++;
      lex->pos++;
    }
    int len = lex->pos - start;
    char *str = malloc(len + 1);
    memcpy(str, lex->source + start, len);
    str[len] = '\0';
    if (lex->pos < lex->len)
      lex->pos++;
    Token *tok = make_token(TOKEN_STRING, str, lex->line);
    free(str);
    return tok;
  }

  // Characters
  if (c == '\'') {
    lex->pos++;
    start = lex->pos;
    if (lex->source[lex->pos] == '\\' && lex->pos + 1 < lex->len) {
      lex->pos += 2;
    } else {
      lex->pos++;
    }
    int len = lex->pos - start;
    char *char_val = malloc(len + 1);
    memcpy(char_val, lex->source + start, len);
    char_val[len] = '\0';
    if (lex->pos < lex->len && lex->source[lex->pos] == '\'') {
      lex->pos++;
    }
    Token *tok = make_token(TOKEN_CHARACTER, char_val, lex->line);
    free(char_val);
    return tok;
  }

  // Arrow operator
  if (c == '-' && lex->pos + 1 < lex->len && lex->source[lex->pos + 1] == '>') {
    lex->pos += 2;
    return make_token(TOKEN_ARROW, "->", lex->line);
  }

 // Operators and punctuation
char op_text[4] = {c, '\0', '\0', '\0'};
lex->pos++;

// Check for multi-character operators
if (lex->pos < lex->len) {
    char next = lex->source[lex->pos];
    
    // Try three-character first
    if (lex->pos + 1 < lex->len) {
        char third = lex->source[lex->pos + 1];
        for (const MultiCharOp *op = multi_char_ops; op->token; op++) {
            if (c == op->first && next == op->second && 
                op->third != '\0' && third == op->third) {
                strcpy(op_text, op->token);
                lex->pos += 2;  // Already moved past first char
                goto make_op_token;
            }
        }
    }
    
    // Try two-character
    for (const MultiCharOp *op = multi_char_ops; op->token; op++) {
        if (c == op->first && next == op->second && op->third == '\0') {
            strcpy(op_text, op->token);
            lex->pos++;
            goto make_op_token;
        }
    }
}

make_op_token:
TokenType type = strchr("{}[]();,.:", c) ? TOKEN_PUNCTUATION : TOKEN_OPERATOR;
return make_token(type, op_text, lex->line);
}

// ============================================================================
// AST - Abstract Syntax Tree
// ============================================================================

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
  AST_FUNC_PTR_DECL,
  AST_TYPEDEF,
  AST_PASSTHROUGH,
  AST_NULL,
  AST_CAST,
  AST_ENUM_DEF,
  AST_ENUM_VALUE,
  AST_POSTFIX_OP,
} ASTType;

typedef struct ASTNode {
  ASTType type;
  char *value;
  SuffixInfo suffix_info;
  struct ASTNode **children;
  int child_count;
  int child_cap;
} ASTNode;

typedef struct {
  Lexer *lexer;
  Token *current;
  TypeTable *type_table;
  bool had_error;
} Parser;

typedef struct FuncDecl {
  char *name;
  SuffixInfo return_type;
  ASTNode *params;
  struct FuncDecl *next;
} FuncDecl;

typedef struct {
    const char *op;
    int precedence;
    bool left_assoc;
    bool is_binary;  // true for binary, false for unary
} OpInfo;

static const OpInfo operator_table[] = {
    // Multiplicative (highest precedence for binary)
    {"*",   10, true,  true},
    {"/",   10, true,  true},
    {"%",   10, true,  true},
    
    // Additive
    {"+",   9,  true,  true},
    {"-",   9,  true,  true},
    
    // Shift
    {"<<",  8,  true,  true},
    {">>",  8,  true,  true},
    
    // Relational
    {"<",   7,  true,  true},
    {">",   7,  true,  true},
    {"<=",  7,  true,  true},
    {">=",  7,  true,  true},
    
    // Equality
    {"==",  6,  true,  true},
    {"!=",  6,  true,  true},
    
    // Bitwise AND
    {"&",   5,  true,  true},
    
    // Bitwise XOR
    {"^",   4,  true,  true},
    
    // Bitwise OR
    {"|",   3,  true,  true},
    
    // Logical AND
    {"&&",  2,  true,  true},
    
    // Logical OR
    {"||",  1,  true,  true},
    
    // Assignment (right associative)
    {"=",   0,  false, true},
    {"+=",  0,  false, true},
    {"-=",  0,  false, true},
    {"*=",  0,  false, true},
    {"/=",  0,  false, true},
    {"%=",  0,  false, true},
    {"&=",  0,  false, true},
    {"|=",  0,  false, true},
    {"^=",  0,  false, true},
    {"<<=", 0,  false, true},
    {">>=", 0,  false, true},
    
    {NULL,  0,  false, false}  // Sentinel
};
// =======
// PARSER
// =======

static ASTNode *parse_statement(Parser *p);
static ASTNode *parse_expression(Parser *p);
static ASTNode *parse_block(Parser *p);
static ASTNode *parse_member_access(Parser *p);
static ASTNode *parse_ternary(Parser *p);
static ASTNode *parse_initializer_list(Parser *p);
static ASTNode *parse_call(Parser *p);
static ASTNode *parse_typedef(Parser *p);
static ASTNode *parse_enum_definition(Parser *p);
static ASTNode *parse_unary(Parser *p);
static ASTNode *parse_postfix(Parser *p);

// AST helper functions
static ASTNode *create_node(ASTType type, const char *value) {
  ASTNode *node = calloc(1, sizeof(ASTNode));
  node->type = type;
  node->value = value ? clone_string(value) : NULL;
  node->child_cap = 2;
  node->children = calloc(node->child_cap, sizeof(ASTNode *));
  return node;
}

static void add_child(ASTNode *parent, ASTNode *child) {
  if (!parent || !child)
    return;
  if (parent->child_count >= parent->child_cap) {
    parent->child_cap *= 2;
    parent->children = realloc(parent->children, parent->child_cap * sizeof(ASTNode *));
  }
  parent->children[parent->child_count++] = child;
}

void ast_destroy(ASTNode *node) {
  if (!node)
    return;
  for (int i = 0; i < node->child_count; i++) {
    ast_destroy(node->children[i]);
  }
  free(node->value);
  free(node->children);
  free(node);
}

static Token *advance(Parser *p) {
  Token *previous = p->current;
  p->current = lexer_next(p->lexer);
  return previous;
}

static bool check(Parser *p, TokenType type) {
  return p->current->type == type;
}

static void parser_error(Parser *p, const char *message) {
  if (!p->had_error) {
    fprintf(stderr, "Parse Error on line %d near '%s': %s\n", p->current->line,
            p->current->text, message);
    p->had_error = true;
  }
}

static bool match_and_consume(Parser *p, TokenType type, const char *text) {
  if (p->current->type == type &&
      (!text || strcmp(p->current->text, text) == 0)) {
    token_free(advance(p));
    return true;
  }
  return false;
}

static void expect(Parser *p, TokenType type, const char *text, const char *error_message) {
  if (p->current->type == type &&
      (!text || strcmp(p->current->text, text) == 0)) {
    token_free(advance(p));
  } else {
    parser_error(p, error_message);
  }
}

static ASTNode *parse_primary(Parser *p) {
  // Initializer lists
  if (check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, "{") == 0) {
    return parse_initializer_list(p);
  }
  
  // Handle parentheses
  if (match_and_consume(p, TOKEN_PUNCTUATION, "(")) {
    ASTNode *expr = parse_expression(p);
    expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after expression.");
    return expr;
  }
  
  // Handle identifiers (including complex expressions)
    if (check(p, TOKEN_IDENTIFIER)) {
    	Token *tok = advance(p);
    
    // Special case: handle cast_<type> syntax
    if (tok->base_name && strcmp(tok->base_name, "cast") == 0) {
      ASTNode *node = create_node(AST_CAST, NULL);
      node->suffix_info = tok->suffix_info;
      token_free(tok);
      
      expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' after cast type.");
      add_child(node, parse_expression(p));
      expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after cast expression.");
      return node;
    }
    
    ASTNode *node = create_node(AST_IDENTIFIER, tok->base_name ? tok->base_name : tok->text);
    if (tok->base_name) {
      node->suffix_info = tok->suffix_info;
    }
    token_free(tok);
    return node;
  }
  // Numbers
  if (check(p, TOKEN_NUMBER)) {
    Token *tok = advance(p);
    ASTNode *node = create_node(AST_NUMBER, tok->text);
    token_free(tok);
    return node;
  }

  // Strings
  if (check(p, TOKEN_STRING)) {
    Token *tok = advance(p);
    ASTNode *node = create_node(AST_STRING, tok->text);
    token_free(tok);
    return node;
  }

  // Characters
  if (check(p, TOKEN_CHARACTER)) {
    Token *tok = advance(p);
    ASTNode *node = create_node(AST_CHARACTER, tok->text);
    token_free(tok);
    return node;
  }

  // sizeof
  if (match_and_consume(p, TOKEN_KEYWORD, "sizeof")) {
    ASTNode *node = create_node(AST_SIZEOF, "sizeof");
    expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' after 'sizeof'.");
    if (check(p, TOKEN_IDENTIFIER)) {
      Token *type_tok = advance(p);
      // Always keep the identifier as a child for sizeof
      ASTNode *id_node = create_node(AST_IDENTIFIER, type_tok->base_name ? type_tok->base_name : type_tok->text);
      if (type_tok->base_name) {
        id_node->suffix_info = type_tok->suffix_info;
      }
      add_child(node, id_node);
      token_free(type_tok);
    }
    expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after sizeof argument.");
    return node;
  }
  // null keyword
  if (match_and_consume(p, TOKEN_KEYWORD, "null")) {
    return create_node(AST_NULL, "NULL");
  }

  // cast keyword
  if (match_and_consume(p, TOKEN_KEYWORD, "cast")) {
    expect(p, TOKEN_PUNCTUATION, "_", "Expected '_' after 'cast'.");

    // Parse the type suffix
    Token *type_tok = advance(p);
    if (type_tok->type != TOKEN_IDENTIFIER) {
      parser_error(p, "Expected type suffix after 'cast_'.");
      token_free(type_tok);
      return NULL;
    }

    ASTNode *node = create_node(AST_CAST, NULL);
    node->suffix_info = type_tok->suffix_info;
    token_free(type_tok);

    expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' after cast type.");
    add_child(node, parse_expression(p));
    expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after cast expression.");

    return node;
  }
  parser_error(p, "Expected expression.");
  return NULL;
}

static ASTNode *parse_call(Parser *p) {
  ASTNode *expr = parse_member_access(p);

  while (match_and_consume(p, TOKEN_PUNCTUATION, "(")) {
    ASTNode *call_node = create_node(AST_CALL, NULL);
    // The thing being called is the expression we just parsed
    add_child(call_node, expr);

    // Now parse the arguments
    if (!check(p, TOKEN_PUNCTUATION) || strcmp(p->current->text, ")") != 0) {
      do {
        add_child(call_node, parse_expression(p));
      } while (match_and_consume(p, TOKEN_PUNCTUATION, ","));
    }
    expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after arguments.");
    expr = call_node;
  }
  return expr;
}

static ASTNode *parse_subscript(Parser *p) {
  ASTNode *expr = parse_primary(p);
  
  while (true) {
    if (match_and_consume(p, TOKEN_PUNCTUATION, "[")) {
      ASTNode *node = create_node(AST_SUBSCRIPT, NULL);
      add_child(node, expr);
      add_child(node, parse_expression(p));
      expect(p, TOKEN_PUNCTUATION, "]", "Expected ']' after subscript index.");
      expr = node;
    } else {
      break;
    }
  }
  return expr;
}

static ASTNode *parse_member_access(Parser *p) {
  ASTNode *left = parse_subscript(p);

  while (true) {
    if (match_and_consume(p, TOKEN_PUNCTUATION, ".")) {
      ASTNode *node = create_node(AST_MEMBER_ACCESS, ".");
      add_child(node, left);

      Token *member = advance(p);
      if (member->type != TOKEN_IDENTIFIER) {
        parser_error(p, "Expected member name after '.'.");
      }
      add_child(node, create_node(AST_IDENTIFIER, member->base_name ? member->base_name : member->text));
      token_free(member);
      left = node;
    } else if (match_and_consume(p, TOKEN_ARROW, "->")) {
      ASTNode *node = create_node(AST_MEMBER_ACCESS, "->");
      add_child(node, left);

      Token *member = advance(p);
      if (member->type != TOKEN_IDENTIFIER) {
        parser_error(p, "Expected member name after '->'.");
      }
      ASTNode *member_node = create_node(AST_IDENTIFIER, member->base_name ? member->base_name : member->text);
      if (member->base_name) {
        node->suffix_info = member->suffix_info;
      }
      add_child(node, member_node);
      token_free(member);
      left = node;
    } else if (check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, "[") == 0) {
      // Handle array indexing after member access
      token_free(advance(p)); // Consume '['
      ASTNode *subscript_node = create_node(AST_SUBSCRIPT, NULL);
      add_child(subscript_node, left);
      add_child(subscript_node, parse_expression(p));
      expect(p, TOKEN_PUNCTUATION, "]", "Expected ']' after subscript index.");
      left = subscript_node;
    } else {
      break;
    }
  }
  return left;
}

static ASTNode *parse_typedef(Parser *p) {
  expect(p, TOKEN_KEYWORD, "typedef", "Expected 'typedef' keyword.");

  // Parse the type specification
  Token *type_tok = advance(p);
  if (type_tok->type != TOKEN_IDENTIFIER) {
    parser_error(p, "Expected type name after 'typedef'.");
    token_free(type_tok);
    return NULL;
  }

  // Parse the new type name
  Token *name_tok = advance(p);
  if (name_tok->type != TOKEN_IDENTIFIER) {
    parser_error(p, "Expected type alias name.");
    token_free(type_tok);
    token_free(name_tok);
    return NULL;
  }

  // Create typedef node
  ASTNode *node = create_node(AST_TYPEDEF, name_tok->text);

  // Store the original type information
  ASTNode *type_node = create_node(AST_IDENTIFIER, type_tok->base_name ? type_tok->base_name : type_tok->text);
  if (type_tok->base_name) {
    type_node->suffix_info = type_tok->suffix_info;
  }
  add_child(node, type_node);
  type_table_add_typedef((TypeTable *)p->type_table, name_tok->text, &type_tok->suffix_info);
  expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after typedef.");

  token_free(type_tok);
  token_free(name_tok);
  return node;
}

static ASTNode *parse_unary(Parser *p) {
  if (check(p, TOKEN_OPERATOR) && (strcmp(p->current->text, "-") == 0 ||
                                   strcmp(p->current->text, "!") == 0 ||
                                   strcmp(p->current->text, "&") == 0 ||
                                   strcmp(p->current->text, "*") == 0 ||
                                   strcmp(p->current->text, "++") == 0 ||  // prefix
                                   strcmp(p->current->text, "--") == 0)) {  // prefix{
    Token *op_tok = advance(p);
    ASTNode *node = create_node(AST_UNARY_OP, op_tok->text);
    add_child(node, parse_unary(p));
    token_free(op_tok);
    return node;
  }
  return parse_postfix(p);
}

static ASTNode *parse_postfix(Parser *p) {
    ASTNode *expr = parse_call(p);
    
    if (check(p, TOKEN_OPERATOR) && 
        (strcmp(p->current->text, "++") == 0 || 
         strcmp(p->current->text, "--") == 0)) {
        Token *op = advance(p);
        ASTNode *node = create_node(AST_POSTFIX_OP, op->text);
        add_child(node, expr);
        token_free(op);
        return node;
    }
    return expr;
}

static ASTNode *parse_binary_expr(Parser *p, int min_precedence) {
    ASTNode *left = parse_unary(p);
    
    while (true) {
        // Find operator in table
        const OpInfo *op_info = NULL;
        if (check(p, TOKEN_OPERATOR)) {
            for (const OpInfo *op = operator_table; op->op; op++) {
                if (op->is_binary && strcmp(p->current->text, op->op) == 0 &&
                    op->precedence >= min_precedence) {
                    op_info = op;
                    break;
                }
            }
        }
        
        if (!op_info) break;
        
        Token *op_tok = advance(p);
        
        // Calculate next minimum precedence
        int next_min_prec = op_info->left_assoc ? 
                           (op_info->precedence + 1) : op_info->precedence;
        
        ASTNode *right = (op_info->precedence == 0) ? 
                        parse_ternary(p) :  // Assignment recurses to ternary
                        parse_binary_expr(p, next_min_prec);
        
        ASTNode *node = create_node(AST_BINARY_OP, op_tok->text);
        add_child(node, left);
        add_child(node, right);
        token_free(op_tok);
        left = node;
    }
    
    return left;
}


static ASTNode *parse_expression(Parser *p) {
    return parse_ternary(p);  
}


static ASTNode *parse_ternary(Parser *p) {
    ASTNode *condition = parse_binary_expr(p, 0); 
    
    if (match_and_consume(p, TOKEN_OPERATOR, "?")) {
        ASTNode *ternary_node = create_node(AST_TERNARY_OP, "?");
        add_child(ternary_node, condition);
        add_child(ternary_node, parse_expression(p));
        expect(p, TOKEN_PUNCTUATION, ":", "Expected ':' for ternary operator.");
        add_child(ternary_node, parse_ternary(p));
        return ternary_node;
    }
    
    return condition;
}

static ASTNode *parse_initializer_list(Parser *p) {
  ASTNode *list = create_node(AST_INITIALIZER_LIST, NULL);
  expect(p, TOKEN_PUNCTUATION, "{", "Expected '{' to begin initializer list.");

  if (!check(p, TOKEN_PUNCTUATION) || strcmp(p->current->text, "}") != 0) {
    do {
      add_child(list, parse_expression(p));
    } while (match_and_consume(p, TOKEN_PUNCTUATION, ","));
  }

  expect(p, TOKEN_PUNCTUATION, "}", "Expected '}' to end initializer list.");
  return list;
}

static ASTNode *parse_var_decl(Parser *p) {
  Token *name = advance(p);
  if (name->type != TOKEN_IDENTIFIER) {
    parser_error(p, "Expected variable name.");
    token_free(name);
    return NULL;
  }

  ASTNode *node = create_node(AST_VAR_DECL, name->base_name ? name->base_name : name->text);
  if (name->base_name) {
    node->suffix_info = name->suffix_info;
  }
  
  // Check if this is an array declaration (has array type suffix)
  if (node->suffix_info.type == TYPE_ARRAY) {
    // Array declaration with size
    if (check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, "[") == 0) {
      token_free(advance(p)); // Consume '['
      add_child(node, parse_expression(p));  // Child 0: array size
      expect(p, TOKEN_PUNCTUATION, "]", "Expected ']' after array size.");
    }
    
    // Optional initializer for array
    if (match_and_consume(p, TOKEN_OPERATOR, "=")) {
      // Check if it's a char array and next token is a string literal
      if (node->suffix_info.array_base_type == TYPE_CHAR && 
          check(p, TOKEN_STRING)) {
        // String literal initialization for char array
        Token *str_tok = advance(p);
        ASTNode *str_node = create_node(AST_STRING, str_tok->text);
        add_child(node, str_node);  // Child 1: string initializer
        token_free(str_tok);
      } else {
        // Regular initializer list
        add_child(node, parse_initializer_list(p));  // Child 1: initializer
      }
    }
  } else {
    // Regular variable initialization
    if (match_and_consume(p, TOKEN_OPERATOR, "=")) {
      add_child(node, parse_expression(p));  // Child 0: initializer
    }
  }

  token_free(name);
  return node;
}

static ASTNode *parse_if_statement(Parser *p) {
  ASTNode *node = create_node(AST_IF, "if");
  expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' after 'if'.");
  add_child(node, parse_expression(p));
  expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after if condition.");
  add_child(node, parse_block(p));

  if (match_and_consume(p, TOKEN_KEYWORD, "else")) {
    if (check(p, TOKEN_KEYWORD) && strcmp(p->current->text, "if") == 0) {
      token_free(advance(p));
      add_child(node, parse_if_statement(p));
    } else {
      add_child(node, parse_block(p));
    }
  }
  return node;
}

static ASTNode *parse_while_statement(Parser *p) {
  ASTNode *node = create_node(AST_WHILE, "while");
  expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' after 'while'.");
  add_child(node, parse_expression(p));
  expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after while condition.");
  add_child(node, parse_block(p));
  return node;
}

static ASTNode *parse_do_statement(Parser *p) {
  ASTNode *node = create_node(AST_DO, "do");
  add_child(node, parse_block(p));
  expect(p, TOKEN_KEYWORD, "while", "Expected 'while' after do-block.");
  expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' after 'while'.");
  add_child(node, parse_expression(p));
  expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after do-while condition.");
  expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after do-while statement.");
  return node;
}

static ASTNode *parse_for_statement(Parser *p) {
  ASTNode *node = create_node(AST_FOR, "for");
  expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' after 'for'.");

  // Initializer
  if (match_and_consume(p, TOKEN_PUNCTUATION, ";")) {
    add_child(node, NULL);
  } else {
    if (check(p, TOKEN_KEYWORD) && strcmp(p->current->text, "let") == 0) {
      token_free(advance(p));
      add_child(node, parse_var_decl(p));
    } else {
      add_child(node, parse_expression(p));
    }
    expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after for loop initializer.");
  }

  // Condition
  if (match_and_consume(p, TOKEN_PUNCTUATION, ";")) {
    add_child(node, NULL);
  } else {
    add_child(node, parse_expression(p));
    expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after for loop condition.");
  }

  // Increment
  if (check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, ")") == 0) {
    add_child(node, NULL);
  } else {
    add_child(node, parse_expression(p));
  }

  expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after for loop clauses.");
  add_child(node, parse_block(p));

  return node;
}

static ASTNode *parse_switch_statement(Parser *p) {
  ASTNode *node = create_node(AST_SWITCH, "switch");
  expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' after 'switch'.");
  add_child(node, parse_expression(p));
  expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after switch expression.");
  expect(p, TOKEN_PUNCTUATION, "{", "Expected '{' to begin switch body.");

  while (!match_and_consume(p, TOKEN_PUNCTUATION, "}")) {
    if (check(p, TOKEN_EOF)) {
      parser_error(p, "Unterminated switch statement.");
      break;
    }

    if (match_and_consume(p, TOKEN_KEYWORD, "case")) {
      ASTNode *case_node = create_node(AST_CASE, "case");
      add_child(case_node, parse_expression(p));
      expect(p, TOKEN_PUNCTUATION, ":", "Expected ':' after case value.");
      add_child(node, case_node);

      while (true) {
        if (check(p, TOKEN_EOF) ||
            (check(p, TOKEN_PUNCTUATION) &&
             strcmp(p->current->text, "}") == 0) ||
            (check(p, TOKEN_KEYWORD) &&
             strcmp(p->current->text, "case") == 0) ||
            (check(p, TOKEN_KEYWORD) &&
             strcmp(p->current->text, "default") == 0)) {
          break;
        }
        add_child(case_node, parse_statement(p));
      }
    } else if (match_and_consume(p, TOKEN_KEYWORD, "default")) {
      ASTNode *default_node = create_node(AST_DEFAULT, "default");
      expect(p, TOKEN_PUNCTUATION, ":", "Expected ':' after 'default'.");
      add_child(node, default_node);

      while (true) {
        if (check(p, TOKEN_EOF) ||
            (check(p, TOKEN_PUNCTUATION) &&
             strcmp(p->current->text, "}") == 0) ||
            (check(p, TOKEN_KEYWORD) &&
             strcmp(p->current->text, "case") == 0)) {
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

static ASTNode *parse_statement(Parser *p) {

  if (check(p, TOKEN_PASSTHROUGH)) {
    Token *pass = advance(p);
    ASTNode *node = create_node(AST_PASSTHROUGH, pass->text);
    token_free(pass);
    // Passthrough is a full statement, but Dust syntax requires a semicolon
    expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after @c(...) statement.");
    return node;
  }

  if (check(p, TOKEN_KEYWORD)) {
    if (strcmp(p->current->text, "let") == 0) {
      token_free(advance(p));
      ASTNode *decl = parse_var_decl(p);
      expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after variable declaration.");
      return decl;
    }
    if (strcmp(p->current->text, "if") == 0) {
      token_free(advance(p));
      return parse_if_statement(p);
    }
    if (strcmp(p->current->text, "while") == 0) {
      token_free(advance(p));
      return parse_while_statement(p);
    }
    if (strcmp(p->current->text, "do") == 0) {
      token_free(advance(p));
      return parse_do_statement(p);
    }
    if (strcmp(p->current->text, "for") == 0) {
      token_free(advance(p));
      return parse_for_statement(p);
    }
    if (strcmp(p->current->text, "switch") == 0) {
      token_free(advance(p));
      return parse_switch_statement(p);
    }
    if (strcmp(p->current->text, "break") == 0) {
      token_free(advance(p));
      expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after 'break'.");
      return create_node(AST_BREAK, "break");
    }
    if (strcmp(p->current->text, "continue") == 0) {
      token_free(advance(p));
      expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after 'continue'.");
      return create_node(AST_CONTINUE, "continue");
    }
    if (strcmp(p->current->text, "return") == 0) {
      token_free(advance(p));
      ASTNode *node = create_node(AST_RETURN, "return");
      if (!(check(p, TOKEN_PUNCTUATION) &&
            strcmp(p->current->text, ";") == 0)) {
        add_child(node, parse_expression(p));
      }
      expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after return value.");
      return node;
    }
    if (check(p, TOKEN_PASSTHROUGH)) {
      Token *pass = advance(p);
      ASTNode *node = create_node(AST_PASSTHROUGH, pass->text);
      token_free(pass);
      return node;
    }
  }

  ASTNode *expr = parse_expression(p);
  expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after expression.");
  return expr;
}

static ASTNode *parse_block(Parser *p) {
  expect(p, TOKEN_PUNCTUATION, "{", "Expected '{' to begin a block.");
  ASTNode *block = create_node(AST_BLOCK, NULL);

  while (!(check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, "}") == 0) &&
         !check(p, TOKEN_EOF)) {
    add_child(block, parse_statement(p));
  }

  expect(p, TOKEN_PUNCTUATION, "}", "Expected '}' to end a block.");
  return block;
}

static ASTNode *parse_struct_definition(Parser *p) {
  Token *name_tok = advance(p);
  if (name_tok->type != TOKEN_IDENTIFIER) {
    parser_error(p, "Expected struct name.");
    token_free(name_tok);
    return NULL;
  }

  type_table_add((TypeTable *)p->type_table, name_tok->text);
  ASTNode *struct_node = create_node(AST_STRUCT_DEF, name_tok->text);
  token_free(name_tok);

  expect(p, TOKEN_PUNCTUATION, "{", "Expected '{' after struct name.");

  while (!check(p, TOKEN_PUNCTUATION) || strcmp(p->current->text, "}") != 0) {
    if (check(p, TOKEN_EOF)) {
      parser_error(p, "Unterminated struct definition.");
      ast_destroy(struct_node);
      return NULL;
    }

    if (check(p, TOKEN_IDENTIFIER)) {
      Token *member_tok = advance(p);

      // Check if the member is a function pointer
      if (member_tok->suffix_info.type == TYPE_FUNC_POINTER) {
        ASTNode *fp_node = create_node(AST_FUNC_PTR_DECL, member_tok->base_name);

        expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' for function pointer signature.");

        // Parse return type and parameter types
        do {
          if (p->current->type != TOKEN_IDENTIFIER) {
            parser_error(p, "Expected a type specifier (e.g., dummy_i) in signature.");
            break;
          }
          Token *type_tok = advance(p);
          ASTNode *type_node = create_node(AST_IDENTIFIER, NULL); // Name doesn't matter
          type_node->suffix_info = type_tok->suffix_info;
          add_child(fp_node, type_node);
          token_free(type_tok);
        } while (match_and_consume(p, TOKEN_PUNCTUATION, ","));

        expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' to close signature.");
        add_child(struct_node, fp_node);

      } else { // It's a regular variable or array
        ASTNode *member_node = create_node(AST_VAR_DECL, member_tok->base_name ? member_tok->base_name : member_tok->text);
        member_node->suffix_info = member_tok->suffix_info;

        // Check for array declaration
        if (match_and_consume(p, TOKEN_PUNCTUATION, "[")) {
          add_child(member_node, parse_expression(p));
          expect(p, TOKEN_PUNCTUATION, "]", "Expected ']' after array size.");
        }
        add_child(struct_node, member_node);
      }

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

static ASTNode *parse_enum_definition(Parser *p) {
  Token *name_tok = advance(p);
  if (name_tok->type != TOKEN_IDENTIFIER) {
    parser_error(p, "Expected enum name.");
    token_free(name_tok);
    return NULL;
  }

  type_table_add_enum((TypeTable *)p->type_table, name_tok->text);
  ASTNode *enum_node = create_node(AST_ENUM_DEF, name_tok->text);
  token_free(name_tok);

  expect(p, TOKEN_PUNCTUATION, "{", "Expected '{' after enum name.");

  int next_value = 0;  // Auto-increment counter
  
  while (!check(p, TOKEN_PUNCTUATION) || strcmp(p->current->text, "}") != 0) {
    if (check(p, TOKEN_EOF)) {
      parser_error(p, "Unterminated enum definition.");
      ast_destroy(enum_node);
      return NULL;
    }

    // Parse enum member name
    if (check(p, TOKEN_IDENTIFIER)) {
      Token *member_tok = advance(p);
      ASTNode *member_node = create_node(AST_ENUM_VALUE, member_tok->text);
      
      // Check for explicit value assignment
      if (match_and_consume(p, TOKEN_OPERATOR, "=")) {
        // Parse the value
        if (check(p, TOKEN_NUMBER)) {
          Token *val_tok = advance(p);
          ASTNode *val_node = create_node(AST_NUMBER, val_tok->text);
          add_child(member_node, val_node);
          next_value = atoi(val_tok->text) + 1;  // Update auto-increment
          token_free(val_tok);
        } else {
          parser_error(p, "Expected number after '=' in enum.");
        }
      } else {
        // Use auto-incremented value
        char val_str[32];
        snprintf(val_str, sizeof(val_str), "%d", next_value);
        ASTNode *val_node = create_node(AST_NUMBER, val_str);
        add_child(member_node, val_node);
        next_value++;
      }
      
      add_child(enum_node, member_node);
      token_free(member_tok);
      
      // Handle comma or end of enum
      if (check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, ",") == 0) {
        token_free(advance(p));  // Consume comma
        // Allow trailing comma
        if (check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, "}") == 0) {
          break;
        }
      } else if (!check(p, TOKEN_PUNCTUATION) || strcmp(p->current->text, "}") != 0) {
        parser_error(p, "Expected ',' or '}' after enum value.");
      }
    } else {
      parser_error(p, "Expected enum member name.");
      token_free(advance(p));
    }
  }

  expect(p, TOKEN_PUNCTUATION, "}", "Expected '}' to close enum definition.");
  expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after enum definition.");

  return enum_node;
}

static ASTNode *parse_function(Parser *p) {
  Token *name = advance(p);
  if (name->type != TOKEN_IDENTIFIER) {
    parser_error(p, "Expected function name.");
    token_free(name);
    return NULL;
  }

  ASTNode *func_node = create_node(AST_FUNCTION, name->base_name ? name->base_name : name->text);
  if (name->base_name) {
    func_node->suffix_info = name->suffix_info;
  }

  expect(p, TOKEN_PUNCTUATION, "(", "Expected '(' after function name.");

  ASTNode *params_node = create_node(AST_VAR_DECL, "params");
  add_child(func_node, params_node);

  if (!(check(p, TOKEN_PUNCTUATION) && strcmp(p->current->text, ")") == 0)) {
    do {
      Token *param_tok = advance(p);
      if (param_tok->type != TOKEN_IDENTIFIER) {
        parser_error(p, "Expected parameter name.");
        token_free(param_tok);
        break;
      }
      ASTNode *param_node = create_node(AST_VAR_DECL, param_tok->base_name ? param_tok->base_name : param_tok->text);
      if (param_tok->base_name) {
        param_node->suffix_info = param_tok->suffix_info;
      }
      add_child(params_node, param_node);
      token_free(param_tok);
    } while (match_and_consume(p, TOKEN_PUNCTUATION, ","));
  }

  expect(p, TOKEN_PUNCTUATION, ")", "Expected ')' after parameters.");
  add_child(func_node, parse_block(p));

  token_free(name);
  return func_node;
}

Parser *parser_create(const char *source, const TypeTable *type_table) {
  Parser *p = calloc(1, sizeof(Parser));
  p->type_table = (TypeTable *)type_table;
  p->lexer = lexer_create(source, p->type_table);
  p->current = lexer_next(p->lexer);
  return p;
}

ASTNode *parser_parse(Parser *p) {
  ASTNode *program = create_node(AST_PROGRAM, NULL);

  while (!check(p, TOKEN_EOF)) {
    if (check(p, TOKEN_DIRECTIVE)) {
      Token *dir_tok = advance(p);
      add_child(program, create_node(AST_DIRECTIVE, dir_tok->text));
      token_free(dir_tok);
    } else if (strcmp(p->current->text, "let") == 0) {
        token_free(advance(p));
        ASTNode *global = parse_var_decl(p);
        expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after global variable.");
        add_child(program, global);
    } else if (check(p, TOKEN_PASSTHROUGH)) {
      Token *pass = advance(p);
      add_child(program, create_node(AST_PASSTHROUGH, pass->text));
      token_free(pass);
      expect(p, TOKEN_PUNCTUATION, ";", "Expected ';' after @c(...) statement.");
    } else if (check(p, TOKEN_KEYWORD)) {
      if (strcmp(p->current->text, "typedef") == 0) {
        add_child(program, parse_typedef(p));
      } else if (strcmp(p->current->text, "func") == 0) {
        token_free(advance(p));
        add_child(program, parse_function(p));
      } else if (strcmp(p->current->text, "struct") == 0) {
        token_free(advance(p));
        add_child(program, parse_struct_definition(p));
      } else if (strcmp(p->current->text, "enum") == 0) {
        token_free(advance(p));
        add_child(program, parse_enum_definition(p));
      } else {
        parser_error(p, "Unexpected keyword at top level.");
        token_free(advance(p));
      }
    } 
    else {
      parser_error(p, "Unexpected token at top level.");
      token_free(advance(p));
    }
  }

  return program;
}

void parser_destroy(Parser *p) {
  if (p) {
    token_free(p->current);
    lexer_destroy(p->lexer);
    free(p);
  }
}

// ============================================================================
// CODE GENERATOR
// ============================================================================

static FILE *output_file;
static const TypeTable *codegen_type_table;
static void emit_statement(ASTNode *node);
static void emit_node(ASTNode *node);
static void emit_typedef(ASTNode *node);
static void emit_function(ASTNode *node);
static void emit_var_decl(ASTNode *node);
FuncDecl *collect_functions(ASTNode *node, FuncDecl *list);
void emit_forward_declarations(FuncDecl *decls, FILE *out);
void free_func_decls(FuncDecl *decls);

static void emit_statement(ASTNode *node) {
  if (node->type == AST_BLOCK || node->type == AST_IF ||
      node->type == AST_WHILE || node->type == AST_FOR ||
      node->type == AST_SWITCH) {
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

static void emit_function(ASTNode *node) {
  const char *return_type = get_c_type(&node->suffix_info);
  fprintf(output_file, "%s %s(", return_type, node->value);

  if (node->child_count > 0) {
    ASTNode *params_node = node->children[0];
    for (int i = 0; i < params_node->child_count; i++) {
      if (i > 0)
        fprintf(output_file, ", ");
      ASTNode *param = params_node->children[i];

      // Context-aware: arrays in parameters become pointers
      if (param->suffix_info.type == TYPE_ARRAY) {
        const char *base_type = "void";
        switch (param->suffix_info.array_base_type) {
        case TYPE_INT:
          base_type = "int";
          break;
        case TYPE_FLOAT:
          base_type = "float";
          break;
        case TYPE_CHAR:
          base_type = "char";
          break;
        case TYPE_USER:
          if (param->suffix_info.array_user_type_name) {
            base_type = param->suffix_info.array_user_type_name;
          }
          break;
        default:
          break;
        }
        fprintf(output_file, "%s* %s", base_type, param->value);
      } else {
        const char *param_type = get_c_type(&param->suffix_info);
        fprintf(output_file, "%s %s", param_type, param->value);
      }
    }
  }
  fprintf(output_file, ") ");
  if (node->child_count > 1) {
    emit_node(node->children[1]);
  } else {
    fprintf(output_file, "{}\n");
  }
}

static void emit_var_decl(ASTNode *node) {
  const char *c_type = get_c_type(&node->suffix_info);
  fprintf(output_file, "%s %s", c_type, node->value);

// Handle arrays properly
if (node->suffix_info.type == TYPE_ARRAY) {
    fprintf(output_file, "[");
    
    // For char arrays initialized with string literals, leave size empty
    if (node->suffix_info.array_base_type == TYPE_CHAR && 
        node->child_count > 0 && 
        node->children[0]->type == AST_STRING) {
        // Don't emit size - let C infer from string literal
    } else if (node->child_count > 0 && node->children[0]) {
        emit_node(node->children[0]); // Array size
    }
    
    fprintf(output_file, "]");

    // Array initializer (could be child 0 or 1 depending on whether size was specified)
    if (node->child_count > 0) {
        ASTNode *initializer = NULL;
        
        // Find the initializer (it's the string or initializer list)
        for (int i = 0; i < node->child_count; i++) {
            if (node->children[i]->type == AST_STRING || 
                node->children[i]->type == AST_INITIALIZER_LIST) {
                initializer = node->children[i];
                break;
            }
        }
        
        if (initializer) {
            fprintf(output_file, " = ");
            if (initializer->type == AST_STRING) {
                fprintf(output_file, "\"%s\"", initializer->value);
            } else {
                emit_node(initializer);
            }
        }
    }
    } else {
        // Regular variable initializer is child 0
        if (node->child_count > 0) {
            fprintf(output_file, " = ");
            emit_node(node->children[0]);
        }
  }
}

static void emit_typedef(ASTNode *node) {
  if (node->child_count < 1)
    return;

  ASTNode *original_type = node->children[0];
  const char *original_c_type = get_c_type(&original_type->suffix_info);

  fprintf(output_file, "typedef %s %s;\n", original_c_type, node->value);
}

static void emit_node(ASTNode *node) {
  if (!node)
    return;

  switch (node->type) {
    // Update the AST_PROGRAM case to emit in correct order:
    case AST_PROGRAM:
      // 1. Emit directives (includes)
      for (int i = 0; i < node->child_count; i++) {
        if (node->children[i]->type == AST_DIRECTIVE) {
          emit_node(node->children[i]);
        }
      }
      fprintf(output_file, "\n");
      
      // 2. Emit typedefs, enums, and structs (type definitions)
      for (int i = 0; i < node->child_count; i++) {
        if (node->children[i]->type == AST_STRUCT_DEF ||
            node->children[i]->type == AST_ENUM_DEF ||
            node->children[i]->type == AST_TYPEDEF ||
            node->children[i]->type == AST_PASSTHROUGH) {
          emit_node(node->children[i]);
          fprintf(output_file, "\n");
        }
      }
      for (int i = 0; i < node->child_count; i++) {
        if (node->children[i]->type == AST_VAR_DECL) {
            emit_node(node->children[i]);
            fprintf(output_file, ";\n");
        }
      }
      // 3. NOW emit forward declarations (after types are defined)
      FuncDecl *funcs = collect_functions(node, NULL);
      if (funcs) {
        emit_forward_declarations(funcs, output_file);
        free_func_decls(funcs);
      }
      
      // 4. Finally emit function implementations
      for (int i = 0; i < node->child_count; i++) {
        if (node->children[i]->type == AST_FUNCTION) {
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
    fprintf(output_file, ");");
    break;

  case AST_FOR:
    fprintf(output_file, "for (");
    if (node->children[0])
      emit_node(node->children[0]);
    fprintf(output_file, "; ");
    if (node->children[1])
      emit_node(node->children[1]);
    fprintf(output_file, "; ");
    if (node->children[2])
      emit_node(node->children[2]);
    fprintf(output_file, ") ");
    if (node->children[3])
      emit_node(node->children[3]);
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

  case AST_BREAK:
    fprintf(output_file, "break");
    break;

  case AST_CONTINUE:
    fprintf(output_file, "continue");
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
    fprintf(output_file, "%s", node->value);
    emit_node(node->children[0]);

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
    if (node->child_count < 1)
      break; // Should not happen

    // Child 0 is the callee (the function name or expression)
    emit_node(node->children[0]);

    fprintf(output_file, "(");

    // Children 1 to N are the arguments
    for (int i = 1; i < node->child_count; i++) {
      if (i > 1)
        fprintf(output_file, ", ");
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
    if (node->child_count > 0) {
      ASTNode *child = node->children[0];
      // Special case: if base name is "let", it's a type specifier not a
      // variable
      if (strcmp(child->value, "let") == 0 &&
          child->suffix_info.type != TYPE_VOID) {
        fprintf(output_file, "%s", get_c_type(&child->suffix_info));
      } else {
        // It's a real variable or struct name - emit it directly
        const char *type_name = type_table_lookup(codegen_type_table, child->value);
        fprintf(output_file, "%s", type_name ? type_name : child->value);
      }
    }
    fprintf(output_file, ")");
    break;

  case AST_STRUCT_DEF:
    // Check for forward declaration (no children)
    if (node->child_count == 0) {
      fprintf(output_file, "struct %s;", node->value);
      break;
    }

    fprintf(output_file, "typedef struct %s %s;\n", node->value, node->value);
    fprintf(output_file, "struct %s {\n", node->value);
    for (int i = 0; i < node->child_count; i++) {
      ASTNode *member = node->children[i];
      if (member->type == AST_VAR_DECL) {
        // --- FINAL, CORRECT LOGIC FOR STRUCT MEMBERS ---
        // 1. Print indentation, type, and name
        fprintf(output_file, "    %s %s", get_c_type(&member->suffix_info),
                member->value);

        // 2. If there's a child node, it MUST be the array size.
        //    (No initializers are allowed here).
        if (member->child_count > 0) {
          fprintf(output_file, "[");
          emit_node(member->children[0]); // Emit the size expression
          fprintf(output_file, "]");
        }

        // 3. End the declaration
        fprintf(output_file, ";\n");

      } else if (member->type == AST_FUNC_PTR_DECL) {
        // Function pointers are handled correctly by the main emitter
        emit_node(member);
      }
    }
    fprintf(output_file, "};");
    break;
  case AST_FUNC_PTR_DECL:
    if (node->child_count < 1)
      break; // Invalid signature

    // Child 0 is the return type
    const char *return_type = get_c_type(&node->children[0]->suffix_info);
    fprintf(output_file, "%s (*%s)(", return_type, node->value);

    // Children 1..N are the parameter types
    for (int i = 1; i < node->child_count; i++) {
      if (i > 1)
        fprintf(output_file, ", ");
      const char *param_type = get_c_type(&node->children[i]->suffix_info);
      fprintf(output_file, "%s", param_type);
    }
    // Handle case of no parameters (e.g. func(void))
    if (node->child_count == 1) {
      if (node->children[0]->suffix_info.type != TYPE_VOID) {
        fprintf(output_file, "void");
      }
    }
    fprintf(output_file, ");\n");
    break;

  case AST_TYPEDEF:
    emit_typedef(node);
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

  case AST_EXPRESSION:
    if (node->child_count > 0)
      emit_node(node->children[0]);
    break;

  case AST_PASSTHROUGH:
    fprintf(output_file, "%s", node->value);
    break;

  case AST_NULL:
    fprintf(output_file, "NULL");
    break;

  case AST_CAST:
    fprintf(output_file, "(");
    fprintf(output_file, "%s", get_c_type(&node->suffix_info));
    fprintf(output_file, ")");
    emit_node(node->children[0]);
    break;

  case AST_ENUM_DEF:
    fprintf(output_file, "typedef enum %s {\n", node->value);
    for (int i = 0; i < node->child_count; i++) {
      ASTNode *member = node->children[i];
      if (member->type == AST_ENUM_VALUE) {
        fprintf(output_file, "    %s", member->value);
        // If there's an explicit value
        if (member->child_count > 0) {
          fprintf(output_file, " = ");
          emit_node(member->children[0]);
        }
        // Add comma except for last item
        if (i < node->child_count - 1) {
          fprintf(output_file, ",");
        }
        fprintf(output_file, "\n");
      }
    }
    fprintf(output_file, "} %s;", node->value);
    break;
  case AST_POSTFIX_OP:
    emit_node(node->children[0]);  // Emit the operand
    fprintf(output_file, "%s", node->value);  // Emit ++ or --
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

// Collect all function declarations from the AST
FuncDecl *collect_functions(ASTNode *node, FuncDecl *list) {
  if (!node) return list;
  
  if (node->type == AST_FUNCTION) {
    FuncDecl *decl = malloc(sizeof(FuncDecl));
    decl->name = clone_string(node->value);
    decl->return_type = node->suffix_info;
    decl->params = node->child_count > 0 ? node->children[0] : NULL;
    decl->next = list;
    return decl;
  }
  
  // Recurse through children
  for (int i = 0; i < node->child_count; i++) {
    list = collect_functions(node->children[i], list);
  }
  return list;
}

// Emit forward declarations
void emit_forward_declarations(FuncDecl *decls, FILE *out) {
  fprintf(out, "// Forward declarations\n");
  
  for (FuncDecl *d = decls; d; d = d->next) {
    const char *return_type = get_c_type(&d->return_type);
    fprintf(out, "%s %s(", return_type, d->name);
    
    if (d->params && d->params->child_count > 0) {
      for (int i = 0; i < d->params->child_count; i++) {
        if (i > 0) fprintf(out, ", ");
        ASTNode *param = d->params->children[i];
        
        // Handle array parameters as pointers
        if (param->suffix_info.type == TYPE_ARRAY) {
          const char *base_type = "void";
          switch (param->suffix_info.array_base_type) {
            case TYPE_INT: base_type = "int"; break;
            case TYPE_FLOAT: base_type = "float"; break;
            case TYPE_CHAR: base_type = "char"; break;
            case TYPE_USER:
              if (param->suffix_info.array_user_type_name) {
                base_type = param->suffix_info.array_user_type_name;
              }
              break;
            default: break;
          }
          fprintf(out, "%s* %s", base_type, param->value);
        } else {
          const char *param_type = get_c_type(&param->suffix_info);
          fprintf(out, "%s %s", param_type, param->value);
        }
      }
    }
    fprintf(out, ");\n");
  }
  fprintf(out, "\n");
}

void free_func_decls(FuncDecl *decls) {
  while (decls) {
    FuncDecl *next = decls->next;
    free(decls->name);
    free(decls);
    decls = next;
  }
}
void codegen(ASTNode *ast, const TypeTable *table, FILE *out) {
  output_file = out;
  codegen_type_table = table;
  emit_node(ast);
}

static void pre_scan_for_types(const char *source, TypeTable *table) {
  const char *cursor = source;

  // Scan for structs first
  while ((cursor = strstr(cursor, "struct"))) {
    cursor += strlen("struct");
    while (*cursor && isspace(*cursor))
      cursor++;

    const char *name_start = cursor;
    while (*cursor && (isalnum(*cursor) || *cursor == '_')) {
      cursor++;
    }

    if (cursor > name_start) {
      size_t name_len = cursor - name_start;
      while (*cursor && isspace(*cursor))
        cursor++;

      if (*cursor == '{') {
        char type_name[128];
        if (name_len < sizeof(type_name)) {
          strncpy(type_name, name_start, name_len);
          type_name[name_len] = '\0';
          type_table_add(table, type_name);
        }
      }
    }
  }
  
  cursor = source;
  while ((cursor = strstr(cursor, "enum"))) {
    cursor += strlen("enum");
    while (*cursor && isspace(*cursor))
      cursor++;

    const char *name_start = cursor;
    while (*cursor && (isalnum(*cursor) || *cursor == '_')) {
      cursor++;
    }

    if (cursor > name_start) {
      size_t name_len = cursor - name_start;
      while (*cursor && isspace(*cursor))
        cursor++;

      if (*cursor == '{') {
        char type_name[128];
        if (name_len < sizeof(type_name)) {
          strncpy(type_name, name_start, name_len);
          type_name[name_len] = '\0';
          type_table_add_enum(table, type_name);
        }
      }
    }
  }
}

static char *read_file(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f)
    return NULL;

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *buffer = malloc(size + 1);
  if (!buffer) {
    fclose(f);
    return NULL;
  }

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

  char *source = read_file(argv[1]);
  if (!source) {
    fprintf(stderr, "Error: Cannot read file '%s'\n", argv[1]);
    return 1;
  }

  TypeTable *type_table = type_table_create();
  pre_scan_for_types(source, type_table);

  Parser *parser = parser_create(source, type_table);
  ASTNode *ast = parser_parse(parser);

  if (parser->had_error) {
    fprintf(stderr, "Compilation failed.\n");
    ast_destroy(ast);
    parser_destroy(parser);
    type_table_destroy(type_table);
    free(source);
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
    ast_destroy(ast);
    parser_destroy(parser);
    type_table_destroy(type_table);
    free(source);
    return 1;
  }

  codegen(ast, type_table, out);
  fclose(out);

  printf("Successfully compiled '%s' to '%s'\n", argv[1], outname);

  ast_destroy(ast);
  parser_destroy(parser);
  type_table_destroy(type_table);
  free(source);

  return 0;
}

