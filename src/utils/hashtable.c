#include "hashtable.h"

#include <stdlib.h>
#include <string.h>

#define HASHTABLE_DEFAULT_CAPACITY 16
#define HASHTABLE_LOAD_CUTOFF 75
#define DJB2_INITIAL_VALUE 5381

// from http://www.cse.yorku.ca/~oz/hash.html
size_t djb2_hash(uint8_t* str)
{
    size_t hash = DJB2_INITIAL_VALUE;

    for (size_t i = 0; i < strlen(str); i++)
    {
        hash = (((hash << 5) + hash) ^ str[i]); /* hash(i) = hash(i - 1) * 33 ^ str[i] */
    }

    return hash;
}


hashtable* hashtable_new()
{
    hashtable* table = malloc(sizeof(hashtable));
    
    table->size = 0;
    table->capacity = HASHTABLE_DEFAULT_CAPACITY;

    // allocate space for keys and item pointers
    table->keys = malloc(sizeof(char*) * table->capacity);
    table->items = malloc(sizeof(void*) * table->capacity);
    
    memset(table->keys, 0, sizeof(char*) * table->capacity);
    memset(table->items, 0, sizeof(void*) * table->capacity);

    return table;
}
void hashtable_free(hashtable* table)
{
    if (table == 0)
    {
        return;
    }

    for (int i = 0; i < table->capacity; i++)
    {
        if (table->keys[i] != 0)
        {
            free(table->keys[i]);
        }
    }

    free(table->keys);
    free(table->items);
    free(table);
}
hashtable* hashtable_copy(hashtable* table)
{
    hashtable* copy = malloc(sizeof(hashtable));

    copy->size = 0;
    copy->capacity = table->capacity;
    
    copy->keys = malloc(sizeof(char*) * table->capacity);
    copy->items = malloc(sizeof(void*) * table->capacity);

    memset(copy->keys, 0, sizeof(char*) * copy->capacity);
    memset(copy->items, 0, sizeof(void*) * copy->capacity);

    for (int i = 0; i < table->capacity; i++)
    {
        if (table->keys[i] != 0)
        {
            copy->keys[i] = strdup(table->keys[i]);
            copy->items[i] = table->items[i];
        }
    }

    return copy;
}

void hashtable_resize(hashtable* table)
{
    size_t old_capacity = table->capacity; 
    char** old_keys = table->keys;
    void** old_items = table->items;

    table->capacity *= 2; // increase exponentially
    table->keys = malloc(sizeof(char*) * table->capacity);
    table->items = malloc(sizeof(void*) * table->capacity);

    memset(table->keys, 0, sizeof(char*) * table->capacity);
    memset(table->items, 0, sizeof(void*) * table->capacity);

    for (int i = 0; i < old_capacity; i++)
    {
        if (old_keys[i] != 0)
        {
            hashtable_set_item(table, old_keys[i], old_items[i]);
            free(old_keys[i]);
        }
    }

    free(old_keys);
    free(old_items);
}
size_t hashtable_find_slot(hashtable* table, char* key)
{
    size_t i = djb2_hash(key) % table->capacity;

    while(table->keys[i] != 0 && strcmp(key, table->keys[i]) != 0)
    {
        i = (i + 1) % table->capacity;
    }

    return i;
}
void hashtable_set_item(hashtable* table, char* key, void* item)
{
    size_t i = hashtable_find_slot(table, key);
    if (table->keys[i] != 0)
    {
        // existing key
        table->items[i] = item;
        return;
    }
    // otherwise new key

    // too many slots in use, which will slow down lookups due to collisions 
    if ((table->size * 100 / table->capacity) >= HASHTABLE_LOAD_CUTOFF)
    {
        hashtable_resize(table);
    }

    table->keys[i] = strdup(key);
    table->items[i] = item;
    table->size += 1;
}
void* hashtable_get_item(hashtable* table, char* key)
{
    size_t i = hashtable_find_slot(table, key);
    if (table->keys[i] == 0)
    {
        return 0;
    }
    return table->items[i];
}
void hashtable_delete_item(hashtable* table, char* key)
{
    size_t i = hashtable_find_slot(table, key);
    if (table->keys[i] == 0)
    {
        return;
    }

    free(table->keys[i]);
    table->keys[i] = 0;
    table->items[i] = 0;
    table->size -= 1;
}
vector* hashtable_to_vector(hashtable* table)
{
    vector* vec = vector_new();
    for (int i = 0; i < table->capacity; i++)
    {
        if (table->keys[i] != 0)
        {
            vector_add_item(vec, table->items[i]);
        }
    }

    return vec;
}