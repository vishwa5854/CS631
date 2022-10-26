#include"print.h"
#include"sort.h"
#include<stdbool.h>
#include<stdio.h>
#include<string.h>
#include"structures.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<fts.h>

void ls(char* const* file_names, int FTS_OPTIONS, int (*sorter)(const FTSENT **, const FTSENT **), bool is_dir, FLAGS* flags) {
    PF* print_buffer_head = NULL;
    PF* print_buffer_current = (PF*)malloc(sizeof(PF));
    MP* max_map = (MP*)malloc(sizeof(MP));
    int n_files = 0, j = 0;
    FTS* handle = fts_open(file_names, FTS_OPTIONS, sorter);
    FTSENT* node;

    while ((node = fts_read(handle)) != NULL) {
        bool errors = (node->fts_info == FTS_ERR) ||
                     (node->fts_info == FTS_DNR);
        
        if (errors) {
            fprintf(stderr, "ls: Error while traversing %s.\n", strerror(node->fts_errno));
            continue;
        }

        if ((n_files++ == 0) && is_dir) {
            (void)printf("%s:\n", node->fts_path);
            continue;
        }

        if (node->fts_info != FTS_DP) {
            print_buffer_current->next = (PF*)malloc(sizeof(PF));
            print_buffer_current = print_buffer_current->next;
            print(flags, node, print_buffer_current, max_map);
            n_files++;
        }

        bool skip_conditions = (node->fts_info == FTS_D) && !is_dir;
        skip_conditions = skip_conditions || ((node->fts_level == 1) && is_dir);

        if (skip_conditions) {
            (void)fts_set(handle, node, FTS_SKIP);
        }

        /** Add a new entry for the upcoming iteration. */
        if (print_buffer_head == NULL) {
            print_buffer_head = print_buffer_current;
        }
    }

    while ((print_buffer_head != NULL) && (j < (n_files - 1))) {
        flush(print_buffer_head, max_map, flags);
        print_buffer_head = print_buffer_head->next;
        j++;
    }
    (void)fts_close(handle);
    free(print_buffer_head);
    free(print_buffer_current);
    free(max_map);
}

void ls_directories(char* const* file_names, int FTS_OPTIONS, int (*sorter)(const FTSENT **, const FTSENT **)) {
    PF* print_buffer_head = NULL;
    PF* print_buffer_current = (PF*)malloc(sizeof(PF));
    MP* max_map = (MP*)malloc(sizeof(MP));
    int n_files = 0;
    FTS* handle = fts_open(file_names, FTS_OPTIONS, sorter);
    FTSENT* node;

    while ((node = fts_read(handle)) != NULL) {
        bool errors = (node->fts_info == FTS_ERR) ||
                     (node->fts_info == FTS_DNR);
        
        if (errors) {
            fprintf(stderr, "ls: Error while traversing %s.\n", strerror(node->fts_errno));
            continue;
        }

        if (n_files++ == 0) {
            (void)printf("%s:\n", node->fts_path);
            continue;
        }

        if (node->fts_info != FTS_DP) {
            (void)printf("%s\n", node->fts_name);
        }

        if (node->fts_level == 1) {
            (void)fts_set(handle, node, FTS_SKIP);
        }
        n_files++;
    }
    (void)fts_close(handle);
    free(print_buffer_head);
    free(print_buffer_current);
    free(max_map);
}
