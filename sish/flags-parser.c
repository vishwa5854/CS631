//
// Created by z on 12/17/22.
//
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "flags-parser.h"

void parse_flags(FLAGS *flags, int argc, char **argv) {
    /** First let's flush the flags->command with null characters so that we don't get weird chars */
    bzero(flags->command, sizeof(flags->command));
    int option;

    while ((option = getopt(argc, argv, "c:x")) != -1) {
        switch (option) {
            case 'c':
                (void) strncpy(flags->command, optarg, strnlen(optarg, MAX_COMMAND_SIZE));
                flags->command[strnlen(optarg, MAX_COMMAND_SIZE)] = '\0';
                flags->c = true;
                break;
            case 'x':
                flags->x = true;
                break;
            default:
                (void) fprintf(stderr, "Usage: %s [ −x] [ −c command]\n", PROGRAM_NAME);
                exit(EXIT_FAILURE);
        }
    }
    argc -= optind;

    if (argc != 0) {
        (void) fprintf(stderr, "Usage: %s [ −x] [ −c command]", PROGRAM_NAME);
        exit(EXIT_FAILURE);
    }
}