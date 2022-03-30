#pragma once

#include "ast.h"
#include "vector.h"

#include "backend/syscalls.h"
#include "backend/common.h"

void write_declaration( ast_node* node ) 
{
    trace( "Declaration:" );
    
    size_t size = -1;
    char* name = NULL;
    char* type = NULL;

    for ( int i = 0; i < node->children->size; i++ )
    {
        ast_node* child = (ast_node*)vector_get_item( node->children, i );
        switch( child->type )  
        {
            case AstSymbol:
                name = child->value.symbol->name;
                type = child->value.symbol->data_type;
                if ( !child->value.symbol->is_initialised )
                {
                    trace( "TODO: Can't do anything with uninitialized symbols yet");
                }

                trace( "\tSymbol: %s", name );
                trace( "\tType: %s", type );
                break;
            case AstStringLit:
                size = sizeof( child->value.string );
                break;
            case AstIntegerLit:
                if ( strcmp( type, "u8" ) == 0 || strcmp( type, "i8" ) == 0 )
                    size = 8;
                else if ( strcmp( type, "u16" ) == 0 || strcmp( type, "i16" ) == 0 )
                    size = 16;
                else if ( strcmp( type, "u32" ) == 0 || strcmp( type, "i32" ) == 0 )
                    size = 32;
                else
                    trace( "TODO: Can't do anything with this integer literal yet" );
                break;
            default:
                trace( "Tried to declare unrecognised type" );
                break;
        }

        switch ( size ) {
            case 8:
                trace( "\tSize: 8" );
                break;
            case 16:
                trace( "\tSize: 16" );
                break;
            case 32:
                trace( "\tSize: 32" );
                break;
            default:
                trace( "\tSize: %d", size );
                break;
        }
    }
}