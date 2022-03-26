#pragma once

#include "ast.h"
#include "vector.h"

#include "backend/syscalls.h"
#include "backend/common.h"

void write_func( ast_node* node ) 
{
    char* name = NULL;

    add_newline();

    for ( int i = 0; i < node->children->size; i++ )
    {
        ast_node* child = (ast_node*)vector_get_item( node->children, i );
        switch( child->type )  
        {
            case AstSymbol:
                name = child->value.symbol->name;

                if ( strcmp( name, "main" ) == 0 ) 
                {
                    add_comment( "Entry point", name );
                    name = "__start";
                }
                else 
                {
                    add_comment( "Function %s", name );
                }
                break;
            case AstScope:
                break;
            default:
                compiler_error( "Unknown child node %s in AstFunction\n", ast_type_names[ child->type ] );
                break;
        }
    }

    trace( "Writing function %s", name );
    add_function( name );
}