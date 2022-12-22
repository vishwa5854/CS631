//
// Created by z on 12/17/22.
//
#include "shell-builtins.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "data-structures.h"

void cd(char *path) {
    if (path == NULL) {
        if (getlogin_r(current_user, LOGIN_NAME_MAX) != 0) {
            perror("cd:");
        }
    }

    if (chdir(path) < 0) {
        perror("cd:");
        /** We should not exit because the shell should continue even when cd fails :) */
    }
}

void exit_shell() { exit(errno); }

void echo(char *word) {
    // TODO: Test whether strlen(word) works or not
    if (strncmp(word, "$?", strlen(word)) == 0) {
    } else if (strncmp(word, "$$", strlen(word)) == 0) {
    } else {
        (void)printf("%s\n", word);
    }
}