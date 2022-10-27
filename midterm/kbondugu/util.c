#include<stdio.h>
#include"structures.h"
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fts.h>
#include<unistd.h>

char* which_month(int month) {
    char* months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    return months[month];
}

long int get_number_of_digits(long int number) {
    long int count = 0;

    if (number == 0) {
        // return 1;
    }

    while (number != 0) {
      number = number / 10;
      count++;
    }
    return count;
}

long int just_max(long int a, long int b) {
    return a > b ? a : b;
}

long int max_of_two(long int a, long int b) {
    long int A = a;
    long int B = get_number_of_digits(b);
    return A >= B ? A : B;
}

void swap(char** x, char** y) {
    char* temp;
    temp = *x;
    *x = *y;
    *y = temp;
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

void convert_bytes_to_human_readable(double size, char *buf) {
    int i = 0;
    const char* units[] = {"B", "K", "M", "G", "T", "P", "E", "Z", "Y"};
    int scope_of_this_function = 8;

    while (size > 1024) {
        size /= 1024;
        i++;
    }

    if (i <= scope_of_this_function) {
        sprintf(buf, "%.*f%s", i, size, units[i]);
    }
    return;
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

int init_flags_from_args(FLAGS* flags, char* args) {
    int FTS_FLAGS = FTS_LOGICAL;
    size_t n_args = strlen(args);
    size_t i;

    for (i = 0; i < n_args; i++) {
        if (args[i] == '-') {
            continue;
        }

        switch (args[i]) {
            case 'A':
                flags->A = true;
                break;
            case 'a':
                flags->a = true;
                flags->A = true;
                FTS_FLAGS = FTS_FLAGS | FTS_SEEDOT;
                break;
            case 'c':
                flags->c = true;
                break;
            case 'd':
                flags->d = true;
                break;
            case 'F':
                flags->F = true;
                break;
            case 'f':
                flags->f = true;
                break;
            case 'h':
                flags->h = true;
                break;
            case 'i':
                flags->i = true;
                break;
            case 'k':
                flags->k = true;
                break;
            case 'l':
                flags->l = true;
                break;
            case 'n':
                flags->n = true;
                break;
            case 'q':
                flags->q = true;
                break;
            case 'R':
                flags->R = true;
                break;
            case 'r':
                flags->r = true;
                break;
            case 'S':
                flags->S = true;
                break;
            case 's':
                flags->s = true;
                break;
            case 't':
                flags->t = true;
                break;
            case 'u':
                flags->u = true;
                break;
            case 'w':
                flags->w = true;
                break;
        }
    }
    return FTS_FLAGS;
}