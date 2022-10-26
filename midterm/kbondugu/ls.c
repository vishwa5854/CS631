#include<fts.h>
#include<stdbool.h>
#include<stdio.h>
#include<string.h>
#include"structures.h"

void ls(char* const* file_names, int FTS_OPTIONS, int* sorter) {
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

        if (node->fts_info != FTS_DP) {
            (void)printf("%s\n", node->fts_name);
        }

        if (node->fts_info == FTS_D) {
            if (!flags.R) {
                (void)fts_set(handle, node, FTS_SKIP);
            }
        }
        n_files++;
    }
    (void)fts_close(handle);
    free(print_buffer_head);
    free(print_buffer_current);
    free(max_map);
    free(handle);
    free(node);
}