#include "ast.h"

#include <stdio.h>

bool has_dtype(enum ast_node_type type)
{
    return type == AstDataType || type == AstAdd || type == AstDiv || type == AstMod || type == AstMul || type == AstPow || type == AstSub || type == AstBitwiseAnd || type == AstBitwiseOr || type == AstBitwiseXor || type == AstShiftLeft || type == AstShiftRight || type == AstGreaterThan || type == AstGreaterThanOrEqual  || type == AstLessThan || type == AstLessThanOrEqual  || type == AstEqual || type == AstNotEqual || type == AstAnd || type == AstNot || type == AstOr;
}

ast_node* ast_new(enum ast_node_type type)
{
    // allocate space for the node struct
    ast_node* node = malloc(sizeof(ast_node));

    node->type = type;
    node->parent = 0;
    node->src_line = 0;

    node->children = vector_new();
    
    if (node->type == AstRoot || node->type == AstScope)
    {
        node->value.symbol_table = hashtable_new();
    }
    else if (node->type == AstBoolLit)
    {
        ast_node* data_type_node = ast_new(AstDataType);
        data_type_node->value.dtype = data_type_new("bool");
        data_type_node->value.dtype->pointer_level = 0;
        ast_add_child(node, data_type_node);
    }
    else if (node->type == AstStringLit)
    {
        ast_node* data_type_node = ast_new(AstDataType);
        data_type_node->value.dtype = data_type_new("i8");
        data_type_node->value.dtype->pointer_level = 1;
        ast_add_child(node, data_type_node);
    }
    else if (has_dtype(node->type))
    {
        node->value.dtype = 0;
    }

    return node;
}

void ast_free(ast_node* node, bool keep_symbols)
{
    if (node == 0)
    {
        return;
    }

    for (size_t i = 0; i < node->children->size; i++)
    {
        ast_free(vector_get_item(node->children, i), keep_symbols);
    }
    vector_free(node->children);
    
    if (node->type == AstAsm || node->type == AstIdentifier || node->type == AstInclude || node->type == AstNamespace || node->type == AstStringLit)
    {
        free(node->value.string);
    }
    else if ((node->type == AstRoot || node->type == AstScope))
    {
        if (!keep_symbols)
        {
            for (size_t i = 0; i < node->value.symbol_table->capacity; i++)
            {
                if (node->value.symbol_table->keys[i] != 0)
                {
                    symbol_free(node->value.symbol_table->items[i]);
                }
            }
        }

        hashtable_free(node->value.symbol_table);
    }
    else if (has_dtype(node->type) && node->value.dtype != 0)
    {
        data_type_free(node->value.dtype);
    }

    // don't free symbols in symbol nodes since they're just references to the symbol table !!

    if (node->src_line != 0)
    {
        free(node->src_line);
    }
    
    free(node);
}

ast_node* ast_copy(ast_node* node)
{
    ast_node* copy = malloc(sizeof(ast_node));

    copy->parent = node->parent;
    copy->type = node->type;
    copy->value = node->value;

    if (node->type == AstAsm || node->type == AstIdentifier || node->type == AstInclude || node->type == AstNamespace || node->type == AstStringLit)
    {
        copy->value.string = strdup(node->value.string);
    }
    else if (node->type == AstRoot || node->type == AstScope)
    {
        copy->value.symbol_table = hashtable_copy(node->value.symbol_table);
    }
    else if (has_dtype(node->type))
    {
        copy->value.dtype = data_type_copy(node->value.dtype);
    }

    if (node->src_line != 0)
    {
        copy->src_line = strdup(node->src_line);
    }
    else
    {
        copy->src_line = 0;
    }

    copy->children = vector_copy(node->children);

    for (size_t i = 0; i < node->children->size; i++)
    {
        ast_node* child_copy = ast_copy(vector_get_item(node->children, i));
        child_copy->parent = copy;
        vector_set_item(copy->children, i, child_copy);
    }

    return copy;
}

void ast_add_child(ast_node* node, ast_node* child)
{
    vector_add_item(node->children, child);
    child->parent = node;    
}

void ast_set_child(ast_node* node, size_t index, ast_node* new_child)
{
    if (new_child != 0)
    {
        new_child->parent = node;
    }
    vector_set_item(node->children, index, new_child);
}
void ast_insert_child(ast_node* node, size_t index, ast_node* new_child)
{
    vector_insert_item(node->children, index, new_child);
    new_child->parent = node;
}
ast_node* ast_get_child(ast_node* node, size_t index)
{
    return vector_get_item(node->children, index);
}

size_t ast_get_child_index(ast_node* node, ast_node* child)
{
    return vector_get_item_index(node->children, child);
}

void ast_delete_child(ast_node* node, ast_node* child)
{
    vector_delete_item(node->children, child);
    ast_free(child, 0);
}

ast_node* ast_get_current_function(ast_node* node)
{
    while (node != 0)
    {
        if (node->type == AstFunction)
        {
            return node;
        }

        node = node->parent;
    }

    return 0;
}

hashtable* ast_get_closest_symtable(ast_node* node)
{
    // traverse up the tree and check each symbol table for the given identifier
    while (node != 0)
    {
        if (node->type == AstRoot || node->type == AstScope)
        {
            return node->value.symbol_table;
        }

        node = node->parent;
    }

    return 0;
}

data_type* ast_find_data_type(ast_node* node)
{
    if (has_dtype(node->type))
    {
        return node->value.dtype;
    }
    else if (node->type == AstSymbol)
    {
        return node->value.symbol->dtype;
    }

    for (int i = 0; i < node->children->size; i++)
    {
        data_type* found_dtype = ast_find_data_type(ast_get_child(node, i));
        if (found_dtype != 0)
        {
            return found_dtype;
        }
    }

    return 0;
}

symbol *ast_find_symbol(ast_node *node, char *symbol_key)
{
    symbol* sym = 0;

    // traverse up the tree and check each symbol table for the given symbol name
    while (sym == 0 && node != 0)
    {
        if (node->type == AstRoot || node->type == AstScope)
        {
            sym = (symbol*)hashtable_get_item(node->value.symbol_table, symbol_key);
        }

        node = node->parent;
    }

    return sym;
}


char* ast_find_closest_src_line(ast_node* node)
{
    while (node != 0 && node->src_line == 0)
    {
        node = node->parent;
    }

    if (node == 0)
    {
        return 0;
    }

    return node->src_line;
}


void ast_print_to_file(ast_node* node, FILE* file)
{
    fprintf(file, "{\"type\": \"%s\"", ast_type_names[node->type]);

    if (node->src_line != 0)
    {
        fprintf(file, ",\"src_line\": \"%s\"", node->src_line);
    }

    if (node->type == AstAsm || node->type == AstIdentifier || node->type == AstInclude || node->type == AstNamespace || node->type == AstStringLit)
    {
        fprintf(file, ",\"str_value\": \"%s\"", node->value.string);
    }
    else if (node->type == AstBoolLit)
    {
        fprintf(file, ",\"bool_value\": %s", node->value.boolean ? "true" : "false");
    }
    else if (node->type == AstFloatLit)
    {
        fprintf(file, ",\"float_value\": %f", node->value.floating);
    }
    else if (node->type == AstIntegerLit)
    {
        data_type* dtype = ast_find_data_type(node);
        if (dtype->_signed) 
        {
            fprintf(file, ",\"int_value\": %ld", node->value.integer);
        }
        else
        {
            fprintf(file, ",\"int_value\": %lu", *(uint64_t*)&node->value.integer);
        }
    }
    else if (node->type == AstSymbol)
    {
        fprintf(file, ",\"symbol_key\":\"%s\",\"str_value\": \"%s\"", node->value.symbol->key, node->value.symbol->name);
    }
    else if ((node->type == AstRoot || node->type == AstScope) && node->value.symbol_table->size > 0)
    {
        fprintf(file, ",\"symbol_table\": ");
        symtable_print_to_file(node->value.symbol_table, file);
    }

    if (node->type == AstDataType)
    {
        fprintf(file, ",\"name\": \"%s\",\"pointer_level\": %ld", node->value.dtype->name, node->value.dtype->pointer_level);
    }

    if (node->children->size > 0)
    {
        fprintf(file, ",\"children\": [");

        for (size_t i = 0; i < node->children->size; i++)
        {
            ast_print_to_file(vector_get_item(node->children, i), file);
            if (i < node->children->size - 1) // don't print a comma after the last child
            {
                fprintf(file, ",");
            }
        }
        fprintf(file, "]");
    }
    fprintf(file, "}");
}