#pragma once

#include "ast.h"
#include "vector.h"

#include "backend/common.h"

void write_i32( ast_node* node ) 
{
    return;

    char* name = node->value.string;
    trace( "TODO: Add i32 '%s' to data table", name );
}