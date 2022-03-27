#pragma once

#include "ast.h"
#include "vector.h"
#include "backend/common.h"

void write_string( ast_node* node ) 
{
    trace( "TODO: Add string '%s' to string table", node->value.string );
    rodata_add( "string", 0, 4, node->value.string );
}