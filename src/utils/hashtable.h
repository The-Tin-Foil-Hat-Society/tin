#pragma once

#include "vector.h"
#include <inttypes.h>
#include <stddef.h>

typedef struct hashtable hashtable;
struct hashtable
{
    size_t size;
    size_t capacity;

    char** keys;
    void** items;
};

hashtable* hashtable_new();
void hashtable_free(hashtable* table);
// do not forget that values will not be deep-copied
hashtable* hashtable_copy(hashtable* table);

void hashtable_resize(hashtable* table);
size_t hashtable_find_slot(hashtable* table, char* key);
void hashtable_set_item(hashtable* table, char* key, void* item);
void* hashtable_get_item(hashtable* table, char* key);
// do not forget that only the entry is freed, the memory being referenced will NOT be freed
void hashtable_delete_item(hashtable* table, char* key);
// returns a vector rendition of the hashtable, useful for iterating over all items
vector* hashtable_to_vector(hashtable* table);