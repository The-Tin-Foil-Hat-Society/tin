#pragma once

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

void vector_printf( vector* table, const char* fmt, ... )
{
    va_list args;

    va_start( args, fmt );
    char* instruction = malloc( 1024 );
    vsprintf( instruction, fmt, args );
    // Add newline to the end
    strcat( instruction, "\n" );
    va_end( args );
    
    vector_add_item( table, instruction );
}

/*
 * Prologue / epilogue
 * One main step to the calling convention is to add the prologue and epilogue.
 * This is where guarantees are made about the stack and registers.
 * These guarantees are:
 * - The stack pointer (sp) will have the same value when exiting the function
 *   as it did when entering the function.
 * - The s registers will have the same values when exiting the function as
 *   they did when entering the function.
 * - The function will return to the value stored in the return address
 *   register (ra).
 * 
 * Resources:
 * - https://inst.eecs.berkeley.edu/~cs61c/resources/RISCV_Calling_Convention.pdf
 */

void write_function_prologue( char* name ) 
{
    instructions_add_n( "%s:", name );
    instructions_add( "addi sp, sp, -16" );
    instructions_add( "sw ra, 12(sp)" );
    instructions_add( "sw s0, 8(sp)" );
    instructions_add( "addi s0, sp, 16" );
}

void write_function_epilogue( void )
{
    instructions_add( "lw ra, 12(sp)" );
    instructions_add( "lw s0, 8(sp)" );
    instructions_add( "addi sp, sp, 16" );
    instructions_add( "jr ra" );
}

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