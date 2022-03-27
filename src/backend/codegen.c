#include <time.h>

#include "codegen.h"
#include "ast.h"

#include "backend/common.h"

#include "backend/handlers/keywords/print.h"
#include "backend/handlers/keywords/input.h"
#include "backend/handlers/keywords/asm.h"
#include "backend/handlers/keywords/alloc.h"

#include "backend/handlers/literals/str.h"
#include "backend/handlers/literals/i32.h"

#include "backend/handlers/scoping/func.h"
#include "backend/handlers/scoping/func_call.h"
#include "backend/handlers/scoping/return.h"

void walk_through_nodes( FILE* file, ast_node* node ) 
{
    for (int i = 0; i < node->children->size; i++)
    {
        ast_node* child = vector_get_item( node->children, i );

        trace( "\n==== Node %s ====", ast_type_names[child->type] );

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
                trace( "Node wasn't handled" );
                break;
        }

        walk_through_nodes( file, child );

        if ( child->type == AstFunction )
        {
            write_function_epilogue();
            add_comment( "End function" );
        }
    }
}

void write_string_table( FILE* file ) 
{
    trace( "Writing string table" );

    // Write strings
    for (int i = 0; i < string_table_index; i++)
    {
        write_to_file( "str_%d:\n", i );
        write_to_file( "\t.string \"%s\"\n", string_table[i] );
    }
}

void write_instructions( FILE* file ) 
{
    trace( "Writing instructions" );

    // Write instructions
    for (int i = 0; i < instruction_table_index; i++)
    {
        write_to_file( "%s", instruction_table[i] );
    }
}

void write_data_table( FILE* file ) 
{
    trace( "Writing data table" );

    // Write data
    for (int i = 0; i < data_table_index; i++)
    {
        write_to_file( "%s", data_table[i] );
    }
}

bool codegen_generate( module* mod, ast_node* node, FILE* file ) 
{
    trace( "Running assembly codegen..." );

    // Walk through each node so that we know what we're writing
    walk_through_nodes( file, node );

    write_to_file( "# \n" );
    write_to_file( "# Auto-generated file \n" );

    // Add date/time
    time_t t = time( NULL );
    struct tm* time = localtime( &t );
    write_to_file( "# Generated on %s", asctime( time ) );
    write_to_file( "# \n" );

    //
    // Definitions
    //
    write_to_file( ".globl __start\n" );
    write_to_file( "\n" );

    //
    // Data table
    //
    write_to_file( ".data\n" );
    write_data_table( file );
    write_to_file( "\n" );

    //
    // String table
    //
    write_to_file( ".rodata\n" );
    write_string_table( file );
    write_to_file( "\n" );

    //
    // Text section
    //
    write_to_file( ".text\n" );
    write_to_file( "\n" );

    //
    // Instructions
    //
    write_instructions( file );

    //
    // Program exit point
    //
    write_to_file( "\tli a0, 10\n" );
    write_to_file( "\tecall\n" );
}