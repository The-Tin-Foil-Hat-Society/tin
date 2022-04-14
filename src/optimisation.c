#include "optimisation.h"
#include <math.h>

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

int literals[4] = {
    AstBoolLit,
    AstIntegerLit,
    AstStringLit,
    AstFloatLit
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
}*/

ast_node* replace_if_statements(ast_node* node, bool determinable)
{
    for (size_t i = 0; i < node->children->size; i++)
    {
        ast_node* child = ast_get_child(node, i);

        if (node->type == AstFor || node->type == AstWhile)
        {
            continue;
        }
        else if (child->type == AstIf)
        {
            ast_node* condition = ast_get_child(child, 0);
            ast_node* scope = ast_get_child(child, 1);

            if (condition->type == AstBoolLit)
            {
                if (condition->value.boolean)
                {
                    scope->parent = child->parent;
                    ast_set_child(node, i, scope);
                    child = ast_get_child(node, i);
                }
                else
                {
                    if (child->children->size > 2)
                    {
                        ast_node* else_scope = ast_get_child(child, 2);
                        else_scope->parent = child->parent;
                        ast_set_child(node, i, else_scope);
                        child = ast_get_child(node, i);
                    }
                    else
                    {
                        ast_delete_child(node, child);
                        continue;
                    }
                }
            }
        }
        replace_if_statements(child, true);
    }
    return node;
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
                //printf(name);
            }
        }

        remove_variables(ast_get_child(node, i));
    }
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

bool compare_value(ast_node* node1, ast_node* node2)
{
    data_type* left_dtype = ast_find_data_type(node1);
    data_type* right_dtype = ast_find_data_type(node2);

    /*if (is_int(left_dtype) && is_int(right_dtype))
    {
        return (node1->value.integer == node2->value.integer);
    }
    else if (is_bool(left_dtype) && is_bool(right_dtype))
    {
        return (node1->value.boolean == node2->value.boolean);
    }
    else if (is_string(left_dtype) && is_string(right_dtype))
    {
        return (node1->value.string == node2->value.string);
    }*/

    return (node1->value.integer == node2->value.integer
        && node1->value.boolean == node2->value.boolean
        && node1->value.string == node2->value.string);

    return false;
}


ast_node* evaluate_expression(ast_node* node, bool determinable)
{
    size_t size = node->children->size;

    if (size == 1 && determinable)
    {
        ast_node* child = ast_get_child(node, 0);
        if (node->type == AstNot)
        {
            // fix size check when calling func
            if (child->type == AstSymbol)
            {
                symbol* sym = child->value.symbol;
                if (sym != 0)
                {
                    if (!sym->is_literal)
                    {
                        return node;
                    }
                }
            }
            else if (child->type == AstFunctionCall)
            {
                // replace in future version
                return node;
            }

            ast_node* new_node = ast_new(AstBoolLit);
            node->type = AstBoolLit;
            node->value.boolean = !child->value.boolean;
            node->children = new_node->children;
            ast_get_child(node, 0)->parent = node;
        }
    }

    if (size == 2 && determinable)
    {
        ast_node* node1 = ast_get_child(node, 0);
        ast_node* node2 = ast_get_child(node, 1);

        if (node1->type == AstSymbol)
        {
            symbol* symbol1 = node1->value.symbol;
            if (symbol1 != 0)
            {
                if (!symbol1->is_literal)
                {
                    return node;
                }
            }
        }
        else if (node1->type == AstFunctionCall)
        {
            // replace in future version
            return node;
        }
        
        if (node2->type == AstSymbol)
        {
            symbol* symbol2 = node2->value.symbol;
            if (symbol2 != 0)
            {
                if (!symbol2->is_literal)
                {
                    return node;
                }
            }
        }
        else if (node2->type == AstFunctionCall)
        {
            // replace in future version
            return node;
        }
        
        // if nodes aren't literals, don't continue
        if (!(check_children(node1, literals, sizeof(literals))
            && check_children(node2, literals, sizeof(literals))))
        {
            return node;
        }

        if (node->type == AstAdd)
        {
            if (node1->type == AstIntegerLit && node2->type == AstIntegerLit)
            {
                int result = node1->value.integer + node2->value.integer;
                ast_node* data_type_node = ast_new(AstDataType);
                node->type = AstIntegerLit;
                node->value.integer = result;

                data_type_node->value.dtype = data_type_new(
                    result < 0 ? "i64" : "u64");
                data_type_node->value.dtype->pointer_level = 0;
                ast_delete_children(node);
                ast_add_child(node, data_type_node);
            }
            else
            {
                float value1 = (node1->type == AstIntegerLit ?
                    node1->value.integer : node1->value.floating);
                float value2 = (node2->type == AstIntegerLit ?
                    node2->value.integer : node2->value.floating);
                
                float result = value1 + value2;
                ast_node* data_type_node = ast_new(AstDataType);
                node->type = AstFloatLit;
                node->value.floating = result;

                data_type_node->value.dtype = data_type_new("f64");
                data_type_node->value.dtype->pointer_level = 0;
                ast_delete_children(node);
                ast_add_child(node, data_type_node);
            }
        }
        else if (node->type == AstSub)
        {
            if (node1->type == AstIntegerLit && node2->type == AstIntegerLit)
            {
                int result = node1->value.integer - node2->value.integer;
                ast_node* data_type_node = ast_new(AstDataType);
                node->type = AstIntegerLit;
                node->value.integer = result;

                data_type_node->value.dtype = data_type_new(
                    result < 0 ? "i64" : "u64");
                data_type_node->value.dtype->pointer_level = 0;
                ast_delete_children(node);
                ast_add_child(node, data_type_node);
            }
            else
            {
                float value1 = (node1->type == AstIntegerLit ?
                    node1->value.integer : node1->value.floating);
                float value2 = (node2->type == AstIntegerLit ?
                    node2->value.integer : node2->value.floating);
                
                float result = value1 - value2;
                ast_node* data_type_node = ast_new(AstDataType);
                node->type = AstFloatLit;
                node->value.floating = result;

                data_type_node->value.dtype = data_type_new("f64");
                data_type_node->value.dtype->pointer_level = 0;
                ast_delete_children(node);
                ast_add_child(node, data_type_node);
            }
        }
        else if (node->type == AstMul)
        {
            if (node1->type == AstIntegerLit && node2->type == AstIntegerLit)
            {
                int result = node1->value.integer * node2->value.integer;
                ast_node* data_type_node = ast_new(AstDataType);
                node->type = AstIntegerLit;
                node->value.integer = result;

                data_type_node->value.dtype = data_type_new(
                    result < 0 ? "i64" : "u64");
                data_type_node->value.dtype->pointer_level = 0;
                ast_delete_children(node);
                ast_add_child(node, data_type_node);
            }
            else
            {
                float value1 = (node1->type == AstIntegerLit ?
                    node1->value.integer : node1->value.floating);
                float value2 = (node2->type == AstIntegerLit ?
                    node2->value.integer : node2->value.floating);
                
                float result = value1 * value2;
                ast_node* data_type_node = ast_new(AstDataType);
                node->type = AstFloatLit;
                node->value.floating = result;

                data_type_node->value.dtype = data_type_new("f64");
                data_type_node->value.dtype->pointer_level = 0;
                ast_delete_children(node);
                ast_add_child(node, data_type_node);
            }
        }
        else if (node->type == AstDiv)
        {
            int value1 = node1->value.integer;
            int value2 = node2->value.integer;
            if (node1->type == AstIntegerLit && node2->type == AstIntegerLit && value1 % value2 == 0)
            {
                int result = node1->value.integer / node2->value.integer;
                ast_node* data_type_node = ast_new(AstDataType);
                node->type = AstIntegerLit;
                node->value.integer = result;

                data_type_node->value.dtype = data_type_new(
                    result < 0 ? "i64" : "u64");
                data_type_node->value.dtype->pointer_level = 0;
                ast_delete_children(node);
                ast_add_child(node, data_type_node);
            }
            else
            {
                float value1 = (node1->type == AstIntegerLit ?
                    node1->value.integer : node1->value.floating);
                float value2 = (node2->type == AstIntegerLit ?
                    node2->value.integer : node2->value.floating);
                
                float result = value1 / value2;
                ast_node* data_type_node = ast_new(AstDataType);
                node->type = AstFloatLit;
                node->value.floating = result;

                data_type_node->value.dtype = data_type_new("f64");
                data_type_node->value.dtype->pointer_level = 0;
                ast_delete_children(node);
                ast_add_child(node, data_type_node);
            }
        }
        else if (node->type == AstMod)
        {
            if (node1->type == AstIntegerLit && node2->type == AstIntegerLit)
            {
                int result = node1->value.integer % node2->value.integer;
                ast_node* data_type_node = ast_new(AstDataType);
                node->type = AstIntegerLit;
                node->value.integer = result;

                data_type_node->value.dtype = data_type_new(
                    result < 0 ? "i64" : "u64");
                data_type_node->value.dtype->pointer_level = 0;
                ast_delete_children(node);
                ast_add_child(node, data_type_node);
            }
        }
        else if (node->type == AstPow)
        {
            if (node1->type == AstIntegerLit && node2->type == AstIntegerLit)
            {
                int result = pow(node1->value.integer, node2->value.integer);
                ast_node* data_type_node = ast_new(AstDataType);
                node->type = AstIntegerLit;
                node->value.integer = result;

                data_type_node->value.dtype = data_type_new(
                    result < 0 ? "i64" : "u64");
                data_type_node->value.dtype->pointer_level = 0;
                ast_delete_children(node);
                ast_add_child(node, data_type_node);
            }
            else
            {
                float value1 = (node1->type == AstIntegerLit ?
                    node1->value.integer : node1->value.floating);
                float value2 = (node2->type == AstIntegerLit ?
                    node2->value.integer : node2->value.floating);
                
                float result = pow(value1, value2);
                ast_node* data_type_node = ast_new(AstDataType);
                node->type = AstFloatLit;
                node->value.floating = result;

                data_type_node->value.dtype = data_type_new("f64");
                data_type_node->value.dtype->pointer_level = 0;
                ast_delete_children(node);
                ast_add_child(node, data_type_node);
            }
        }
        
        else if (node->type == AstEqual)
        {
            ast_node* new_node = ast_new(AstBoolLit);
            node->type = AstBoolLit;
            if (node1->type == AstFloatLit)
            {
                node->value.boolean = node1->value.floating == node2->value.floating;
            }
            else
            {
                node->value.boolean = (node1->value.integer == node2->value.integer
                    && node1->value.boolean == node2->value.boolean
                    && node1->value.string == node2->value.string);
            }
            
            if (node1->type == AstStringLit) {
                node->value.boolean = strcmp(node1->value.string, node2->value.string) == 0;
            }

            node->children = new_node->children;
            ast_get_child(node, 0)->parent = node;
        }
        else if (node->type == AstNotEqual)
        {
            ast_node* new_node = ast_new(AstBoolLit);
            node->type = AstBoolLit;
            if (node1->type == AstFloatLit)
            {
                node->value.boolean = node1->value.floating != node2->value.floating;
            }
            else
            {
                node->value.boolean = (node1->value.integer != node2->value.integer
                    && node1->value.boolean != node2->value.boolean
                    && node1->value.string != node2->value.string);
            }
            node->children = new_node->children;
            ast_get_child(node, 0)->parent = node;
        }
        else if (node->type == AstGreaterThan)
        {
            ast_node* new_node = ast_new(AstBoolLit);
            node->type = AstBoolLit;
            node->value.boolean = (node1->type == AstIntegerLit ?
                (node1->value.integer > node2->value.integer) :
                (node1->value.floating > node2->value.floating));
            node->children = new_node->children;
            ast_get_child(node, 0)->parent = node;
        }
        else if (node->type == AstGreaterThanOrEqual)
        {
            ast_node* new_node = ast_new(AstBoolLit);
            node->type = AstBoolLit;
            node->value.boolean = (node1->type == AstIntegerLit ?
                (node1->value.integer >= node2->value.integer) :
                (node1->value.floating >= node2->value.floating));
            node->children = new_node->children;
            ast_get_child(node, 0)->parent = node;
        }
        else if (node->type == AstLessThan)
        {
            ast_node* new_node = ast_new(AstBoolLit);
            node->type = AstBoolLit;
            node->value.boolean = (node1->type == AstIntegerLit ?
                (node1->value.integer < node2->value.integer) :
                (node1->value.floating < node2->value.floating));
            node->children = new_node->children;
            ast_get_child(node, 0)->parent = node;
        }
        else if (node->type == AstLessThanOrEqual)
        {
            ast_node* new_node = ast_new(AstBoolLit);
            node->type = AstBoolLit;
            node->value.boolean = (node1->type == AstIntegerLit ?
                (node1->value.integer <= node2->value.integer) :
                (node1->value.floating <= node2->value.floating));
            node->children = new_node->children;
            ast_get_child(node, 0)->parent = node;
        }

        else if (node->type == AstAnd)
        {
            ast_node* new_node = ast_new(AstBoolLit);
            node->type = AstBoolLit;
            node->value.boolean = node1->value.boolean && node2->value.boolean;
            node->children = new_node->children;
            ast_get_child(node, 0)->parent = node;
            //else if (node1->type == AstIntegerLit && node1->value.integer != 0)
            //else if (node1->type == AstStringLit && strlen(node1->value.string) > 0)
        }
        else if (node->type == AstOr)
        {
            ast_node* new_node = ast_new(AstBoolLit);
            node->type = AstBoolLit;
            node->value.boolean = node1->value.boolean || node2->value.boolean;
            node->children = new_node->children;
            ast_get_child(node, 0)->parent = node;
            //else if (node1->type == AstIntegerLit && node1->value.integer != 0)
            //else if (node1->type == AstStringLit && strlen(node1->value.string) > 0)
        }

        //printf("%lld\n", node->value.integer);
    }

    return node;
}

ast_node* simplify_expression(ast_node* node, bool determinable)
{
    size_t size = node->children->size;

    if (size == 2 && check_children(node, comparitive_nodes, sizeof(comparitive_nodes)))
    {
        for (int i = 0; i < size; i++)
        {
            ast_node* child = simplify_expression(ast_get_child(node, i), determinable);
            ast_set_child(node, i, child);

            /*if (child->type == AstSymbol && determinable)
            {
                symbol* variable = ast_find_symbol(node, child->value.symbol->name);

                if (is_int(variable->dtype) && variable->is_literal)
                {
                    ast_node* new_child = ast_new(AstIntegerLit);
                    new_child->value.integer = variable->value.integer;
                    
                    ast_set_child(node, i, new_child);

                    //printf("%lld, %lld\n", child->value.integer, variable->value.integer);
                }
            }*/
        }

        evaluate_expression(node, determinable);
        //printf("%lld\n", node->value.integer);
    }
    else if (node->type == AstSymbol && determinable)
    {
        symbol* variable = node->value.symbol;//ast_find_symbol(node, node->value.symbol->name);
        //printf("%s: %d\n", variable->name, variable->value.integer);

        if (is_int(variable->dtype) && variable->is_literal)
        {
            ast_node* new_node = ast_new(AstIntegerLit);
            ast_node* data_type_node = ast_new(AstDataType);
            node->type = AstIntegerLit;
            node->value.integer = variable->value.integer;

            data_type_node->value.dtype = data_type_new(
                variable->value.integer < 0 ? "i64" : "u64");
            data_type_node->value.dtype->pointer_level = 0;
            ast_add_child(node, data_type_node);
        }
        else if (is_float(variable->dtype) && variable->is_literal)
        {
            ast_node* new_node = ast_new(AstFloatLit);
            ast_node* data_type_node = ast_new(AstDataType);
            node->type = AstFloatLit;
            node->value.floating = variable->value.floating;

            data_type_node->value.dtype = data_type_new("f64");
            data_type_node->value.dtype->pointer_level = 0;
            ast_add_child(node, data_type_node);
        }
        else if (is_bool(variable->dtype) && variable->is_literal)
        {
            ast_node* new_node = ast_new(AstBoolLit);
            node->type = AstBoolLit;
            node->value.boolean = variable->value.boolean;
            node->children = new_node->children;
            ast_get_child(node, 0)->parent = node;
        }
        else if (is_string(variable->dtype) && variable->is_literal)
        {
            ast_node* new_node = ast_new(AstStringLit);
            node->type = AstStringLit;
            node->value.string = variable->value.string;
            node->children = new_node->children;
            ast_get_child(node, 0)->parent = node;
        }
    }
    else if (node->type == AstNot && determinable)
    {
        for (int i = 0; i < size; i++)
        {
            ast_node* child = simplify_expression(ast_get_child(node, i), determinable);
            ast_set_child(node, i, child);
        }

        evaluate_expression(node, determinable);
    }
    else if (node->type == AstFunctionCall)
    {
        symbol* func_call = ast_get_child(node, 0)->value.symbol;
        if (!func_call->is_called)
        {
            func_call->is_called = true;
            find_expressions(func_call->function_node, false);
        }
    }

    return node;
}

ast_node* assign_variable(ast_node* node, bool determinable)
{
    symbol* variable = ast_get_child(node, 0)->value.symbol;

    if (determinable)
    {
        ast_node* child = ast_get_child(node, 1);
        child = simplify_expression(child, determinable);

        //printf("%d", (int)child->type);

        /*if (child->type == AstIntegerLit)
        {
            ast_node* new_child = ast_new(AstIntegerLit);
            new_child->value.integer = child->value.integer;
            ast_set_child(node, 1, new_child);
        }
        else
        {
            variable->is_literal = false;
        }*/

        //("%s, %lld; ", variable->name, (int)child->value.integer);

        if (check_children(child, literals, sizeof(literals)) && variable->is_literal)
        {
            //symbol* variable = ast_find_symbol(child, variable->name);

            if (is_int(variable->dtype))
            {
                variable->value.integer = child->value.integer;
                variable->is_assigned = true;
                ////printf("%i", *((int*)variable->value));
            }
            else if (is_float(variable->dtype))
            {
                variable->value.floating = child->value.floating;
                variable->is_assigned = true;
            }
            else if (is_string(variable->dtype))
            {
                variable->value.string = child->value.string;
                variable->is_assigned = true;
                //printf("%s", variable->value);
            }
            else if (is_bool(variable->dtype))
            {
                variable->value.boolean = child->value.boolean;
                //printf(variable->name);
                variable->is_assigned = true;
                //printf("%i", *((bool*)variable->value));
            }
        }
        else
        {
            variable->is_literal = false;
        }
    }
    else
    {
        variable->is_literal = false;
    }
}

ast_node* find_expressions(ast_node* node, bool determinable)
{
    bool allow_scopes = false;

    if (check_children(node, comparitive_nodes, sizeof(comparitive_nodes)))
    {
        //node = simplify_expression(node);
        //printf("%lld\n", node->value.integer);
    }

    if (node->type == AstAssignment)
    {
        assign_variable(node, determinable);
        return NULL;
    }
    else if (node->type == AstIf)
    {
        if (determinable)
        {
            determinable = false;
            allow_scopes = true;
        }
    }
    else if (node->type == AstFor || node->type == AstWhile)
    {
        determinable = false;
    }
    else if (node->type == AstInput)
    {
        symbol* variable = ast_get_child(node, 0)->value.symbol;
        variable->is_literal = false;
        determinable = false;
    }
    else if (check_children(node, comparitive_nodes, sizeof(comparitive_nodes))
        || (node->type == AstSymbol && node->value.symbol->is_assigned)
        || node->type == AstNot)
    {
        
        node = simplify_expression(node, determinable);
    }
    else if (node->type == AstFunctionCall)
    {
        symbol* func_call = ast_get_child(node, 0)->value.symbol;
        if (!func_call->is_called)
        {
            func_call->is_called = true;
            find_expressions(func_call->function_node, false);
        }
    }

    for (size_t i = 0; i < node->children->size; i++)
    {
        ast_node* child = ast_get_child(node, i);
        ast_node* new_child;

        if (allow_scopes)
        {
            if (child->type == AstScope)
            {
                new_child = find_expressions(child, determinable);
            }
            else
            {
                //printf("%d\n", (int)child->type);
                new_child = find_expressions(child, true);
            }
        }
        else
        {
            new_child = find_expressions(child, determinable);
        }
        

        if (new_child)
        {
            ast_set_child(node, i, new_child);
        }
    }

    return NULL;
}

void reset_variables(ast_node* node)
{
    hashtable* symbols = node->value.symbol_table;

    if ((node->type == AstRoot || node->type == AstScope) && symbols->size > 0)
    {
        vector* table_vec = hashtable_to_vector(symbols);
        for (int i = 0; i < table_vec->size; i++)
        {
            symbol* variable = vector_get_item(table_vec, i);
            variable->is_literal = true;
            variable->is_assigned = false;
            variable->is_called = false;
        }
        vector_free(table_vec);
    }

    for (size_t i = 0; i < node->children->size; i++)
    {
        ast_node* child = ast_get_child(node, i);
        reset_variables(child);
    }
}

void optimize(module* mod, ast_node* node)
{
    // only optimises main function for now
    // support for other functions and modules coming next

    int count = 0;
    vector* children;

    while (children != node->children && count < 10)
    {
        count++;
        children = vector_copy(node->children);
        
        for (size_t i = 0; i < node->children->size; i++)
        {
            ast_node* child = ast_get_child(node, i);
            if (child->type == AstAssignment)
            {
                assign_variable(child, true);
            }
            else if (child->type == AstFunction)
            {
                ast_node* declaration = ast_get_child(child, 0);

                if (strcmp(declaration->value.symbol->name, "main") == 0)
                {
                    //ast_node* new_child =
                    find_expressions(child, true);
                    replace_if_statements(child, true);
                    reset_variables(node);
                    //new_child = remove_variables(new_child);
                    //ast_set_child(node, i, new_child);
                    //child = new_child;
                    break;
                }
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