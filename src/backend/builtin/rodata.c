#include "backend/builtin/rodata.h"

#include <stdlib.h>

void rodata_init() 
{
    rodata = hashtable_new();
}

variable* rodata_get( char* name )
{
    for (int i = 0; i < rodata->size; i++)
    {
        variable* var = hashtable_get_item( rodata, name );
        if (strcmp(var->name, name) == 0)
        {
            return var;
        }
    }

    return NULL;
}

void rodata_add( char* name, int offset, int size, char* value )
{
    variable* var = malloc(sizeof(variable));
    var->name = name;
    var->offset = offset;
    var->size = size;
    var->value = value;
    
    hashtable_set_item( rodata, name, var );
}

void rodata_write( FILE* file ) 
{
    fprintf( file, ".rodata\n" );
    
    for (int i = 0; i < rodata->size; i++)
    {
        variable* var = (variable*)rodata->items[i];
        fprintf( file, "%s:\n", var->name );
        fprintf( file, "\t.string \"%s\"\n", var->value );
    }

    fprintf( file, "\n" );
}