#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"
#include <string.h>

void preprocess_include(preproc_state* state, ast_node* node)
{
    // TODO : implement general file handling utils
    char* filename = node->value.string;
    char* filename_copy = strdup(filename);
    char* name;
    char* s;

    // get rid of extension
    s = strrchr(filename_copy, '.');
    if (s != 0)
    {
        s[0] = '\0';
    }

    // get name without the directory, if it is present
    s = strrchr(filename_copy, '\\');
    if (s == 0)
    {
        name = strdup(filename_copy);
    }
    else
    {
        name = strdup(s + 1);
    }

    module* dependency = module_find_dependency(state->mod, name);
    if (dependency == 0)
    {
        // if the dependency doesn't exist in the program, parse it 
        dependency = module_new();
        dependency->parent = state->mod;
        dependency->name = name;

        if (module_parse(dependency, filename) != 0)
        {
            preproc_error(state, node, "could not parse %s\n", filename);
            free(dependency);
            dependency = 0;
        }
    }
    
    if (dependency != 0)
    {
        module_add_dependency(state->mod, dependency);
    }

    free(filename_copy);
    free(name);
}