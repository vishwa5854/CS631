#include<unistd.h>
#include<stdlib.h>
#include<string.h>
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

#define DEFAULT_BLOCK_SIZE 512

extern char **environ;

void print(struct FLAGS_STRUCT* flags, FTSENT* node, PF* print_buffer, MP* max_map) {
    if (flags->i) {
        print_buffer->st_ino = node->fts_statp->st_ino;
        max_map->st_ino = max_of_two(max_map->st_ino, node->fts_statp->st_ino);
    }

    if (flags->s) {
        int header_length;
        long block_size;
        (void)getbsize(&header_length, &block_size);
        float block_size_factor = 0;
        float effective_number_of_blocks;

        if (flags->h) {
            convert_bytes_to_human_readable(node->fts_statp->st_size, print_buffer->bytes_in_human_readable);
            max_map->bytes_in_human_readable = just_max(max_map->bytes_in_human_readable, strlen(print_buffer->bytes_in_human_readable));
        } else {
            if (block_size != DEFAULT_BLOCK_SIZE) {
                block_size_factor = block_size / DEFAULT_BLOCK_SIZE;
                effective_number_of_blocks = node->fts_statp->st_blocks / block_size_factor;
                print_buffer->effective_number_of_blocks = ceil(effective_number_of_blocks);
                max_map->effective_number_of_blocks = max_of_two(max_map->effective_number_of_blocks, print_buffer->effective_number_of_blocks);
            } else {
                print_buffer->st_blocks = node->fts_statp->st_blocks;
                max_map->st_blocks = max_of_two(max_map->effective_number_of_blocks, print_buffer->st_blocks);
            }
        }
    }

    if (flags->l || flags->n) {
        strmode(node->fts_statp->st_mode, print_buffer->mode_string);
        max_map->mode_string = just_max(max_map->mode_string, strlen(print_buffer->mode_string));
        
        print_buffer->st_nlink = node->fts_statp->st_nlink;
        max_map->st_nlink = max_of_two(max_map->st_nlink, node->fts_statp->st_nlink);

        if (flags->n) {
            print_buffer->st_uid = node->fts_statp->st_uid;
            max_map->st_uid = max_of_two(max_map->st_uid, node->fts_statp->st_uid);
        } else {
            struct passwd *user_info;
            user_info = getpwuid(node->fts_statp->st_uid);

            if ((user_info != NULL) && (strlen(user_info->pw_name) > 0)) {
                (void)strncpy(print_buffer->pw_name, user_info->pw_name, strlen(user_info->pw_name));
                print_buffer->pw_name[strlen(user_info->pw_name)] = '\0';
                max_map->pw_name = just_max(max_map->pw_name, strlen(user_info->pw_name));
            } else {
                print_buffer->st_uid = node->fts_statp->st_uid;
                max_map->st_uid = max_of_two(max_map->st_uid, node->fts_statp->st_uid);
            }
            max_map->st_uid = max_of_two(max_map->pw_name, max_map->st_uid);
            max_map->pw_name = max_of_two(max_map->pw_name, max_map->st_uid);
        }

        if (flags->n) {
            print_buffer->st_gid = node->fts_statp->st_gid;
            max_map->st_gid = max_of_two(max_map->st_gid, node->fts_statp->st_gid);
        } else {
            struct group *group_info;
            group_info = getgrgid(node->fts_statp->st_gid);

            if ((group_info != NULL) && (strlen(group_info->gr_name) > 0)) {
                (void)strncpy(print_buffer->gr_name, group_info->gr_name, strlen(group_info->gr_name));
                print_buffer->gr_name[strlen(group_info->gr_name)] = '\0';
                max_map->gr_name = just_max(max_map->gr_name, strlen(group_info->gr_name));
            } else {
                print_buffer->st_gid = node->fts_statp->st_gid;
                max_map->st_gid = max_of_two(max_map->st_gid, node->fts_statp->st_gid);
            }
            max_map->st_gid = max_of_two(max_map->gr_name, max_map->st_gid);
            max_map->gr_name = max_of_two(max_map->gr_name, max_map->st_gid);
        }

        if (flags->h) {
            convert_bytes_to_human_readable(node->fts_statp->st_size, print_buffer->bytes_in_human_readable);
            max_map->bytes_in_human_readable = just_max(max_map->bytes_in_human_readable, strlen(print_buffer->bytes_in_human_readable));
        } else {
            print_buffer->st_size = node->fts_statp->st_size;
            max_map->st_size = max_of_two(max_map->st_size, node->fts_statp->st_size);
        }
        
        struct tm *tm;

        if ((tm = localtime(&node->fts_statp->st_mtim.tv_sec)) == NULL) {
            // For now I am going to skip this error.
            fprintf(stderr, "ls: Error while fetching the local time");
        }

        (void)strncpy(print_buffer->which_month, which_month(tm->tm_mon), strlen(which_month(tm->tm_mon)));
        print_buffer->which_month[strlen(which_month(tm->tm_mon))] = '\0';
        max_map->which_month = just_max(max_map->which_month, strlen(which_month(tm->tm_mon)));

        print_buffer->tm_mday = tm->tm_mday;
        max_map->tm_mday = max_of_two(max_map->tm_mday, tm->tm_mday);     

        print_buffer->tm_hour = tm->tm_hour;
        max_map->tm_hour = max_of_two(max_map->tm_hour, tm->tm_hour);

        print_buffer->tm_min = tm->tm_min;
        max_map->tm_min = max_of_two(max_map->tm_mday, tm->tm_min);
    }
    char* file_name = node->fts_name;

    if (flags->q) {
        char temp[node->fts_namelen];
        char* replacement = "?";
        size_t j = 0;

        for (; j < node->fts_namelen; j++) {
            if (isprint(file_name[j]) != 0) {
                (void)strncat(temp, &file_name[j], 1);
            } else {
                (void)strncat(temp, replacement, 1);
            }
        }
        file_name = temp;
    }

    strncpy(print_buffer->file_name, file_name, strlen(file_name));
    print_buffer->file_name[strlen(file_name)] = '\0';
    max_map->file_name = just_max(max_map->file_name, strlen(file_name));

    strncpy(print_buffer->file_path, node->fts_accpath, strlen(node->fts_accpath));
    print_buffer->file_path[strlen(node->fts_accpath)] = '\0';

    /** F arg prep */
    if (flags->F) {
        print_buffer->st_mode = node->fts_statp->st_mode;        
    } else {
    }
}

MP* init_max_map(MP* max_map) {
    max_map->st_ino = -1;
    max_map->bytes_in_human_readable = -1;
    max_map->effective_number_of_blocks = -1;
    max_map->st_blocks = -1;
    max_map->mode_string = -1;
    max_map->st_nlink = -1;
    max_map->st_uid = -1;
    max_map->pw_name = -1;
    max_map->stat = -1;
    max_map->gr_name = -1;
    max_map->st_size = -1;
    max_map->which_month = -1;
    max_map->tm_mday = -1;
    max_map->tm_hour = -1;
    max_map->tm_min = -1;
    max_map->file_name = -1;

    return max_map;
}

void print_empty_spaces(long int how_many) {
    long int i = 0;

    for (; i < how_many; i++) {
        (void)printf(" ");
    }
}

void flush(PF* print_buffer, MP* max_map, struct FLAGS_STRUCT* flags) {    
    if (flags->i) {
        print_empty_spaces(max_map->st_ino - get_number_of_digits(print_buffer->st_ino));
        (void)printf("%ld ", print_buffer->st_ino);
    }

    if (flags->s) {
        if (flags->h) {
            print_empty_spaces(max_map->bytes_in_human_readable - strlen(print_buffer->bytes_in_human_readable));
            (void)printf("%s ", print_buffer->bytes_in_human_readable);
        } else {
            if (max_map->effective_number_of_blocks > -1) {
                print_empty_spaces(max_map->effective_number_of_blocks - get_number_of_digits(print_buffer->effective_number_of_blocks));
                (void)printf("%f ", print_buffer->effective_number_of_blocks);
            } else {
                print_empty_spaces(max_map->st_blocks - get_number_of_digits(print_buffer->st_blocks));
                (void)printf("%ld ", print_buffer->st_blocks);
            }
        }
    }

    if (flags->l || flags->n) {
        print_empty_spaces(max_map->mode_string - strlen(print_buffer->mode_string));
        (void)printf("%s ", print_buffer->mode_string);

        print_empty_spaces(max_map->st_nlink - get_number_of_digits(print_buffer->st_nlink));
        (void)printf("%d ",print_buffer->st_nlink);

        if (flags->n) {
            print_empty_spaces(max_map->st_uid - get_number_of_digits(print_buffer->st_uid));
            (void)printf("%d ", print_buffer->st_uid);
        } else {
            if ((max_map->pw_name > 0) && (strlen(print_buffer->pw_name) > 0)) {
                print_empty_spaces(max_map->pw_name - strlen(print_buffer->pw_name));
                (void)printf("%s ", print_buffer->pw_name);
            } else {
                print_empty_spaces(max_map->st_uid - get_number_of_digits(print_buffer->st_uid));
                (void)printf("%d ", print_buffer->st_uid);
            }
        }

        if (flags->n) {
            print_empty_spaces(max_map->st_gid - get_number_of_digits(print_buffer->st_gid));
            (void)printf("%d ", print_buffer->st_gid);
        } else {
            if (max_map->gr_name > -1) {
                print_empty_spaces(max_map->gr_name - strlen(print_buffer->gr_name));
                (void)printf("%s ", print_buffer->gr_name);
            } else {
                print_empty_spaces(max_map->st_gid - get_number_of_digits(print_buffer->st_gid));
                (void)printf("%d ", print_buffer->st_gid);
            }
        }

        if (flags->h) {
            print_empty_spaces(max_map->bytes_in_human_readable - strlen(print_buffer->bytes_in_human_readable));
            (void)printf("%s ", print_buffer->bytes_in_human_readable);
        } else {
            print_empty_spaces(max_map->st_size - get_number_of_digits(print_buffer->st_size));
            (void)printf("%ld ", print_buffer->st_size);
        }

        print_empty_spaces(max_map->which_month - strlen(print_buffer->which_month));
        (void)printf("%s ", print_buffer->which_month);

        print_empty_spaces(max_map->tm_mday - get_number_of_digits(print_buffer->tm_mday));
        (void)printf("%02d ", print_buffer->tm_mday);

        (void)printf("%02d:%02d ", print_buffer->tm_hour, print_buffer->tm_min);
    }

    if (strlen(print_buffer->file_name) == 0) {
        return;
    }

    if (flags->d) {
        if (max_map->file_name > -1) {
            (void)printf("%s\n", print_buffer->file_name);
        }

        return;
    }

    /** F arg prep */
    if (flags->F) {
        if (S_ISDIR(print_buffer->st_mode)) {
            (void)printf("%s/\n", print_buffer->file_name);
        } else if (S_IEXEC & print_buffer->st_mode) {
            (void)printf("%s*\n", print_buffer->file_name);
        } else if (S_ISLNK(print_buffer->st_mode)) {
            (void)printf("%s@ -> %s\n", print_buffer->file_name, print_buffer->file_path);
        } else if (S_IFCHR & print_buffer->st_mode) {
            (void)printf("%s%c\n", print_buffer->file_name, '%');
        } else if (S_ISSOCK(print_buffer->st_mode)) {
            (void)printf("%s=\n", print_buffer->file_name);
        } else if (S_ISFIFO(print_buffer->st_mode)) {
            (void)printf("%s|\n", print_buffer->file_name);
        } else {
            (void)printf("%s\n", print_buffer->file_name);
        }
    } else {
        if (S_ISLNK(print_buffer->st_mode)) {
            (void)printf("%s -> %s\n", print_buffer->file_name, print_buffer->file_path);
        } else {
            (void)printf("%s\n", print_buffer->file_name);
        }
    }
}
