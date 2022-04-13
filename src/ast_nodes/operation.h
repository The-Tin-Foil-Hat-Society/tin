#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_operation(preproc_state* state, ast_node* node)
{
    data_type* left_dtype = ast_find_data_type(ast_get_child(node, 0));
    data_type* right_dtype = ast_find_data_type(ast_get_child(node, 1));

    if (is_string(left_dtype) || is_string(right_dtype))
    {
        preproc_error(state, node, "cannot perform operations on strings\n");
    }

    if (is_int(left_dtype) && is_int(right_dtype) && !data_type_compare(left_dtype, right_dtype))
    {
        preproc_verb(state, node, "implicit integer conversion\n");
    }
    else if (is_float(left_dtype) && is_float(right_dtype) && !data_type_compare(left_dtype, right_dtype))
    {
        preproc_verb(state, node, "implicit float conversion\n");
    }
    else if (!data_type_compare(left_dtype, right_dtype) && (left_dtype->pointer_level ^ right_dtype->pointer_level))
    {
        preproc_verb(state, node, "implicit pointer conversion\n");
    }
    else if (!data_type_compare(left_dtype, right_dtype))
    {
        preproc_error(state, node, "operation: the left hand data type does not match the right hand data type\n");
    }

    node->value.dtype = data_type_copy(left_dtype); // copy the data type to make validation easier for the upper nodes
}