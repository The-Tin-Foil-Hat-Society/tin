#pragma once

#include "ast.h"
#include "vector.h"

#include "backend/syscalls.h"
#include "backend/common.h"

void write_input( ast_node* node ) 
{
    add_comment( "Read input" );
    
    add_instruction( "li a0, %d", ReadString );
    add_instruction( "la a1, data_%d", 0 ); // TODO
    add_instruction( "li a2, %d", 256 );
    add_instruction( "ecall" );

    trace( "Read input from data_%d", 0 );  // TODO

    add_newline();
}