#include<fts.h>
#include<stdbool.h>

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

// struct PRINTER_FRAME {
//     long int inode_max;
//     long int strmode_max;
//     long int st_uid_max;
//     long int st_gid_max;
//     long int st_size_max;
//     long int which_month_max;
//     long int tm_mday_max;
//     long int tm_hour_max;
//     long int tm_min_max;
//     long int fts_name_max;
// };

void print(struct FLAGS_STRUCT *flags, FTSENT* node);