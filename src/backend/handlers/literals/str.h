#pragma once

#include "ast.h"
#include "vector.h"
#include "backend/common.h"

void write_string( ast_node* node ) 
{
    add_string( node->value.string );
}