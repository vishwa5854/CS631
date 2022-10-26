/** @fileoverview Header for sorting */

#ifndef SORT_H_
#define SORT_H_

#include<stdbool.h>
#include"structures.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<fts.h>

int sort(const FTSENT** one, const FTSENT** two, FLAGS* sort_flags);

int sort_by_name(const void* a, const void* b);

#endif
