#include "object.h"
#include "array.h"
#include "common.h"
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


static ObjString* allocate_string(string chars, int len) {

    ObjString* ostring = allocate_obj(ObjString, OBJ_STRING);
    ostring->len = len;
    ostring->chars = chars;

    return ostring;
}


ObjString* take_string(char* chars, int len) {

    return allocate_string(chars, len);
}


ObjString* copy_string(const char* chars, int len) {

    char* heapChars = (char*)calloc(len + 1, sizeof(char));
    memcpy(heapChars, chars, len);
    heapChars[len] = NIL;

    return allocate_string(heapChars, len);
}


void objprintf(Value val) {

    switch (obj_type(val)) {
    
        case OBJ_STRING:

            printf("%s", as_cstring(val));
            break;
    }
}








