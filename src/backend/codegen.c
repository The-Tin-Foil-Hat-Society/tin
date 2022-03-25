#include <stdarg.h>

#include "codegen.h"
#include "ast.h"

//
// Define string table
//
#define MAX_STRING_COUNT 1024
char *string_table[MAX_STRING_COUNT];
int string_table_index = 0;

//
// Define instruction table
//
#define MAX_INSTRUCTION_COUNT 4096
char *instruction_table[MAX_INSTRUCTION_COUNT];
int instruction_table_index = 0;

void add_instruction( const char* fmt, ... ) 
{
    va_list args;

    va_start( args, fmt );
    char* instruction = malloc( 1024 );
    vsprintf( instruction, fmt, args );
    va_end( args );

    instruction_table[instruction_table_index++] = instruction;
}

#define add_string( str ) string_table[string_table_index++] = str
#define write_to_file( ... ) fprintf( file, __VA_ARGS__ )

void walk_through_nodes( FILE* file, ast_node* node ) 
{
    for (int i = 0; i < node->children->size; i++)
    {
        ast_node* child = vector_get_item( node->children, i );

        switch (child->type)
        {
            case AstPrint: 
            {
                // String is a child of this node
                ast_node* string_node = vector_get_item( child->children, 0 );
                add_string( string_node->value.string );

                add_instruction( "# Print node\n" );
                add_instruction( "li a0, 4\n" );
                add_instruction( "la a1, str_%d\n", string_table_index - 1 );
                add_instruction( "ecall\n" );
                add_instruction( "\n" );
                break;
            }
        }

        walk_through_nodes( file, child );
    }
}

void write_string_table( FILE* file ) 
{
    // Write strings
    for (int i = 0; i < string_table_index; i++)
    {
        write_to_file( "str_%d:\n", i );
        write_to_file( "\t.string \"%s\\n\"\n", string_table[i] );
    }
}

void write_instructions( FILE* file ) 
{
    // Write instructions
    for (int i = 0; i < instruction_table_index; i++)
    {
        write_to_file( "\t%s", instruction_table[i] );
    }
}

bool codegen_generate( module* mod, ast_node* node, FILE* file ) 
{
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
    // String table
    //
    write_to_file( ".rodata\n" );
    write_string_table( file );

    //
    // Text section
    //
    write_to_file( ".text\n" );
    write_to_file( "\n" );

    //
    // Program entry point
    //
    write_to_file( "__start:\n" );
    write_instructions( file );

    //
    // Program exit point
    //
    write_to_file( "\tli a0, 10\n" );
    write_to_file( "\tecall\n" );
}