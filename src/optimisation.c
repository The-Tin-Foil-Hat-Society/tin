#include "optimisation.h"

/*
0 - condition can't be evaluated
1 - reference first node
2 - reference second node
*/
int eval_condition(ast_node* condition, ast_node* node1, ast_node* node2)
{
    long long int value1 = node1->value.integer;
    long long int value2 = node2->value.integer;

    if (condition->type == AstEqual)
    {
        return value1 == value2 ? 1 : 2;
    }
    else if (condition->type == AstGreaterThan)
    {
        return value1 > value2 ? 1 : 2;
    }
    else if (condition->type == AstGreaterThanOrEqual)
    {
        return value1 >= value2 ? 1 : 2;
    }
    else if (condition->type == AstLessThan)
    {
        return value1 < value2 ? 1 : 2;
    }
    else if (condition->type == AstLessThanOrEqual)
    {
        return value1 <= value2 ? 1 : 2;
    }

    return 0;
}

ast_node* find_conditions(ast_node* node)
{   
    if (node->type == AstIf)
    {
        ast_node* condition = ast_get_child(node, 0);

        if (condition->children->size == 2)
        {
            ast_node* node1 = ast_get_child(condition, 0);
            ast_node* node2 = ast_get_child(condition, 1);
            
            if (node1->type == AstIntegerLit && node2->type == AstIntegerLit)
            {
                int response = eval_condition(condition, node1, node2);

                if (response != 0)
                {
                    return ast_get_child(node, response);
                }
            }
        }
    }

    if (node->children->size > 0)
    {
        for (size_t i = 0; i < node->children->size; i++)
        {
            ast_node* scope = find_conditions(ast_get_child(node, i));
            if (scope)
            {
                ast_set_child(node, i, scope);
            }
        }
    }

    return NULL;
}

void optimize(module* mod, ast_node* node)
{
    find_conditions(node);
}