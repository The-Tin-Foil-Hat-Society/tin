#pragma once

#include "ast.h"
#include "vector.h"

#include "backend/syscalls.h"
#include "backend/common.h"

void write_asm( ast_node* node ) 
{
    ast_node* child_node = vector_get_item( node->children, 0 );

    add_instruction( "%s", node->value.string );
    trace( "ASM: '%s'", node->value.string );
}