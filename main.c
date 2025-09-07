// ===== main.c (Final Version) =====
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "type_table.h" // NEW: We need to manage the type table
#include "parser.h"
#include "codegen.h"

// A simple pre-scanner to find struct definitions and populate the type table.
// In a real multi-pass compiler, this would be more robust.

static void pre_scan_for_types(const char* source, TypeTable* table) {
    const char* cursor = source;
    
    while ((cursor = strstr(cursor, "struct"))) {
        cursor += strlen("struct");
        
        // Skip whitespace
        while (*cursor && isspace(*cursor)) cursor++;
        
        // Check if this is a struct definition (not just usage)
        const char* name_start = cursor;
        while (*cursor && (isalnum(*cursor) || *cursor == '_')) {
            cursor++;
        }
        
        if (cursor > name_start) {
            size_t name_len = cursor - name_start;
            
            // Skip whitespace after name
            while (*cursor && isspace(*cursor)) cursor++;
            
            // Only add if followed by '{' (actual definition)
            if (*cursor == '{') {
                char type_name[128];
                if (name_len < sizeof(type_name)) {
                    strncpy(type_name, name_start, name_len);
                    type_name[name_len] = '\0';
                    
                    // Add the type to the table
                    type_table_add(table, type_name);
                    //printf("Debug: Found struct type: %s\n", type_name);  // Debug output
                }
            }
        }
    }
}

static char* read_file(const char* path) {
    FILE* f = fopen(path, "rb"); // Use "rb" for binary-safe reading
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* buffer = malloc(size + 1);
    if (!buffer) { fclose(f); return NULL; }
    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    
    fclose(f);
    return buffer;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: dustc <file.dust>\n");
        return 1;
    }
    
    char* source = read_file(argv[1]);
    if (!source) {
        fprintf(stderr, "Error: Cannot read file '%s'\n", argv[1]);
        return 1;
    }
    
    // NEW: Create and populate the Type Table.
    TypeTable* type_table = type_table_create();
    pre_scan_for_types(source, type_table);

    // FIXED: Pass the populated type table to the parser.
    Parser* parser = parser_create(source, type_table);
    ASTNode* ast = parser_parse(parser);
    
    // Check if the parser encountered errors.
    if (parser->had_error) {
        fprintf(stderr, "Compilation failed.\n");
        // Cleanup and exit
        ast_destroy(ast);
        parser_destroy(parser);
        type_table_destroy(type_table);
        free(source);
        return 1;
    }

    // Generate output filename (e.g., "input.dust" -> "input.c")
    char outname[256];
    strncpy(outname, argv[1], sizeof(outname) - 3);
    outname[sizeof(outname) - 3] = '\0'; // Prevent buffer overflow
    char* dot = strrchr(outname, '.');
    if (dot) {
        strcpy(dot, ".c");
    } else {
        strcat(outname, ".c");
    }
    
    FILE* out = fopen(outname, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file '%s'\n", outname);
        // Cleanup and exit
        ast_destroy(ast);
        parser_destroy(parser);
        type_table_destroy(type_table);
        free(source);
        return 1;
    }
    
    // Run the code generator to write the final C code.
    codegen(ast, type_table, out);
    fclose(out);
    
    printf("Successfully compiled '%s' to '%s'\n", argv[1], outname);
    
    // Final cleanup of all allocated memory.
    ast_destroy(ast);
    parser_destroy(parser);
    type_table_destroy(type_table);
    free(source);
    
    return 0;
}

