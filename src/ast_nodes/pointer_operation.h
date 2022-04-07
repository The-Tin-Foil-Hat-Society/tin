#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_pointer_operation(preproc_state* state, ast_node* node)
{
    ast_node* symbol_node = ast_get_child(node, 0);

    if (symbol_node->type != AstSymbol || symbol_node->value.symbol->dtype->pointer_level == 0)
    {
        preproc_error(state, node, "%svariable must be a pointer\n", "");
    }
}