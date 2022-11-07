#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<signal.h>
#include<sys/wait.h>

#define CUSTOM_EXIT_FAILURE (-1)

int command(const char* string, char* outbuf, int outlen, char* errbuf, int errlen) {
    pid_t pid;
    int fd_out[2], fd_err[2];
    int status = -1;
    sigset_t mask;

    (void)signal(SIGINT, SIG_IGN);
    (void)signal(SIGQUIT, SIG_IGN);

    if ((sigemptyset(&mask) == -1) || (sigaddset(&mask, SIGCHLD) == -1)) return CUSTOM_EXIT_FAILURE;

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) return CUSTOM_EXIT_FAILURE;

    if (string == NULL) return CUSTOM_EXIT_FAILURE;

    if (pipe(fd_err) < 0) return CUSTOM_EXIT_FAILURE;

    if (pipe(fd_out) < 0) return CUSTOM_EXIT_FAILURE;

    if ((pid = fork()) < 0) {
        return CUSTOM_EXIT_FAILURE;
    } else if (pid > 0) {
        /** Parent, closing the write end */
        (void)close(fd_out[1]);
        (void)close(fd_err[1]);

        if (read(fd_out[0], outbuf, outlen) < 0) return CUSTOM_EXIT_FAILURE;

        if (read(fd_err[0], errbuf, errlen) < 0) return CUSTOM_EXIT_FAILURE;

        char temp_buff[BUFSIZ];

        /** Gotta empty the pipe if not pipe will be blocked and waitpid will wait forever */
        while (read(fd_out[0], temp_buff, BUFSIZ) > 0);
        while (read(fd_err[0], temp_buff, BUFSIZ) > 0);

        if (waitpid(pid, &status, 0) < 0) return CUSTOM_EXIT_FAILURE;

        (void)close(fd_out[0]);
        (void)close(fd_err[0]);

        return status;
    } else if (pid == 0) {
        /** Child, closing the read end */
        (void)close(fd_out[0]);
        (void)close(fd_err[0]);

        if (dup2(fd_out[1], STDOUT_FILENO) != STDOUT_FILENO) {
            _exit(CUSTOM_EXIT_FAILURE);
        }

        if (dup2(fd_err[1], STDERR_FILENO) != STDERR_FILENO) {
            _exit(CUSTOM_EXIT_FAILURE);
        }

        (void)close(fd_err[1]);
        (void)close(fd_out[1]);

        if (execl("/bin/sh", "sh", "-c", string, (char *) NULL) < 0) {
            _exit(127);
        }

        _exit(EXIT_SUCCESS);
    }

    return status;
}


int main() {
    char out[BUFSIZ], err[BUFSIZ];

    if (command("ls -l", out, BUFSIZ-1, err, BUFSIZ-1) == -1) {
        perror("command");
        exit(EXIT_FAILURE);
    }
    out[BUFSIZ-1] = '\0';
    err[BUFSIZ-1] = '\0';
    (void)printf("stdout:\n%s\n", out);
    (void)printf("stderr:\n%s\n", err);
    return EXIT_SUCCESS;
}
