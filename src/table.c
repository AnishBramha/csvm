#include "table.h"
#include "common.h"
#include "value.h"
#include "object.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define TABLE_MAX_LOAD 0.75


void init_table(Table* table) {

    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}


void free_table(Table* table) {

    if (table->entries)
        free(table->entries);

    table->entries = NULL;

    init_table(table);
}


static Entry* find_entry(Entry* entries, int capacity, ObjString* key) {

    uint32_t idx = key->hash % capacity;
    Entry* tombstone = NULL;

    loop {

        Entry* entry = &entries[idx];

        if (!entry->key) {

            if (is_nil(entry->value))
                return tombstone ? tombstone : entry;

            else {

                if (!tombstone)
                    tombstone = entry;
            }

        } else if (entry->key == key)
            return entry;

        idx = (idx + 1) % capacity;
    }
}


static void adjust_capacity(Table* table, int capacity) {

    Entry* entries = (Entry*)calloc(capacity, sizeof(Entry));
    
    forrange(int, i, 0, capacity, 1) {

        entries[i].key = NULL;
        entries[i].value = nil_val;
    }

    table->count = 0;
    forrange(int, i, 0, table->capacity, 1) {

        Entry* entry = &table->entries[i];
        if (!entry->key)
            continue;

        Entry* dest = find_entry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;

        table->count++;
    }

    free(table->entries);
    table->entries = entries;
    table->capacity = capacity;
}


bool table_set(Table* table, ObjString* key, Value value) {

    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {

        int capacity = table->capacity < 8 ? 8 : table->capacity * 1.5;
        adjust_capacity(table, capacity);
    }

    Entry* entry = find_entry(table->entries, table->capacity, key);

    bool isNewKey = !entry->key;
    if (isNewKey && is_nil(entry->value))
        table->count++;

    entry->key = key;
    entry->value = value;

    return isNewKey;
}


void table_add_all(Table* from, Table* to) {

    forrange(int, i, 0, from->capacity, 1) {

        Entry* entry = &from->entries[i];
        if (entry->key)
            table_set(to, entry->key, entry->value);
    }
}


bool table_get(Table* table, ObjString* key, Value* value) {

    if (!table->count)
        return false;

    Entry* entry = find_entry(table->entries, table->capacity, key);
    if (!entry->key)
        return false;

    *value = entry->value;
    return true;
}


bool table_delete(Table* table, ObjString* key) {

    if (!table->count)
        return false;

    Entry* entry = find_entry(table->entries, table->capacity, key);
    if (!entry->key)
        return false;

    entry->key = NULL;
    entry->value = bool_val(true);
    
    return true;
}


ObjString* table_find_string(Table* table, const char* chars, int len, uint32_t hash) {

    if (!table->count)
        return NULL;

    uint32_t idx = hash % table->capacity;
    loop {

        Entry* entry = &table->entries[idx];

        if (!entry->key) {

            if (is_nil(entry->value))
                return NULL;

        } else if (entry->key->len == len && entry->key->hash == hash &&
            !memcmp(entry->key->chars, chars, len))
            return entry->key;

        idx = (idx + 1) % table->capacity;
    }
}









