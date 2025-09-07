#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "type_table.h"
#include "parser.h"
#include "codegen.h"

// A simple pre-scanner to find struct definitions and populate the type table.
// In a real multi-pass compiler, this would be more robust.

static void pre_scan_for_types(const char* source, TypeTable* table) {
    const char* cursor = source;
    
    while ((cursor = strstr(cursor, "struct"))) {
        cursor += strlen("struct");
        
        while (*cursor && isspace(*cursor)) cursor++;
        
        const char* name_start = cursor;
        while (*cursor && (isalnum(*cursor) || *cursor == '_')) {
            cursor++;
        }
        
        if (cursor > name_start) {
            size_t name_len = cursor - name_start;
            
            while (*cursor && isspace(*cursor)) cursor++;
            
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
}

static char* read_file(const char* path) {
    FILE* f = fopen(path, "rb");
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
    
    TypeTable* type_table = type_table_create();
    pre_scan_for_types(source, type_table);

    Parser* parser = parser_create(source, type_table);
    ASTNode* ast = parser_parse(parser);
    
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
    char* dot = strrchr(outname, '.');
    if (dot) {
        strcpy(dot, ".c");
    } else {
        strcat(outname, ".c");
    }
    
    FILE* out = fopen(outname, "w");
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

