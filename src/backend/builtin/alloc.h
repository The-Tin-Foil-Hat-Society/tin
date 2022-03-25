#pragma once

#include "ast.h"
#include "vector.h"
#include "backend/common.h"

void write_alloc( ast_node* node ) 
{
    size_t size = -1;
    char* name = NULL;

    for ( int i = 0; i < node->children->size; i++ )
    {
        ast_node* child = (ast_node*)vector_get_item( node->children, i );
        switch( child->type )  
        {
            case AstSymbol:
                break;
            case AstIntegerLit:
                size = child->value.integer;
                break;
            default:
                break;
        }
    }

    trace( "Alloc %s %zu", name, size );

    add_data( size );
}