#ifndef LS_H_
#define LS_H_

#include<sys/types.h>
#include<sys/stat.h>
#include<fts.h>

void ls(char* const* file_names, int FTS_OPTIONS, int (*sorter)(const FTSENT **, const FTSENT **));

#endif