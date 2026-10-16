#pragma once

#include "header.h"
#include <stdlib.h>

// [Header][Array]
//          ^

#define arr(T) T*
typedef arr(char) string;

#define arr_len(arr) ((arr) ? ((Header*)(arr) - 1)->count : 0)

#define arr_push(arr, x)                                                                  \
                                                                                          \
    do {                                                                                  \
                                                                                          \
        if (!(arr)) {                                                                     \
                                                                                          \
            Header* header = malloc(sizeof(Header) + sizeof(*(arr)) * ARR_INIT_CAPACITY); \
            header->count = 0;                                                            \
            header->capacity = ARR_INIT_CAPACITY;                                         \
            (arr) = (void*)(header + 1);                                                  \
        }                                                                                 \
                                                                                          \
        Header* header = (Header*)(arr) - 1;                                              \
                                                                                          \
        if (header->count >= header->capacity) {                                          \
                                                                                          \
            header->capacity *= 2;                                                        \
            header = realloc(header, sizeof(Header) + sizeof(*(arr)) * header->capacity); \
            (arr) = (void*)(header + 1);                                                  \
        }                                                                                 \
                                                                                          \
        (arr)[header->count++] = (x);                                                     \
                                                                                          \
    } while (0)


#define arr_append(arr, elems, n)                                                         \
                                                                                          \
    do {                                                                                  \
                                                                                          \
        if (!(arr)) {                                                                     \
                                                                                          \
            size_t size = ((n) < ARR_INIT_CAPACITY) ? ARR_INIT_CAPACITY : (n);            \
            Header* header = malloc(sizeof(Header) + sizeof(*(arr)) * size);              \
            header->count = 0;                                                            \
            header->capacity = size;                                                      \
            (arr) = (void*)(header + 1);                                                  \
        }                                                                                 \
                                                                                          \
        Header* header = (Header*)(arr) - 1;                                              \
                                                                                          \
        if (header->count + (n) > header->capacity) {                                     \
                                                                                          \
            header->capacity = (header->count + (n)) * 2;                                 \
            header = realloc(header, sizeof(Header) + sizeof(*(arr)) * header->capacity); \
            (arr) = (void*)(header + 1);                                                  \
        }                                                                                 \
                                                                                          \
        memcpy((arr) + header->count, (elems), (n) * sizeof(*(arr)));                     \
        header->count += (n);                                                             \
                                                                                          \
    } while (0)


#define arr_alloc(arr, size)                                                      \
                                                                                  \
    do {                                                                          \
                                                                                  \
        Header* header = (arr) ? (Header*)(arr) - 1 : NULL;                       \
                                                                                  \
        size_t count = header ? header->count : 0;                                \
        size_t capacity = header ? header->capacity : 0;                          \
                                                                                  \
        if (count + (size) > capacity) {                                          \
                                                                                  \
            capacity = (count + (size)) * 2;                                      \
                                                                                  \
            header = realloc(header, sizeof(Header) + sizeof(*(arr)) * capacity); \
                                                                                  \
            if (!(arr))                                                           \
                header->count = 0;                                                \
                                                                                  \
            header->capacity = capacity;                                          \
            (arr) = (void*)(header + 1);                                          \
        }                                                                         \
                                                                                  \
    } while (0)


#define arr_reverse(arr)                                        \
                                                                \
    do {                                                        \
                                                                \
        if (arr) {                                              \
                                                                \
            Header* header = (Header*)(arr) - 1;                \
                                                                \
            size_t i = 0;                                       \
            size_t j = header->count ? header->count - 1 : 0;   \
                                                                \
            unsigned char tmp[sizeof(*(arr))];                  \
                                                                \
            while (i < j) {                                     \
                                                                \
                memcpy(tmp, &(arr)[i], sizeof(*(arr)));         \
                memcpy(&(arr)[i++], &(arr)[j], sizeof(*(arr))); \
                memcpy(&(arr)[j--], tmp, sizeof(*(arr)));       \
            }                                                   \
        }                                                       \
                                                                \
    } while (0)


#define arr_free(arr)                 \
                                      \
    do {                              \
                                      \
        if (arr) {                    \
                                      \
            free((Header*)(arr) - 1); \
            (arr) = NULL;             \
        }                             \
                                      \
    } while (0)


#define string_to_cstr(str)            \
                                       \
    do {                               \
                                       \
        arr_push((str), NIL);          \
        ((Header*)(str) - 1)->count--; \
                                       \
    } while (0)





