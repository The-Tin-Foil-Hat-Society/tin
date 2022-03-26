#pragma once

#include "ast.h"
#include "vector.h"

#include "backend/syscalls.h"
#include "backend/common.h"

void write_return( ast_node* node ) 
{
    add_instruction( "ret" );
    trace( "Return" );
    add_newline();
}