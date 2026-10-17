#include <stdio.h>
#include <stdlib.h>
#include <utils.h>
#include "chunk.h"
#include "debug.h"
#include "vm.h"
#include "common.h"
#include <sysexits.h>


static void repl(void) {

    char line[CHUNK];
    loop {

        printf("\n>>> ");

        if (!fgets(line, sizeof line, stdin)) {

            putchar('\n');
            break;
        }

        interpret(line);
    }
}


static char* read_file(const char* path) {

    FILE* file = fopen(path, "rb");
    if (!file)
        panic("Could not open file `%s`\n", path);

    fseek(file, 0L, SEEK_END);
    size_t filesize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(filesize + 1);
    if (!buffer)
        panic("malloc failed");

    size_t rbytes = fread(buffer, sizeof(char), filesize, file);
    buffer[rbytes] = NIL;

    fclose(file);
    file = NULL;

    return buffer;
}


static void run_file(const char* path) {

    char* src = read_file(path);
    InterpretResult result = interpret(src);
    free(src);

    if (result == INTERPRET_COMPILE_ERR)
        exit(EX_DATAERR);

    if (result == INTERPRET_RUNTIME_ERR)
        exit(EX_SOFTWARE);
}


int main(int argc, char** argv) {

    boot_vm();

    switch (argc) {

        case 1: repl(); break;
        case 2: run_file(argv[1]); break;
        case 3: eprintf("Usage: csvm [path]\n"); exit(EX_USAGE);
    }

    shutdown_vm();

    return 0;
}





