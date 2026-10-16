#include "value.h"
#include "array.h"
#include "common.h"
#include <stdio.h>


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

    printf("%g", value);
}



