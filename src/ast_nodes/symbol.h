#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "typechecking.h"

void preprocess_symbol(preproc_state* state, ast_node* node)
{
    symbol* sym = node->value.symbol;

    int index_in_parent = ast_get_child_index(node->parent, node);

    bool is_function_call_assignment = node->parent->type == AstFunctionCall && node->parent->parent->type == AstAssignment;
    bool is_being_assigned_to = (
           (node->parent->type == AstAlloc && index_in_parent == 0) 
        || (node->parent->type == AstAssignment && index_in_parent == 0)
        ||  node->parent->type == AstDefinition 
        ||  node->parent->type == AstInput
        );

    if (!is_being_assigned_to && !sym->is_initialised)
    {
        printf("%s\n", ast_find_closest_src_line(node));
        printf("error: %s is not initialized\n", node->value.string);
        state->error_counter += 1;
    }

    if (!is_being_assigned_to && node->parent->type == AstAlloc)
    {
        if (!is_int(sym->data_type) || sym->pointer_level > 0)
        {
            printf("%s\n", ast_find_closest_src_line(node));
            printf("error: size must be an integer type\n");
            state->error_counter += 1;
        }
    }
    else if (!is_being_assigned_to && (node->parent->type == AstAssignment || is_function_call_assignment))
    {
        symbol* left_sym;
        if (is_function_call_assignment)
        {
            left_sym = ast_get_child(node->parent->parent, 0)->value.symbol;
        }
        else if (ast_get_child(node->parent, 0)->type == AstDefinition)
        {
            left_sym = ast_get_child(ast_get_child(node->parent, 0), 0)->value.symbol;
        }
        else
        {
            left_sym = ast_get_child(node->parent, 0)->value.symbol;
        }


        if (strcmp(left_sym->data_type, sym->data_type) != 0)
        {
            printf("%s\n", ast_find_closest_src_line(node));
            printf("error: %s has type %s while %s has type %s\n", left_sym->name, left_sym->data_type, sym->name, sym->data_type);
            state->error_counter += 1;
        }
        else if (left_sym->pointer_level != sym->pointer_level)
        {
            printf("%s\n", ast_find_closest_src_line(node));
            printf("warning: %s is a level %ld pointer while %s is a level %ld pointer\n", left_sym->name, left_sym->pointer_level, sym->name, sym->pointer_level);
        }
    }
}