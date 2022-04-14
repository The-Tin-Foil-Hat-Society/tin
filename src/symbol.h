#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "data_type.h"
#include "utils/hashtable.h"
#include "utils/vector.h"

typedef struct
{
    char *name;
    char *key;

    data_type *dtype;

    bool is_initialised;
    bool is_function;

    void *function_node; // for the interperter to follow function symbols
} symbol;

symbol *symbol_new(char* name, void* mod_ptr);
void symbol_free(symbol *sym);

// generates a unique key for the given symbol name in the given module/namepsaec
char* symbol_generate_key(char* name, void* mod_ptr);

// prints a json representation of the symbol to the given file (can be stdout)
void symbol_print_to_file(symbol *sym, FILE *file);

// prints a json representation of the symbol table to the given file (can be stdout)
void symtable_print_to_file(hashtable *table, FILE *file);