#pragma once

#include "ast.h"
#include "symbol.h"

typedef struct 
{
    ast_node* ast_root;
    char* src_code; // for debugging
} module;

module* module_new(void);
void module_free(module* mod);

void module_set_src_file(module* mod, FILE* file);
char* module_get_src_line(module* mod, int lineno);