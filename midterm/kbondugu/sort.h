#include<fts.h>
#include<stdbool.h>

struct SORT_FLAGS {
    bool f;
    bool S;
    bool t;
    bool c;
    bool u;
    bool r;
    bool l;
};

int sort(const FTSENT** one, const FTSENT** two, struct SORT_FLAGS* sort_flags);