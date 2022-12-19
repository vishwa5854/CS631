//
// Created by z on 12/17/22.
//
#include <stdlib.h>

#include "flags-parser.h"
#include "data-structures.h"
#include "shell-builtins.h"

FLAGS flags;

int main(int argc, char** argv) {
    parse_flags(&flags, argc, argv);

    /** If -c is not given then we should just open a simple prompt */
    while (1) {
        // if both redirection and pipeline are there only
        // command linked list
        // is_space
        // execvp
    }
    return EXIT_SUCCESS;
}