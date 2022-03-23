#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifndef SYMTABLE_DEFAULT_CAPACITY
#define SYMTABLE_DEFAULT_CAPACITY 8
#endif

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

typedef struct symtable symtable;
struct symtable {
    size_t symbols_size;
    size_t symbols_capacity;
    symbol** symbols;   // TODO: reimplement as a hashtable instead of a linear list
};

symtable* symtable_new(void);
void symtable_free(symtable* table);
void symtable_resize(symtable* table);
void symtable_add_symbol(symtable* table, symbol* sym);
symbol* symtable_find_symbol(symtable* table, char* name);

// prints a json representation of the symbol table to the console
void symtable_print(symtable* table);
// prints a json representation of the symbol table to the given file
void symtable_print_to_file(symtable* table, FILE* file);