#include "backend/builtin/instructions.h"

#include <string.h>

void instructions_init()
{
    instructions = vector_new();
}

void instructions_add_n( char* text, ... ) {
    va_list args;

    va_start( args, text );
    char* instruction = malloc( 1024 );
    vsprintf( instruction, text, args );

    // Add newline to the end
    strcat( instruction, "\n" );
    va_end( args );

    vector_add_item( instructions, instruction );
}

void instructions_add( char* text, ... ) {
    va_list args;

    va_start( args, text );
    char* instruction = malloc( 1024 );
    vsprintf( instruction, text, args );

    // Add newline to the end
    strcat( instruction, "\n" );

    // Prefix with a tab
    char* prefixed = malloc( 1024 );
    sprintf( prefixed, "\t%s", instruction );

    va_end( args );

    vector_add_item( instructions, prefixed );
}

void instructions_write( FILE* file )
{
    for (int i = 0; i < instructions->size; i++)
    {
        char* instruction = (char*)instructions->items[i];
        fprintf( file, "%s", instruction );
    }
}