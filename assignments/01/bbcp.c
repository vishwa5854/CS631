/**
 * This simple program will trivially copy a file.
*/
#include <sys/stat.h>

#include<stdio.h>
#include<fcntl.h>
#include<err.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#define SOURCE 1
#define MINIMUM_TARGETS 2

#ifndef BUFFSIZE
#define BUFFSIZE 32768
#endif

void
copy(char *source, char *target) {
    struct stat sourceFileInfo;
    char buffer[BUFFSIZE];
    int n, targetFd = -1, sourceFd = -1;

    if (stat(source, &sourceFileInfo) < 0) {
        fprintf(stderr, "bbcp: cannot stat '%s': %s\n", source, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (S_ISDIR(sourceFileInfo.st_mode)) {
        fprintf(stderr, "bbcp: source cannot be a dir\n");
        exit(EXIT_FAILURE);
    }
    
    if ((sourceFd = open(source, O_RDONLY)) < 0) {
        fprintf(stderr, "bbcp: cannot open '%s': %s\n", source, strerror(errno));
        exit(EXIT_FAILURE);
    }
    struct stat targetFileInfo;

    if (stat(target, &targetFileInfo) < 0) {
        if (ENOENT != errno) {
            fprintf(stderr, "bbcp: cannot stat '%s': %s\n", target, strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (errno == ENOENT) {
            if ((targetFd = creat(target, sourceFileInfo.st_mode)) < 0) {
                fprintf(stderr, "bbcp: cannot open '%s': %s\n", target, strerror(errno));
                exit(EXIT_FAILURE);
            }
            printf("%d", targetFd);
        }
    }

    if (targetFd == -1) {
        if ((targetFd = open(target, O_WRONLY | O_TRUNC)) < 0) {
            fprintf(stderr, "bbcp: cannot open '%s': %s\n", target, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

// && S_ISREG(targetFileInfo.st_mode)
    if ((targetFd != -1)) {
        printf("Induloki ochana raleda ?");
        while((n = read(sourceFd, buffer, BUFFSIZE)) > 0) {
            if (write(targetFd, buffer, n) != n) {
                fprintf(stderr, "bbcp: cannot write into '%s' : %s\n", target, strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }
    (void)close(sourceFd);
    (void)close(targetFd);
    return;
}

int
main(int argc, char **argv) {
    char *message = "Please specify a target";

    if (argc < MINIMUM_TARGETS + 1) {
        if (argc < MINIMUM_TARGETS) {
            message = "Source cannot be empty";
        }
        fprintf(stderr, message);
        return EXIT_FAILURE;   
    }
    
    for (int i = MINIMUM_TARGETS; i < argc; i ++) {
        printf("%s\n", argv[i]);
        copy(argv[SOURCE], argv[i]);
    }
    return EXIT_SUCCESS;
}
