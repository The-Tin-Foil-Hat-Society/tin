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
    size /= sizeof(int);

    for (size_t i = 0; i < size; i++)
    {
        if (node->type && (int)node->type == array[i])
        {
            return true;
        }
    }
    return false;
}

void reclass_as_bool(ast_node* parent, int child_index, bool value)
{
    ast_node* node = ast_get_child(parent, child_index);
    ast_node* data_type_node = ast_new(AstDataType);
    ast_node* new_node = ast_new(AstBoolLit);

    data_type_node->value.dtype = data_type_new("bool");
    new_node->value.boolean = value;

    ast_delete_children(node);
    ast_add_child(new_node, data_type_node);
    ast_set_child(parent, child_index, new_node);
    ast_free(node, 0);
}

void reclass_as_num(ast_node* parent, int child_index, float result, bool is_integer)
{
    ast_node* node = ast_get_child(parent, child_index);
    ast_node* data_type_node = ast_new(AstDataType);
    ast_node* new_node;

    if (is_integer)
    {
        int result_int = result;
        data_type_node->value.dtype = data_type_new(
            result_int < 0 ? "i64" : "u64");
        new_node = ast_new(AstIntegerLit);
        new_node->value.integer = result_int;
    }
    else
    {
        new_node = ast_new(AstFloatLit);
        data_type_node->value.dtype = data_type_new("f64");
        new_node->value.floating = result;
    }

    ast_delete_children(node);
    ast_add_child(new_node, data_type_node);
    ast_set_child(parent, child_index, new_node);
    ast_free(node, 0);
}

void reclass_node(ast_node* parent, int child_index)
{
    ast_node* node = ast_get_child(parent, child_index);
    ast_node* node1 = ast_get_child(node, 0);
    ast_node* node2 = ast_get_child(node, 1);

    float value1 = (node1->type == AstIntegerLit ?
        node1->value.integer : node1->value.floating);
    float value2 = (node2->type == AstIntegerLit ?
        node2->value.integer : node2->value.floating);

    switch (node->type)
    {
        case AstAdd:
            reclass_as_num(parent, child_index, value1 + value2,
                node1->type == AstIntegerLit && node2->type == AstIntegerLit);
            break;
        
        case AstSub:
            reclass_as_num(parent, child_index, value1 + value2,
                node1->type == AstIntegerLit && node2->type == AstIntegerLit);
            break;
        
        case AstMul:
            reclass_as_num(parent, child_index, value1 * value2,
                node1->type == AstIntegerLit && node2->type == AstIntegerLit);
            break;
        
        case AstDiv:
            reclass_as_num(parent, child_index, value1 / value2,
                node1->type == AstIntegerLit && node2->type == AstIntegerLit);
            break;
        
        case AstMod:
            reclass_as_num(parent, child_index,
                node1->value.integer % node2->value.integer, true);
            break;
        
        case AstPow:
            reclass_as_num(parent, child_index, pow(value1, value2),
                node1->type == AstIntegerLit && node2->type == AstIntegerLit);
            break;
        
        case AstEqual:
            reclass_as_bool(parent, child_index, node1->type == AstFloatLit ?
                (node1->value.floating == node2->value.floating) :
                (node1->value.integer == node2->value.integer
                    && node1->value.boolean == node2->value.boolean
                    && node1->value.string == node2->value.string));
            break;
        
        case AstNotEqual:
            reclass_as_bool(parent, child_index, node1->type == AstFloatLit ?
                (node1->value.floating != node2->value.floating) :
                (node1->value.integer != node2->value.integer
                    && node1->value.boolean != node2->value.boolean
                    && node1->value.string != node2->value.string));
            break;
        
        case AstGreaterThan:
            reclass_as_bool(parent, child_index, (node1->type == AstIntegerLit ?
                (node1->value.integer > node2->value.integer) :
                (node1->value.floating > node2->value.floating)));
            break;
        
        case AstGreaterThanOrEqual:
            reclass_as_bool(parent, child_index, (node1->type == AstIntegerLit ?
                (node1->value.integer >= node2->value.integer) :
                (node1->value.floating >= node2->value.floating)));
            break;
        
        case AstLessThan:
            reclass_as_bool(parent, child_index, (node1->type == AstIntegerLit ?
                (node1->value.integer < node2->value.integer) :
                (node1->value.floating < node2->value.floating)));
            break;
        
        case AstLessThanOrEqual:
            reclass_as_bool(parent, child_index, (node1->type == AstIntegerLit ?
                (node1->value.integer <= node2->value.integer) :
                (node1->value.floating <= node2->value.floating)));
            break;
        
        case AstAnd:
            reclass_as_bool(parent, child_index,
                node1->value.boolean && node2->value.boolean);
            break;
        
        case AstOr:
            reclass_as_bool(parent, child_index,
                node1->value.boolean || node2->value.boolean);
            break;
        
        default:
            return;
    }
}

void replace_if_statements(ast_node* node)
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
                    ast_node* new_child = ast_copy(scope);
                    ast_delete_children(child);
                    ast_set_child(node, i, new_child);
                    ast_free(child, 0);
                    
                    child = new_child;
                }
                else
                {
                    if (child->children->size > 2)
                    {
                        ast_node* else_scope = ast_get_child(child, 2);
                        ast_node* new_child = ast_copy(else_scope);
                        ast_delete_children(child);
                        ast_set_child(node, i, new_child);
                        ast_free(child, 0);

                        child = new_child;
                    }
                    else
                    {
                        ast_delete_child(node, child);
                        continue;
                    }
                }
            }
        }
        replace_if_statements(child);
    }
}

void evaluate_expression(ast_node* parent, int child_index, bool determinable)
{
    ast_node* node = ast_get_child(parent, child_index);
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
                        return;
                    }
                }
            }
            else if (child->type == AstFunctionCall)
            {
                // replace in future version
                return;
            }

            reclass_as_bool(parent, child_index, !child->value.boolean);
        }
    }

    if (size == 2 && determinable)
    {
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
                        return;
                    }
                }
            }
            else if (child->type == AstFunctionCall)
            {
                // replace in future version
                return;
            }

            // if nodes aren't literals, don't continue
            if (!check_children(child, literals, sizeof(literals)))
            {
                return;
            }
        }

        reclass_node(parent, child_index);
    }

    return;
}

void simplify_expression(ast_node* parent, int child_index, bool determinable)
{
    ast_node* node = ast_get_child(parent, child_index);

    if (node->children->size == 2 && check_children(node, comparitive_nodes, sizeof(comparitive_nodes)))
    {
        for (int i = 0; i < 2; i++)
        {
            simplify_expression(node, i, determinable);
        }

        evaluate_expression(parent, child_index, determinable);
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
        for (size_t i = 0; i < node->children->size; i++)
        {
            simplify_expression(node, i, determinable);
        }
        
        evaluate_expression(parent, child_index, determinable);
    }
    else if (node->type == AstFunctionCall)
    {
        symbol* func_call = ast_get_child(node, 0)->value.symbol;
        if (!func_call->is_called)
        {
            func_call->is_called = true;
            find_expressions(func_call->function_node, 2, false);
        }
    }

    return;
}

void assign_variable(ast_node* node, bool determinable)
{
    symbol* variable = ast_get_child(node, 0)->value.symbol;

    if (determinable)
    {
        simplify_expression(node, 1, determinable);
        ast_node* child = ast_get_child(node, 1);

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

void find_expressions(ast_node* parent, int child_index, bool determinable)
{
    ast_node* node = ast_get_child(parent, child_index);
    bool allow_scopes = false;

    if (node->type == AstAssignment)
    {
        assign_variable(node, determinable);
        return;
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
        simplify_expression(parent, child_index, determinable);
        // node = simpl...
    }
    
    if (node->type == AstFunctionCall)
    {
        symbol* func_call = ast_get_child(node, 0)->value.symbol;

        if (!func_call->is_called)
        {
            // 2 represents function scope
            func_call->is_called = true;
            find_expressions(func_call->function_node, 2, false);
        }
    }

    node = ast_get_child(parent, child_index);

    for (size_t i = 0; i < node->children->size; i++)
    {
        ast_node* child = ast_get_child(node, i);

        if (allow_scopes)
        {
            if (child->type == AstScope)
            {
                find_expressions(node, i, determinable);
            }
            else
            {
                find_expressions(node, i, true);
            }
        }
        else
        {
            find_expressions(node, i, determinable);
        }
    }

    return;
}

void reset_variables(ast_node* node)
{
    hashtable* symbols = node->value.symbol_table;

    if ((node->type == AstRoot || node->type == AstScope) && symbols->size > 0)
    {
        vector* table_vec = hashtable_to_vector(symbols);
        for (size_t i = 0; i < table_vec->size; i++)
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

void optimise(ast_node* node)
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
        reset_variables(node);
        
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
                declaration->value.symbol->function_node = child;

                if (strcmp(declaration->value.symbol->name, "main") == 0)
                {
                    find_expressions(child, 2, true);
                    replace_if_statements(child);
                    remove_assignments(node);
                    
                    break;
                }
            }
        }
    }

    vector_free(children);
}