#pragma once

#include "ast.h"
#include "vector.h"

#include "backend/syscalls.h"
#include "backend/common.h"

void write_func_call( ast_node* node ) 
{
    char* name = get_function_name( node );
    trace( "Function call %s", name );
    add_comment( "Call function %s", name );

    /*
     * 'call' pseudoinstruction is equivalent to:
     * auipc x6, offset[31:12]
     * jalr x1, x6, offset[11:0]
     */
    // TODO: Tail calls where posible
    instructions_add( "call %s", name );
}