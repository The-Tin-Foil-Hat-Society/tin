#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_type.h"
#include "hashtable.h"
#include "symbol.h"
#include "vector.h"


// TODO: document child count and types for each node type
enum ast_node_type
{ 
    AstRoot,

    AstIdentifier,
    AstBoolLit,
    AstIntegerLit,
    AstStringLit,

    AstOffset,
    AstReference,
    AstDereference,

    AstAdd,
    AstAlloc,
    AstAnd,
    AstArgumentList,
    AstAsm,
    AstAssignment,
    AstBlock,
    AstBreak,
    AstContinue,
    AstDataType,
    AstDefinition,
    AstDefinitionList,
    AstDiv,
    AstEqual,
    AstFree,
    AstFunction,
    AstFunctionCall,
    AstGoto,
    AstGreaterThan,
    AstGreaterThanOrEqual,
    AstIf,
    AstInput,
    AstLessThan,
    AstLessThanOrEqual,
    AstMod,
    AstMul,
    AstNot,
    AstNotEqual,
    AstOr,
    AstPow,
    AstPrint,
    AstReturn,
    AstScope,
    AstSub,
    AstSymbol,
    AstWhile
};

// for printing purposes
static char ast_type_names[44][32] = { "AstRoot","AstIdentifier","AstBoolLit","AstIntegerLit","AstStringLit","AstOffset","AstReference","AstDereference","AstAdd","AstAlloc","AstAnd","AstArgumentList","AstAsm","AstAssignment","AstBlock","AstBreak","AstContinue","AstDataType","AstDefinition","AstDefinitionList","AstDiv","AstEqual","AstFree","AstFunction","AstFunctionCall","AstGoto","AstGreaterThan","AstGreaterThanOrEqual","AstIf","AstInput","AstLessThan","AstLessThanOrEqual","AstMod","AstMul","AstNot","AstNotEqual","AstOr","AstPow","AstPrint","AstReturn","AstScope","AstSub","AstSymbol","AstWhile" };

typedef struct ast_node ast_node; 
struct ast_node
{
    enum ast_node_type type;
    ast_node* parent;

    vector* children;

    union 
    {
        bool boolean;            // AstBoolLit
        data_type* dtype;        // AstDataType, AstAdd, AstDiv, AstMod, AstMul, AstPow, AstSub
        int64_t integer;         // AstIntegerLit
        char* string;            // AstAsm, AstIdentifier, AstStringLit
        symbol* symbol;          // AstSymbol
        hashtable* symbol_table; // AstRoot, AstScope
    } value; 

    char* src_line; // for debug
};

ast_node* ast_new(enum ast_node_type type);
void ast_free(ast_node* node);
ast_node* ast_copy(ast_node* node);

void ast_resize(ast_node* node);
void ast_add_child(ast_node* node, ast_node* child);
void ast_set_child(ast_node* node, size_t index, ast_node* new_child);
ast_node* ast_get_child(ast_node* node, size_t index);
size_t ast_get_child_index(ast_node* node, ast_node* child);
void ast_delete_child(ast_node* node, ast_node* child);

ast_node* ast_get_current_function(ast_node* node);
hashtable* ast_get_closest_symtable(ast_node* node);
// searched for the closest data type in the children and their children (used for expressions)
data_type* ast_find_data_type(ast_node* node);
symbol* ast_find_symbol(ast_node* node, char* name);

char* ast_find_closest_src_line(ast_node* node);

// prints a json representation of the AST to the console
void ast_print(ast_node* node, bool recursive);
// prints a json representation of the AST to the given file
void ast_print_to_file(ast_node* node, FILE* file, bool recursive);