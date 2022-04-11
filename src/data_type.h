#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

typedef struct data_type data_type;
struct data_type
{
    char* name;
    size_t pointer_level;
    size_t size;
    bool _signed;
};

data_type* data_type_new(char* name);
void data_type_free(data_type* dtype);
data_type* data_type_copy(data_type* dtype);

size_t get_size(data_type* dtype);

bool is_bool(data_type* dtype);
bool is_float(data_type* dtype);
bool is_valid_float(data_type* dtype, double value);
bool is_int(data_type* dtype);
bool is_valid_int(data_type* dtype, int64_t value);
bool is_string(data_type* dtype);

bool data_type_compare(data_type* dtype, data_type* other);