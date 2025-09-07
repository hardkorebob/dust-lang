#include <string.h>
#include <stdlib.h>

char* clone_string(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str);
    char* new_str = malloc(len + 1);
    if (!new_str) return NULL; 
    memcpy(new_str, str, len);
    new_str[len] = '\0';
    return new_str;
}
