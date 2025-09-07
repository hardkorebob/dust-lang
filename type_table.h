#ifndef TYPE_TABLE_H
#define TYPE_TABLE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char** names;
    size_t count;
    size_t capacity;
} TypeTable;

TypeTable* type_table_create(void);
void type_table_destroy(TypeTable* table);
bool type_table_add(TypeTable* table, const char* type_name);
const char* type_table_lookup(const TypeTable* table, const char* type_name);

#endif 
