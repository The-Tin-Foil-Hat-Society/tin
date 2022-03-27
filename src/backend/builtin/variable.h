#pragma once

#include "hashtable.h"

typedef struct variable variable;

struct variable
{
    char* name;
    int offset;
    int size;
    char* value;
};
