#include<sys/types.h>
#include<sys/stat.h>

#include<fts.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

/** executable [options] [file_name] */
#define MIN_NUM_ARGS 2

struct FLAGS_STRUCT {
    bool A;
    bool a;
    bool c;
    bool i;
    bool show_hidden_files;
} flags;

int compare(const FTSENT** one, const FTSENT** two) {
    if (flags.c) {
        return ((*one)->fts_statp->st_ctime > (*two)->fts_statp->st_ctime);
    }

    return (strcmp((*one)->fts_name, (*two)->fts_name));
}

void swap(char** x, char** y) {
    char* temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

int move_args_and_non_existent_files_to_top(int N, char ** paths) {
    int i;
    int slot_available = 1;

    /** These are the arguments passed to our ls. */
    if ((N > 1) && (paths[slot_available][0] == '-')) {
        slot_available++;
    }

    for (i = slot_available; i < N; i ++) {
        if (access(paths[i], F_OK) != 0) {
            swap(&paths[slot_available++], &paths[i]);
        }
    }

    /** Here the slot_available would represent the number of
     * non-existent files + arguments to ls (-lah)
     * NOTE: It is not slot_available - 1 but it is slot_available
     * since argv contains the name of the executable as entry
     */
    return slot_available;
}

void create_paths(int N, char ** paths, int start, char** required_paths) {
    int i = start;

    while (i < N) {
        required_paths[i - start] = paths[i];
        i++;
    }
}

void report_errors(int N, char** argv) {
    int i = 1;

    /** These are the arguments passed to our ls. */
    if (argv[i][0] == '-') {
        i++;
    }

    for (; i < N; i++) {
        fprintf(stderr, "ls: %s: No such file or directory\n", argv[i]);
    }
}

int set_args_to_struct(char *raw_arguments) {
    int FTS_FLAGS = FTS_LOGICAL;
    size_t n = strlen(raw_arguments);
    size_t i;
    flags.show_hidden_files = false;

    if (geteuid() == 0) {
        FTS_FLAGS = FTS_FLAGS | FTS_SEEDOT;
        flags.show_hidden_files = true;
    }

    for (i = 0; i < n; i++) {
        if (raw_arguments[i] != '-') {
            switch (raw_arguments[i]) {
                case 'A':
                    flags.A = true;
                    FTS_FLAGS = FTS_FLAGS | FTS_SEEDOT;
                    flags.show_hidden_files = true;
                    break;
                case 'a':
                    flags.a = true;
                    FTS_FLAGS = FTS_FLAGS | FTS_SEEDOT;
                    flags.show_hidden_files = true;
                    break;
                case 'c':
                    flags.c = true;
                    break;
                case 'i':
                    flags.i = true;
                    break;
                default:
                    break;
            }
        }
    }

    return FTS_FLAGS;
}

void print(struct FLAGS_STRUCT *flags, FTSENT* node) {
    if (flags->i) {
        printf("%ld ", node->fts_statp->st_ino);
    }
    printf("%s\n", node->fts_name);
}

int main(int argc, char ** argv) {
    FTS* handle = NULL;
    FTSENT* node = NULL;
    int FTS_FLAGS = FTS_LOGICAL;
    char* default_path[2] = {".", NULL};

    /** First of all let's check for the existence of files or dirs passed in argv */
    int number_of_errors = move_args_and_non_existent_files_to_top(argc, argv);
    char* paths[argc - number_of_errors + 1];

    if (argc >= MIN_NUM_ARGS) {
        FTS_FLAGS = set_args_to_struct(argv[1]);
        report_errors(number_of_errors, argv);
        create_paths(argc, argv, number_of_errors, paths);
        paths[argc - number_of_errors] = NULL;
    }
    char* const* file_paths = (argc >= MIN_NUM_ARGS) && (argv[argc - 1][0] != '-') ? 
                                                            paths : default_path;
    handle = fts_open(file_paths, FTS_FLAGS, &compare);

    while (fts_read(handle) != NULL) {
        node = fts_children(handle, 0);

        while ((node != NULL) && (node->fts_level == 1)) {
            size_t length_of_file_name = strlen(node->fts_name);
            bool conditions = !flags.a;
            conditions = conditions && ((strncmp(".", node->fts_name, length_of_file_name) == 0) ||
                                        (strncmp("..", node->fts_name, length_of_file_name) == 0));
            conditions = conditions || (!flags.show_hidden_files &&
                                        (node->fts_name[0] == '.'));

            if (conditions) {
                node = node->fts_link;
                continue;
            }
            print(&flags, node);
            node = node->fts_link;
        }
    }
    (void)fts_close(handle);
    return EXIT_SUCCESS;
}