#pragma once

#include <stdarg.h>

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

//
// Define data table
//
#define MAX_DATA_COUNT 4096
int data_table[MAX_DATA_COUNT];
int data_table_index = 0;

void add_instruction( const char* fmt, ... ) 
{
    va_list args;

    va_start( args, fmt );
    char* instruction = malloc( 1024 );
    vsprintf( instruction, fmt, args );
    // Add newline to the end
    strcat( instruction, "\n" );
    va_end( args );

    instruction_table[instruction_table_index++] = instruction;
}


#define add_newline() instruction_table[instruction_table_index++] = "\n"

#define add_comment( fmt, ... ) add_instruction( "# " fmt, ##__VA_ARGS__ )
#define add_string( str ) string_table[string_table_index++] = str
#define add_data( size ) data_table[data_table_index++] = size
#define write_to_file( ... ) fprintf( file, __VA_ARGS__ )

#ifdef TIN_DEBUG_VERBOSE
#define trace( ... ) printf( __VA_ARGS__ ); printf( "\n" )
#else
#define trace( ... )
#endif // TIN_COMPILE_VERBOSE