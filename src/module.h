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
    hashtable* dependency_store; // stores ALL dependencies in the module tree for redundancy 

    char* filename;
    char* dir;
    char* src_code; // for debugging
};

module* module_new(void);
// creates a new module from the given file, parent can be 0 but is required for include'd files
module* module_parse(char* filename, module* parent);
void module_free(module* mod);

void module_add_dependency(module* mod, module* dependency);
// get the dependency from the specified module
module* module_get_dependency(module* mod, char* name);
// find a dependency in the whole program
module* module_find_dependency(module* mod, char* name);

void module_set_src_file(module* mod, FILE* file);
char* module_get_src_line(module* mod, int lineno);

// prints a json representation of the module to the given file (can be stdout)
void module_print_to_file(module* mod, FILE* file);