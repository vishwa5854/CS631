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

void execute_the_fucking_command(PCommand *p_command) {
    pid_t pid;

    if ((pid = fork()) < 0) {
        (void)fprintf(stderr, "SISH: Failed to execute %s: %s\n", p_command->exec_name,
                      strerror(errno));
        return;
    } else if (pid > 0) {
        /** Parent */
        int status = -1;

        if (waitpid(pid, &status, 0) < 0) {
            // for now, we will say child failed that's it
            (void)fprintf(stdout, "Child Failed\n");
        }
    } else if (pid == 0) { /** Child */
        /** We will dup the stdin to the file descriptor of the given file bruh
         */
        if (p_command->input_redirection != NULL) {
            int input_redirection_fd;

            if ((input_redirection_fd = open(p_command->input_redirection, O_RDONLY)) == -1) {
                (void)fprintf(stderr, "SISH: Cannot read file %s: %s\n",
                              p_command->input_redirection, strerror(errno));
                _exit(EXIT_FAILURE);
            }

            if (dup2(input_redirection_fd, STDIN_FILENO) != STDIN_FILENO) {
                (void)fprintf(stderr, "SISH: Input redirection failed for %s: %s\n",
                              p_command->input_redirection, strerror(errno));
                _exit(EXIT_FAILURE);
            }
        }

        /** We will dup the stdout to the file descriptor of the given file bruh
         */
        if (p_command->output_redirection != NULL) {
            int output_redirection_fd;
            int oflag = O_WRONLY;

            if (p_command->append) {
                oflag = oflag | O_APPEND;
            }

            if ((output_redirection_fd = open(p_command->output_redirection, oflag | O_CREAT,
                                              S_IRUSR | S_IWUSR)) == -1) {
                (void)fprintf(stderr, "SISH: Cannot write file %s: %s\n",
                              p_command->output_redirection, strerror(errno));
                _exit(EXIT_FAILURE);
            }

            if (dup2(output_redirection_fd, STDOUT_FILENO) != STDOUT_FILENO) {
                (void)fprintf(stderr, "SISH: Output redirection failed for %s: %s\n",
                              p_command->output_redirection, strerror(errno));
                _exit(EXIT_FAILURE);
            }
        }

        if (execvp(p_command->exec_name, p_command->args) == -1) {
            perror("Failed to execute command.");
            _exit(EXIT_FAILURE);
        }
    }
}