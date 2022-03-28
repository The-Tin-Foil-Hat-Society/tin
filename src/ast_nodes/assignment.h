#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"

void preprocess_assignment(preproc_state* state, ast_node* node)
{
    symbol* left_sym = ast_get_child(node, 0)->value.symbol; // 1st child should always be an identifier/symbol
    ast_node* right_node = ast_get_child(node, 1);

    if (right_node->type == AstAdd || right_node->type == AstDiv || right_node->type == AstExp || right_node->type == AstMod || right_node->type == AstMul || right_node->type == AstSub)
    {
        // just look for the closest data type node, expressions should already be validated to have the same data type for all values
        char* found_type = 0;
        size_t found_pointer_level = 0;

        ast_node* current_node = right_node;
        while (current_node != 0)
        {
            current_node = ast_get_child(current_node, 0);

            if (current_node->type == AstDataType)
            {
                found_type = current_node->value.string;
                found_pointer_level = current_node->pointer_level;
                break;
            }
            else if (current_node->type == AstSymbol)
            {
                found_type = current_node->value.symbol->data_type;
                found_pointer_level = current_node->value.symbol->pointer_level;
                break;
            }
        }

        if (found_type == 0)
        {
            // this should not happen
            printf("%s\n", ast_find_closest_src_line(node));
            printf("error: %s, could not determine the data type of the right hand value (pre processor bug)\n", left_sym->name);
            state->error_counter += 1;
        }

        if (strcmp(left_sym->data_type, found_type) != 0)
        {
            printf("%s\n", ast_find_closest_src_line(node));
            printf("error: %s has type %s while the right hand value has type %s\n", left_sym->name, left_sym->data_type, found_type);
            state->error_counter += 1;
        }
        if (left_sym->pointer_level != found_pointer_level)
        {
            printf("%s\n", ast_find_closest_src_line(node));
            printf("warning: %s is a level %ld pointer while the right hand value is a level %ld pointer\n", left_sym->name, left_sym->pointer_level, found_pointer_level);
        }
    }
    
    left_sym->is_initialised = true;
}