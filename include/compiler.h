#pragma once

#include "vm.h"
#include "scanner.h"


typedef struct {

    Token current;
    Token previous;
    bool hadError;
    bool panicMode;

} Parser;


extern Parser parser;
extern Chunk* compilingChunk;


typedef enum {

    PREC_NONE,
    PREC_ASSIGN,
    PREC_OR,
    PREC_AND,
    PREC_EQ,
    PREC_CMP,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY,
    PREC_CALL,
    PREC_PRIMARY,

} Precedence;


typedef void (*ParseFn)(void);


typedef struct {

    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;

} ParseRule;


extern ParseRule rules[];


bool compile(const char* src, Chunk*);





