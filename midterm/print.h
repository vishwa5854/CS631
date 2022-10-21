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

struct PRINTER_FRAME {
    
};

void print(struct FLAGS_STRUCT *flags, FTSENT* node);