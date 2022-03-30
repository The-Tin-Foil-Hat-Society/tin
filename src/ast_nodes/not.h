#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_not(preproc_state* state, ast_node* node)
{
    data_type* child_dtype = ast_find_data_type(ast_get_child(node, 0));

    if (!is_bool(child_dtype))
    {
        preproc_error(state, node, "can only perform NOT (!) on bools\n");
    }

    node->value.dtype = data_type_new();
    node->value.dtype->name = strdup("bool");
}