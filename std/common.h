#pragma once

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define ERRMSG_LEN_MAX 256
#define CHUNK 1024

#define NIL '\0'
#define loop for (;;)

#define forrange(T, idx, start, end, step) \
    for (T idx = (start); idx < (end); idx += (step))

#define downrange(T, idx, start, end, step) \
    for (T idx = (start); idx > (end); idx -= (step))

#define until(T, idx, start, end, step) \
    for (T idx = (start); (end); (step))

#define unused(var) (void)(var)

#undef unreachable
#define unreachable()                                                                                          \
                                                                                                               \
    do {                                                                                                       \
                                                                                                               \
        fprintf(stderr, "PANIC: Unreachable statement on line %d reached in file `%s`\n", __LINE__, __FILE__); \
        abort();                                                                                               \
                                                                                                               \
    } while (0)


typedef enum {

    DEBUG,
    CRASH,

} Handle;

#define todo(response, task)                                                                            \
                                                                                                        \
    do {                                                                                                \
                                                                                                        \
        fprintf(stderr, "TODO: Pending task `%s` on line %d in file `%s`\n", task, __LINE__, __FILE__); \
                                                                                                        \
        if (response == CRASH)                                                                          \
            abort();                                                                                    \
                                                                                                        \
    } while (0)


#define panic(fmt, ...)                                                       \
                                                                              \
    do {                                                                      \
                                                                              \
        char msg[ERRMSG_LEN_MAX];                                             \
        snprintf(msg, ERRMSG_LEN_MAX - 1, fmt __VA_OPT__(,) __VA_ARGS__);     \
        fprintf(stderr, "PANIC: %s | REASON: %s\n", msg, strerror(errno));    \
        abort();                                                              \
                                                                              \
    } while (0)


#define eprintf(fmt, ...) fprintf(stderr, fmt __VA_OPT__(,) __VA_ARGS__)


#define random(harvest, start, end)                   \
                                                      \
    do {                                              \
                                                      \
        srand((unsigned)time(NULL));                  \
        (harvest) = ((a) + rand() % ((b) - (a) + 1)); \
                                                      \
    } while (0)







