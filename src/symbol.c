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
    sym->is_literal = true;
    sym->function_node = 0;
}

void symbol_free(symbol* sym)
{   
    free(sym->name);
    data_type_free(sym->dtype);
    free(sym);
}

void symbol_print_to_file(symbol* sym, FILE* file)
{
    fprintf(file, "{\"id\":\"%p\",\"name\":\"%s\",\"data_type\":\"%s\",\"data_type_pointer_level\":%ld,\"data_type_size\":%ld", sym, sym->name, sym->dtype->name, sym->dtype->pointer_level, sym->dtype->size);

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

void symtable_print_to_file(hashtable* table, FILE* file)
{
    fprintf(file, "[");

    vector* table_vec = hashtable_to_vector(table);
    for (int i = 0; i < table_vec->size; i++)
    {
        symbol_print_to_file(vector_get_item(table_vec, i), file);
        if (i < table_vec->size - 1) // don't print a comma after the last item
        {
            fprintf(file, ",");
        }
    }
    vector_free(table_vec);

    fprintf(file, "]");
}