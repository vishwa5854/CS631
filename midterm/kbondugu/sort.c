#include<sys/stat.h>
#include"sort.h"
#include<string.h>

int sort(const FTSENT** one, const FTSENT** two, FLAGS* flags) {
    /** f is the KING, it will override all the sort options irrespective of the order. */
    if (flags->f) {
        return 0;
    }

    if (flags->t || (flags->l && !flags->S)) {
        if (flags->c) {
            /** Lexicographical sort in case of equal values of ctime */
            if ((*one)->fts_statp->st_ctime == (*two)->fts_statp->st_ctime) {
                return flags->r ? 
                            strcmp((*two)->fts_name, (*one)->fts_name) :
                            strcmp((*one)->fts_name, (*two)->fts_name);
            }
            return flags->r ? 
                        (((*two)->fts_statp->st_ctime < (*one)->fts_statp->st_ctime) ? 1 : -1) : 
                        (((*one)->fts_statp->st_ctime < (*two)->fts_statp->st_ctime) ? 1 : -1);
        }

        if (flags->u) {
            if ((*one)->fts_statp->st_atime == (*two)->fts_statp->st_atime) {
                return flags->r ? 
                            strcmp((*two)->fts_name, (*one)->fts_name) :
                            strcmp((*one)->fts_name, (*two)->fts_name);
            }
            return flags->r ? 
                        (((*two)->fts_statp->st_atime < (*one)->fts_statp->st_atime) ? 1 : -1) :
                        (((*one)->fts_statp->st_atime < (*two)->fts_statp->st_atime) ? 1 : -1);
        }

        if (flags->t) {
            if ((*one)->fts_statp->st_mtime == (*two)->fts_statp->st_mtime) {
                return flags->r ? 
                            strcmp((*two)->fts_name, (*one)->fts_name) :
                            strcmp((*one)->fts_name, (*two)->fts_name);
            }
            return flags->r ? 
                        (((*two)->fts_statp->st_mtime < (*one)->fts_statp->st_mtime) ? 1 : -1) : 
                        (((*one)->fts_statp->st_mtime < (*two)->fts_statp->st_mtime) ? 1 : -1);
        }
    }

    if (flags->S) {
        if ((*one)->fts_statp->st_size == (*two)->fts_statp->st_size) {
            return flags->r ? 
                        strcmp((*two)->fts_name, (*one)->fts_name) :
                        strcmp((*one)->fts_name, (*two)->fts_name);
        }
        return flags->r ? 
                    (((*two)->fts_statp->st_size < (*one)->fts_statp->st_size) ? 1 : -1) : 
                    (((*one)->fts_statp->st_size < (*two)->fts_statp->st_size) ? 1 : -1);
    }
    return flags->r ? strcmp((*two)->fts_name, (*one)->fts_name)
                   : strcmp((*one)->fts_name, (*two)->fts_name);
}
