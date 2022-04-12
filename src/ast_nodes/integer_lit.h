#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_integer_lit(preproc_state* state, ast_node* node)
{
    if (node->parent->type == AstAlloc)
    {
        if (node->value.integer < 0)
        {
            preproc_error(state, node, "cannot allocate less than 0 bytes\n");
        }
        else if (node->value.integer > UINT64_MAX)
        {
            preproc_error(state, node, "cannot allocate more than u64 max bytes\n");
        }  
    }
    else if (node->parent->type == AstAssignment)
    {
        symbol* left_sym = ast_get_child(node->parent, 0)->value.symbol;
        if(!is_valid_int(left_sym->dtype, node->value.integer))
        {
            preproc_error(state, node, "value does not fit the data type of variable %s\n", left_sym->name);
        }
    }
}