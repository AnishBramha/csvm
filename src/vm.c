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
    vm.frameCount = 0;
}


static void runtime_error(const char* fmt, ...) {

    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);

    va_end(args);
    fputc('\n', stderr);

    downrange(int, i, vm.frameCount - 1, -1, 1) {

        CallFrame* frame = &vm.frames[i];
        ObjFunction* function = frame->function;
        size_t instruction = frame->ip - function->chunk.code - 1;

        eprintf("[line %d] in ", function->chunk.lines[instruction]);

        if (!function->name)
            eprintf("script\n");

        else
            eprintf("%s()\n", function->name->chars);
    }

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
static bool call_value(Value callee, int argc);


static InterpretResult run(void) {

    CallFrame* frame = &vm.frames[vm.frameCount - 1];

    #define read_byte() (*frame->ip++)
    #define read_short() (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))
    #define read_constant() (frame->function->chunk.constants.values[read_byte()])
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
        disassemble_instruction(&frame->function->chunk, (int)(frame->ip - frame->function->chunk.code));

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

            case OP_GET_LOCAL: {

                uint8_t slot = read_byte();
                push(frame->slots[slot]);

            } break;

            case OP_SET_LOCAL: {

                uint8_t slot = read_byte();
                frame->slots[slot] = peek(0);

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

            case OP_JUMP: {

                uint16_t offset = read_short();
                frame->ip += offset;

            } break;

            case OP_JUMP_IF_FALSE: {

                uint16_t offset = read_short();
                
                if (is_falsey(peek(0)))
                    frame->ip += offset;

            } break;

            case OP_LOOP: {

                uint16_t offset = read_short();
                frame->ip -= offset;

            } break;

            case OP_CALL: {

                int argc = read_byte();
                
                if (!call_value(peek(argc), argc))
                    return INTERPRET_RUNTIME_ERR;

                frame = &vm.frames[vm.frameCount - 1];

            } break;

            case OP_RETURN: {

                Value result = pop();
                vm.frameCount--;

                if (!vm.frameCount) {

                    pop();
                    return INTERPRET_OK;
                }

                vm.stackTop = frame->slots;
                push(result);
                frame = &vm.frames[vm.frameCount - 1];

            } break;
        }
    }

    #undef read_byte
    #undef read_short
    #undef read_string
    #undef read_constant
    #undef binary_op
}


static bool call(ObjFunction* function, int argc) {

    if (argc != function->arity) {

        runtime_error("Expected %d arguments but got %d.", function->arity, argc);
        return false;
    }

    if (vm.frameCount == FRAMES_MAX) {

        runtime_error("Stack overflow.");
        return false;
    }

    CallFrame* frame = &vm.frames[vm.frameCount++];
    frame->function = function;
    frame->ip = function->chunk.code;
    frame->slots = vm.stackTop - argc - 1;

    return true;
}


InterpretResult interpret(const char* src) {

    ObjFunction* function = compile(src);

    if (!function)
        return INTERPRET_COMPILE_ERR;

    push(obj_val(function));
    call(function, 0);

    return run();
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

static bool call_value(Value callee, int argc) {

    if (is_obj(callee)) {

        switch (obj_type(callee)) {
        
            case OBJ_FUNCTION:
                return call(as_function(callee), argc);

            default: break;
        }
    }

    runtime_error("Can only call functions and classes.");
    return false;
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

        case OBJ_FUNCTION: {

            ObjFunction* function = (ObjFunction*)object;
            free_chunk(&function->chunk);
            free(object); object = NULL;
    
        } break;
    
        case OBJ_STRING: {

            ObjString* ostring = (ObjString*)object;
            free(ostring->chars);
            free(object); object = NULL;

        } break;
    }
}








