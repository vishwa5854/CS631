#include<sys/types.h>
#include<fts.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

struct FLAGS_STRUCT {
    bool A;
    bool a;
    bool c;
    bool d;
    bool F;
    bool f;
    bool h;
    bool i;
    bool k;
    bool l;
    bool n;
    bool q;
    bool R;
    bool r;
    bool S;
    bool s;
    bool t;
    bool u;
    bool w;
    bool show_hidden_files;
};

void print(struct FLAGS_STRUCT *flags, FTSENT* node);

void recurse(FTSENT* one, FTS* handle) {
    while ((one = fts_read(handle)) != NULL) {
        char* paths[2];
        paths[0] = one->fts_path;
        paths[1] = NULL;
        char* const* req = paths;
        FTS* twoHandler = fts_open(req, FTS_LOGICAL, 0);
        FTSENT* two = NULL;

        if (one->fts_info == FTS_DP) {
            continue;
        }

        if (one->fts_info == FTS_D) {
            (void)printf("\n%s:", one->fts_accpath);
        }
        two = fts_read(twoHandler);

        while ((two = fts_read(twoHandler)) != NULL) {
            if (two->fts_info == FTS_DP) {
                continue;
            }
            (void)printf("%s\n", two->fts_name);

            if (two->fts_info == FTS_D) {
                (void)fts_set(twoHandler, two, FTS_SKIP);
            }
        }
        (void)fts_close(twoHandler);
        if (one->fts_info == FTS_D)  {
            (void)printf("\n");
        }
    }
}

int main(int argc, char* const argv[]) {
    FTSENT* node = NULL;
    // FTSENT* child = NULL;
    struct FLAGS_STRUCT flags;
    flags.R = false;
    char* paths[3];
    paths[0] = argv[1];
    paths[1] = argv[2];
    paths[1] = NULL;

    char* const* required = paths;

    FTS* handle = fts_open(required, FTS_LOGICAL, 0);
    int return_value = EXIT_SUCCESS;

    (void)printf("\n%d\n", argc);
    node = fts_read(handle);

    // If the given file is not a directory.
    if (node->fts_info != FTS_D) {
        (void)printf("%s\n", node->fts_name);
    }

    FTSENT* child = NULL;
    if (flags.R) {
        (void)printf("Going into R\n");
        recurse(node, handle);
        return return_value;
    }
    while ((node = fts_read(handle)) != NULL) {
        int parent_err_conditions = (node->fts_info == FTS_ERR) ||
                                    (node->fts_info == FTS_DNR);
        if (parent_err_conditions) {
            /** There is no point in pursuing this parent. */
            fprintf(stderr, "ls: Error while traversing %s.\n", strerror(node->fts_errno));
            return_value = EXIT_FAILURE;
            continue;
        }

        if (node->fts_info != FTS_DP)
            (void)printf("%s\n", node->fts_name);

        if (node->fts_info == FTS_D) {
            if (!flags.R) {
                (void)fts_set(handle, node, FTS_SKIP);
            }
        }
    }
    (void)fts_close(handle);
    return return_value;
}


/**
 * Test Cases
 * 1. Dir with ending /
 * 2. Dir without ending /
 * 3. Normal file
*/
