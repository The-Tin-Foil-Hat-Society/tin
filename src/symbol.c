#include "symbol.h"
#include <stdlib.h>
#include <string.h>

symbol* symbol_new(char* name, void* mod_ptr)
{
    symbol* sym = malloc(sizeof(symbol));

    sym->name = strdup(name);
    sym->key = symbol_generate_key(name, mod_ptr);

    sym->dtype = 0;

    sym->is_initialised = false;
    sym->is_function = false;
    sym->function_node = 0;
}

void symbol_free(symbol* sym)
{   
    if (sym == 0)
    {
        return;
    }

    free(sym->name);
    free(sym->key);
    data_type_free(sym->dtype);
    free(sym);
}

char* symbol_generate_key(char* name, void* mod_ptr)
{
    char* key = malloc(2 + 16 + strlen(name) + 1); // 5 chars for the format + max 16 hex chars for the pointer + name length + null terminator
    sprintf(key, "_%p_%s", mod_ptr, name);
    return key;
}

void symbol_print_to_file(symbol* sym, FILE* file)
{
    fprintf(file, "{\"key\":\"%s\",\"name\":\"%s\",\"data_type\":\"%s\",\"data_type_pointer_level\":%ld,\"data_type_size\":%ld", sym->key, sym->name, sym->dtype->name, sym->dtype->pointer_level, sym->dtype->size);

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