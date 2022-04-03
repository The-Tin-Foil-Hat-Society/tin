#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"
#include <string.h>

void preprocess_include(preproc_state* state, ast_node* node)
{
    // TODO : implement general file handling utils
    char* filename_copy = strdup(node->value.string);
    char* name;
    char* s;

    // get rid of extension
    s = strrchr(filename_copy, '.');
    if (s != 0)
    {
        s[0] = '\0';
    }

    // get name without the directory, if it is present
    s = strrchr(filename_copy, '/');
    if (s == 0)
    {
        name = strdup(filename_copy);
    }
    else
    {
        name = strdup(s + 1);
    }
    free(filename_copy);

    module* dependency = module_find_dependency(state->mod, name);
    free(name);

    if (dependency == 0)
    {
        // if the dependency doesn't exist in the program, parse it 
        dependency = module_parse(node->value.string, state->mod);
        if (dependency == 0)
        {
            preproc_error(state, node, "%scould not parse include\n", "");
            free(dependency);
            dependency = 0;
        }
    }
    
    if (dependency != 0)
    {
        module_add_dependency(state->mod, dependency);
    }

    ast_delete_child(node->parent, node);
}