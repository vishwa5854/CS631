bool conditions = !flags.a;
        conditions = conditions && ((strncmp(".", node->fts_name, length_of_file_name) == 0) ||
                                    (strncmp("..", node->fts_name, length_of_file_name) == 0));
        conditions = conditions || (!flags.show_hidden_files &&
                                    (node->fts_name[0] == '.'));


#include<sys/stat.h>

#include<fts.h>
#include"sort.h"
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include"util.h"
#include"print.h"

/** executable [options] [file_name] */
#define MIN_NUM_ARGS 2
#define DEFAULT_LEVEL 1

struct FLAGS_STRUCT flags;
struct SORT_FLAGS sort_flags;

int set_sort_flags_and_call_sort(const FTSENT** one, const FTSENT** two) {
    sort_flags.f = flags.f;
    sort_flags.S = flags.S;
    sort_flags.t = flags.t;
    sort_flags.c = flags.c;
    sort_flags.u = flags.u;
    sort_flags.r = flags.r;
    sort_flags.l = flags.l;

    return sort(one, two, &sort_flags);
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

    if (raw_arguments[0] != '-') {
        return FTS_FLAGS;
    }

    for (i = 0; i < n; i++) {
        if (raw_arguments[i] != '-') {
            switch (raw_arguments[i]) {
                /** Dot Options*/
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

                /** Sorting Options */    
                case 'c':
                    flags.c = true;
                    flags.u = false;
                    break;
                case 'u':
                    flags.c = false;
                    flags.u = true;
                    break;
                case 't':
                    flags.t = true;
                    flags.S = false;
                    break;
                case 'S':
                    flags.S = true;
                    flags.t = false;
                    break;
                case 'f':
                    flags.f = true;
                    break;
                case 'r':
                    flags.r = true;
                    break;
                
                case 'i':
                    flags.i = true;
                    break;
                case 'l':
                    flags.l = true;
                    flags.n = false;
                    break;
                case 'n':
                    flags.l = false;
                    flags.n = true;
                    break;
                case 'd':
                    flags.d = true;
                    break;
                case 'q':
                    flags.q = true;
                    flags.w = false;
                    break;
                case 'w':
                    flags.w = true;
                    flags.q = false;
                    break;
                case 'F':
                    flags.F = true;
                    break;
                case 's':
                    flags.s = true;
                    break;
                case 'h':
                    flags.h = true;
                    break;
                case 'R':
                    flags.R = !flags.d && true;
                    break;
                default:
                    break;
            }
        }
    }

    return FTS_FLAGS;
}

void ls(FTS* handle, FTSENT* node, int FTS_FLAGS, char* const* file_paths) {
    // int return_value = EXIT_SUCCESS;
    handle = fts_open(file_paths, FTS_FLAGS, &set_sort_flags_and_call_sort);
    PF* print_buffer_head = NULL;
    PF* print_buffer_current = (PF*)malloc(sizeof(PF));
    MP* max_map = (MP*)malloc(sizeof(MP));
    max_map = init_max_map(max_map);

    node = fts_read(handle);

    // If the given file is not a directory.
    if (node->fts_info != FTS_D) {
        print(&flags, node, print_buffer_current, max_map);
    }

    while ((node = fts_read(handle)) != NULL) {
        int parent_err_conditions = (node->fts_info == FTS_ERR) ||
                                    (node->fts_info == FTS_DNR);
        if (parent_err_conditions) {
            /** There is no point in pursuing this parent. */
            fprintf(stderr, "ls: Error while traversing %s.\n", strerror(node->fts_errno));
            // return_value = EXIT_FAILURE;
            continue;
        }

        if (node->fts_info != FTS_DP) {
            print(&flags, node, print_buffer_current, max_map);
        }

        if (node->fts_info == FTS_D) {
            if (!flags.R) {
                (void)fts_set(handle, node, FTS_SKIP);
            }
        }

        /** Add a new entry for the upcoming iteration. */
        if (print_buffer_head == NULL) {
            print_buffer_head = print_buffer_current;
        }
        print_buffer_current->next = (PF*)malloc(sizeof(PF));
        print_buffer_current = print_buffer_current->next;
    }
    print_buffer_current = NULL;

    while ((print_buffer_head != NULL) && (print_buffer_head->next != NULL)) {
        flush(print_buffer_head, max_map, &flags);
        print_buffer_head = print_buffer_head->next;
    }
    (void)free(print_buffer_head);
    (void)fts_close(handle);
}

int main(int argc, char ** argv) {
    FTS* handle = NULL;
    FTSENT* node = NULL;
    int FTS_FLAGS = FTS_PHYSICAL;
    char* default_path[2] = {".", NULL};

    /** First of all let's check for the existence of files or dirs passed in argv */
    int number_of_errors = move_args_and_non_existent_files_to_top(argc, argv);
    char* paths[argc - number_of_errors + 1];

    if (argc >= MIN_NUM_ARGS) {
        FTS_FLAGS = set_args_to_struct(argv[1]);
        report_errors(number_of_errors, argv);

        /** There is a case where all the files passed don't exist and it leads to a SEGFAULT. */
        if ((argc == number_of_errors) && (argv[argc - 1][0] != '-')) {            
            return EXIT_FAILURE;
        }
        create_paths(argc, argv, number_of_errors, paths);
        paths[argc - number_of_errors] = NULL;
    }

    /** Iteratively call ls for each and every option once the errors are reported :) */
    int iteration = 0;
    int number_of_valid_files = argc - number_of_errors + 1;
    
    printf("%d\n", number_of_valid_files);
    if (number_of_valid_files <= 1) {
        char* const* file_paths = default_path;
        ls(handle, node, FTS_FLAGS, file_paths);
    }

    for (; iteration < number_of_valid_files - 1; iteration++) {
        char* effective_paths[2] = {paths[iteration], NULL}; 
        char* const* file_paths = (argc >= MIN_NUM_ARGS) && (argv[argc - 1][0] != '-') ?
                                                                effective_paths : default_path;
        ls(handle, node, FTS_FLAGS, file_paths);
    }
    return EXIT_SUCCESS;
}
