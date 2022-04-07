#include "optimisation.h"

char* variables[1024];
int pointer = 0;

hashtable* used_vars;

int comparitive_nodes[15] = {
    AstAdd,
    AstAnd,
    AstDiv,
    AstEqual,
    AstGreaterThan,
    AstGreaterThanOrEqual,
    AstLessThan,
    AstLessThanOrEqual,
    AstMod,
    AstMul,
    AstNot,
    AstNotEqual,
    AstOr,
    AstPow,
    AstSub
};

int literals[3] = {
    AstBoolLit,
    AstIntegerLit,
    AstStringLit
};

/*
0 - condition can't be evaluated
1 - reference first node
2 - reference second node
*/
/*int eval_condition(ast_node* condition, ast_node* node1, ast_node* node2)
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



ast_node* initial_iteration(ast_node* node, hashtable* symbols)
{
    for (size_t i = 0; i < node->children->size; i++)
    {
        ast_node* child = ast_get_child(node, i);

        if (child->type == AstSymbol)
        {
            if (node->type != AstAssignment && node->type != AstDefinition)
            {
                variables[pointer] = child->value.symbol->name;
                pointer++;

                char* name = child->value.symbol->name;
                hashtable_set_item(used_vars, name, name);
            }
        }

        initial_iteration(child, symbols);
    }
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

    for (size_t i = 0; i < node->children->size; i++)
    {
        ast_node* scope = find_conditions(ast_get_child(node, i));
        if (scope)
        {
            ast_set_child(node, i, scope);
        }
    }

    return NULL;
}

ast_node* remove_variables(ast_node* node)
{
    for (size_t i = 0; i < node->children->size; i++)
    {
        ast_node* child = ast_get_child(node, i);
        ast_node* symbol = NULL;

        if (child->type == AstAssignment)
        {
            symbol = ast_get_child(child, 0);
        }
        else if (child->type == AstDefinition)
        {
            symbol = ast_get_child(child, 1);
        }

        if (symbol) {
            char* name = symbol->value.symbol->name;
            void* item = hashtable_get_item(used_vars, name);

            if (!item)
            {
                printf(name);
            }
        }

        remove_variables(ast_get_child(node, i));
    }
}*/

ast_node* remove_variables(ast_node* node)
{

}

bool check_children(ast_node* node, int array[], size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        if (node->type && node->type == array[i])
        {
            return true;
        }
    }
    return false;
}

ast_node* evaluate_expression(ast_node* node)
{
    size_t size = node->children->size;

    if (size == 2)
    {
        ast_node* node1 = ast_get_child(node, 0);
        ast_node* node2 = ast_get_child(node, 1);

        if (node->type == AstAdd)
        {
            node->type = AstIntegerLit;
            node->value.integer = node1->value.integer + node2->value.integer;
        }

        else if (node->type == AstMul)
        {
            node->type = AstIntegerLit;
            node->value.integer = node1->value.integer * node2->value.integer;
        }

        //printf("%lld\n", node->value.integer);
    }
    else
    {
        ast_node* child = ast_get_child(node, 0);

    }

    return node;
}

ast_node* simplify_expression(ast_node* node)
{
    size_t size = node->children->size;

    if (size == 2 && check_children(node, comparitive_nodes, sizeof(comparitive_nodes)))
    {
        for (int i = 0; i < size; i++)
        {
            ast_node* child = simplify_expression(ast_get_child(node, i));

            if (child->type == AstSymbol)
            {
                symbol* variable = ast_find_symbol(node, child->value.symbol->name);

                if (is_int(variable->dtype))
                {
                    ast_node* new_child = ast_new(AstIntegerLit);
                    new_child->value.integer = variable->value.integer;
                    
                    ast_set_child(node, i, new_child);

                    //printf("%lld, %lld\n", child->value.integer, variable->value.integer);
                }
            }
        }

        node = evaluate_expression(node);
        //printf("%lld\n", node->value.integer);
    }
    else if (node->type == AstSymbol)
    {
        symbol* variable = ast_find_symbol(node, node->value.symbol->name);

        if (is_int(variable->dtype))
        {
            node->type = AstIntegerLit;
            node->value.integer = variable->value.integer;

            //printf("%lld, %lld\n", child->value.integer, variable->value.integer);
        }
    }

    return node;
}

ast_node* find_expressions(ast_node* node)
{
    if (check_children(node, comparitive_nodes, sizeof(comparitive_nodes)))
    {
        //node = simplify_expression(node);
        //printf("%lld\n", node->value.integer);
    }

    if (node->type == AstAssignment)
    {
        symbol* variable = ast_get_child(node, 0)->value.symbol;
        ast_node* child = ast_get_child(node, 1);
        child = simplify_expression(child);

        if (child->type == AstIntegerLit)
        {
            ast_node* new_child = ast_new(AstIntegerLit);
            new_child->value.integer = child->value.integer;
            ast_set_child(node, 1, new_child);
        }

        printf("%s, %lld; ", variable->name, (int)child->value.integer);

        if (check_children(child, literals, sizeof(literals)))
        {
            //symbol* variable = ast_find_symbol(child, name);
            
            if (is_int(variable->dtype))
            {
                variable->value.integer = child->value.integer;
                //printf("%i", *((int*)variable->value));
            }
            else if (is_string(variable->dtype))
            {
                variable->value.string = child->value.string;
                //printf("%s", variable->value);
            }
            else if (is_bool(variable->dtype))
            {
                variable->value.boolean = child->value.boolean;
                //printf("%i", *((bool*)variable->value));
            }
        }
    }
    else if (check_children(node, comparitive_nodes, sizeof(comparitive_nodes)))
    {
        node = simplify_expression(node);
    }

    for (size_t i = 0; i < node->children->size; i++)
    {
        ast_node* child = find_expressions(ast_get_child(node, i));
        if (child)
        {
            ast_set_child(node, i, child);
        }
    }

    return NULL;
}

void optimize(module* mod, ast_node* node)
{
    // only optimises main function for now
    // support for other functions and modules coming next
    for (size_t i = 0; i < node->children->size; i++)
    {
        ast_node* child = ast_get_child(node, i);
        if (child->type == AstFunction)
        {
            ast_node* declaration = ast_get_child(child, 0);
            if (declaration->value.string == "main")
            {
                printf("%s\n", "test");
                ast_node* new_child = find_expressions(child);
                ast_set_child(node, i, new_child);
            }
        }
    }
   
    //remove_variables(node);
    
    //used_vars = hashtable_new();
    //hashtable* symbols = hashtable_new();

    //initial_iteration(node, symbols);
    //remove_variables(node);

    // temporary function that removes basic if statements
    //find_conditions(node);
    // currently working on finding a sensible solution
    // to pre-determining conditions where possible,
    // which will make all functions here much more effective
}