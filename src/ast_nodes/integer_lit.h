#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "typechecking.h"

void preprocess_integer_lit(preproc_state* state, ast_node* node)
{
    if (node->parent->type == AstAlloc)
    {
        if (node->value.integer < 0)
        {
            printf("%s\n", ast_find_closest_src_line(node));
            printf("error: cannot allocate less than 0 bytes\n");
            state->error_counter += 1;
        }
        else if (node->value.integer > UINT32_MAX)
        {
            printf("%s\n", ast_find_closest_src_line(node));
            printf("error: cannot allocate more than u32 max bytes\n");
            state->error_counter += 1;
        }  
    }
    else if (node->parent->type == AstAssignment)
    {
        symbol* left_sym = ast_get_child(node->parent, 0)->value.symbol;
        if(!is_valid_int(left_sym->data_type, node->value.integer))
        {
            printf("%s\n", ast_find_closest_src_line(node));
            printf("error: value does not fit the data type of variable %s\n", left_sym->name);
            state->error_counter += 1;
        }
    }
}