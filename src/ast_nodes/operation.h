#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_operation(preproc_state* state, ast_node* node)
{
    ast_node* left_node = ast_get_child(node, 0);
    ast_node* right_node = ast_get_child(node, 0);

}