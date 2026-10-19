#pragma once

#include "value.h"


typedef struct {

    ObjString* key;
    Value value;

} Entry;


typedef struct {

    int count;
    int capacity;
    Entry* entries;

} Table;


void init_table(Table*);
void free_table(Table*);
bool table_get(Table*, ObjString* key, Value* value);
bool table_set(Table*, ObjString* key, Value value);
bool table_delete(Table*, ObjString* key);
void table_add_all(Table* from, Table* to);
ObjString* table_find_string(Table*, const char* chars, int len, uint32_t hash);









