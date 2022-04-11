#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_assignment(preproc_state* state, ast_node* node)
{
    symbol* left_sym = ast_get_child(node, 0)->value.symbol; // 1st child should always be a symbol
    ast_node* right_node = ast_get_child(node, 1);

    // just look for the closest data type, expressions should already be validated to have the same data type for all values
    data_type* found_dtype = ast_find_data_type(right_node);
    if (found_dtype == 0)
    {
        // this should not happen
        preproc_error(state, node, "%sassignment, could not determine the data type of the right hand value (preprocessor bug)\n", "");
    }

    // TODO: implement these checks somewhere else, they're repeated in many nodes
    if (is_int(left_sym->dtype) && is_int(found_dtype) && !data_type_compare(left_sym->dtype, found_dtype))
    {
        preproc_verb(state, node, "%s, implicit integer conversion\n", left_sym->name);
    }
    else if (is_float(left_sym->dtype) && is_float(found_dtype) && !data_type_compare(left_sym->dtype, found_dtype))
    {
        preproc_verb(state, node, "%s, implicit float conversion\n", left_sym->name);
    }
    else if (!data_type_compare(left_sym->dtype, found_dtype))
    {
        preproc_error(state, node, "%s has type %s while the right hand value has type %s\n", left_sym->name, left_sym->dtype->name, found_dtype->name);
    }
    if (left_sym->dtype->pointer_level != found_dtype->pointer_level)
    {
        preproc_warn(state, node, "%s is a level %ld pointer while the right hand value is a level %ld pointer\n", left_sym->name, left_sym->dtype->pointer_level, found_dtype->pointer_level);
    }
    
    left_sym->is_initialised = true;
}