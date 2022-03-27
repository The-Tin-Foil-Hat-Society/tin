#pragma once

#include "ast.h"
#include "vector.h"

#include "backend/syscalls.h"
#include "backend/common.h"

void write_func( ast_node* node ) 
{
    char* name = get_function_name( node );
    trace( "Writing function %s", name );
    write_function_prologue( name );
}