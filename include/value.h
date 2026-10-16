#pragma once

#include "utils.h"
#include "array.h"


typedef double Value;


typedef struct {

    arr(Value) values;

} ValueArray;


void init_value_array(ValueArray*);
void write_value_array(ValueArray*, Value);
void free_value_array(ValueArray*);
void valprintf(Value);




