#pragma once

#include "ast.h"
#include "hashtable.h"
#include "symbol.h"

typedef struct module module;
struct module 
{
    module* parent;
    char* name;
    ast_node* ast_root;
    hashtable* dependencies;
    char* src_code; // for debugging
};

module* module_new(void);
void module_free(module* mod);

int module_parse(module* mod, char* filename);

void module_add_dependency(module* mod, module* dependency);
module* module_find_dependency(module* mod, char* name);

void module_set_src_file(module* mod, FILE* file);
char* module_get_src_line(module* mod, int lineno);