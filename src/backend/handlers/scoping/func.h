#pragma once

#include "ast.h"
#include "vector.h"

#include "backend/syscalls.h"
#include "backend/common.h"

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
    
    add_comment( "Function prologue" );
    instructions_add( "addi sp, sp, -16" );
    instructions_add( "sw ra, 12(sp)" );
    instructions_add( "sw s0, 8(sp)" );
    instructions_add( "addi s0, sp, 16" );
}

void write_function_epilogue( char* name )
{
    add_comment( "Function epilogue" );

    if ( strcmp( name, "__start" ) == 0 )
    {
        instructions_add( "li a0, %d", Exit );
        instructions_add( "ecall" );
    }
    else
    {
        instructions_add( "lw ra, 12(sp)" );
        instructions_add( "lw s0, 8(sp)" );
        instructions_add( "addi sp, sp, 16" );
        instructions_add( "jr ra" );
    }
}

void write_func( ast_node* node ) 
{
    char* name = get_function_name( node );
    trace( "Writing function %s", name );
    write_function_prologue( name );
}
