#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_if(preproc_state* state, ast_node* node)
{
    data_type* condition_dtype = ast_find_data_type(ast_get_child(node, 0));
    if (condition_dtype == 0 || !is_bool(condition_dtype))
    {
        preproc_error(state, node, "%sif condition is not valid\n", "");
    }
}