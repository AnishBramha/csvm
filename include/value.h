#pragma once

#include "utils.h"
#include "array.h"


typedef enum {

    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,

} ValueType;


typedef struct {

    ValueType type;

    union {

        bool boolean;
        double number;

    } as;

} Value;


#define bool_val(val)   ((Value) { VAL_BOOL, { .boolean = val }, })
#define nil_val         ((Value) { VAL_NIL, { .number = 0 }, })
#define number_val(val) ((Value) { VAL_NUMBER, { .number = val }, })

#define as_bool(val)   ((val).as.boolean)
#define as_number(val) ((val).as.number)


#define is_bool(val)   ((val).type == VAL_BOOL)
#define is_nil(val)    ((val).type == VAL_NIL)
#define is_number(val) ((val).type == VAL_NUMBER)


typedef struct {

    arr(Value) values;

} ValueArray;


bool values_eq(Value, Value);
void init_value_array(ValueArray*);
void write_value_array(ValueArray*, Value);
void free_value_array(ValueArray*);
void valprintf(Value);




