#pragma once

#include "backend/builtin/variable.h"
#include "hashtable.h"

#include <stdio.h>
#include <string.h>

hashtable* rodata;

void rodata_init();
variable* rodata_get( char* name );
void rodata_add( char* name, int size, char* value );

void rodata_write( FILE* file );
