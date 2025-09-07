#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include <stdio.h>
#include <ctype.h>

void codegen(ASTNode* ast, const TypeTable* table, FILE* out);
#endif


