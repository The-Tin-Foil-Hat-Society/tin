#pragma once

#include <math.h>

#include "ast.h"
#include "vector.h"
#include "backend/common.h"

void write_string( ast_node* node ) 
{
    //
    // Adding string to string table is typically handled by AstDeclaration,
    // but if we're writing a raw string literal, we need to do it here.
    //
    // TODO: Implement AstDeclaration
    if ( node->parent->type == AstDeclaration )
    {
        trace( "Parent is AstDeclaration, skipping" );
        return;
    }

    int rodata_index = rodata->size;
    char* rodata_name = malloc( strlen( "STR" ) + 1 );
    sprintf( rodata_name, "STR%d", rodata_index );
    
    trace( "Adding string '%s' as read-only data '%s'", node->value.string, rodata_name );

    rodata_add( rodata_name, 4, node->value.string );
    free( rodata_name );
}