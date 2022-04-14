#include "data_type.h"
#include <stdlib.h>
#include <float.h>

data_type* data_type_new(char* name)
{
    data_type* dtype = malloc(sizeof(data_type));

    dtype->name = strdup(name);
    dtype->pointer_level = 0;
    dtype->size = get_size(dtype);
    dtype->_signed = false;

    if (strcmp(name, "i8") == 0 || strcmp(name, "i16") == 0 || strcmp(name, "i32") == 0 || strcmp(name, "i64") == 0)
    {
        dtype->_signed = true;
    }

    return dtype;
}

void data_type_free(data_type* dtype)
{
    if (dtype == 0)
    {
        return;
    }

    free(dtype->name);
    free(dtype);
}

data_type* data_type_copy(data_type* dtype)
{
    data_type* copy = data_type_new(dtype->name);

    copy->pointer_level = dtype->pointer_level;
    copy->_signed = dtype->_signed;

    return copy;
}

size_t get_size(data_type* dtype)
{
    if (dtype->pointer_level > 0)
    {
        return 4;
    }
    else if (strcmp(dtype->name, "void") == 0)
    {
        return 8;
    }
    else if (strcmp(dtype->name, "i8") == 0 || strcmp(dtype->name, "u8") == 0 || strcmp(dtype->name, "bool") == 0)
    {
        return 1;
    }
    else if (strcmp(dtype->name, "i16") == 0 || strcmp(dtype->name, "u16") == 0)
    {
        return 2;
    }
    else if (strcmp(dtype->name, "i32") == 0 || strcmp(dtype->name, "u32") == 0 || strcmp(dtype->name, "f32") == 0)
    {
        return 4;
    }
    else if (strcmp(dtype->name, "i64") == 0 || strcmp(dtype->name, "u64") == 0 || strcmp(dtype->name, "f64") == 0)
    {
        return 8;
    }
}

bool is_bool(data_type* dtype)
{
    if (dtype->pointer_level > 0)
    {
        return false;
    }

    return strcmp(dtype->name, "bool") == 0;
}

bool is_float(data_type* dtype)
{
    if (dtype->pointer_level > 0)
    {
        return false;
    }

    return strcmp(dtype->name, "f32") == 0 || strcmp(dtype->name, "f64") == 0;
}

bool is_valid_float(data_type* dtype, double value)
{
    if (dtype->pointer_level > 0)
    {
        return false;
    }

    if (strcmp(dtype->name, "f64") == 0 && ( value < DBL_MIN || value > DBL_MAX))
    {
        return false;
    }
    else if (strcmp(dtype->name, "f32") == 0 && ( value < FLT_MIN || value > FLT_MAX))
    {
        return false;
    }
    return true;
}

bool is_int(data_type* dtype)
{
    if (dtype->pointer_level > 0)
    {
        return false;
    }

    return strcmp(dtype->name, "i64") == 0
        || strcmp(dtype->name, "i32") == 0
        || strcmp(dtype->name, "i16") == 0
        || strcmp(dtype->name, "i8") == 0
        || strcmp(dtype->name, "u64") == 0
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

    if (strcmp(dtype->name, "i64") == 0 && ( value < INT64_MIN || value > INT64_MAX))
    {
        return false;
    }
    else if (strcmp(dtype->name, "i32") == 0 && ( value < INT32_MIN || value > INT32_MAX))
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
    else if (strcmp(dtype->name, "u64") == 0 && ( *(uint64_t*)&value < 0 || *(uint64_t*)&value > UINT64_MAX))
    {
        return false;
    }
    else if (strcmp(dtype->name, "u32") == 0 && ( *(uint64_t*)&value < 0 || *(uint64_t*)&value > UINT32_MAX))
    {
        return false;
    }
    else if (strcmp(dtype->name, "u16") == 0 && ( *(uint64_t*)&value < 0 || *(uint64_t*)&value > UINT16_MAX))
    {
        return false;
    }
    else if (strcmp(dtype->name, "u8") == 0 && ( *(uint64_t*)&value < 0 || *(uint64_t*)&value > UINT8_MAX))
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