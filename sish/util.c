//
// Created by z on 12/21/22.
//

#include "util.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int is_redirection(char *token) {
    return (strncmp(token, "<", 1) == 0) || (strncmp(token, ">", 1) == 0) ||
           (strncmp(token, ">>", 2) == 0);
}

void check_permissions(char *redirection_token, char *file_path) {
    if ((strncmp(redirection_token, ">", 1) == 0) || (strncmp(redirection_token, ">>", 2) == 0)) {
        if (access(file_path, W_OK) == -1) {
            if (errno != ENOENT) {
                (void)printf("SISH: %s: %s", file_path, strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    } else if (strncmp(redirection_token, "<", 1) == 0) {
        if (access(file_path, R_OK) == -1) {
            (void)printf("SISH: %s: %s", file_path, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
}

/** All you gotta do is check for the existence of a file on the right */
void has_valid_redirection(TokenizedIndividualCommand *top) {
    /** If the current token is a redirection, then the next token should be a
     * file name */
    while ((top != NULL) && (top->command_str != NULL)) {
        if (is_redirection(top->command_str)) {
            /** Gotta check if we have a next token or not and also if we have a
             * token then let's check for file permissions as well */
            if ((top->next == NULL) || (top->next->command_str == NULL)) {
                puts("Invalid Syntax of redirection");
                exit(EXIT_FAILURE);
            }
            check_permissions(top->command_str, top->next->command_str);
        }
        top = top->next;
    }
}

int is_input_redirection(char *token) { 
    return (strncmp(token, "<", 1) == 0); 
}

int is_output_redirection(char *token) {
    return (strncmp(token, ">", 1) == 0) || (strncmp(token, ">>", 2) == 0);
}

int is_append_redirection(char *token) { 
    return strncmp(token, ">>", 2) == 0; 
}

int number_of_pipes(char *token) {
    int pipes = 0;
    size_t n_token = strlen(token), i = 0;

    for (; i < n_token; i++) {
        if (token[i] == '|') {
            pipes++;
        }
    }
    return pipes;
}