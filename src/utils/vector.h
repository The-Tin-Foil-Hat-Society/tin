#pragma once

#include <stddef.h>

#define VECTOR_DEFAULT_CAPACITY 8

typedef struct vector vector;
struct vector {
    size_t size;
    size_t capacity;
    void** items;
};

vector* vector_new();
void vector_free(vector* vec);
// do not forget that values will not be deep-copied
vector* vector_copy(vector* vec);

void vector_resize(vector* vec);
void vector_add_item(vector* vec, void* item);
void vector_set_item(vector* vec, size_t index, void* item);
void vector_insert_item(vector* vec, size_t index, void* item);
void* vector_get_item(vector* vec, size_t index);
size_t vector_get_item_index(vector* vec, void* item);
void vector_delete_item_at(vector* vec, size_t index);
void vector_delete_item(vector* vec, void* item);

void vector_push(vector* vec, void* item);
void* vector_pop(vector* vec);