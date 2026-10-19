#include "value.h"
#include "array.h"
#include "common.h"
#include "object.h"
#include <stdio.h>
#include <string.h>


void init_value_array(ValueArray* array) {

    array->values = NULL;
    unused(array);
}


void write_value_array(ValueArray* array, Value value) {

    arr_push(array->values, value);
}


void free_value_array(ValueArray* array) {

    arr_free(array->values);
    init_value_array(array);
}


void valprintf(Value value) {

    switch (value.type) {

        case VAL_BOOL:

            printf(as_bool(value) ? "true" : "false");
            break;

        case VAL_NIL:

            printf("nil");
            break;

        case VAL_NUMBER:

            printf("%g", as_number(value));
            break;

        case VAL_OBJ:

            objprintf(value);
            break;

        default: unreachable();
    }
}


bool values_eq(Value a, Value b) {

    if (a.type != b.type)
        return false;

    switch (a.type) {
    
        case VAL_BOOL:   return as_bool(a) == as_bool(b);
        case VAL_NIL:    return true;
        case VAL_NUMBER: return as_number(a) == as_number(b);
        case VAL_OBJ:    return as_obj(a) == as_obj(b);

        default: unreachable();
    }
}





