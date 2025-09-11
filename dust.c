#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef struct Arena Arena;
struct Arena {
char* data;
size_t size;
size_t used;
};
typedef enum DataType {
TYPEVOID = 0,
TYPEINT = 1,
TYPEFLOAT = 2,
TYPECHAR = 3,
TYPESTRING = 4,
TYPEPOINTER = 5,
TYPEARRAY = 6,
TYPEUSER = 7,
TYPEFUNC_POINTER = 8,
TYPESIZE_T = 9,
TYPEUINT8 = 10,
TYPEUINT16 = 11,
TYPEUINT32 = 12,
TYPEUINT64 = 13,
TYPEINT8 = 14,
TYPEINT16 = 15,
TYPEINT32 = 16,
TYPEINT64 = 17,
TYPEUINTPTR = 18,
TYPEINTPTR = 19,
TYPEOFF = 20,
TYPEBOOL = 21
} DataType;
typedef enum SemanticRole {
ROLEOWNED = 0,
ROLEBORROWED = 1,
ROLEREFERENCE = 2,
ROLERESTRICT = 3,
ROLENONE = 4
} SemanticRole;
typedef struct SuffixInfo SuffixInfo;
struct SuffixInfo {
DataType type;
DataType arrBasetype;
SemanticRole role;
bool isConst;
bool isStatic;
bool isExtern;
int ptrLevel;
const char* userTypename;
const char* arrUsertypename;
};
typedef struct TypedefInfo TypedefInfo;
struct TypedefInfo {
char* name;
SuffixInfo typeInfo;
};
typedef struct TypeTable TypeTable;
struct TypeTable {
TypedefInfo* typedefs;
char** structNames;
Arena typeArena;
size_t structCount;
size_t structCap;
size_t typedefCount;
size_t typedefCap;
};
