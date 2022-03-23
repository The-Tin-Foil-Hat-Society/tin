#include "interpreter.h"

void interpret(module* mod, ast_node* node)
{
    for (int i = 0; i < node->children_size; i++)
    {
        interpret(mod, node->children[i]);
    }

    if (node->type == AstDeclaration)
    {
        symbol* sym = node->children[0]->value.symbol;

        if (strcmp(sym->data_type, "i32") == 0)
        {
            //sym->value = &node->children[1]->value.integer;
        }
        else if (strcmp(sym->data_type, "u8") == 0 && sym->pointer_level == 1)
        {
            //sym->value = node->children[1]->value.string;
        }
    }

    /// ...
}