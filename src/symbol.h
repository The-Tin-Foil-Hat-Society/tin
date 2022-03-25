#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "vector.h"

typedef struct
{
    char* name;
    char* data_type;
    size_t pointer_level;
    bool is_initialised;
    bool is_function;

    void* value;  // for the interpreter
} symbol;

symbol* symbol_new(void);
void symbol_free(symbol* sym);

// prints a json representation of the symbol to the console
void symbol_print(symbol* sym);
// prints a json representation of the symbol to the given file
void symbol_print_to_file(symbol* sym, FILE* file);


// TODO: reimplement as a hashtable instead of a vector
symbol* symtable_find_symbol(vector* table, char* name);

// prints a json representation of the symbol table to the console
void symtable_print(vector* table);
// prints a json representation of the symbol table to the given file
void symtable_print_to_file(vector* table, FILE* file);