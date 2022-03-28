#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "typechecking.h"

void preprocess_string_lit(preproc_state* state, ast_node* node)
{
    if (node->parent->type == AstAlloc)
    {
        printf("%s\n", ast_find_closest_src_line(node));
        printf("error: alloc size must be an integer\n");
        state->error_counter += 1;
    }
    else if (node->parent->type == AstAssignment)
    {
        symbol* left_sym = ast_get_child(node->parent, 0)->value.symbol;
        if (strcmp(left_sym->data_type, "u8") != 0 || left_sym->pointer_level != 1)  // must be (ptr u8)
        {
            printf("%s\n", ast_find_closest_src_line(node));
            printf("error: %s must be of type ptr u8\n", left_sym->name);
            state->error_counter += 1;
        }
    }
}