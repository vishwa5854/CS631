//
// Created by z on 12/19/22.
//
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

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

    return (int)n_tokens;
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

int is_redirection(char *token) {
    return (strncmp(token, "<", 1) == 0) ||
           (strncmp(token, ">", 1) == 0) ||
           (strncmp(token, ">>", 2) == 0);
}

void check_permissions(char *redirection_token, char *file_path) {
    if (
            (strncmp(redirection_token, ">", 1) == 0) ||
            (strncmp(redirection_token, ">>", 2) == 0)
            ) {
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
void has_valid_redirection(Command *top) {
    /** If the current token is a redirection, then the next token should be a file name */
    while ((top != NULL) && (top->command_str != NULL)) {
        if (is_redirection(top->command_str)) {
            /** Gotta check if we have a next token or not and also if we have a token then
             * let's check for file permissions as well */
            if (
                    (top->next == NULL) ||
                    (top->next->command_str == NULL)
                    ) {
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
    return (strncmp(token, ">", 1) == 0) ||
           (strncmp(token, ">>", 2) == 0);
}

int is_append_redirection(char *token) {
    return strncmp(token, ">>", 2) == 0;
}

typedef struct ParsedCommand {
    char *exec_name;
    char **args;
    /** Note that we could only have one input or output redirections */
    char *input_redirection;
    char *output_redirection;
    int append;
    int n_args;
} PCommand;
PCommand *parsed_command;

void find_the_executable_and_args(Command *top, int n_args, PCommand *p_command) {
    char *args[n_args];
    bzero(args, sizeof(args));
    int start = 0;
    char *exec = NULL;
    char *in_redirection = NULL, *out_redirection = NULL;
    p_command->args = (char**) malloc(sizeof(char*) * n_args);

    /** The first string without any redirections is the exec name */
    while ((top != NULL) && (start < n_args) && (top->command_str != NULL)) {
        if (is_redirection(top->command_str)) {
            if (top->next != NULL) {
                if (is_input_redirection(top->command_str)) {
                    in_redirection = top->next->command_str;
                } else if (is_output_redirection(top->command_str)) {
                    if (is_append_redirection(top->command_str)) {
                        p_command->append = 1;
                    } else {
                        p_command->append = 0;
                    }
                    out_redirection = top->next->command_str;
                }
                top = top->next->next;
            } else {
                top = NULL;
            }
        } else {
            /** This token doesn't belong to a redirection bruh */
            if (exec == NULL) {
                exec = top->command_str;
            }
            p_command->args[start++] = top->command_str;
            top = top->next;
        }
    }
    p_command->args[start] = NULL;
    p_command->n_args = start;

    if (exec != NULL) {
        p_command->exec_name = (char*) malloc(sizeof(char) * strlen(exec));
        (void) strncpy(p_command->exec_name, exec, strlen(exec));
    } else {
        p_command->exec_name = NULL;
    }

    if (in_redirection != NULL) {
        p_command->input_redirection = (char*) malloc(sizeof(char) * strlen(in_redirection));
        (void) strncpy(p_command->input_redirection, in_redirection, strlen(in_redirection));
    } else {
        p_command->input_redirection = NULL;
    }

    if (out_redirection != NULL) {
        p_command->output_redirection = (char*) malloc(sizeof(char) * strlen(out_redirection));
        (void) strncpy(p_command->output_redirection, out_redirection, strlen(out_redirection));
    } else {
        p_command->output_redirection = NULL;
    }
}

void execute_the_fucking_command(PCommand* p_command) {
    // after fork, before execvp, dup shit
    pid_t pid;
    if ((pid = fork()) < 0) {
        (void) fprintf(stderr, "SISH: Failed to execute %s: %s", p_command->exec_name, strerror(errno));
        return;
    } else if (pid > 0) {
        /** Parent */
        int status = -1;

        if (waitpid(pid, &status, 0) < 0) {
            // for now we will say child failed that's it
            (void) fprintf(stdout, "Child Failed");
        };
    } else if (pid == 0) { /** Child */
        /** We will dup the stdin to the file descriptor of the given file bruh */
        if (p_command->input_redirection != NULL) {
            int input_redirection_fd;

            if ((input_redirection_fd = open(p_command->input_redirection, O_RDONLY)) == -1) {
                (void) fprintf(stderr, "SISH: Cannot read file %s: %s", p_command->input_redirection,
                               strerror(errno));
                _exit(EXIT_FAILURE);
            }

            if (dup2(input_redirection_fd, STDIN_FILENO) != STDIN_FILENO) {
                (void) fprintf(stderr, "SISH: Input redirection failed for %s: %s",
                               p_command->input_redirection, strerror(errno));
                _exit(EXIT_FAILURE);
            }
        }

        /** We will dup the stdout to the file descriptor of the given file bruh */
        if (p_command->output_redirection != NULL) {
            int output_redirection_fd;
            int oflag = O_WRONLY;

            if (p_command->append) {
                oflag = O_APPEND;
            }

            if ((output_redirection_fd = open(p_command->output_redirection, O_CREAT | oflag, S_IRUSR | S_IWUSR)) == -1) {
                (void) fprintf(stderr, "SISH: Cannot write file %s: %s", p_command->output_redirection,
                               strerror(errno));
                _exit(EXIT_FAILURE);
            }

            if (dup2(output_redirection_fd, STDOUT_FILENO) != STDOUT_FILENO) {
                (void) fprintf(stderr, "SISH: Output redirection failed for %s: %s",
                               p_command->output_redirection, strerror(errno));
                _exit(EXIT_FAILURE);
            }
        }

        execvp(p_command->exec_name, p_command->args);
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

    Command *redirection_head = head;
    has_valid_redirection(redirection_head);

    Command *exec_head = head;
    parsed_command = (PCommand*) malloc(sizeof (PCommand));
    find_the_executable_and_args(exec_head, re, parsed_command);

    execute_the_fucking_command(parsed_command);

    // TODO: Handle for pipe basically strtok by | and call all these functions man
    /**
     * Random thoughts
     * For input redirection, just dup the stdin with the corresponding file's fd
     * For output redirection, just dup the stdin with the corresponding file's fd
     * But for the pipe dup the stdout to stdin of the new process.
     * In the piping bruh, allow redirections in the first command and the last command
     * but not in between as it wouldn't make any sense
     * */
    free(current);
    free(parsed_command);
}