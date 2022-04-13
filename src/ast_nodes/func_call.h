#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_func_call(preproc_state* state, ast_node* node)
{
    symbol* sym = ast_get_child(node, 0)->value.symbol;
    ast_node* function_node = sym->function_node;

    ast_node* arg_def_list_node = ast_get_child(function_node, 1);
    ast_node* arg_list_node = ast_get_child(node, 1);

    if (arg_def_list_node->children->size != arg_list_node->children->size)
    {
        preproc_error(state, node, "%s takes %ld arguments\n", sym->name, arg_def_list_node->children->size);
    }

    for (int i = 0; i < min(arg_def_list_node->children->size, arg_list_node->children->size); i++) // min to prevent accessing non existent nodes
    {
        symbol* arg_def = ast_get_child(arg_def_list_node, i)->value.symbol;
        data_type* arg_dtype = ast_find_data_type(ast_get_child(arg_list_node, i));

        if (is_int(arg_def->dtype) && is_int(arg_dtype) && !data_type_compare(arg_def->dtype, arg_dtype))
        {
            preproc_verb(state, node, "%s, implicit integer conversion\n", arg_def->name);
        }
        else if (is_float(arg_def->dtype) && is_float(arg_dtype) && !data_type_compare(arg_def->dtype, arg_dtype))
        {
            preproc_verb(state, node, "%s, implicit float conversion\n", arg_def->name);
        }
        else if (!data_type_compare(arg_def->dtype, arg_dtype))
        {
            preproc_error(state, node, "%s has type %s while the given value has type %s\n", arg_def->name, arg_def->dtype->name, arg_dtype->name);
        }
        if (arg_def->dtype->pointer_level != arg_dtype->pointer_level)
        {
            preproc_warn(state, node, "%s is a level %ld pointer while the given value is a level %ld pointer\n", arg_def->name, arg_def->dtype->pointer_level, arg_dtype->pointer_level);
        }
    }
}