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


symtable* symtable_new(void)
{
    symtable* table = malloc(sizeof(symtable));

    table->symbols_size = 0;
    table->symbols_capacity = SYMTABLE_DEFAULT_CAPACITY;
    table->symbols = malloc(sizeof(symbol*) * table->symbols_capacity);

    return table;
}

void symtable_free(symtable* table)
{
    for (int i = 0; i < table->symbols_size; i++)
    {
        symbol_free(table->symbols[i]);
    }
    free(table->symbols);

    free(table);
}

void symtable_resize(symtable* table)
{
    size_t new_capacity = table->symbols_capacity * 2; 

    symbol** new_symbols = malloc(sizeof(symbol*) * new_capacity);
    memcpy(new_symbols, table->symbols, sizeof(symbol*) * table->symbols_capacity); // copy old array to the new location
    free(table->symbols); // free the old array

    table->symbols_capacity = new_capacity;
    table->symbols = new_symbols;
}

void symtable_add_symbol(symtable* table, symbol* symbol)
{
    if (table->symbols_size == table->symbols_capacity)
    {
        // need more space to add another child
        symtable_resize(table);
    }

    table->symbols[table->symbols_size] = symbol;
    table->symbols_size += 1;
}

symbol* symtable_find_symbol(symtable* table, char* name)
{
    symbol* found = 0;
    for (int i = 0; i < table->symbols_size; i++)
    {
        if (strcmp(name, table->symbols[i]->name) == 0)
        {
            found = table->symbols[i];
            break;
        }
    }
    return found;
}

void symtable_print(symtable* table)
{
    symtable_print_to_file(table, stdout);
}

void symtable_print_to_file(symtable* table, FILE* file)
{
    fprintf(file, "[");

    for (int i = 0; i < table->symbols_size; i++)
    {
        symbol_print_to_file(table->symbols[i], file);
        if (i < table->symbols_size - 1) // don't print a comma after the last symbol
        {
            fprintf(file, ",");
        }
    }

    fprintf(file, "]");
}