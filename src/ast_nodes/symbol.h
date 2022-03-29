#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_symbol(preproc_state* state, ast_node* node)
{
    symbol* sym = node->value.symbol;

    int index_in_parent = ast_get_child_index(node->parent, node);

    bool is_function_call_assignment = node->parent->type == AstFunctionCall && node->parent->parent->type == AstAssignment;
    bool is_being_assigned_to = (
           (node->parent->type == AstAlloc && index_in_parent == 0) 
        || (node->parent->type == AstAssignment && index_in_parent == 0)
        ||  node->parent->type == AstDefinition 
        ||  node->parent->type == AstInput
        );

    if ((!is_being_assigned_to || node->parent->type == AstFree) && !sym->is_initialised)
    {
        preproc_error(state, node, "%s is not initialized\n", sym->name);
    }
}