#include "object.h"
#include "array.h"
#include "common.h"
#include "table.h"
#include "value.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#define allocate_obj(T, objectT) (T*)allocate_object(sizeof(T), objectT)


static Obj* allocate_object(size_t size, ObjType type) {

    Obj* object = (Obj*)realloc(NULL, size);
    object->type = type;

    object->next = vm.objects;
    vm.objects = object;

    return object;
}


static ObjString* allocate_string(string chars, int len, uint32_t hash) {

    ObjString* ostring = allocate_obj(ObjString, OBJ_STRING);
    ostring->len = len;
    ostring->chars = chars;
    ostring->hash = hash;

    table_set(&vm.strings, ostring, nil_val);

    return ostring;
}


static uint32_t hash_string(const char* key, int len) {

    uint32_t hash = 2166126261u;

    forrange(int, i, 0, len, 1) {

        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }

    return hash;
}


ObjString* take_string(char* chars, int len) {

    uint32_t hash = hash_string(chars, len);
    ObjString* interned = table_find_string(&vm.strings, chars, len, hash);
    
    if (interned) {

        free(chars);
        chars = NULL;

        return interned;
    }

    return allocate_string(chars, len, hash_string(chars, len));
}


ObjString* copy_string(const char* chars, int len) {

    uint32_t hash = hash_string(chars, len);

    ObjString* interned = table_find_string(&vm.strings, chars, len, hash);
    if (interned)
        return interned;

    char* heapChars = (char*)calloc(len + 1, sizeof(char));
    memcpy(heapChars, chars, len);
    heapChars[len] = NIL;

    return allocate_string(heapChars, len, hash);
}


void objprintf(Value val) {

    switch (obj_type(val)) {
    
        case OBJ_STRING:

            printf("%s", as_cstring(val));
            break;
    }
}








