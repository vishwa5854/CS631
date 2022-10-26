#include<errno.h>
#include"preprocess.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>

#define MIN_ARGS 1

void preprocess_files(int argc, char** args, FT* args_meta) {
    args_meta->n_files = 0;
    args_meta->n_directories = 0;
    int i = 0;

    /** Just ./ls */
    if (argc == MIN_ARGS) {
        args_meta->directories[args_meta->n_directories++] = ".";
        return;
    }

    if (argc == MIN_ARGS + 1) {
        /** ./ls [options] */
        if (args[1][0] == '-') {
            args_meta->directories[args_meta->n_directories++] = ".";
            return;
        } else {
            /** ./ls [file] */
            i = 1;
        }
    }

    if (argc > MIN_ARGS + 1) {
        /** ./ls [options] [files] */
        if (args[1][0] == '-') {
            i = 2;
        } else {
            /** ./ls [files] */
            i = 1;
        }
    }
    
    for (; i < argc; i++) {
        struct stat file_info;

        if (stat(args[i], &file_info) < 0) {
            if (errno == ENOENT) {
                fprintf(stderr, "ls: %s: No such file or directory\n", args[i]);
            } else {
                fprintf(stderr, "ls: cannot stat %s : %s\n", args[i], strerror(errno));
            }
            continue;
        }

        if (S_ISDIR(file_info.st_mode)) {
            // args_meta->directories[i] = malloc((strlen(args[i]) + 1) * sizeof(char));
            args_meta->directories[args_meta->n_directories++] = args[i];
            // strncat(args_meta->directories[i], "/", 1);
        } else {
            args_meta->files[args_meta->n_files++] = args[i];
        }
    }
    args_meta->directories[args_meta->n_directories] = NULL;
    args_meta->files[args_meta->n_files] = NULL;
}