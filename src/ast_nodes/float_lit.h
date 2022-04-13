#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_float_lit(preproc_state* state, ast_node* node)
{
    if (node->parent->type == AstAssignment)
    {
        symbol* left_sym = ast_get_child(node->parent, 0)->value.symbol;
        if(!is_valid_float(left_sym->dtype, node->value.floating))
        {
            preproc_error(state, node, "value does not fit the data type of variable %s\n", left_sym->name);
        }
    }
}