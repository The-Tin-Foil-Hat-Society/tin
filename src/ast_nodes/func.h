#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_func(preproc_state* state, ast_node* node)
{
    symbol* sym = ast_get_child(node, 0)->value.symbol;
    ast_node* scope_node = ast_get_child(node, 2);

    if (strcmp(sym->dtype->name, "void") != 0 || sym->dtype->pointer_level > 0)
    {
        bool has_return = false;
        for (int i = 0; i < scope_node->children->size; i++)
        {
            if (ast_get_child(scope_node, i)->type == AstReturn)
            {
                has_return = true;
                break;
            }
        }

        if (!has_return)
        {
            preproc_error(state, node, "%s must have a return statement\n", sym->name);
        }
    }
}