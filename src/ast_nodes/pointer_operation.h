#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_pointer_operation(preproc_state* state, ast_node* node)
{
    if (ast_get_child(node, 0)->value.dtype->pointer_level == 0)
    {
        preproc_error(state, node, "%svariable must be a pointer\n", "");
    }
}