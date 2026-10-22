#pragma once


#include "chunk.h"
#include "utils.h"
#include "value.h"
#include <stdint.h>


extern struct VM vm;


#define obj_type(val)    (as_obj(val)->type)

#define is_function(val) is_obj_type(val, OBJ_FUNCTION)
#define is_string(val)   is_obj_type(val, OBJ_STRING)

#define as_function(val) ((ObjFunction*)as_obj(val))
#define as_string(val)   ((ObjString*)as_obj(val))
#define as_cstring(val)  (((ObjString*)as_obj(val))->chars)


typedef enum {

    OBJ_FUNCTION,
    OBJ_STRING,

} ObjType;


struct Obj {

    ObjType type;
    struct Obj* next;
};


typedef struct ObjFunction {

    Obj obj;
    int arity;
    Chunk chunk;
    ObjString* name;

} ObjFunction;


ObjFunction* new_function(void);


struct ObjString {

    Obj obj;
    int len;
    char* chars;
    uint32_t hash;
};


ObjString* take_string(char* chars, int len);
ObjString* copy_string(const char* chars, int len);
void objprintf(Value);


static inline bool is_obj_type(Value val, ObjType type) {

    return is_obj(val) && as_obj(val)->type == type;
}





