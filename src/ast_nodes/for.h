#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_for(preproc_state* state, ast_node* node)
{
    // convert the for loop to a while loop
    node->type = AstWhile;

    data_type* condition_dtype = ast_find_data_type(ast_get_child(node, 1));
    if (condition_dtype == 0 || !is_bool(condition_dtype))
    {
        preproc_error(state, node, "%sfor condition is not valid\n", "");
    }

    // insert the initialisation to before the loop
    ast_node* init_node = ast_get_child(node, 0);
    ast_insert_child(node->parent, ast_get_child_index(node->parent, node), ast_copy(init_node));

    // add the assignment to the end of the scope
    ast_node* assignment_node = ast_get_child(node, 2);
    ast_add_child(ast_get_child(node, 3), ast_copy(assignment_node));

    // delete the original nodes
    ast_delete_child(node, init_node);
    ast_delete_child(node, assignment_node);

    state->index_offset = 1; // skip once to avoid double parsing this once, since we added a new node beforehand
}