#include<fts.h>
#include<stdbool.h>

struct FLAGS_STRUCT {
    bool A;
    bool a;
    bool c;
    bool i;
    bool l;
    bool d;
    bool R;
    bool show_hidden_files;
};

struct PRINTER_FRAME {
    
};

void print(struct FLAGS_STRUCT *flags, FTSENT* node);