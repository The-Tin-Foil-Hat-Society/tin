#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_string_lit(preproc_state* state, ast_node* node)
{
    if (node->parent->type == AstAlloc)
    {
        preproc_error(state, node, "alloc size must be an integer\n");
    }
    else if (node->parent->type == AstAssignment)
    {
        symbol* left_sym = ast_get_child(node->parent, 0)->value.symbol;
        if (!is_string(left_sym->dtype))  // must be (ptr u8)
        {
            preproc_error(state, node, "%s must be of type ptr u8\n", left_sym->name);
        }
    }
}