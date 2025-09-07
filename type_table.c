#include "type_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"

TypeTable* type_table_create(void) {
    TypeTable* table = malloc(sizeof(TypeTable));
    table->capacity = 8; // Initial capacity
    table->count = 0;
    table->names = malloc(sizeof(char*) * table->capacity);
    return table;
}

void type_table_destroy(TypeTable* table) {
    for (size_t i = 0; i < table->count; i++) {
        free(table->names[i]); // Free each duplicated string
    }
    free(table->names);
    free(table);
}

bool type_table_add(TypeTable* table, const char* type_name) {
    if (type_table_lookup(table, type_name)) {
        return true;
    }
    if (table->count >= table->capacity) {
        table->capacity *= 2;
        table->names = realloc(table->names, sizeof(char*) * table->capacity);
    }
    // FIXED: Use our standard-compliant clone_string instead of strdup
    table->names[table->count++] = clone_string(type_name);
    return true;
}
// Returns the stored type name on success, NULL on failure.
const char* type_table_lookup(const TypeTable* table, const char* type_name) {
    for (size_t i = 0; i < table->count; i++) {
        if (strcmp(table->names[i], type_name) == 0) {
            return table->names[i];
        }
    }
    return NULL;
}
