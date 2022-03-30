#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_logical_expression(preproc_state* state, ast_node* node)
{
    data_type* left_dtype = ast_find_data_type(ast_get_child(node, 0));
    data_type* right_dtype = ast_find_data_type(ast_get_child(node, 1));

    if (!is_bool(left_dtype) || !is_bool(right_dtype))
    {
        preproc_error(state, node, "both sides of the logical expression must be bools\n");
    }

    node->value.dtype = data_type_new("bool");
}