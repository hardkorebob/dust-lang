// ===== component_system.h =====
#ifndef COMPONENT_SYSTEM_H
#define COMPONENT_SYSTEM_H

#include <stdbool.h>
#include "type_table.h" 

typedef enum {
    TYPE_VOID,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_POINTER,
    TYPE_ARRAY,
    TYPE_BOOL,
    TYPE_USER
} DataType;

typedef enum {
    ROLE_OWNED,
    ROLE_BORROWED,
    ROLE_REFERENCE,
    ROLE_NONE
} SemanticRole;

typedef struct {
    DataType type;
    SemanticRole role;
    bool is_pointer;
    bool is_const;
    const char* user_type_name;
    bool is_pointer_to_pointer;
    DataType array_base_type;
    const char* array_user_type_name;
} SuffixInfo;

bool suffix_parse(const char* full_variable_name, const TypeTable* type_table, SuffixInfo* result_info);
const char* get_c_type(const SuffixInfo* info);
const char* find_suffix_separator(const char* name);

#endif

