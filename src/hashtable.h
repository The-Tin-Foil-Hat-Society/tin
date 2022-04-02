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

void hashtable_resize(hashtable* table);
size_t hashtable_find_slot(hashtable* table, char* key);
void hashtable_set_item(hashtable* table, char* key, void* item);
void* hashtable_get_item(hashtable* table, char* key);
void hashtable_delete_item(hashtable* table, char* key);
vector* hashtable_to_vector(hashtable* table);