#pragma once

#include "backend/syscalls.h"

#include "backend/builtin/instructions.h"
#include "backend/builtin/variable.h"
#include "backend/builtin/rodata.h"

#include <stdarg.h>

#ifdef TIN_DEBUG_VERBOSE
    #define trace( ... ) \
        printf( __VA_ARGS__ ); \
        printf( "\n" )
#else
    #define trace( ... )
#endif // TIN_DEBUG_VERBOSE

#define write_to_file( ... ) \
    fprintf( file, __VA_ARGS__ )

#define compiler_error( ... ) \
    fprintf( stderr, "Error: " __VA_ARGS__ ); \
    exit( 1 )

bool function_is_main( ast_node* node ) 
{
    return strcmp( node->value.symbol->name, "main" ) == 0;
}

char* get_function_name( ast_node* node ) 
{
    char* name = NULL;

    for ( int i = 0; i < node->children->size; i++ )
    {
        ast_node* child = (ast_node*)vector_get_item( node->children, i );
        switch( child->type )  
        {
            case AstSymbol:
                name = child->value.symbol->name;

                if ( function_is_main( child ) ) 
                {
                    name = "__start";
                }
                else 
                {
                    name = malloc( strlen( name ) + 4 );
                    sprintf( name, "fn__%s", child->value.symbol->name );
                }
                break;
            default:
                break;
        }
    }

    return name;
}