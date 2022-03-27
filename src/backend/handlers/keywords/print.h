#pragma once

#include "ast.h"
#include "vector.h"

#include "backend/syscalls.h"
#include "backend/common.h"

void write_print( ast_node* node ) 
{
    ast_node* child_node = vector_get_item( node->children, 0 );
    
    switch( child_node->type ) 
    {
        case AstStringLit:
        {
            int rodata_index = rodata->size;
            trace( "Print: string literal '%s'", child_node->value.string );

            add_comment( "Print" );
            instructions_add( "li a0, %d", PrintString );
            instructions_add( "la a1, STR%d", rodata_index );
            instructions_add( "ecall" );

            break;
        }
        case AstSymbol:
            // TODO
            trace( "TODO: print symbol '%s'", child_node->value.symbol->name );
            break;
        default:
            compiler_error( "Tried to print unrecognised type" );
            break;
    }
}