#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_identifier(preproc_state* state, ast_node* node)
{
    bool namespaced_identifier = node->children->size > 0 && ast_get_child(node, 0)->type == AstNamespace;

    // evaluate which module the referenced identifier belongs to if it has a namespace
    module* namespace = state->mod;
    for (int i = node->children->size - 1; i >= 0; i--) // get namespaces in reverse
    {
        ast_node* namespace_node = ast_get_child(node, i);
        if (namespace_node->type != AstNamespace)
        {
            continue;
        }

        namespace = module_get_dependency(namespace, namespace_node->value.string);
    }

    hashtable* table = 0;
    if (namespaced_identifier)
    {
        if (namespace == 0)
        {
            preproc_error(state, node, "%scould not locate given namespace\n", "");
        }
        else
        {
            table = namespace->ast_root->value.symbol_table;
        }
    }

    if (table == 0)
    {
        table = ast_get_closest_symtable(node);
    }

    int index_in_parent = ast_get_child_index(node->parent, node);

    bool is_function_argument_identifier = node->parent->type == AstDefinition && node->parent->parent != 0 && node->parent->parent->type == AstDefinitionList && node->parent->parent->parent->type == AstFunction;
    bool is_being_assigned_to = (node->parent->type == AstAlloc || node->parent->type == AstDefinition || node->parent->type == AstInput || (node->parent->type == AstAssignment && index_in_parent == 0));

    symbol* sym;

    if (is_function_argument_identifier)
    {
        table = ast_get_child(node->parent->parent->parent, 2)->value.symbol_table; // scope should always be the 3rd child as per the grammar if there's also a definition list
        // for definitions for function args, make sure they're in the function's scope and not outside it
        sym = hashtable_get_item(table, node->value.string);
    }
    else if (namespaced_identifier)
    {
        sym = hashtable_get_item(table, node->value.string);
    }
    else
    {
        // look for the symbol in all scopes above us
        sym = ast_find_symbol(node, node->value.string);
    }

    if (sym == 0)
    {
        sym = symbol_new();
        sym->name = strdup(node->value.string);

        if (!is_being_assigned_to)
        {
            preproc_error(state, node, "%s undefined\n", node->value.string);
            sym->dtype = data_type_new("undefined");
            // still continue to create a symbol so we don't want to propage the error, otherwise we'll get a segfault
        }

        if (is_function_argument_identifier)
        {
            sym->is_initialised = true; // if its a function argument, assume it's already initialized. actually check for that during function calls not in function definitions
        }
        
        hashtable_set_item(table, node->value.string, sym);
    }
    else if (sym != 0 && node->parent->type == AstDefinition)
    {
        preproc_error(state, node, "%s is already defined\n", node->value.string); 
    }

    ast_node* symbol_node = ast_new(AstSymbol);
    symbol_node->value.symbol = sym;

    // copy children to the new symbol
    for (int i = 0; i < node->children->size; i++)
    {
        ast_node* child = ast_get_child(node, i);
        if (child->type != AstNamespace) // don't copy namespace nodes though
        {
            ast_add_child(symbol_node, ast_copy(child));
        }
    }

    // replace the indentifier node with a symbol node
    ast_set_child(node->parent, index_in_parent, symbol_node);
    ast_free(node);
}