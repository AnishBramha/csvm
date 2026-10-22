#pragma once

#include "chunk.h"
#include "value.h"
#include "table.h"


#define STACK_MAX 256


typedef struct {

    Chunk* chunk;
    uint8_t* ip;
    Value stack[STACK_MAX];
    Value* stackTop;
    Table globals;
    Table strings;
    Obj* objects;

} VM;


typedef enum {

    INTERPRET_OK,
    INTERPRET_COMPILE_ERR,
    INTERPRET_RUNTIME_ERR,

} InterpretResult;


extern VM vm;


void boot_vm(void);
void shutdown_vm(void);

InterpretResult interpret(const char* src);

void push(Value);
Value pop(void);






