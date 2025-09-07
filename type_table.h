//type_table.h
#ifndef TYPE_TABLE_H
#define TYPE_TABLE_H

#include <stdbool.h>
#include <stddef.h>

// A simple dynamic array to store type names (e.g., "UserProfile").
typedef struct {
    char** names;
    size_t count;
    size_t capacity;
} TypeTable;

// Function prototypes for managing the table.
TypeTable* type_table_create(void);
void type_table_destroy(TypeTable* table);
bool type_table_add(TypeTable* table, const char* type_name);
const char* type_table_lookup(const TypeTable* table, const char* type_name);

#endif // TYPE_TABLE_H
