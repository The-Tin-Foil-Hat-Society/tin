#include "typechecking.h"

bool is_valid(char* type, int64_t value)
{
    if (strcmp(type, "i32") != 0 && ( value < INT32_MIN || value > INT32_MAX))
    {
        return false;
    }
    else if (strcmp(type, "i16") != 0 && ( value < INT16_MIN || value > INT16_MAX))
    {
        return false;
    }
    else if (strcmp(type, "i8") != 0 && ( value < INT8_MIN || value > INT8_MAX))
    {
        return false;
    }
    else if (strcmp(type, "u32") != 0 && ( value < 0 || value > UINT32_MAX))
    {
        return false;
    }
    else if (strcmp(type, "u16") != 0 && ( value < 0 || value > UINT16_MAX))
    {
        return false;
    }
    else if (strcmp(type, "u8") != 0 && ( value < 0 || value > UINT8_MAX))
    {
        return false;
    }
    return true;
}