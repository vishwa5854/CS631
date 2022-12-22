//
// Created by z on 12/17/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command-parser.h"
#include "data-structures.h"
#include "flags-parser.h"

FLAGS flags;
MasterCommand *current_mc;
MasterCommand *head_mc;
PCommand *parsed_command;

int exit_status_of_last_command = 0;
int current_process_id = 0;

int main(int argc, char **argv) {
    parse_flags(&flags, argc, argv);

    /** Execute only command given in -c */
    if (flags.c) {
      parse_and_exec(flags.command, current_mc, parsed_command);
    } else {
        char input[BUFSIZ];

        while (1) {
            current_mc = (MasterCommand *)malloc(sizeof(MasterCommand));
            head_mc = current_mc;
            printf("SISH$ ");
            fgets(input, BUFSIZ, stdin);

            if ((strncmp(input, "exit", strlen("exit")) == 0) ||
                (strncmp(input, "exit\n", strlen("exit\n")) == 0) ||
                (strncmp(input, "exit\r\n", strlen("exit\r\n")) == 0)) {
                break;
            }

            parse_and_exec(input, current_mc, parsed_command);
            free(current_mc);
        }
    }

    return EXIT_SUCCESS;
}