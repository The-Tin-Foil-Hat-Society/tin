#include "data_type.h"
#include <stdlib.h>

data_type* data_type_new()
{
    data_type* dtype = malloc(sizeof(data_type));

    dtype->name = 0;
    dtype->pointer_level = 0;

    return dtype;
}

void data_type_free(data_type* dtype)
{
    free(dtype->name);
    free(dtype);
}

data_type* data_type_copy(data_type* dtype)
{
    data_type* copy = data_type_new();

    copy->name = strdup(dtype->name);
    copy->pointer_level = dtype->pointer_level;

    return copy;
}


bool is_int(data_type* dtype)
{
    if (dtype->pointer_level > 0)
    {
        return false;
    }

    return strcmp(dtype->name, "i32") == 0
        || strcmp(dtype->name, "i16") == 0
        || strcmp(dtype->name, "i8") == 0
        || strcmp(dtype->name, "u32") == 0
        || strcmp(dtype->name, "u16") == 0
        || strcmp(dtype->name, "u8") == 0;
}

bool is_valid_int(data_type* dtype, int64_t value)
{
    if (dtype->pointer_level > 0)
    {
        return false;
    }

    if (strcmp(dtype->name, "i32") == 0 && ( value < INT32_MIN || value > INT32_MAX))
    {
        return false;
    }
    else if (strcmp(dtype->name, "i16") == 0 && ( value < INT16_MIN || value > INT16_MAX))
    {
        return false;
    }
    else if (strcmp(dtype->name, "i8") == 0 && ( value < INT8_MIN || value > INT8_MAX))
    {
        return false;
    }
    else if (strcmp(dtype->name, "u32") == 0 && ( value < 0 || value > UINT32_MAX))
    {
        return false;
    }
    else if (strcmp(dtype->name, "u16") == 0 && ( value < 0 || value > UINT16_MAX))
    {
        return false;
    }
    else if (strcmp(dtype->name, "u8") == 0 && ( value < 0 || value > UINT8_MAX))
    {
        return false;
    }
    return true;
}

bool is_string(data_type* dtype)
{
    return strcmp(dtype->name, "i8") == 0 && dtype->pointer_level == 1;
}

bool data_type_compare(data_type* dtype, data_type* other)
{
    return strcmp(dtype->name, other->name) == 0 && dtype->pointer_level == other->pointer_level;
}