/** @fileoverview Header of all the structures and other data structures used in this project. */

#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include<stdbool.h>
#include<sys/stat.h>
// #include<sys/syslimits.h>
#include<stdlib.h>

#define STRMODE_LENGTH 11
#define MAX_MONTH_NAME_LENGTH 3
#define MAX_BYTES_SIZE 10

#ifndef LOGIN_NAME_MAX
#define LOGIN_NAME_MAX 256
#endif

/** Remove this before submission TODO*/
#ifndef NAME_MAX
#define NAME_MAX 256
#endif

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

typedef struct FLAGS_STRUCT {
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
} FLAGS;

typedef struct PRINTER_FRAME {
    ino_t                  st_ino;
    char                   bytes_in_human_readable[MAX_BYTES_SIZE];
    double                 effective_number_of_blocks;
    blkcnt_t               st_blocks;
    char                   mode_string[STRMODE_LENGTH];
    nlink_t                st_nlink;
    uid_t                  st_uid;
    gid_t                  st_gid;
    char                   pw_name[LOGIN_NAME_MAX];
    gid_t                  stat;
    char                   gr_name[LOGIN_NAME_MAX];
    off_t                  st_size;
    char                   which_month[MAX_MONTH_NAME_LENGTH];
    int                    tm_mday;
    int                    tm_hour;
    int                    tm_min;
    char                   file_name[NAME_MAX];
    char                   file_path[PATH_MAX];
    mode_t                 st_mode;
    struct PRINTER_FRAME*  next;
    int                    fts_info;
    bool                   is_dir;
    int                    major_dev_num;
    int                    minor_dev_num;
} PF;

typedef struct MAX_MAP {
    long int st_ino;
    long int bytes_in_human_readable;
    long int effective_number_of_blocks;
    long int st_blocks;
    long int mode_string;
    long int st_nlink;
    long int st_uid;
    long int st_gid;
    long int pw_name;
    long int stat;
    long int gr_name;
    long int st_size;
    long int which_month;
    long int tm_mday;
    long int tm_hour;
    long int tm_min;
    long int file_name;
    long int total_blocks;
    long int major_dev_num;
    long int minor_dev_num;
} MP;

typedef struct FILE_TYPES {
    char** files;
    char** directories;
    int    n_files;
    int    n_directories;
    int    n_errors;
} FT;

#endif