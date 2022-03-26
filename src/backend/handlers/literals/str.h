#pragma once

#include "ast.h"
#include "vector.h"
#include "backend/common.h"

void write_string( ast_node* node ) 
{
    trace( "Added string '%s' to string table", node->value.string );
    add_string( node->value.string );
}