#include"print.h"
#include"sort.h"
#include<stdbool.h>
#include<stdio.h>
#include<string.h>
#include"structures.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<fts.h>

void recurse(FTSENT* one, FTS* handle, int FTS_FLAGS, 
                int (*sorter)(const FTSENT **, const FTSENT **), FLAGS* flags) {
    while ((one = fts_read(handle)) != NULL) {
        char* paths[2];
        paths[0] = one->fts_path;
        paths[1] = NULL;
        char* const* req = paths;
        FTS* twoHandler = fts_open(req, FTS_FLAGS, sorter);
        FTSENT* two = NULL;

        if (one->fts_info == FTS_DP) {
            continue;
        }

        if (one->fts_info == FTS_D) {
            (void)printf("%s:\n", one->fts_accpath);
        }
        two = fts_read(twoHandler);
        PF* print_buffer_head = NULL;
        PF* print_buffer_current = (PF*)malloc(sizeof(PF));
        MP* max_map = (MP*)malloc(sizeof(MP));
        max_map = init_max_map(max_map);
        int ideal_number_of_entries = 0;
        int j = 0;

        while ((two = fts_read(twoHandler)) != NULL) {
            int parent_err_conditions = (two->fts_info == FTS_ERR) || (two->fts_info == FTS_DNR);
            if (parent_err_conditions) {
                (void)(void)fprintf(stderr, "ls: Error while traversing %s.\n", strerror(two->fts_errno));
                continue;
            }

            if (two->fts_info == FTS_DP) {
                continue;
            }
            ideal_number_of_entries++;
            print(flags, two, print_buffer_current, max_map, true);

            if (two->fts_info == FTS_D) {
                (void)fts_set(twoHandler, two, FTS_SKIP);
            }

            /** Add a new entry for the upcoming iteration. */
            if (print_buffer_head == NULL) {
                print_buffer_head = print_buffer_current;
            }
            print_buffer_current->next = (PF*)malloc(sizeof(PF));
            print_buffer_current = print_buffer_current->next;
        }
        print_buffer_current = NULL;

        while ((print_buffer_head != NULL) && (print_buffer_head->next != NULL) && (j < ideal_number_of_entries)) {
            flush(print_buffer_head, max_map, flags);
            print_buffer_head = print_buffer_head->next;
            j++;
        }
        (void)free(print_buffer_head);

        (void)fts_close(twoHandler);
        if (one->fts_info == FTS_D)  {
            (void)printf("\n");
        }
    }
}

void ls(char* const* file_names, int FTS_OPTIONS, 
    int (*sorter)(const FTSENT **, const FTSENT **), 
    bool is_dir, FLAGS* flags, bool print_dir_name
) {
    PF* print_buffer_current = (PF*)malloc(sizeof(PF));
    PF* print_buffer_head = print_buffer_current;
    MP* max_map = (MP*)malloc(sizeof(MP));
    max_map = init_max_map(max_map);
    int n_files = 0, n_files_usable = 0, iterator = 0;
    FTS* handle = fts_open(file_names, FTS_OPTIONS, flags->f ? NULL : sorter);
    FTSENT* node = NULL;

    if (flags->R && is_dir) {
        recurse(node, handle, FTS_OPTIONS, sorter, flags);
        return;
    }

    while ((node = fts_read(handle)) != NULL) {
        bool errors = (node->fts_info == FTS_ERR) ||
                     (node->fts_info == FTS_DNR) || (node->fts_info == FTS_DC);
        
        if (errors) {
            (void)fprintf(stderr, "ls: Error while traversing %s.\n", strerror(node->fts_errno));
            continue;
        }

        if ((n_files == 0) && is_dir && print_dir_name && !flags->d) {
            n_files++;
            (void)printf("%s:\n", node->fts_path);
            continue;
        }

        bool skip_conditions = (node->fts_info == FTS_D) && !is_dir;
        skip_conditions = skip_conditions || ((node->fts_level == 1) && is_dir);

        if (skip_conditions) {
            (void)fts_set(handle, node, FTS_SKIP);
        }

        if (node->fts_info != FTS_DP) {
            if (flags->d && is_dir) {
                if ((strlen(node->fts_name) > 1) || (node->fts_name[0] != '.')) {
                    continue;
                }
            }

            if ((strlen(node->fts_name) > 0) && (node->fts_name[0] == '.') && !flags->A) {
                continue;
            }

            if (strlen(node->fts_name) > 0) {
                print_buffer_current->next = (PF*)malloc(sizeof(PF));
                print(flags, node, print_buffer_current, max_map, is_dir);
                print_buffer_current = print_buffer_current->next;
                n_files++;
                n_files_usable++;

                if (flags->d && is_dir) {
                    break;
                }
            }
        }
    }

    while ((print_buffer_head != NULL) && (iterator < n_files_usable)) {
        if (strlen(print_buffer_head->file_name) != 0) {
            flush(print_buffer_head, max_map, flags);
        }
        print_buffer_head = print_buffer_head->next;
        iterator++;
    }
    (void)fts_close(handle);
    free(print_buffer_head);
    free(print_buffer_current);
    free(max_map);
}
