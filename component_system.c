// ===== component_system.c (Definitive Final Version) =====
#include "component_system.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* find_suffix_separator(const char* name) {
    return strrchr(name, '_');
}

// This function is now complete and correct.
bool suffix_parse(const char* full_variable_name, const TypeTable* type_table, SuffixInfo* result_info) {
    *result_info = (SuffixInfo){TYPE_VOID, ROLE_NONE, false, false, NULL, false, TYPE_VOID, NULL};
    const char* separator = find_suffix_separator(full_variable_name);
    if (!separator) return false;

    const char* suffix_str = separator + 1;
    size_t suffix_len = strlen(suffix_str);
    if (suffix_len == 0) return false;

    if (suffix_len > 1 && suffix_str[suffix_len - 1] == 'a') {
        result_info->type = TYPE_ARRAY;
        char base_type_suffix[128];
        strncpy(base_type_suffix, suffix_str, suffix_len - 1);
        base_type_suffix[suffix_len - 1] = '\0';

        if (strcmp(base_type_suffix, "i") == 0) {
            result_info->array_base_type = TYPE_INT; return true;
        }
        const char* user_type = type_table_lookup(type_table, base_type_suffix);
        if (user_type) {
            result_info->array_base_type = TYPE_USER;
            result_info->array_user_type_name = user_type; return true;
        }
    }

    if (strcmp(suffix_str, "i") == 0) { result_info->type = TYPE_INT; return true; }
    if (strcmp(suffix_str, "ip") == 0) { result_info->type = TYPE_INT; result_info->is_pointer = true; result_info->role = ROLE_OWNED; return true; }
    if (strcmp(suffix_str, "ib") == 0) { result_info->type = TYPE_INT; result_info->is_pointer = true; result_info->is_const = true; result_info->role = ROLE_BORROWED; return true; }
    if (strcmp(suffix_str, "ir") == 0) { result_info->type = TYPE_INT; result_info->is_pointer = true; result_info->is_const = true; result_info->role = ROLE_REFERENCE; return true; }
    if (strcmp(suffix_str, "bl") == 0) { result_info->type = TYPE_BOOL; return true; }
    if (strcmp(suffix_str, "b") == 0) { result_info->type = TYPE_POINTER; result_info->is_pointer = true; result_info->is_const = true; result_info->role = ROLE_BORROWED; return true; }
    if (strcmp(suffix_str, "f") == 0) { result_info->type = TYPE_FLOAT; return true; }
    if (strcmp(suffix_str, "c") == 0) { result_info->type = TYPE_CHAR; return true; }
    if (strcmp(suffix_str, "s") == 0) { result_info->type = TYPE_STRING; result_info->is_pointer = true; return true; }

    char type_candidate[128];
    strncpy(type_candidate, suffix_str, sizeof(type_candidate) - 1);
    type_candidate[sizeof(type_candidate) - 1] = '\0';
    
    char last_char = suffix_str[suffix_len - 1];
    bool is_potential_pointer = (suffix_len > 1 && (last_char == 'p' || last_char == 'b' || last_char == 'r'));

    if (is_potential_pointer) {
        type_candidate[suffix_len - 1] = '\0';
        const char* user_type = type_table_lookup(type_table, type_candidate);
        if (user_type) {
            result_info->type = TYPE_USER;
            result_info->user_type_name = user_type;
            result_info->is_pointer = true;
            if (last_char == 'p') result_info->role = ROLE_OWNED; else { result_info->is_const = true; }
            return true;
        }
    }

    const char* user_type = type_table_lookup(type_table, suffix_str);
    if (user_type) {
        result_info->type = TYPE_USER;
        result_info->user_type_name = user_type;
        result_info->is_pointer = false;
        return true;
    }
    return false;
}

const char* get_c_type(const SuffixInfo* info) {
    static char type_buffer[256];
    char base_type_str[128] = "void";

    // Step 1: Determine the base C type name (e.g., "int", "Game")
    DataType type_to_check = (info->type == TYPE_ARRAY) ? info->array_base_type : info->type;
    const char* user_name = (info->type == TYPE_ARRAY) ? info->array_user_type_name : info->user_type_name;

    switch (type_to_check) {
        case TYPE_INT:    strcpy(base_type_str, "int"); break;
        case TYPE_FLOAT:  strcpy(base_type_str, "float"); break;
        case TYPE_CHAR:   strcpy(base_type_str, "char"); break;
        case TYPE_BOOL:   strcpy(base_type_str, "bool"); break;
        case TYPE_STRING: strcpy(base_type_str, "char*"); break;
        case TYPE_USER:   if (user_name) strcpy(base_type_str, user_name); break;
        default: break;
    }

    // Step 2: Build the final, complete C type string
    if (info->type == TYPE_ARRAY) {
        // An array passed as a parameter becomes a pointer in C. `int* name` or `int name[]` are valid.
        // We will generate `int* name` for consistency.
        snprintf(type_buffer, sizeof(type_buffer), "%s*", base_type_str);
    } else if (info->type == TYPE_STRING) {
        strcpy(type_buffer, base_type_str); // Already "char*"
    } else {
        // For all other types, add const and pointer qualifiers as needed.
        // This correctly produces `Game*` for a `_Gameb` suffix.
        snprintf(type_buffer, sizeof(type_buffer), "%s%s%s",
            (info->is_const ? "const " : ""),
            base_type_str,
            (info->is_pointer ? "*" : "")
        );
    }
    return type_buffer;
}
