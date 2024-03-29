#include<errno.h>
#include"ls.h"
#include"preprocess.h"
#include<stdio.h>
#include<stdlib.h>
#include"structures.h"
#include"sort.h"
#include<unistd.h>
#include"util.h"

#define MIN_ARGS 1

FLAGS flags;

int sorter(const FTSENT** one, const FTSENT** two) {
    return sort(one, two, &flags);
}

int main(int argc, char ** argv) {
    setprogname(argv[0]);
    FT args_meta;
    int stdout_fd;
    int i;
    int FTS_OPTIONS = FTS_PHYSICAL;
    args_meta.files = malloc(sizeof(char*) * argc);
    args_meta.directories = malloc(sizeof(char*) * argc);
    preprocess_files(argc, argv, &args_meta);

    if ((geteuid() == 0) || (getuid() == 0)) {
        flags.A = true;
    }

    if ((argc > MIN_ARGS) && (argv[1][0] == '-')) {
        FTS_OPTIONS = init_flags_from_args(&flags, argv[1]);
        
        if (FTS_OPTIONS == -1) {
            return EXIT_FAILURE;
        }
    }

    if ((stdout_fd = fileno(stdout)) != -1) {
        if (isatty(stdout_fd) == 1) {
            flags.q = !flags.w && true;
        }
    } else {
        (void)fprintf(stderr, "ls: Cannot recognise the output stream");
    }
    
    if (args_meta.n_files > 0) {
        /** We can group all the files together and call ls. */
        ls(args_meta.files, FTS_OPTIONS, &sorter, false, &flags, false);

        if (args_meta.n_directories > 0) {
            (void)printf("\n");
        }
    }

    if (args_meta.n_directories > 0) {
        qsort(args_meta.directories, args_meta.n_directories, sizeof(char *), sort_by_name);

        for (i = 0; i < args_meta.n_directories; i++) {
            char *const required[2] = { args_meta.directories[i], NULL };
            ls(required, FTS_OPTIONS, &sorter, true, &flags, ((args_meta.n_files + args_meta.n_directories + args_meta.n_errors) > 1));
           
            if (i != (args_meta.n_directories - 1)) {
                (void)printf("\n");
            }
        }
    }

    return errno;
}
