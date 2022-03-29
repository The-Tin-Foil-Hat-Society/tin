#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_free(preproc_state* state, ast_node* node)
{
    ast_node* ptr_node = ast_get_child(node, 0);

    data_type* found_dtype = ast_find_data_type(ptr_node);
    if (found_dtype == 0)
    {
        // this should not happen
        preproc_error(state, node, "%scould not determine the data type of the right hand value (preprocessor bug)\n", "");
    }

    if (found_dtype->pointer_level == 0)
    {
        preproc_error(state, node, "%smust be a pointer\n", "");
    }
}