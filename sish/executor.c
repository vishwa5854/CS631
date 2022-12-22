//
// Created by z on 12/21/22.
//

#include "executor.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

void executor(PCommand *head, int n_commands) {
    int pipes[n_commands][2];
    pid_t p_ids[n_commands];
    int i;
    int waits[n_commands];

    for (i = 0; i < n_commands; i++) {
        waits[i] = 0;
        if (pipe(pipes[i]) < 0) {
            (void) fprintf(stderr, "%s: Cannot create pipe.: %s\n", PROGRAM_NAME, strerror(errno));
            return;
        }
    }

    for (i = 0; i < n_commands; i++) {
        p_ids[i] = fork();

        if (p_ids[i] == -1) {
            (void) fprintf(stderr, "%s: Error while forking: %s\n", PROGRAM_NAME, strerror(errno));
            return;
        } else if (p_ids[i] == 0) {
            /** Child */
            close(pipes[i][0]);

            if (head->input_redirection != NULL) {
                int input_redirection_fd;

                if ((input_redirection_fd = open(head->input_redirection, O_RDONLY)) == -1) {
                  (void)fprintf(stderr, "SISH: Cannot read file %s: %s\n",
                                head->input_redirection, strerror(errno));
                  _exit(EXIT_FAILURE);
                }

                if (dup2(input_redirection_fd, STDIN_FILENO) != STDIN_FILENO) {
                  (void)fprintf(stderr, "SISH: Input redirection failed for %s: %s\n",
                                head->input_redirection, strerror(errno));
                  _exit(EXIT_FAILURE);
                }
            }

            /** We will dup the stdout to the file descriptor of the given file bruh
             */
            if (head->output_redirection != NULL) {
                int output_redirection_fd;
                int oflag = O_WRONLY;

                if (head->append) {
                  oflag = oflag | O_APPEND;
                }

                if ((output_redirection_fd = open(head->output_redirection, oflag | O_CREAT,
                                                  S_IRUSR | S_IWUSR)) == -1) {
                  (void)fprintf(stderr, "SISH: Cannot write file %s: %s\n",
                                head->output_redirection, strerror(errno));
                  _exit(EXIT_FAILURE);
                }

                if (dup2(output_redirection_fd, STDOUT_FILENO) != STDOUT_FILENO) {
                  (void)fprintf(stderr, "SISH: Output redirection failed for %s: %s\n",
                                head->output_redirection, strerror(errno));
                  _exit(EXIT_FAILURE);
                }
            }

            if (i != 0) {
                if (dup2(pipes[i - 1][0], STDIN_FILENO) != STDIN_FILENO) {
                  fprintf(stderr, "ERROR WHILE DUPING STDIN\n");
                  _exit(EXIT_FAILURE);
                }
            }

            if (i != n_commands - 1) {
                if (dup2(pipes[i][1], STDOUT_FILENO) != STDOUT_FILENO) {
                  fprintf(stderr, "ERROR WHILE DUPING STDOUT\n");
                  _exit(EXIT_FAILURE);
                }
            }

            if (execvp(head->exec_name, head->args) == -1) {
                _exit(EXIT_FAILURE);
            }
        } else if (p_ids[i] > 0) {
            char* exec_name = head->exec_name;
            head = head->next;
            close(pipes[i][1]);
            int status = -1;

            if (waitpid(p_ids[i], &status, WNOHANG) < 0) {
                (void) fprintf(stderr, "%s: Failed to wait for command: %s\n", PROGRAM_NAME, exec_name);
                break;
            }

            /** Child has called _exit() */
            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);

                /** Child exited with an error */
                if (exit_code != 0) {
                  (void) fprintf(stderr, "%s: Failed to execute command: %s\n", PROGRAM_NAME, exec_name);
                  break;
                }
            } else {
                waits[i] = 1;
            }
        }
    }

    for (i = 0; i < n_commands; i++) {
        if (waits[i]) {
            int status = -1;

            if (waitpid(p_ids[i], &status, 0) < 0) {
                (void) fprintf(stderr, "%s: Failed to wait for command\n", PROGRAM_NAME);
                break;
            }
        }
    }

}