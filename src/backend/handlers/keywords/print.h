#pragma once

#include "ast.h"
#include "vector.h"

#include "backend/syscalls.h"
#include "backend/common.h"

void write_print( ast_node* node ) 
{
    ast_node* child_node = vector_get_item( node->children, 0 );

    add_comment( "Print" );

    instructions_add( "li a0, %d", PrintString );
    
    switch( child_node->type ) {
        case AstStringLit:
            instructions_add( "la a1, str_%d", 0 ); // TODO
            trace( "Print: string literal '%s'", child_node->value.string );
            break;
        case AstSymbol:
            instructions_add( "la a1, data_0" );
            trace( "Print: symbol data_0" );
            break;
        default:
            compiler_error( "Tried to print unrecognised type" );
            break;
    }

    instructions_add( "ecall" );
}