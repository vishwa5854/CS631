#include<sys/stat.h>
#include"sort.h"
#include<string.h>
#include<stdio.h>

int sort(const FTSENT** one, const FTSENT** two, FLAGS* flags) {
    if (flags->t && flags->l) {
        if (flags->c) {
            /** Lexicographical sort in case of equal values of ctime */
            if ((*one)->fts_statp->st_ctimensec == (*two)->fts_statp->st_ctimensec) {
                return flags->r ? 
                            strcmp((*two)->fts_name, (*one)->fts_name) :
                            strcmp((*one)->fts_name, (*two)->fts_name);
            }
            return flags->r ? 
                        (((*two)->fts_statp->st_ctimensec < (*one)->fts_statp->st_ctimensec) ? 1 : -1) : 
                        (((*one)->fts_statp->st_ctimensec < (*two)->fts_statp->st_ctimensec) ? 1 : -1);
        }

        if (flags->u) {
            if ((*one)->fts_statp->st_atimensec == (*two)->fts_statp->st_atimensec) {
                return flags->r ? 
                            strcmp((*two)->fts_name, (*one)->fts_name) :
                            strcmp((*one)->fts_name, (*two)->fts_name);
            }
            return flags->r ? 
                        (((*two)->fts_statp->st_atimensec < (*one)->fts_statp->st_atimensec) ? 1 : -1) :
                        (((*one)->fts_statp->st_atimensec < (*two)->fts_statp->st_atimensec) ? 1 : -1);
        }

        if (flags->t) {
            if ((*one)->fts_statp->st_mtimensec == (*two)->fts_statp->st_mtimensec) {
                return flags->r ? 
                            strcmp((*two)->fts_name, (*one)->fts_name) :
                            strcmp((*one)->fts_name, (*two)->fts_name);
            }
            return flags->r ? 
                        (((*two)->fts_statp->st_mtimensec < (*one)->fts_statp->st_mtimensec) ? 1 : -1) : 
                        (((*one)->fts_statp->st_mtimensec < (*two)->fts_statp->st_mtimensec) ? 1 : -1);
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

/** 
 * Inspired by GEEKSFORGEEKS 
 * https://www.geeksforgeeks.org/c-program-sort-array-names-strings/ 
 */
int sort_by_name(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b);
}
