#include<bsd/string.h>
#include<grp.h>
#include"print.h"
#include<pwd.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<time.h>
#include"util.h"

#define STRMODE_LENGTH 11

void print(struct FLAGS_STRUCT *flags, FTSENT* node) {
    if (flags->i) {
        printf("%ld ", node->fts_statp->st_ino);
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
        
        printf("%ld ",node->fts_statp->st_nlink);

        if (flags->n) {
            printf("%d ", node->fts_statp->st_uid);
        } else {
            struct passwd *user_info;
            user_info = getpwuid(node->fts_statp->st_uid);
            printf("%s ", user_info->pw_name);
        }

        if (flags->n) {
            printf("%d ", node->fts_statp->st_gid);
        } else {
            struct group *group_info;
            group_info = getgrgid(node->fts_statp->st_gid);
            printf("%s ", group_info->gr_name);
        }

        printf("%ld ", node->fts_statp->st_size);
        
        struct tm *tm;

        if ((tm = localtime(&node->fts_statp->st_mtim.tv_sec)) == NULL) {
            // For now I am going to skip this error.
            fprintf(stderr, "ls: Error while fetching the local time");
        }
        printf("%s ", which_month(tm->tm_mon));
        printf("%d ", tm->tm_mday);
        printf("%02d:%02d ", tm->tm_hour, tm->tm_min);
    }

    if (flags->d) {
        if (node->fts_info == FTS_D) {
            printf("%s\n", node->fts_name);
        }

        return;
    }
    printf("%s\n", node->fts_name);
}
