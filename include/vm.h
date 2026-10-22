#pragma once

#include "chunk.h"
#include "object.h"
#include "value.h"
#include "table.h"
#include <stdint.h>


#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)


typedef struct {

    ObjFunction* function;
    uint8_t* ip;
    Value* slots;

} CallFrame;


typedef struct VM {

    CallFrame frames[STACK_MAX];
    int frameCount;

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






