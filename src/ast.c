#include "ast.h"

#include <stdio.h>

ast_node* ast_new(enum ast_node_type type)
{
    // allocate space for the node struct
    ast_node* node = malloc(sizeof(ast_node));

    node->type = type;
    node->parent = 0;
    node->pointer_level = 0;
    node->src_line = 0;

    node->children = vector_new();
    
    if (node->type == AstRoot || node->type == AstScope)
    {
        node->value.symbol_table = hashtable_new();
    }

    return node;
}

void ast_free(ast_node* node)
{
    for (size_t i = 0; i < node->children->size; i++)
    {
        ast_free(vector_get_item(node->children, i));
    }
    vector_free(node->children);
    
    if (node->type == AstIdentifier || node->type == AstStringLit || node->type == AstDataType)
    {
        free(node->value.string);
    }
    if (node->type == AstRoot || node->type == AstScope)
    {
        for (size_t i = 0; i < node->value.symbol_table->capacity; i++)
        {
            if (node->value.symbol_table->keys[i] != 0)
            {
                symbol_free(node->value.symbol_table->items[i]);
            }
        }

        hashtable_free(node->value.symbol_table);
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
    copy->pointer_level = node->pointer_level;
    copy->type = node->type;
    copy->value = node->value;

    if (node->type == AstIdentifier || node->type == AstStringLit || node->type == AstDataType)
    {
        copy->value.string = strdup(node->value.string);
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
        ast_node* child_copy = vector_get_item(node->children, i);
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
    if (index >= node->children->size)
    {
        return;
    }
    new_child->parent = node;
    vector_set_item(node->children, index, new_child);
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
    ast_free(child);
}

hashtable* ast_get_closest_symtable(ast_node* node)
{
    hashtable* table = 0;

    // traverse up the tree and check each symbol table for the given identifier
    while (table == 0 && node != 0)
    {
        if (node->type == AstRoot || node->type == AstScope)
        {
            table = node->value.symbol_table;
            break;
        }

        node = node->parent;
    }

    if (node == 0)
    {
        return 0;
    }

    return node->value.symbol_table;
}

symbol* ast_find_symbol(ast_node* node, char* name)
{
    symbol* sym = 0;

    // traverse up the tree and check each symbol table for the given symbol name
    while (sym == 0 && node != 0)
    {
        if (node->type == AstRoot || node->type == AstScope)
        {
            sym = (symbol*)hashtable_get_item(node->value.symbol_table, name);
        }

        node = node->parent;
    }

    return sym;
}


char* ast_find_closest_src_line(ast_node* node)
{
    while (node->src_line == 0 && node != 0)
    {
        node = node->parent;
    }

    return node->src_line;
}


void ast_print(ast_node* node, bool recursive)
{
    ast_print_to_file(node, stdout, recursive);
}

void ast_print_to_file(ast_node* node, FILE* file, bool recursive)
{
    fprintf(file, "{\"type\": \"%s\"", ast_type_names[node->type]);

    if (node->src_line != 0)
    {
        fprintf(file, ",\"src_line\": \"%s\"", node->src_line);
    }

    if (node->type == AstIdentifier || node->type == AstStringLit || node->type == AstDataType)
    {
        fprintf(file, ",\"str_value\": \"%s\"", node->value.string);
    }
    else if (node->type == AstIntegerLit)
    {
        fprintf(file, ",\"int_value\": %ld", node->value.integer);
    }
    else if (node->type == AstSymbol)
    {
        fprintf(file, ",\"str_value\": \"%s\"", node->value.symbol->name);
    }
    else if (node->type == AstRoot || node->type == AstScope)
    {
        fprintf(file, ",\"symbol_table\": ");
        symtable_print_to_file(node->value.symbol_table, file);
    }

    if (node->type == AstDataType && node->pointer_level > 0)
    {
        fprintf(file, ",\"pointer\": %ld", node->pointer_level);
    }

    if (recursive && node->children->size > 0)
    {
        fprintf(file, ",\"children\": [");

        for (size_t i = 0; i < node->children->size; i++)
        {
            ast_print_to_file(vector_get_item(node->children, i), file, true);
            if (i <  node->children->size - 1) // don't print a comma after the last child
            {
                fprintf(file, ",");
            }
        }
        fprintf(file, "]");
    }
    fprintf(file, "}");
}