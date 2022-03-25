#include "codegen.h"
#include "ast.h"

#define write_instruction( ... ) fprintf(file, __VA_ARGS__)

void write_string_table( FILE* file, ast_node* node ) 
{
    for (int i = 0; i < node->children->size; i++)
    {
        ast_node* child = vector_get_item( node->children, i );

        if (child->type == AstStringLit)
        {
            write_instruction( "str_0: \t.string \"%s\\n\"\n", child->value.string );
            write_instruction( "\n" );
        }

        write_string_table( file, child );
    }
}

void write_nodes( FILE* file, ast_node* node ) 
{
    for (int i = 0; i < node->children->size; i++)
    {
        ast_node* child = vector_get_item( node->children, i );

        if (child->type == AstPrint)
        {
            write_instruction( "\t# Print node\n" );
            write_instruction( "\tli a0, 4\n" );
            write_instruction( "\tla a1, str_0\n" );
            write_instruction( "\tecall\n" );
            write_instruction( "\n" );
        }

        write_nodes( file, child );
    }
}

bool codegen_generate(module* mod, ast_node* node, FILE* file) 
{
    write_instruction( "# \n" );
    write_instruction( "# Auto-generated file \n" );
    write_instruction( "# \n" );

    //
    // Definitions
    //
    write_instruction( ".globl __start\n" );
    write_instruction( "\n" );

    //
    // String table
    //
    write_instruction( ".rodata\n" );
    write_string_table( file, node );

    //
    // Text section
    //
    write_instruction( ".text\n" );
    write_instruction( "\n" );

    //
    // Program entry point
    //
    write_instruction( "__start:\n" );
    write_nodes( file, node );

    //
    // Program exit point
    //
    write_instruction( "\tli a0, 10\n" );
    write_instruction( "\tecall\n" );
}