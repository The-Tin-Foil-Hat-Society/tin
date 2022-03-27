#pragma once

#include "ast.h"
#include "vector.h"

#include "backend/syscalls.h"
#include "backend/common.h"

void write_input( ast_node* node ) 
{
    add_comment( "Read input" );
    
    instructions_add( "li a0, %d", ReadString );
    instructions_add( "la a1, data_%d", 0 ); // TODO
    instructions_add( "li a2, %d", 256 );
    instructions_add( "ecall" );

    trace( "Read input from data_%d", 0 );  // TODO
}