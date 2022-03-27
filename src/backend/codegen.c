#include <time.h>

#include "codegen.h"
#include "ast.h"

#include "backend/common.h"
#include "backend/handlers/handlers.h"

#include "backend/builtin/rodata.h"

void walk_through_nodes( FILE* file, ast_node* node ) 
{
    for (int i = 0; i < node->children->size; i++)
    {
        ast_node* child = vector_get_item( node->children, i );

        trace( "\n==== Node %s ====", ast_type_names[child->type] );

        bool node_was_handled = true;

        switch (child->type)
        {
            //
            // Keywords
            //
            case AstPrint: 
                write_print( child );
                break;
            case AstInput:
                write_input( child );
                break;
            case AstAlloc:
                write_alloc( child );
                break;
            case AstAsm:
                write_asm( child );
                break;
            
            //
            // Literals
            //
            case AstStringLit:
                write_string( child );
                break;
            case AstIntegerLit:
                write_i32( child );
                break;

            //
            // Scoping
            //
            case AstFunction:
                write_func( child );
                break;
            case AstFunctionCall:
                write_func_call( child );
                break;
            case AstReturn:
                write_return( child );
                break;
            default:
                node_was_handled = false;
                trace( "Node wasn't handled" );
                break;
        }

        if ( node_was_handled )
            add_newline();

        walk_through_nodes( file, child );

        if ( child->type == AstFunction )
        {
            write_function_epilogue();
            add_comment( "End function" );
        }
    }
}

void init( )
{
    instructions_init();
    rodata_init();
}

bool codegen_generate( module* mod, ast_node* node, FILE* file ) 
{
    trace( "Running assembly codegen..." );

    // Initialise all tables
    init( );

    // Walk through each node so that we know what we're writing
    walk_through_nodes( file, node );

    write_to_file( "# \n" );
    write_to_file( "# Auto-generated file \n" );
    write_to_file( "# \n" );

    //
    // Definitions
    //
    write_to_file( ".globl __start\n" );
    write_to_file( "\n" );

    //
    // Read-only data
    //
    rodata_write( file );

    //
    // Text section
    //
    write_to_file( ".text\n" );
    write_to_file( "\n" );

    //
    // Instructions
    //
    instructions_write( file );

    //
    // Program exit point
    //
    write_to_file( "\tli a0, 10\n" );
    write_to_file( "\tecall\n" );
}