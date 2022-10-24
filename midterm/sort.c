#include<sys/stat.h>

#include"sort.h"
#include<string.h>

int sort(const FTSENT** one, const FTSENT** two, struct SORT_FLAGS* sort_flags) {
    /** f is the KING, it will override all the sort options irrespective of the order. */
    if (sort_flags->f) {
        return 0;
    }

    if (sort_flags->t || (sort_flags->l && !sort_flags->S)) {
        if (sort_flags->c) {
            /** Lexicographical sort in case of equal values of ctime */
            if ((*one)->fts_statp->st_ctime == (*two)->fts_statp->st_ctime) {
                return sort_flags->r ? 
                            strcmp((*two)->fts_name, (*one)->fts_name) :
                            strcmp((*one)->fts_name, (*two)->fts_name);
            }
            return sort_flags->r ? 
                        (((*two)->fts_statp->st_ctime < (*one)->fts_statp->st_ctime) ? 1 : -1) : 
                        (((*one)->fts_statp->st_ctime < (*two)->fts_statp->st_ctime) ? 1 : -1);
        }

        if (sort_flags->u) {
            if ((*one)->fts_statp->st_atime == (*two)->fts_statp->st_atime) {
                return sort_flags->r ? 
                            strcmp((*two)->fts_name, (*one)->fts_name) :
                            strcmp((*one)->fts_name, (*two)->fts_name);
            }
            return sort_flags->r ? 
                        (((*two)->fts_statp->st_atime < (*one)->fts_statp->st_atime) ? 1 : -1) :
                        (((*one)->fts_statp->st_atime < (*two)->fts_statp->st_atime) ? 1 : -1);
        }

        if (sort_flags->t) {
            if ((*one)->fts_statp->st_mtime == (*two)->fts_statp->st_mtime) {
                return sort_flags->r ? 
                            strcmp((*two)->fts_name, (*one)->fts_name) :
                            strcmp((*one)->fts_name, (*two)->fts_name);
            }
            return sort_flags->r ? 
                        (((*two)->fts_statp->st_mtime < (*one)->fts_statp->st_mtime) ? 1 : -1) : 
                        (((*one)->fts_statp->st_mtime < (*two)->fts_statp->st_mtime) ? 1 : -1);
        }
    }

    if (sort_flags->S) {
        if ((*one)->fts_statp->st_size == (*two)->fts_statp->st_size) {
            return sort_flags->r ? 
                        strcmp((*two)->fts_name, (*one)->fts_name) :
                        strcmp((*one)->fts_name, (*two)->fts_name);
        }
        return sort_flags->r ? 
                    (((*two)->fts_statp->st_size < (*one)->fts_statp->st_size) ? 1 : -1) : 
                    (((*one)->fts_statp->st_size < (*two)->fts_statp->st_size) ? 1 : -1);
    }
    return sort_flags->r ? strcmp((*two)->fts_name, (*one)->fts_name)
                   : strcmp((*one)->fts_name, (*two)->fts_name);
}
