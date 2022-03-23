#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol.h"

#ifndef AST_DEFAULT_CAPACITY
#define AST_DEFAULT_CAPACITY 4
#endif

enum ast_node_type
{ 
    AstRoot,

    AstIdentifier,
    AstIntegerLit,
    AstStringLit,

    AstAdd,
    AstAlloc,
    AstArgumentList,
    AstAssignment,
    AstBlock,
    AstBreak,
    AstContinue,
    AstDataType,
    AstDeclaration,
    AstDefinition,
    AstDefinitionList,
    AstDiv,
    AstEqual,
    AstExp,
    AstFree,
    AstFunction,
    AstFunctionCall,
    AstGoto,
    AstGreaterThan,
    AstGreaterThanOrEqual,
    AstIdentifierDereference,
    AstIdentifierIndex,
    AstIdentifierReference,
    AstIf,
    AstInput,
    AstLessThan,
    AstLessThanOrEqual,
    AstMod,
    AstMul,
    AstNodeList,
    AstNotEqual,
    AstPrint,
    AstReturn,
    AstScope,
    AstSub,
    AstSymbol,
    AstWhile
};

// for printing purposes
static char ast_type_names[42][32] = { "AstRoot","AstIdentifier","AstIntegerLit","AstStringLit","AstAdd","AstAlloc","AstArgumentList","AstAssignment","AstBlock","AstBreak","AstContinue","AstDataType","AstDeclaration","AstDefinition","AstDefinitionList","AstDiv","AstEqual","AstExp","AstFree","AstFunction","AstFunctionCall","AstGoto","AstGreaterThan","AstGreaterThanOrEqual","AstIdentifierDereference","AstIdentifierIndex","AstIdentifierReference","AstIf","AstInput","AstLessThan","AstLessThanOrEqual","AstMod","AstMul","AstNodeList","AstNotEqual","AstPrint","AstReturn","AstScope","AstSub","AstSymbol","AstWhile" };

typedef struct ast_node ast_node; 
struct ast_node
{
    enum ast_node_type type;
    ast_node* parent;

    size_t children_size;
    size_t children_capacity;
    ast_node** children;    // TODO: reimplement as a hashtable instead of a linear list

    union 
    {
        int64_t integer;        // AstInteger
        char* string;           // AstDataType, AstIdentifier, AstString
        symbol* symbol;         // AstSymbol
        symtable* symbol_table; // AstRoot, AstScope
    } value; 

    size_t pointer_level;       // AstDataType
    char* src_line; // for debug
};

ast_node* ast_new(enum ast_node_type type);
void ast_free(ast_node* node);
ast_node* ast_copy(ast_node* node);

void ast_resize(ast_node* node);
void ast_add_child(ast_node* node, ast_node* child);
ast_node* ast_get_child(ast_node* node, int index);
int ast_get_child_index(ast_node* node, ast_node* child);
void ast_delete_child(ast_node* node, ast_node* child);

symtable* ast_get_closest_symtable(ast_node* node);
symbol* ast_find_symbol(ast_node* node, char* name);

char* ast_find_closest_src_line(ast_node* node);

// prints a json representation of the AST to the console
void ast_print(ast_node* node, bool recursive);
// prints a json representation of the AST to the given file
void ast_print_to_file(ast_node* node, FILE* file, bool recursive);