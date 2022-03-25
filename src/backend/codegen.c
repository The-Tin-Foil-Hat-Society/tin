#include "codegen.h"
#include "ast.h"

void write_string_table( FILE* file, ast_node* node ) 
{
    for (int i = 0; i < node->children->size; i++)
    {
        ast_node* child = vector_get_item( node->children, i );

        if (child->type == AstStringLit)
        {
            fprintf( file, "\t.string \"%s\\n\"\n", child->value.string );
        }

        write_string_table( file, child );
    }
}

bool codegen_generate(module* mod, ast_node* node, FILE* file) 
{
    fprintf( file, "# \n" );
    fprintf( file, "# Auto-generated file \n" );
    fprintf( file, "# \n" );

    //
    // Definitions
    //
    fprintf( file, ".globl __start\n" );
    fprintf( file, "\n" );

    //
    // String table
    //
    fprintf( file, ".rodata\n" );
    write_string_table( file, node );

    // traverse through each node, find nodes that match ast_node_type::AstPrint
    // and print their value.

    fprintf( file, "\n" );

    //
    // Program entry point
    //
    fprintf( file, "__start:\n" );
}