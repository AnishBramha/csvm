#include "array.h"
#include "debug.h"
#include "chunk.h"
#include "object.h"
#include "table.h"
#include "utils.h"
#include "vm.h"
#include "value.h"
#include "common.h"
#include "value.h"
#include "debug.h"
#include "compiler.h"
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


VM vm;


static Value peek(int distance);

static void reset_stack(void) {

    vm.stackTop = vm.stack;
}


static void runtime_error(const char* fmt, ...) {

    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);

    va_end(args);
    fputc('\n', stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];
    eprintf("[line %d] in script\n", line);

    reset_stack();
}


static void free_objects(void);
static void free_object(Obj*);


void boot_vm(void) {

    reset_stack();
    vm.objects = NULL;
    init_table(&vm.globals);
    init_table(&vm.strings);
}


void shutdown_vm(void) {

    free_table(&vm.globals);
    free_table(&vm.strings);
    free_objects();
}


static bool is_falsey(Value);
static void concatenate(void);


static InterpretResult run(void) {

    #define read_byte() (*vm.ip++)
    #define read_constant() (vm.chunk->constants.values[read_byte()])
    #define read_string() as_string(read_constant())

    #define binary_op(valueType, op)                          \
                                                              \
        do {                                                  \
                                                              \
            if (!is_number(peek(0)) || !is_number(peek(1))) { \
                                                              \
                runtime_error("Operands must be numbers.");   \
                return INTERPRET_RUNTIME_ERR;                 \
            }                                                 \
                                                              \
            double b = as_number(pop());                      \
            double a = as_number(pop());                      \
            push(valueType(a op b));                          \
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

            case OP_NIL:   push(nil_val);         break;
            case OP_TRUE:  push(bool_val(true));  break;
            case OP_FALSE: push(bool_val(false)); break;
            case OP_POP:   pop();                 break;

            case OP_DEFINE_GLOBAL: {

                ObjString* name = read_string();
                table_set(&vm.globals, name, peek(0));
                pop();

            } break;

            case OP_GET_GLOBAL: {

                ObjString* name = read_string();
                Value value;

                if (!table_get(&vm.globals, name, &value)) {

                    runtime_error("Undefined variable `%s`.", name->chars);
                    return INTERPRET_RUNTIME_ERR;
                }

                push(value);
                break;

            } break;

            case OP_SET_GLOBAL: {

                ObjString* name = read_string();
                
                if (table_set(&vm.globals, name, peek(0))) {

                    table_delete(&vm.globals, name);
                    runtime_error("Undefined variable `%s`.", name->chars);

                    return INTERPRET_RUNTIME_ERR;
                }

            } break;

            case OP_EQ: {
    
                Value b = pop();
                Value a = pop();
                push(bool_val(values_eq(a, b)));

            } break;

            case OP_GT:       binary_op(bool_val, >);   break;
            case OP_LT:       binary_op(bool_val, <);   break;

            case OP_ADD: {

                if (is_string(peek(0))  && is_string(peek(1)))
                    concatenate();

                else if (is_number(peek(0)) && is_number(peek(1))) {

                    double b = as_number(pop());
                    double a = as_number(pop());
                    push(number_val(a + b));

                } else {

                    runtime_error("Operands must be two numbers or two strings.");
                    return INTERPRET_RUNTIME_ERR;
                }

            } break;

            case OP_SUBTRACT: binary_op(number_val, -); break;
            case OP_MULTIPLY: binary_op(number_val, *); break;
            case OP_DIVIDE:   binary_op(number_val, /); break;

            case OP_NOT: push(bool_val(is_falsey(pop()))); break;

            case OP_NEGATE: {

                if (!is_number(peek(0))) {

                    runtime_error("Operand must be number.");
                    return INTERPRET_RUNTIME_ERR;
                }

                push(number_val(-as_number(pop())));

            } break;
            
            case OP_POW: {

                if (!is_number(peek(0)) || !is_number(peek(1))) {

                    runtime_error("Operands must be numbers.");
                    return INTERPRET_RUNTIME_ERR;
                }
                   
                double b = as_number(pop());
                double a = as_number(pop());
                push(number_val(pow(a, b)));

            } break;

            case OP_IOPUTF: {

                valprintf(pop());
                break;

            } break;

            case OP_IOPUTFN: {

                valprintf(pop());
                putchar('\n');
                break;

            } break;

            case OP_RETURN:
                return INTERPRET_OK;
        }
    }

    #undef read_byte
    #undef read_string
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


static Value peek(int distance) {

    return vm.stackTop[-1 - distance];
}


static bool is_falsey(Value val) {

    return is_nil(val) || (is_bool(val) && !as_bool(val));
}


static void concatenate(void) {

    ObjString* b = as_string(pop());
    ObjString* a = as_string(pop());

    int len = a->len + b->len;
    char* chars = (char*)calloc(len + 1, sizeof(char));
    memcpy(chars, a->chars, a->len);
    memcpy(chars + a->len, b->chars, b->len);
    chars[len] = NIL;

    ObjString* result = take_string(chars, len);
    push(obj_val(result));
}


static void free_objects(void) {

    Obj* object = vm.objects;

    while (object) {

        Obj* next = object->next;
        free_object(object);
        object = next;
    }
}


static void free_object(Obj* object) {

    switch (object->type) {
    
        case OBJ_STRING: {

            ObjString* ostring = (ObjString*)object;
            free(ostring->chars);
            free(object); object = NULL;

        } break;
    }
}








