#pragma once

#include "ast.h"
#include "vector.h"

#include "backend/syscalls.h"
#include "backend/common.h"

void write_print( ast_node* node ) 
{
    ast_node* child_node = vector_get_item( node->children, 0 );

    add_comment( "Print" );
    add_instruction( "li a0, %d", PrintString );
    
    switch( child_node->type ) {
        case AstStringLit:
            add_instruction( "la a1, str_%d", string_table_index );
            trace( "Print: string literal '%s'", child_node->value.string );
            break;
        case AstSymbol:
            add_instruction( "la a1, data_0" );
            trace( "Print: symbol data_0" );
            break;
        default:
            trace( "Print: unknown; should throw error here?" );
            break;
    }

    add_instruction( "ecall" );    
    
    add_newline();
}