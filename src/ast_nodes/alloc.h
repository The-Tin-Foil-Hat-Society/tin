#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_alloc(preproc_state* state, ast_node* node)
{
    symbol* left_sym = ast_get_child(node, 0)->value.symbol; // 1st child should always be an identifier/symbol
    ast_node* right_node = ast_get_child(node, 1);

    if (left_sym->dtype->pointer_level < 1)
    {
        preproc_error(state, node, "error: %s must have a pointer type\n", left_sym->name);
    }

    data_type* found_dtype = ast_find_data_type(right_node);
    if (found_dtype == 0)
    {
        // this should not happen
        preproc_error(state, node, "%salloc, could not determine the data type of the right hand value (preprocessor bug)\n", "");
    }

    if (!is_int(found_dtype))
    {
        preproc_error(state, node, "alloc size must be an integer type\n");
    }
    
    left_sym->is_initialised = true;
}