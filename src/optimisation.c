#include "optimisation.h"

int comparitive_nodes[15] = {
    AstAdd, AstAnd, AstDiv, AstEqual, AstGreaterThan,
    AstGreaterThanOrEqual, AstLessThan,
    AstLessThanOrEqual, AstMod, AstMul, AstNot,
    AstNotEqual, AstOr, AstPow, AstSub
};

int literals[4] = {
    AstBoolLit, AstIntegerLit,
    AstStringLit, AstFloatLit
};

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

void reclass_as_bool(ast_node* node, bool value)
{
    ast_node* new_node = ast_new(AstBoolLit);
    node->type = AstBoolLit;
    node->value.boolean = value;
    node->children = new_node->children;
    ast_get_child(node, 0)->parent = node;
    ast_free(new_node, false);
}

void reclass_as_num(ast_node* node, float result, bool is_integer)
{
    ast_node* data_type_node = ast_new(AstDataType);
    if (is_integer)
    {
        int result_int = result;
        node->type = AstIntegerLit;
        node->value.integer = result_int;
        data_type_node->value.dtype = data_type_new(
            result_int < 0 ? "i64" : "u64");
    }
    else
    {
        node->type = AstFloatLit;
        node->value.floating = result;
        data_type_node->value.dtype = data_type_new("f64");
    }

    data_type_node->value.dtype->pointer_level = 0;
    ast_delete_children(node);
    ast_add_child(node, data_type_node);
}

void reclass_node(ast_node* node, ast_node* node1, ast_node* node2)
{
    float value1 = (node1->type == AstIntegerLit ?
        node1->value.integer : node1->value.floating);
    float value2 = (node2->type == AstIntegerLit ?
        node2->value.integer : node2->value.floating);

    switch (node->type)
    {
        case AstAdd:
            return reclass_as_num(node, value1 + value2,
                node1->type == AstIntegerLit && node2->type == AstIntegerLit);
        
        case AstSub:
            return reclass_as_num(node, value1 + value2,
                node1->type == AstIntegerLit && node2->type == AstIntegerLit);
        
        case AstMul:
            return reclass_as_num(node, value1 * value2,
                node1->type == AstIntegerLit && node2->type == AstIntegerLit);
        
        case AstDiv:
            return reclass_as_num(node, value1 / value2,
                node1->type == AstIntegerLit && node2->type == AstIntegerLit);
        
        case AstMod:
            return reclass_as_num(node, node1->value.integer % node2->value.integer, true);
        
        case AstPow:
            return reclass_as_num(node, pow(value1, value2),
                node1->type == AstIntegerLit && node2->type == AstIntegerLit);
        
        case AstEqual:
            return reclass_as_bool(node, node1->type == AstFloatLit ?
                (node1->value.floating == node2->value.floating) :
                (node1->value.integer == node2->value.integer
                    && node1->value.boolean == node2->value.boolean
                    && node1->value.string == node2->value.string));
        
        case AstNotEqual:
            return reclass_as_bool(node, node1->type == AstFloatLit ?
                (node1->value.floating != node2->value.floating) :
                (node1->value.integer != node2->value.integer
                    && node1->value.boolean != node2->value.boolean
                    && node1->value.string != node2->value.string));
        
        case AstGreaterThan:
            return reclass_as_bool(node, (node1->type == AstIntegerLit ?
                (node1->value.integer > node2->value.integer) :
                (node1->value.floating > node2->value.floating)));
        
        case AstGreaterThanOrEqual:
            return reclass_as_bool(node, (node1->type == AstIntegerLit ?
                (node1->value.integer >= node2->value.integer) :
                (node1->value.floating >= node2->value.floating)));
        
        case AstLessThan:
            return reclass_as_bool(node, (node1->type == AstIntegerLit ?
                (node1->value.integer < node2->value.integer) :
                (node1->value.floating < node2->value.floating)));
        
        case AstLessThanOrEqual:
            return reclass_as_bool(node, (node1->type == AstIntegerLit ?
                (node1->value.integer <= node2->value.integer) :
                (node1->value.floating <= node2->value.floating)));
        
        case AstAnd:
            return reclass_as_bool(node, node1->value.boolean && node2->value.boolean);
        
        case AstOr:
            return reclass_as_bool(node, node1->value.boolean || node2->value.boolean);
    }
}

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
                    ast_free(child, true);
                    ast_set_child(node, i, scope);
                    child = ast_get_child(node, i);
                }
                else
                {
                    if (child->children->size > 2)
                    {
                        ast_node* else_scope = ast_get_child(child, 2);
                        else_scope->parent = child->parent;
                        ast_free(child, true);
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

            reclass_as_bool(node, !child->value.boolean);
        }
    }

    if (size == 2 && determinable)
    {
        ast_node* node1 = ast_get_child(node, 0);
        ast_node* node2 = ast_get_child(node, 1);

        for (int i = 0; i < 2; i++)
        {
            ast_node* child = ast_get_child(node, i);

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

            // if nodes aren't literals, don't continue
            if (!check_children(child, literals, sizeof(literals)))
            {
                return node;
            }
        }

        reclass_node(node, node1, node2);
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
        }

        evaluate_expression(node, determinable);
    }
    else if (node->type == AstSymbol)
    {
        symbol* variable = node->value.symbol;

        variable->variable_uses++;

        if (determinable)
        {
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
                ast_free(new_node, false);
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
                ast_free(new_node, false);
            }
            else if (is_bool(variable->dtype) && variable->is_literal)
            {
                ast_node* new_node = ast_new(AstBoolLit);
                node->type = AstBoolLit;
                node->value.boolean = variable->value.boolean;
                node->children = new_node->children;
                ast_get_child(node, 0)->parent = node;
                ast_free(new_node, false);
            }
            else if (is_string(variable->dtype) && variable->is_literal)
            {
                ast_node* new_node = ast_new(AstStringLit);
                node->type = AstStringLit;
                node->value.string = variable->value.string;
                node->children = new_node->children;
                ast_get_child(node, 0)->parent = node;
                ast_free(new_node, false);
            }
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

        if (check_children(child, literals, sizeof(literals)) && variable->is_literal)
        {
            if (is_int(variable->dtype))
            {
                variable->value.integer = child->value.integer;
                variable->is_assigned = true;
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
            }
            else if (is_bool(variable->dtype))
            {
                variable->value.boolean = child->value.boolean;
                variable->is_assigned = true;
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
            variable->variable_uses = 0;
        }
        vector_free(table_vec);
    }

    for (size_t i = 0; i < node->children->size; i++)
    {
        ast_node* child = ast_get_child(node, i);
        reset_variables(child);
    }
}

void remove_assignments(ast_node* node)
{
    for (size_t i = 0; i < node->children->size; i++)
    {
        ast_node* child = ast_get_child(node, i);

        if (child->type == AstFunction)
        {
            symbol* sym = ast_get_child(child, 0)->value.symbol;
            if (!sym->is_called && strcmp(sym->name, "main") != 0)
            {
                ast_delete_child(node, child);
                continue;
            }
        }

        if (child->type == AstAssignment)
        {
            symbol* sym = ast_get_child(child, 0)->value.symbol;
            if (sym->variable_uses == 0 && sym->is_literal)
            {
                ast_delete_child(node, child);
            }
        }
        else
        {
            remove_assignments(child);
        }
    }
}

void optimize(module* mod, ast_node* node)
{
    /*
    TODO: handle function optimisation better
    currently the optimizer declares any variable assignments in functions as indeterminable
    to prevent issues and weird edge cases
    this should be ideally improved to a state where function calls can be replaced entirely
    with literal values, if that's possible with the given source code
    */
    int count = 0;
    vector* children = vector_new();

    while (children != node->children && count < 10)
    {
        count++;
        vector_free(children);
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
                    //reset_variables(node);
                    find_expressions(child, true);
                    //replace_if_statements(child, true);
                    //remove_assignments(node);
                    
                    break;
                }
            }
        }
    }

    vector_free(children);
}