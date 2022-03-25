#include "symbol.h"
#include <stdlib.h>
#include <string.h>

symbol* symbol_new(void)
{
    symbol* sym = malloc(sizeof(symbol));

    sym->name = 0;
    sym->data_type = 0;
    sym->pointer_level = 0;
    sym->is_initialised = false;
    sym->is_function = false;
    sym->value = 0;
}

void symbol_free(symbol* sym)
{   
    if (sym->value != 0 && !sym->is_function)
    {
        free(sym->value);
    }
    free(sym->name);
    free(sym->data_type);
    free(sym);
}

void symbol_print(symbol* sym)
{
    symbol_print_to_file(sym, stdout);
}

void symbol_print_to_file(symbol* sym, FILE* file)
{
    fprintf(file, "{\"name\":\"%s\",\"data_type\":\"%s\"", sym->name, sym->data_type);

    if (sym->pointer_level > 0)
    {
        fprintf(file, ",\"pointer_level\":%ld", sym->pointer_level);
    }

    if (sym->is_initialised)
    {
        fprintf(file, ",\"is_initialised\":true");
    }
    else
    {
        fprintf(file, ",\"is_initialised\":false");
    }

    if (sym->is_function)
    {
        fprintf(file, ",\"is_function\":true");
    }
    else
    {
        fprintf(file, ",\"is_function\":false");
    }

    fprintf(file, "}");
}


symbol* symtable_find_symbol(vector* table, char* name)
{
    symbol* found = 0;
    for (size_t i = 0; i < table->size; i++)
    {
        if (strcmp(name, ((symbol*)vector_get_item(table, i))->name) == 0)
        {
            found = vector_get_item(table, i);
            break;
        }
    }
    return found;
}

void symtable_print(vector* table)
{
    symtable_print_to_file(table, stdout);
}

void symtable_print_to_file(vector* table, FILE* file)
{
    fprintf(file, "[");

    for (int i = 0; i < table->size; i++)
    {
        symbol_print_to_file(vector_get_item(table, i), file);
        if (i < table->size - 1) // don't print a comma after the last symbol
        {
            fprintf(file, ",");
        }
    }

    fprintf(file, "]");
}