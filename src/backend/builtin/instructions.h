#pragma once

#include <malloc.h>
#include <vector.h>
#include <stdio.h>
#include <stdarg.h>

typedef struct instruction instruction;

struct instruction
{
    char* text;
};

vector* instructions;

void instructions_init();
void instructions_write( FILE* file );
void instructions_add( char* text, ... );
void instructions_add_n( char* text, ... );

/*
 * Debugging macros
 */
#ifdef TIN_DEBUG_VERBOSE
    #define add_comment( fmt, ... ) \
        instructions_add_n( "# " fmt, ##__VA_ARGS__ )
#else
    #define add_comment( ... )
#endif // TIN_DEBUG_VERBOSE
