#pragma once

#include "ast.h"
#include "vector.h"

#include "backend/syscalls.h"
#include "backend/common.h"

void write_func_call( ast_node* node ) 
{
    char* name = NULL;

    for ( int i = 0; i < node->children->size; i++ )
    {
        ast_node* child = (ast_node*)vector_get_item( node->children, i );
        switch( child->type )  
        {
            case AstSymbol:
                name = child->value.symbol->name;

                if ( strcmp( name, "main" ) == 0 ) 
                {
                    add_comment( "Entry point", name );
                    name = "__start";
                }
                else 
                {
                    add_comment( "Call %s", name );
                }
                break;
            case AstScope:
                break;
            default:
                compiler_error( "Unknown child node %s in AstFunction\n", ast_type_names[ child->type ] );
                break;
        }
    }

    trace( "Function call %s", name );

    /*
     * 'call' pseudoinstruction is equivalent to:
     * auipc x6, offset[31:12]
     * jalr x1, x6, offset[11:0]
     */
    // TODO: Tail calls where posible
    add_instruction( "call %s", name ); 
    add_newline();
}