#pragma once

#include "ast.h"
#include "vector.h"
#include "backend/common.h"

void write_print( ast_node* node ) 
{
    // String is a child of this node
    ast_node* string_node = vector_get_item( node->children, 0 );

    switch( string_node->type ) {
        case AstStringLit:
            add_string( string_node->value.string );

            add_comment( "Print" );
            add_instruction( "li a0, 4" );
            add_instruction( "la a1, str_%d", string_table_index - 1 );
            add_instruction( "ecall" );

            trace( "Print: string literal '%s'", string_node->value.string );
            break;
        case AstSymbol:
            add_comment( "Print" );
            add_instruction( "li a0, 4" );
            add_instruction( "la a1, data_0" );
            add_instruction( "ecall" );

            trace( "Print: symbol data_0" );
            break;
        default:
            trace( "Print: unknown; should throw error here?" );
            break;
    }
    
    end_instruction();
}