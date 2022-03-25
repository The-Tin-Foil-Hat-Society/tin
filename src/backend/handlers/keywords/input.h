#pragma once

#include "ast.h"
#include "vector.h"
#include "backend/common.h"

void write_input( ast_node* node ) 
{
    add_comment( "Read input" );
    add_instruction( "li a0, 8" );
    add_instruction( "la a1, data_%d", data_table_index - 1 );
    add_instruction( "li a2, %d", 256 );
    add_instruction( "ecall" );

    trace( "Read input from data_%d", data_table_index - 1 );

    add_newline();
}