#pragma once

#include "preprocessor.h"
#include "ast.h"

void preprocess_definition(preproc_state* state, ast_node* node)
{
    ast_node* data_type_node = ast_get_child(node, 0);
    ast_node* symbol_node = ast_get_child(node, 1);
    symbol* sym = symbol_node->value.symbol;

    sym->dtype = data_type_copy(data_type_node->value.dtype);

    if (node->parent->type == AstFunction)
    {
        sym->is_function = true;
        sym->is_initialised = true;
        sym->function_node = node->parent;
    }

    if (node->parent->type != AstRoot && node->parent->type != AstScope)
    {
        // definition node no longer needed, move up the symbol in its place
        // interpreter and codegen should make definitions from the symbol tables at the start of the program/scope
        symbol_node = ast_copy(symbol_node);
            
        ast_set_child(node->parent, ast_get_child_index(node->parent, node), symbol_node);
        ast_free(node, 0);

        state->index_offset = -1;
    }
}