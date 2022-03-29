#include "symbol.h"
#include <stdlib.h>
#include <string.h>

symbol* symbol_new(void)
{
    symbol* sym = malloc(sizeof(symbol));

    sym->name = 0;

    sym->dtype;

    sym->is_initialised = false;
    sym->is_function = false;
    sym->function_node = 0;
}

void symbol_free(symbol* sym)
{   
    free(sym->name);
    data_type_free(sym->dtype);
    free(sym);
}

void symbol_print(symbol* sym)
{
    symbol_print_to_file(sym, stdout);
}

void symbol_print_to_file(symbol* sym, FILE* file)
{
    fprintf(file, "{\"name\":\"%s\",\"data_type\":\"%s\",\"pointer_level\":%ld", sym->name, sym->dtype->name, sym->dtype->pointer_level);

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


void symtable_print(hashtable* table)
{
    symtable_print_to_file(table, stdout);
}

void symtable_print_to_file(hashtable* table, FILE* file)
{
    fprintf(file, "[");

    for (int i = 0; i < table->capacity; i++)
    {
        if (table->keys[i] != 0)
        {
            symbol_print_to_file(table->items[i], file);
            fprintf(file, ",");
        }
    }

    fprintf(file, "{}]");
}