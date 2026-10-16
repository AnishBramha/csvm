#include "chunk.h"
#include "array.h"
#include "common.h"
#include "value.h"


void init_chunk(Chunk* chunk) {

    chunk->code = NULL;
    chunk->lines = NULL;
    unused(chunk);

    init_value_array(&chunk->constants);
}


void write_chunk(Chunk* chunk, uint8_t byte, int line) {

    arr_push(chunk->code, byte);
    arr_push(chunk->lines, line);
}


void free_chunk(Chunk* chunk) {

    arr_free(chunk->code);
    arr_free(chunk->lines);
    free_value_array(&chunk->constants);

    init_chunk(chunk);
}


int add_constant(Chunk* chunk, Value value) {

    write_value_array(&chunk->constants, value);
    return (int)arr_len(chunk->constants.values) - 1;
}



