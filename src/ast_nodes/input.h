#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_input(preproc_state* state, ast_node* node)
{
    ast_get_child(node, 0)->value.symbol->is_initialised = true;
}