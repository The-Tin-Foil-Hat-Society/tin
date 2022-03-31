#pragma once

#include "ast.h"
#include "vector.h"

#include "backend/syscalls.h"
#include "backend/common.h"

void write_symbol( ast_node* node ) 
{
    //
    // Tells the compiler "hey, we want this now, please move it
    // to an appropriate register".
    //

    // Find the symbol in the node symbol table
    symbol* symbol = node->value.symbol;

    // TODO: Just move the symbol to a0 for now 
    // (TODO: we'll handle more than this later)

    trace( "TODO: Symbol" );
}