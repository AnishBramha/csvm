#pragma once

#include "utils.h"
#include "array.h"
#include "value.h"


typedef enum {

    OP_CONSTANT,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_POP,
    OP_EQ,
    OP_DEFINE_GLOBAL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_GT,
    OP_LT,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_IOPUTF,
    OP_IOPUTFN,
    OP_POW,
    OP_RETURN,

} OpCode;


typedef struct {

    arr(uint8_t) code;
    arr(int) lines;
    ValueArray constants;

} Chunk;


void init_chunk(Chunk*);
void write_chunk(Chunk*, uint8_t byte, int line);
int add_constant(Chunk*, Value);

void free_chunk(Chunk*);




