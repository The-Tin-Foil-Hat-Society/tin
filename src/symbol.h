#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "data_type.h"
#include "hashtable.h"
#include "vector.h"

typedef struct
{
    char* name;

    data_type* dtype;

    bool is_initialised;
    bool is_function;

    void* function_node;  // for the interperter to follow function symbols
} symbol;

symbol* symbol_new(void);
void symbol_free(symbol* sym);

// prints a json representation of the symbol to the console
void symbol_print(symbol* sym);
// prints a json representation of the symbol to the given file
void symbol_print_to_file(symbol* sym, FILE* file);


// prints a json representation of the symbol table to the console
void symtable_print(hashtable* table);
// prints a json representation of the symbol table to the given file
void symtable_print_to_file(hashtable* table, FILE* file);