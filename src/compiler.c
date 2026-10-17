#include "compiler.h"
#include "chunk.h"
#include "common.h"
#include "utils.h"
#include "scanner.h"
#include <stdint.h>
#include <stdio.h>
#include "value.h"
#include "vm.h"
#include <stdlib.h>

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif


Parser parser;
Chunk* compilingChunk;


static Chunk* current_chunk(void) {

    return compilingChunk;
}


static void error_at(Token* token, const char* msg) {

    if (parser.panicMode)
        return;

    parser.panicMode = true;

    eprintf("[line %d] Error", token->line);

    if (token->type == TOK_EOF)
        eprintf(" at end");

    else if (token->type == TOK_ERR);

    else
        eprintf(" at `%.*s`", token->len, token->start);

    eprintf(": %s\n", msg);
    parser.hadError = true;
}


static void error_at_current(const char* msg) {

    error_at(&parser.current, msg);
}


static void error(const char* msg) {

    error_at(&parser.previous, msg);
}


static void advance(void) {

    parser.previous = parser.current;

    loop {

        parser.current = scan_token();

        if (parser.current.type != TOK_ERR)
            break;

        error_at_current(parser.current.start);
    }
}


static void consume(TokenType type, const char* msg) {

    if (parser.current.type == type) {

        advance();
        return;
    }

    error_at_current(msg);
}


static void emit_byte(uint8_t byte) {

    write_chunk(current_chunk(), byte, parser.previous.line);
}


static void emit_bytes(uint8_t byte1, uint8_t byte2) {

    emit_byte(byte1);
    emit_byte(byte2);
}


static void emit_return(void) {

    emit_byte(OP_RETURN);
}


static uint8_t make_constant(Value value) {

    int constant = add_constant(current_chunk(), value);

    if (constant > UINT8_MAX) {

        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}


static void emit_constant(Value value) {

    emit_bytes(OP_CONSTANT, make_constant(value));
}


static ParseRule* get_rule(TokenType);


static void parse_precedence(Precedence precedence) {

    advance();

    ParseFn prefixRule = get_rule(parser.previous.type)->prefix;
    if (!prefixRule) {

        error("Expect expression.");
        return;
    }

    prefixRule();

    while (precedence <= get_rule(parser.current.type)->precedence) {

        advance();
        ParseFn infixRule = get_rule(parser.previous.type)->infix;
        infixRule();
    }
}


static void end_compiler(void) {

    emit_return();

    #ifdef DEBUG_PRINT_CODE
    
    if (!parser.hadError)
        disassemble_chunk(current_chunk(), "code");

    #endif
}


static void number(void);
static void grouping(void);
static void unary(void);
static void binary(void);
static void literal(void);
static void str(void);


ParseRule rules[] = {

    [TOK_AND]        = {NULL,     NULL,   PREC_NONE},
    [TOK_AS]         = {NULL,     NULL,   PREC_NONE},
    [TOK_ASSIGN]     = {NULL,     NULL,   PREC_NONE},
    [TOK_BANG]       = {unary,    NULL,   PREC_NONE},
    [TOK_CLASS]      = {NULL,     NULL,   PREC_NONE},
    [TOK_COMMA]      = {NULL,     NULL,   PREC_NONE},
    [TOK_DEF]        = {NULL,     NULL,   PREC_NONE},
    [TOK_DO]         = {NULL,     NULL,   PREC_NONE},
    [TOK_DOT]        = {NULL,     NULL,   PREC_NONE},
    [TOK_ELSE]       = {NULL,     NULL,   PREC_NONE},
    [TOK_EOF]        = {NULL,     NULL,   PREC_NONE},
    [TOK_EQ]         = {NULL,     binary, PREC_EQ},
    [TOK_ERR]        = {NULL,     NULL,   PREC_NONE},
    [TOK_FALSE]      = {literal,  NULL,   PREC_NONE},
    [TOK_FOR]        = {NULL,     NULL,   PREC_NONE},
    [TOK_GTE]        = {NULL,     binary, PREC_CMP},
    [TOK_GT]         = {NULL,     binary, PREC_CMP},
    [TOK_IDENTIFIER] = {NULL,     NULL,   PREC_NONE},
    [TOK_IF]         = {NULL,     NULL,   PREC_NONE},
    [TOK_IOPUTFN]    = {NULL,     NULL,   PREC_NONE},
    [TOK_IOPUTF]     = {NULL,     NULL,   PREC_NONE},
    [TOK_JUMP]       = {NULL,     NULL,   PREC_NONE},
    [TOK_LBRACE]     = {NULL,     NULL,   PREC_NONE},
    [TOK_LPAREN]     = {grouping, NULL,   PREC_NONE},
    [TOK_LTE]        = {NULL,     binary, PREC_CMP},
    [TOK_LT]         = {NULL,     binary, PREC_CMP},
    [TOK_MINUS]      = {unary,    binary, PREC_TERM},
    [TOK_NEQ]        = {NULL,     binary, PREC_EQ},
    [TOK_NIL]        = {literal,  NULL,   PREC_NONE},
    [TOK_NUMBER]     = {number,   NULL,   PREC_NONE},
    [TOK_OR]         = {NULL,     NULL,   PREC_NONE},
    [TOK_PLUS]       = {NULL,     binary, PREC_TERM},
    [TOK_POW]        = {NULL,     binary, PREC_FACTOR},
    [TOK_RBRACE]     = {NULL,     NULL,   PREC_NONE},
    [TOK_RPAREN]     = {NULL,     NULL,   PREC_NONE},
    [TOK_SEMICOLON]  = {NULL,     NULL,   PREC_NONE},
    [TOK_SLASH]      = {NULL,     binary, PREC_FACTOR},
    [TOK_STAR]       = {NULL,     binary, PREC_FACTOR},
    [TOK_STRING]     = {str,      NULL,   PREC_NONE},
    [TOK_SUPER]      = {NULL,     NULL,   PREC_NONE},
    [TOK_THEN]       = {NULL,     NULL,   PREC_NONE},
    [TOK_THIS]       = {NULL,     NULL,   PREC_NONE},
    [TOK_TRUE]       = {literal,  NULL,   PREC_NONE},
    [TOK_VAR]        = {NULL,     NULL,   PREC_NONE},
    [TOK_WHILE]      = {NULL,     NULL,   PREC_NONE},
};


static ParseRule* get_rule(TokenType type) {

    return &rules[type];
}


static void binary(void) {

    TokenType operatorType = parser.previous.type;
    ParseRule* rule = get_rule(operatorType);
    parse_precedence((Precedence)(rule->precedence + 1));

    switch (operatorType) {

        case TOK_NEQ:   emit_bytes(OP_EQ, OP_NOT); break;
        case TOK_EQ:    emit_byte(OP_EQ);          break;
        case TOK_GT:    emit_byte(OP_GT);          break;
        case TOK_GTE:   emit_bytes(OP_LT, OP_NOT); break;
        case TOK_LT:    emit_byte(OP_LT);          break;
        case TOK_LTE:   emit_bytes(OP_GT, OP_NOT); break;
    
        case TOK_PLUS:  emit_byte(OP_ADD);         break;
        case TOK_MINUS: emit_byte(OP_SUBTRACT);    break;
        case TOK_STAR:  emit_byte(OP_MULTIPLY);    break;
        case TOK_SLASH: emit_byte(OP_DIVIDE);      break;
        case TOK_POW:   emit_byte(OP_POW);         break;

        default: unreachable();
    }
}


static void literal(void) {

    switch (parser.previous.type) {
    
        case TOK_FALSE: emit_byte(OP_FALSE); break;
        case TOK_NIL:   emit_byte(OP_TRUE);  break;
        case TOK_TRUE:  emit_byte(OP_TRUE);  break;

        default: unreachable();
    }
}


static void expression(void) {

    parse_precedence(PREC_ASSIGN);
}


static void grouping(void) {

    expression();
    consume(TOK_RPAREN, "Expect `)` after expression.");
}


static void number(void) {

    double value = strtod(parser.previous.start, NULL);
    emit_constant(number_val(value));
}


static void unary(void) {

    TokenType operatorType = parser.previous.type;

    parse_precedence(PREC_UNARY);

    switch (operatorType) {

        case TOK_BANG:  emit_byte(OP_NOT);    break;
        case TOK_MINUS: emit_byte(OP_NEGATE); break;

        default: unreachable();
    }
}


static void str(void) {

    emit_constant(obj_val(copy_string(parser.previous.start + 1, parser.previous.len - 2)));
}


bool compile(const char* src, Chunk* chunk) {

    init_scanner(src);
    compilingChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;

    advance();
    expression();
    consume(TOK_EOF, "Expect end of expression.");
    end_compiler();

    return !parser.hadError;
}








