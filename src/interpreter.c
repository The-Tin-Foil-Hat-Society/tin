#include "interpreter.h"

bool interpret(module* mod, ast_node* node, vector* stack)
{
    
}

// wrapper for interpret that does some stack operations for a function before interpreting it
bool interpret_function(module* mod, ast_node* node, vector* stack)
{
    // get arguments from the stack

    // continue interpreting
    ast_node* scope = ast_get_child(node, node->children->size - 1); // should always be the last child 
    return interpret(mod, node, stack);
}

// interprets the code outside of functions, i.e. global declarations
// returns false if an error occurred
bool interpret_program(module* mod, ast_node* node, vector* stack)
{
    if (node = 0)
    {
        node = mod->ast_root;
    }

    if (stack = 0)
    {
        stack = vector_new();
    }

    for (size_t i = 0; i < node->children->size; i++)
    {
        if (!interpret_program(mod, node, stack))
        {
            return false;
        }
    }

    if (node->type == AstFunction && strcmp(ast_get_child(node, 0)->value.symbol->name, "main") == 0)
    {
        return interpret_function(mod, node, stack);
    }
    else if (node->type == AstFunction)
    {
        return true;
    }

    return interpret(mod, node, stack);
}