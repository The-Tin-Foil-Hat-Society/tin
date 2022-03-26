#pragma once

#include "ast.h"
#include "vector.h"
#include "backend/common.h"

void write_i32( ast_node* node ) 
{
    return;

    char* name = node->value.string;
    trace( "Added i32 '%s' to data table", name );

    add_data( "i32__%s: .word 0", name );
}