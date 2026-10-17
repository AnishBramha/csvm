#pragma once


typedef enum {

    // single-char tokens
    
    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
    TOK_COMMA, TOK_DOT, TOK_MINUS, TOK_PLUS, TOK_POW,
    TOK_SEMICOLON, TOK_SLASH, TOK_STAR,

    // one/two-char tokens

    TOK_BANG, TOK_NEQ,
    TOK_ASSIGN, TOK_EQ,
    TOK_GT, TOK_GTE, TOK_LT, TOK_LTE,

    // literals

    TOK_IDENTIFIER, TOK_STRING, TOK_NUMBER,

    // keywords
    
    TOK_AND, TOK_CLASS, TOK_ELSE, TOK_FALSE,
    TOK_FOR, TOK_IF, TOK_NIL, TOK_OR,
    TOK_IOPUTF, TOK_IOPUTFN, TOK_SUPER, TOK_THIS,
    TOK_TRUE, TOK_VAR, TOK_WHILE,
    TOK_DO, TOK_AS, TOK_DEF, TOK_THEN, TOK_JUMP,

    // misc

    TOK_ERR, TOK_EOF,

} TokenType;


typedef struct {

    const char* start;
    const char* current;
    int line;

} Scanner;


extern Scanner scanner;


typedef struct {

    TokenType type;
    const char* start;
    int len;
    int line;

} Token;


void init_scanner(const char* src);
Token scan_token(void);







