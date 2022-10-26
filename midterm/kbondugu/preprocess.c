#include<errno.h>
#include"preprocess.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>

extern int RETURN_VALUE;

void preprocess_files(int argc, char** args, FT* args_meta) {
    /** ukik 0 is exec name, 1 is flags */
    int i = 2;
    
    for (; i < argc; i++) {
        struct stat file_info;

        if (stat(args[i], &file_info) < 0) {
            fprintf(stderr, "ls: cannot stat %s : %s\n", args[i], strerror(errno));
            RETURN_VALUE = EXIT_FAILURE;
        }
    }
}