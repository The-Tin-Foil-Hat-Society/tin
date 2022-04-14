#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"

void preprocess_root(preproc_state* state, ast_node* node)
{
    char* symbol_key = symbol_generate_key("main", state->mod);
    if (state->mod->parent == 0 && ast_find_symbol(node, symbol_key) == 0)
    {
        preproc_error(state, node, "no main function defined\n"); 
    }
    free(symbol_key);
}