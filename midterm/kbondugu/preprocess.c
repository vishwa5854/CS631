#include<errno.h>
#include"preprocess.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>

void preprocess_files(int argc, char** args, FT* args_meta) {
    /** ukik 0 is exec name, 1 is flags */
    int i = 2;
    
    for (; i < argc; i++) {
        struct stat file_info;

        if (stat(args[i], &file_info) < 0) {
            if (errno == ENOENT) {
                fprintf(stderr, "ls: %s: No such file or directory\n", argv[i]);
            } else {
                fprintf(stderr, "ls: cannot stat %s : %s\n", args[i], strerror(errno));
            }
            continue;
        }

        if (S_ISDIR(file_info.st_mode)) {
            args_meta->directories[args_meta->n_directories++] = args[i];
        } else {
            args_meta->files[args_meta->n_files++] = args[i];
        }
    }
}