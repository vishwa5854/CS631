#include<stdio.h>

char* which_month(int month) {
    char* months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    return months[month];
}

long int get_number_of_digits(long int number) {
    long int count = 0;

    while (number != 0) {
      number = number / 10;
      count++;
    }
    return count;
}

int max(int a, int b) {
    return a >= b ? a : b;
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