#include<bsd/stdlib.h>
#include<bsd/string.h>
#include<ctype.h>
#include<grp.h>
#include<math.h>
#include"print.h"
#include<pwd.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<time.h>
#include"util.h"

#define STRMODE_LENGTH 11
#define DEFAULT_BLOCK_SIZE 512

extern char **environ;

void print(struct FLAGS_STRUCT *flags, FTSENT* node) {
    if (flags->i) {
        printf("%ld ", node->fts_statp->st_ino);
    }

    if (flags->s) {
        int header_length;
        long block_size;
        (void)getbsize(&header_length, &block_size);
        float block_size_factor = 0;
        float effective_number_of_blocks;

        if (block_size != DEFAULT_BLOCK_SIZE) {
            block_size_factor = block_size / DEFAULT_BLOCK_SIZE;
            effective_number_of_blocks = node->fts_statp->st_blocks / block_size_factor;
            printf("%d ", (int)ceil(effective_number_of_blocks));
        } else {
            printf("%ld ", node->fts_statp->st_blocks);
        }
    }

    if (flags->l || flags->n) {
        /**
         * 1. file mode
         * 2. number of links
         * 3. owner name
         * 4. group name
         * 5. number of bytes in file
         * 6. abbreviated month file was last modified
         * 7. day-of-month file was last modified
         * 8. hour and minute file was last modified
         * 9. pathname
        */

        // uncomment this on netbsd -- TODO while submission get rid of all linux hacks.
        char mode_string[STRMODE_LENGTH];
        strmode(node->fts_statp->st_mode, mode_string);
        printf("%s ", mode_string);
        
        printf("%d ",node->fts_statp->st_nlink);

        if (flags->n) {
            printf("%d ", node->fts_statp->st_uid);
        } else {
            struct passwd *user_info;
            user_info = getpwuid(node->fts_statp->st_uid);

            if (user_info != NULL) {
                printf("%s ", user_info->pw_name);
            } else {
                printf("%d ", node->fts_statp->st_uid);
            }
        }

        if (flags->n) {
            printf("%d ", node->fts_statp->st_gid);
        } else {
            struct group *group_info;
            group_info = getgrgid(node->fts_statp->st_gid);

            if (group_info != NULL) {
                printf("%s ", group_info->gr_name);
            } else {
                printf("%d ", node->fts_statp->st_gid);
            }
        }

        if (flags->h) {

        } else {
            printf("%ld ", node->fts_statp->st_size);
        }
        
        struct tm *tm;

        if ((tm = localtime(&node->fts_statp->st_mtim.tv_sec)) == NULL) {
            // For now I am going to skip this error.
            fprintf(stderr, "ls: Error while fetching the local time");
        }
        printf("%s ", which_month(tm->tm_mon));
        printf("%d ", tm->tm_mday);
        printf("%02d:%02d ", tm->tm_hour, tm->tm_min);
    }
    char* file_name = node->fts_name;

    if (flags->q) {
        char temp[node->fts_namelen];
        char* replacement = "?";
        int j = 0;

        for (; j < node->fts_namelen; j++) {
            if (isprint(file_name[j]) != 0) {
                (void)strncat(temp, &file_name[j], 1);
            } else {
                (void)strncat(temp, replacement, 1);
            }
        }
        file_name = temp;
    }

    if (flags->d) {
        if (node->fts_info == FTS_D) {
            printf("%s\n", file_name);
        }

        return;
    }

    /** F arg prep */
    if (flags->F) {
        if (S_ISDIR(node->fts_statp->st_mode)) {
            printf("%s/\n", file_name);
        } else if (S_IEXEC & node->fts_statp->st_mode) {
            printf("%s*\n", file_name);
        } else if (S_ISLNK(node->fts_statp->st_mode)) {
            printf("%s@\n", file_name);
        } else if (S_IFCHR & node->fts_statp->st_mode) {
            printf("%s%c\n", file_name, '%');
        } else if (S_ISSOCK(node->fts_statp->st_mode)) {
            printf("%s=\n", file_name);
        } else if (S_ISFIFO(node->fts_statp->st_mode)) {
            printf("%s|\n", file_name);
        } else {
            printf("%s\n", file_name);
        }
    } else {
        printf("%s\n", file_name);
    }
}
