#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "scanner.h"
#include "common.h"


Scanner scanner;


void init_scanner(const char* src) {

    scanner.start = src;
    scanner.current = src;
    scanner.line = 1;
}


static bool is_alpha(char c) {

    return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || c == '_';
}


static bool is_digit(char c) {

    return '0' <= c && c <= '9';
}


static bool is_at_end(void) {

    return *scanner.current == NIL;
}


static char advance(void) {

    scanner.current++;
    return scanner.current[-1];
}


static char peek(void) {

    return *scanner.current;
}


static char peek_next(void) {

    return is_at_end() ? NIL : scanner.current[1];
}


static bool match(char expected) {

    if (is_at_end())
        return false;

    if (*scanner.current != expected)
        return false;

    scanner.current++;
    return true;
}


static Token make_token(TokenType type) {

    return (Token) {

        .type = type,
        .start = scanner.start,
        .len = (int)(scanner.current - scanner.start),
        .line = scanner.line,
    };
}


static Token error_token(const char* msg) {

    return (Token) {

        .type = TOK_ERR,
        .start = msg,
        .len = (int)strlen(msg),
        .line = scanner.line,
    };
}


static void skip(void) {

    loop {

        switch (peek()) {

            case '\n':
                scanner.line++;
            
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;

            case '/':
                    
                if (peek_next() == '/') {

                    while (peek() != '\n' && !is_at_end())
                        advance();

                } else
                    return;

            default:
                return;
        }
    }
}


static TokenType check_keyword(int start, int len, const char* rest, TokenType type) {

    if (scanner.current - scanner.start == start + len && !memcmp(scanner.start + start, rest, len))
        return type;

    return TOK_IDENTIFIER;
}


static TokenType identifier_type(void) {

    switch (scanner.start[0]) {
    
        case 'c': return check_keyword(1, 4, "lass", TOK_CLASS);
        case 'e': return check_keyword(1, 3, "lse", TOK_ELSE);
        case 'j': return check_keyword(1, 3, "ump", TOK_JUMP);
        case 'n': return check_keyword(1, 2, "il", TOK_NIL);
        case 'o': return check_keyword(1, 1, "r", TOK_OR);
        case 's': return check_keyword(1, 4, "uper", TOK_SUPER);
        case 'v': return check_keyword(1, 2, "ar", TOK_VAR);
        case 'w': return check_keyword(1, 4, "hile", TOK_WHILE);

        case 'f': {

            if (scanner.current - scanner.start > 1) {

                switch (scanner.start[1]) {
                
                    case 'a': return check_keyword(2, 3, "lse", TOK_FALSE);
                    case 'o': return check_keyword(2, 1, "r", TOK_FOR);
                }
            }

        } break;

        case 't': {

            if (scanner.current - scanner.start > 1) {

                switch (scanner.start[1]) {
                
                    case 'h': {

                        if (scanner.current - scanner.start > 1) {

                            switch (scanner.start[2]) {

                                case 'i': 
                                    return check_keyword(3, 1, "s", TOK_THIS);
        
                                case 'e':
                                    return check_keyword(3, 1, "n", TOK_THEN);
                            }
                        }
                    }

                    case 'r': return check_keyword(2, 2, "ue", TOK_TRUE);

                } break;
            }

        } break;

        case 'd': {

            if (scanner.current - scanner.start > 1) {

                switch (scanner.start[1]) {
                
                    case 'o':
                        return check_keyword(2, 0, "", TOK_DO);
    
                    case 'e':
                        return check_keyword(2, 1, "f", TOK_DEF);
                }
            }

        } break;

        case 'a': {

            if (scanner.current - scanner.start > 1) {

                switch (scanner.start[1]) {
                
                    case 'n':
                        return check_keyword(2, 1, "d", TOK_AND);

                    case 's':
                        return check_keyword(2, 0, "", TOK_AS);
                }
            }

        } break;

        case 'i': {

            if (scanner.current - scanner.start > 1) {

                switch (scanner.start[1]) {

                    case 'f':
                        return check_keyword(2, 0, "", TOK_IF);

                    case 'o':
                        return (check_keyword(2, 5, "putfn", TOK_IOPUTFN) == TOK_IOPUTFN) ? TOK_IOPUTFN : check_keyword(2, 4, "putf", TOK_IOPUTF);
                }
            }

        } break;
    }

    return TOK_IDENTIFIER;
}


static Token identifier(void) {

    while (is_alpha(peek()) || is_digit(peek()))
        advance();

    return make_token(identifier_type());
}


static Token number(void) {

    while (is_digit(peek()))
        advance();

    if (peek() == '.' && is_digit(peek_next())) {

        advance();

        while (is_digit(peek()))
            advance();
    }

    return make_token(TOK_NUMBER);
}


static Token str(void) {

    while (peek() != '\"' && !is_at_end()) {

        if (peek() == '\n')
            scanner.line++;

        advance();
    }

    if (is_at_end())
        return error_token("Unterminated string literal.");

    advance();
    return make_token(TOK_STRING);
}


Token scan_token(void) {

    skip();
    scanner.start = scanner.current;

    if (is_at_end())
        return make_token(TOK_EOF);

    char c = advance();

    if (is_alpha(c))
        return identifier();

    if (is_digit(c))
        return number();

    switch (c) {

        case '(': return make_token(TOK_LPAREN);
        case ')': return make_token(TOK_RPAREN);
        case '{': return make_token(TOK_LBRACE);
        case '}': return make_token(TOK_RBRACE);
        case ';': return make_token(TOK_SEMICOLON);
        case ',': return make_token(TOK_COMMA);
        case '.': return make_token(TOK_DOT);
        case '-': return make_token(TOK_MINUS);
        case '+': return make_token(TOK_PLUS);
        case '*': return make_token(TOK_STAR);
        case '^': return make_token(TOK_POW);
        case '=': return make_token(TOK_EQ);
        case '!': return make_token(TOK_BANG);

        case '<': return make_token(match('=') ? TOK_LTE : TOK_LT);
        case '>': return make_token(match('=') ? TOK_GTE : TOK_GT);
        case '/': return make_token(match('=') ? TOK_NEQ : TOK_SLASH);
        case ':': return make_token(match('=') ? TOK_ASSIGN : TOK_ERR);

        case '\"': return str();
    }

    return error_token("Unexpected character.");
}







