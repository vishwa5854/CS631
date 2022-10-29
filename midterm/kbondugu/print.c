#include<ctype.h>
#include<errno.h>
#include<grp.h>
#include<math.h>
#include"print.h"
#include<pwd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<time.h>
#include"util.h"
#include<unistd.h>

#define DEFAULT_BLOCK_SIZE 512
#define KILO_BLOCK_SIZE 1024

extern char **environ;

void print(FLAGS* flags, FTSENT* node, PF* print_buffer, MP* max_map, bool is_dir) {
    int header_length = 0;
    long block_size = 0;
    (void)getbsize(&header_length, &block_size);
    print_buffer->fts_info = node->fts_info;
    print_buffer->is_dir = is_dir;

    if (flags->i) {
        print_buffer->st_ino = node->fts_statp->st_ino;
        max_map->st_ino = max_of_two(max_map->st_ino, node->fts_statp->st_ino);
    }

    if ((flags->l || flags->s || flags->n) && (is_dir)) {
        if (flags->k) {
            max_map->total_blocks += effective_number_of_blocks(DEFAULT_BLOCK_SIZE, KILO_BLOCK_SIZE, node->fts_statp->st_blocks);
        } else if (flags->h) {
            max_map->total_blocks += node->fts_statp->st_size;
        } else {
            if (block_size != DEFAULT_BLOCK_SIZE) {
                max_map->total_blocks += effective_number_of_blocks(DEFAULT_BLOCK_SIZE, block_size, node->fts_statp->st_blocks);
            } else {
                max_map->total_blocks += node->fts_statp->st_blocks;
            }
        }
    }

    if (flags->s) {
        int header_length;
        long block_size;
        (void)getbsize(&header_length, &block_size);

        if (flags->k) {
            block_size = 1024;
        }

        if (flags->h) {
            convert_bytes_to_human_readable(node->fts_statp->st_size, print_buffer->bytes_in_human_readable);
            max_map->bytes_in_human_readable = just_max(max_map->bytes_in_human_readable, strlen(print_buffer->bytes_in_human_readable));
        } else {
            if (block_size != DEFAULT_BLOCK_SIZE) {
                print_buffer->effective_number_of_blocks = effective_number_of_blocks(DEFAULT_BLOCK_SIZE, block_size, node->fts_statp->st_blocks);
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
        time_t* which_time = &node->fts_statp->st_mtim.tv_sec;

        if (flags->c) {
            which_time = &node->fts_statp->st_ctim.tv_sec;
        } else if (flags->u) {
            which_time = &node->fts_statp->st_atim.tv_sec;
        }

        if ((tm = localtime(which_time)) == NULL) {
            (void)fprintf(stderr, "ls: Error while fetching the local time");
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
    char file_name[node->fts_namelen];
    strncpy(file_name, node->fts_name, node->fts_namelen);

    if (flags->q) {
        char temp[node->fts_namelen + 1];
        char replacement = '?';
        size_t j = 0;

        for (; j < node->fts_namelen; j++) {
            if (isprint(file_name[j]) != 0) {
                temp[j] = file_name[j];
            } else {
                temp[j] = replacement;
            }
        }
        temp[node->fts_namelen + 1] = '\0';
        strncpy(file_name, temp, node->fts_namelen);
    }
    
    strncpy(print_buffer->file_name, file_name, node->fts_namelen);
    print_buffer->file_name[strlen(file_name)] = '\0';
    max_map->file_name = just_max(max_map->file_name, strlen(file_name));

    strncpy(print_buffer->file_path, node->fts_path, strlen(node->fts_path));
    print_buffer->file_path[strlen(node->fts_path)] = '\0';

    if (flags->F || flags->l) {
        print_buffer->st_mode = node->fts_statp->st_mode;        
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
    max_map->total_blocks = 0;

    return max_map;
}

void print_empty_spaces(long int how_many) {
    long int i = 0;

    for (; i < how_many; i++) {
        (void)printf(" ");
    }
}

void flush(PF* print_buffer, MP* max_map, FLAGS* flags) {
    if (
        (max_map->total_blocks != -1) &&
        (flags->l || flags->s || flags->n) &&
        (print_buffer->is_dir)
    ) {
        if (flags->h) {
            char hr_blocks_buffer[MAX_BYTES_SIZE];
            convert_bytes_to_human_readable(max_map->total_blocks, hr_blocks_buffer);
            (void)printf("total %s \n", hr_blocks_buffer);
        } else {
            (void)printf("total %ld\n", max_map->total_blocks);
        }
        max_map->total_blocks = -1;
    }

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
                (void)printf("%ld ", (long int)print_buffer->effective_number_of_blocks);
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

        // print_empty_spaces(max_map->tm_mday - get_number_of_digits(print_buffer->tm_mday));
        (void)printf("%02d ", print_buffer->tm_mday);

        (void)printf("%02d:%02d ", print_buffer->tm_hour, print_buffer->tm_min);
    }

    if (strlen(print_buffer->file_name) == 0) {
        (void)printf("\n");
        return;
    }

    if (flags->d) {
        if (max_map->file_name > -1) {
            (void)printf("%s", print_buffer->file_name);
        }
        (void)printf("\n");
        return;
    }

    if (flags->F) {
        if (S_ISDIR(print_buffer->st_mode)) {
            (void)printf("%s/", print_buffer->file_name);
        } else if (S_ISLNK(print_buffer->st_mode)) {
            (void)printf("%s@", print_buffer->file_name);
        } else if (print_buffer->fts_info == FTS_W) {
            (void)printf("%s%c", print_buffer->file_name, '%');
        } else if (S_ISSOCK(print_buffer->st_mode)) {
            (void)printf("%s=", print_buffer->file_name);
        } else if (S_ISFIFO(print_buffer->st_mode)) {
            (void)printf("%s|", print_buffer->file_name);
        } else if (
            (S_IXUSR & print_buffer->st_mode) || 
            (S_IXGRP & print_buffer->st_mode) || 
            (S_IXOTH & print_buffer->st_mode)
        ) {
            (void)printf("%s*", print_buffer->file_name);
        } else {
            (void)printf("%s", print_buffer->file_name);
        }
    } else {
        if (S_ISLNK(print_buffer->st_mode)) {
            char buffer[PATH_MAX];
            ssize_t len;

            if ((len = readlink(print_buffer->file_path, buffer, sizeof(buffer)-1)) == -1) {
                len = 0;
            }
            buffer[len] = '\0';
            (void)printf("%s -> %s", print_buffer->file_name, buffer);
        } else {
            (void)printf("%s", print_buffer->file_name);
        }
    }

    (void)printf("\n");
}
