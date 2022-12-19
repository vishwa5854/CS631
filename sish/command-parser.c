//
// Created by z on 12/19/22.
//
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
//#include "command-parser.h"

typedef struct command {
    char *command_str;
    struct command *next;
} Command;

Command *current;
Command *head;

/** TODO: Please add an empty space at the end of original_command to get all tokens */
int parser(char *original_command) {
    size_t n_original_command = strlen(original_command);
    size_t i = 0, word_length = 0, n_tokens = 0;
    char token[BUFSIZ];
    bzero(token, sizeof(token));
    int i_updated = 0;
    int out_redirection = 0;
    int in_redirection = 0;

    while (i < n_original_command) {
        if (isspace(original_command[i]) || (original_command[i] == '>') || (original_command[i] == '<')) {
            /** Time to flush the token and there might be a white space in the beginning */
            if (word_length > 0) {
                current->command_str = (char*) malloc(sizeof(char) * word_length);
                (void)strncpy(current->command_str, token, word_length);
                current->next = (Command*)malloc(sizeof(Command));
                current = current->next;
                n_tokens++;
                bzero(token, sizeof(token));
            }

            if ((original_command[i] == '>') || (original_command[i] == '<')) {
                /** We already had an stdout redirection bruh */
                if (out_redirection && (original_command[i] == '>')) {
                    return -1;
                }

                /** We already had an stdin redirection bruh */
                if (in_redirection && (original_command[i] == '<')) {
                    return -1;
                }
                out_redirection = original_command[i] == '>';
                in_redirection = original_command[i] == '<';

                /** Append Condition */
                if ((original_command[i] == '>') && ((i + 1) < n_original_command) && (original_command[i + 1] == '>')) {
                    current->command_str = (char*) malloc(sizeof(char) * 2);
                    (void) strncpy(current->command_str, ">>", 2);
                    i += 2;
                    i_updated = 1;
                } else {
                    current->command_str = (char*) malloc(sizeof(char));
                    current->command_str[0] = original_command[i];
                }

                current->next = (Command*)malloc(sizeof(Command));
                current = current->next;
                n_tokens++;
            }
            word_length = 0;
        } else {
            /** I am currently not going to accept individual token more than BUFSIZ */
            if (word_length < BUFSIZ) {
                token[word_length++] = original_command[i];
            }
        }

        if (!i_updated) {
            i++;
        } else {
            i_updated = 0;
        }
    }
    i = 0;

    while ((head != NULL) && (i < n_tokens)) {
//        if (head->is_pipe) {
//            (void) printf("It is pipe duh\n");
//        } else {

        (void)printf("Token is # %s\n", head->command_str);
//        }
        head = head->next;
        i++;
    }

    return n_tokens;
}


/**
 * While creating the linked list itself see how may number of commands are there?
 * Count the number of pipes
 * find out which is the command and which is the arg (If the arg has a redirection then it is not the base)
 *
 * */

void not_so_dumb_parser(char *full_command) {
    char *token = strtok(full_command, "|");
    int n_commands = 0;

    while (token != NULL) {
        /** We have one individual command now :) */
        int n_tokens = parser(token), i = 0;

        while ((head != NULL) && (i < n_tokens)) {
            /** Redirection Cases */
            if (
                    (strncmp(head->command_str, "<", 1) == 0) ||
                    (strncmp(head->command_str, ">", 1) == 0) ||
                    (strncmp(head->command_str, ">>", 1) == 0) ||
                    (strchr(head->command_str, '<') != NULL) ||
                    /**  This condition should cover both > and >> */
                    (strchr(head->command_str, '>') != NULL)
                    ) {

            }

            head = head->next;
            i++;
        }

        n_commands += 1;
        token = strtok(full_command, "|");
    }
}

/** All you gotta do is check for the existence of a file on the right */
void has_valid_redirection(Command *head) {

}

int is_redirection(char *token) {
    return (strncmp(head->command_str, "<", 1) == 0) ||
           (strncmp(head->command_str, ">", 1) == 0) ||
           (strncmp(head->command_str, ">>", 2) == 0);
}

void find_the_executable(Command *head) {
    /** The first string without any redirections before it is the exec name */
    if (
            (head->command_str[0] != '<') &&
            (head->command_str[0] != '>')
            ) {
        (void)puts(head->command_str);
    } else {

    }


}

int main() {
    current = (Command*)malloc(sizeof(Command));
    head = current;
    char input[BUFSIZ];
    printf("Enter the fucking string: ");
    fgets(input, BUFSIZ, stdin);
    int re = parser(input);
    if (re == -1) {
        puts("Invalid command");
    } else {
        printf("Number of tokens are %d\n", re);
    }
//    free(head);
    free(current);
}