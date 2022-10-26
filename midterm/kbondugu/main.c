#include"preprocess.h"
#include"structures.h"
#include<errno.h>

#define MIN_ARGS 1

int main(int argc, char ** argv) {
    FT args_meta;
    args_meta.files = malloc(sizeof(char) * argc);
    args_meta.directories = malloc(sizeof(char) * argc);
    preprocess_files(argc, argv, &args_meta);
    
    /** We can group all the files together and call ls. */

    free(&args_meta);
    return errno;
}