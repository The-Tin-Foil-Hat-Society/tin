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

    node->children_size = 0;
    node->children_capacity = AST_DEFAULT_CAPACITY;

    // allocate space for child pointers
    node->children = malloc(sizeof(ast_node*) * node->children_capacity);
    
    if (node->type == AstRoot || node->type == AstScope)
    {
        symtable* symbol_table = symtable_new();
        node->value.symbol_table = symbol_table;
    }

    return node;
}

void ast_free(ast_node* node)
{
    for (int i = 0; i < node->children_size; i++)
    {
        ast_free(node->children[i]);
    }
    free(node->children);
    
    if (node->type == AstIdentifier || node->type == AstStringLit || node->type == AstDataType)
    {
        free(node->value.string);
    }
    if (node->type == AstRoot || node->type == AstScope)
    {
        symtable_free(node->value.symbol_table);
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

    copy->children_size = node->children_size;
    copy->children_capacity = node->children_capacity;
    copy->children = malloc(sizeof(ast_node*) * node->children_capacity);

    for (int i = 0; i < node->children_size; i++)
    {
        ast_node* child_copy = ast_copy(node->children[i]);
        child_copy->parent = copy;
        copy->children[i] = child_copy;
    }

    return copy;
}


void ast_resize(ast_node* node)
{
    size_t new_capacity = node->children_capacity * 2; // increase capacity exponentially

    ast_node** new_children = malloc(sizeof(ast_node*) * new_capacity);
    memcpy(new_children, node->children, sizeof(ast_node*) * node->children_capacity); // copy old array to the new location
    free(node->children); // free the old array

    node->children_capacity = new_capacity;
    node->children = new_children;
}

void ast_add_child(ast_node* node, ast_node* child)
{
    if (node->children_size == node->children_capacity)
    {
        // need more space to add another child
        ast_resize(node);
    }

    child->parent = node;
    node->children[node->children_size] = child;
    node->children_size += 1;
}

ast_node* ast_get_child(ast_node* node, int index)
{
    if (index > node->children_size)
    {
        return 0;
    }
    
    return node->children[index];
}

int ast_get_child_index(ast_node* node, ast_node* child)
{
    int child_i = 0;

    for (int i = 0; i < node->children_size; i++)
    {
        if (node->children[i] == child)
        {
            child_i = i;
            break;
        }
    }

    return child_i;
}

void ast_delete_child(ast_node* node, ast_node* child)
{
    int child_i = ast_get_child_index(node, child);

    if (child_i != 0)
    {
        free(child);
        node->children[child_i] = 0;
        node->children_size -= 1;
    }
}

symtable* ast_get_closest_symtable(ast_node* node)
{
    symtable* table = 0;

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
            sym = symtable_find_symbol(node->value.symbol_table, name);
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

    if (recursive && node->children_size > 0)
    {
        fprintf(file, ",\"children\": [");

        for (int i = 0; i < node->children_size; i++)
        {
            ast_print_to_file(node->children[i], file, true);
            if (i < node->children_size - 1) // don't print a comma after the last child
            {
                fprintf(file, ",");
            }
        }
        fprintf(file, "]");
    }
    fprintf(file, "}");
}