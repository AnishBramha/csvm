#include "debug.h"
#include "chunk.h"
#include "utils.h"
#include "vm.h"
#include "common.h"
#include "value.h"
#include "debug.h"
#include <stdio.h>


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
            double a = pop(); \
            double b = pop(); \
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


InterpretResult interpret(Chunk* chunk) {

    vm.chunk = chunk;
    vm.ip = vm.chunk->code;

    return run();
}


void push(Value value) {

    *vm.stackTop = value;
    vm.stackTop++;
}


Value pop(void) {

    return *(--vm.stackTop);
}





