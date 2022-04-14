#include "vector.h"
#include <stdlib.h>
#include <string.h>

vector* vector_new()
{
    vector* vec = malloc(sizeof(vector));

    vec->size = 0;
    vec->capacity = VECTOR_DEFAULT_CAPACITY;

    // allocate space for item pointers
    vec->items = malloc(sizeof(vector*) * vec->capacity);
    memset(vec->items, 0, sizeof(void*) * vec->capacity);

    return vec;
}
void vector_free(vector* vec)
{
    if (vec == 0)
    {
        return;
    }

    free(vec->items);
    free(vec);
}
vector* vector_copy(vector* vec)
{
    vector* copy = malloc(sizeof(vector));

    copy->size = vec->size;
    copy->capacity = vec->capacity;
    copy->items = malloc(sizeof(vector*) * vec->capacity);

    memset(copy->items, 0, copy->capacity);

    for (size_t i = 0; i < vec->size; i++)
    {
        copy->items[i] = vec->items[i];
    }

    return copy;
}

void vector_resize(vector* vec)
{
    size_t new_capacity = vec->capacity * 2; // increase capacity exponentially

    void** new_items = malloc(sizeof(vector*) * new_capacity);
    memcpy(new_items, vec->items, sizeof(vector*) * vec->capacity); // copy old array to the new location
    free(vec->items); // free the old array

    vec->capacity = new_capacity;
    vec->items = new_items;
}
void vector_add_item(vector* vec, void* item)
{
    if (vec->size == vec->capacity)
    {
        // need more space to add another item
        vector_resize(vec);
    }

    vec->items[vec->size] = item;
    vec->size += 1;
}
void vector_set_item(vector* vec, size_t index, void* item)
{
    if (index >= vec->size)
    {
        return;
    }
    
    vec->items[index] = item;
}
void vector_insert_item(vector* vec, size_t index, void* item)
{
    if (index >= vec->size)
    {
        return;
    }

    for (int i = vec->size; i > index; i--)
    {
        vec->items[i] = vec->items[i - 1];
    }

    vec->items[index] = item;
    vec->size += 1;
}
void* vector_get_item(vector* vec, size_t index)
{
    if (index >= vec->size)
    {
        return 0;
    }
    
    return vec->items[index];
}
size_t vector_get_item_index(vector* vec, void* item)
{
    size_t item_i = -1;

    for (size_t i = 0; i < vec->size; i++)
    {
        if (vec->items[i] == item)
        {
            item_i = i;
            break;
        }
    }

    return item_i;
}
void vector_delete_item_at(vector* vec, size_t index)
{
    vec->size -= 1;

    for (int i = index; i < vec->size; i++)
    {
        vec->items[i] = vec->items[i+1];
    }
    vec->items[vec->size] = 0;
}
void vector_delete_item(vector* vec, void* item)
{
    size_t item_i = vector_get_item_index(vec, item);

    if (item_i != -1)
    {
        vector_delete_item_at(vec, item_i);
    }
}
void vector_push(vector* vec, void* item)
{
    vector_add_item(vec, item);
}
void* vector_pop(vector* vec)
{
    if (vec->size == 0)
    {
        return 0;
    }

    void* item = vec->items[vec->size - 1];
    vector_delete_item_at(vec, vec->size - 1);
}
