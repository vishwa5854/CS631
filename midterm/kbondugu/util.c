#include<stdio.h>
#include<unistd.h>

char* which_month(int month) {
    char* months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    return months[month];
}

long int get_number_of_digits(long int number) {
    long int count = 0;

    if (number == 0) {
        return 1;
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