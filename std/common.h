#pragma once

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>


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


#define panic(fmt, ...)                                                                                              \
                                                                                                                     \
    do {                                                                                                             \
                                                                                                                     \
        char msg[ERRMSG_LEN_MAX];                                                                                    \
        snprintf(msg, ERRMSG_LEN_MAX - 1, fmt __VA_OPT__(,) __VA_ARGS__);                                            \
        fprintf(stderr, "[File: %s | Line: %d] PANIC: %s | REASON: %s\n", __FILE__, __LINE__, msg, strerror(errno)); \
        abort();                                                                                                     \
                                                                                                                     \
    } while (0)


#define eprintf(fmt, ...) fprintf(stderr, fmt __VA_OPT__(,) __VA_ARGS__)


#define ensure_random() srand((unsigned)time(NULL));


#define randint(harvest, start, end)                            \
                                                                \
    do {                                                        \
                                                                \
        assert((start) <= (end));                               \
        (harvest) = ((start) + rand() % ((end) - (start) + 1)); \
                                                                \
    } while (0)


#define random(harvest, start, end)                                              \
                                                                                 \
    do {                                                                         \
                                                                                 \
        assert((start) <= (end));                                                \
        (harvest) = (start) + (((double)rand() / RAND_MAX) * ((end) - (start))); \
                                                                                 \
    } while (0)


#define randnormal(harvest) random(harvest, 0, 1)






