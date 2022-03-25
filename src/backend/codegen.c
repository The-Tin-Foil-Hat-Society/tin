#include "codegen.h"

bool codegen_generate(module* mod, ast_node* node, FILE* file) 
{
    fprintf( file, "# \n" );
    fprintf( file, "# Auto-generated file \n" );
    fprintf( file, "# \n" );

    //
    // Definitions
    //
    fprintf( file, ".globl __start\n" );

    //
    // Program entry point
    //
    fprintf( file, "__start:\n" );
}