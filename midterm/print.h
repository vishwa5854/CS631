#include<fts.h>
#include<stdbool.h>
#include<sys/param.h>

#define MAX_BYTES_SIZE 10
#define STRMODE_LENGTH 11
#define MAX_MONTH_LENGTH 4

/** I couldn't find the system limits in time, so declaring constants instead. */
#define MAX_FILE_NAME_LENGTH 255
#define MAX_USER_NAME_LENGTH 255
#define MAX_GROUP_NAME_LENGTH 255
#define MAX_MONTH_NAME_LENGTH 3

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

typedef struct PRINTER_FRAME {
    ino_t                  st_ino;
    char                   bytes_in_human_readable[MAX_BYTES_SIZE];
    double                 effective_number_of_blocks;
    blkcnt_t               st_blocks;
    char                   mode_string[STRMODE_LENGTH];
    nlink_t                st_nlink;
    uid_t                  st_uid;
    gid_t                  st_gid;
    /** A lesson learnt hard way that never use char* inside of struct :(*/
    char                   pw_name[MAX_USER_NAME_LENGTH];
    gid_t                  stat;
    char                   gr_name[MAX_GROUP_NAME_LENGTH];
    off_t                  st_size;
    char                   which_month[MAX_MONTH_NAME_LENGTH];
    int                    tm_mday;
    int                    tm_hour;
    int                    tm_min;
    char                   file_name[MAX_FILE_NAME_LENGTH];
    char                   file_path[MAXPATHLEN];
    mode_t                 st_mode;
    struct PRINTER_FRAME*  next;
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
} MP;

void print(struct FLAGS_STRUCT* flags, FTSENT* node, PF* print_buffer, MP* max_map);

void flush(PF* print_buffer, MP* max_map, struct FLAGS_STRUCT* flags);

MP* init_max_map(MP* max_map);