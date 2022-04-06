#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"
#include "path.h"
#include <string.h>

void preprocess_include(preproc_state* state, ast_node* node)
{
    char* name = path_get_filename_wo_ext(node->value.string);
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