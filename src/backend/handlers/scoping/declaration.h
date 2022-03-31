#pragma once

#include "ast.h"
#include "vector.h"

#include "backend/syscalls.h"
#include "backend/common.h"

void write_assignment( ast_node* node ) 
{
    size_t size = -1;
    char* name = NULL;
    char* type = NULL;
    char* opcode = NULL;

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
                
                //
                // Work out size in memory
                //
                if ( strcmp( type, "u8" ) == 0 || strcmp( type, "i8" ) == 0 )
                    size = 8;
                else if ( strcmp( type, "u16" ) == 0 || strcmp( type, "i16" ) == 0 )
                    size = 16;
                else if ( strcmp( type, "u32" ) == 0 || strcmp( type, "i32" ) == 0 )
                    size = 32;
                break;
            default:
                break;
        }
    }

    switch ( size ) 
    {
        case 8:
            opcode = "sb"; // Store byte
            break;
        case 16:
            opcode = "sh"; // Store half
            break;
        case 32:
            opcode = "sw"; // Store word
            break;
        default:
            break;
    }

    trace( "%s", node->src_line );
    trace( "\tSymbol: %s", name );
    trace( "\tType: %s", type );
    trace( "\tSize: %zd", size );
    trace( "\tOpcode: %s", opcode );
}