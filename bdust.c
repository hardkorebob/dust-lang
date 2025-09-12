#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef enum DataType {
TYPEVOID = 0,
TYPEINT = 1,
TYPEFLOAT = 2,
TYPECHAR = 3,
TYPESTRING = 4,
TYPEPOINTER = 5,
TYPEARRAY = 6,
TYPEUSER = 7,
TYPEFPTR = 8,
TYPESIZET = 9,
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
typedef struct Arena Arena;
struct Arena {
char* data;
size_t size;
size_t used;
};
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
typedef struct SuffixMapping SuffixMapping;
struct SuffixMapping {
const char* suffix;
bool isPtr;
bool isConst;
DataType type;
SemanticRole role;
};
typedef struct TypeMapping TypeMapping;
struct TypeMapping {
DataType type;
const char* cType;
};
static const TypeMapping typeMap[] = { { TYPEVOID, "void" }, { TYPEINT, "int" }, { TYPEFLOAT, "float" }, { TYPECHAR, "char" }, { TYPESTRING, "char*" }, { TYPESIZET, "size_t" }, { TYPEUINT8, "uint8_t" }, { TYPEUINT16, "uint16_t" }, { TYPEUINT32, "uint32_t" }, { TYPEUINT64, "uint64_t" }, { TYPEINT8, "int8_t" }, { TYPEINT16, "int16_t" }, { TYPEINT32, "int32_t" }, { TYPEINT64, "int64_t" }, { TYPEUINTPTR, "uintptr_t" }, { TYPEINTPTR, "intptr_t" }, { TYPEOFF, "off_t" }, { TYPEBOOL, "bool" }, { TYPEVOID, NULL } };
static const SuffixMapping suffixTable[] = { { "i", TYPE_INT, ROLE_NONE, false, false }, { "f", TYPE_FLOAT, ROLE_NONE, false, false }, { "c", TYPE_CHAR, ROLE_NONE, false, false }, { "s", TYPE_STRING, ROLE_NONE, false, false }, { "v", TYPE_VOID, ROLE_NONE, false, false }, { "bl", TYPE_BOOL, ROLE_NONE, false, false }, { "t", TYPE_SIZE_T, ROLE_NONE, false, false }, { "fp", TYPE_FUNC_POINTER, ROLE_NONE, false, false }, { "u8", TYPE_UINT8, ROLE_NONE, false, false }, { "u16", TYPE_UINT16, ROLE_NONE, false, false }, { "u32", TYPE_UINT32, ROLE_NONE, false, false }, { "u64", TYPE_UINT64, ROLE_NONE, false, false }, { "i8", TYPE_INT8, ROLE_NONE, false, false }, { "i16", TYPE_INT16, ROLE_NONE, false, false }, { "i32", TYPE_INT32, ROLE_NONE, false, false }, { "i64", TYPE_INT64, ROLE_NONE, false, false }, { "ux", TYPE_UINTPTR, ROLE_NONE, false, false }, { "ix", TYPE_INTPTR, ROLE_NONE, false, false }, { "off", TYPE_OFF, ROLE_NONE, false, false }, { NULL, TYPE_VOID, ROLE_NONE, false, false } };
