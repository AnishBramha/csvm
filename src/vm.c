#include "debug.h"
#include "chunk.h"
#include "utils.h"
#include "vm.h"
#include "common.h"
#include "value.h"
#include "debug.h"
#include "compiler.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


VM vm;


static void reset_stack(void) {

    vm.stackTop = vm.stack;
}


void boot_vm(void) {

    reset_stack();
}


void shutdown_vm(void) {


}


static InterpretResult run(void) {

    #define read_byte() (*vm.ip++)
    #define read_constant() (vm.chunk->constants.values[read_byte()])

    #define binary_op(op)     \
                              \
        do {                  \
                              \
            double b = pop(); \
            double a = pop(); \
            push(a op b);     \
                              \
        } while (false);

    loop {

        #ifdef DEBUG_TRACE_EXECUTION

        printf("          ");

        forrange(Value*, slot, vm.stack, vm.stackTop, 1) {

            printf("[ ");
            valprintf(*slot);
            printf(" ]");
        }
    
        putchar('\n');
        disassemble_instruction(vm.chunk, (int)(vm.ip - vm.chunk->code));

        #endif

        uint8_t instruction;
        switch (instruction = read_byte()) {

            case OP_CONSTANT: {

                Value constant = read_constant();
                push(constant);

            } break;

            case OP_ADD:      binary_op(+); break;
            case OP_SUBTRACT: binary_op(-); break;
            case OP_MULTIPLY: binary_op(*); break;
            case OP_DIVIDE:   binary_op(/); break;
            case OP_NEGATE:   push(-pop()); break;
            
            case OP_POW: {

                double b = pop();
                double a = pop();
                push(pow(a, b));

            } break;

            case OP_RETURN:

                valprintf(pop());
                putchar('\n');

                return INTERPRET_OK;
        }
    }

    #undef read_byte
    #undef read_constant
    #undef binary_op
}


InterpretResult interpret(const char* src) {

    Chunk chunk;
    init_chunk(&chunk);

    if (!compile(src, &chunk)) {

        free_chunk(&chunk);
        return INTERPRET_COMPILE_ERR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    free_chunk(&chunk);
    return result;
}


void push(Value value) {

    *vm.stackTop = value;
    vm.stackTop++;
}


Value pop(void) {

    return *(--vm.stackTop);
}





