#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_return(preproc_state* state, ast_node* node)
{
    symbol* function = ast_get_child(ast_get_current_function(node), 0)->value.symbol;

    if (node->children->size == 0)
    {
        if (strcmp(function->dtype->name, "void") != 0 || function->dtype->pointer_level > 0)
        {
            preproc_error(state, node, "%s requires a return of type %s\n", function->name, function->dtype->name);
        }
    }
    else
    {
        data_type* return_dtype = ast_find_data_type(ast_get_child(node, 0));

        if (is_int(function->dtype) && is_int(return_dtype) && !data_type_compare(function->dtype, return_dtype))
        {
            preproc_verb(state, node, "%s, implicit integer conversion\n", function->name);
        }
        else if (is_float(function->dtype) && is_float(return_dtype) && !data_type_compare(function->dtype, return_dtype))
        {
            preproc_verb(state, node, "%s, implicit float conversion\n", function->name);
        }
        else if (!data_type_compare(function->dtype, return_dtype))
        {
            preproc_error(state, node, "%s has type %s while the return value has type %s\n", function->name, function->dtype->name, return_dtype->name);
        }
        if (function->dtype->pointer_level != return_dtype->pointer_level)
        {
            preproc_warn(state, node, "%s is a level %ld pointer while the return value is a level %ld pointer\n", function->name, function->dtype->pointer_level, return_dtype->pointer_level);
        }
    }
}