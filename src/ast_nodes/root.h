#pragma once

#include "preprocessor.h"
#include "ast.h"

void preprocess_root(preproc_state* state, ast_node* node)
{
    if (ast_find_symbol(node, "main") == 0)
    {
        preproc_error(state, node, "no main function defined\n"); 
    }
}