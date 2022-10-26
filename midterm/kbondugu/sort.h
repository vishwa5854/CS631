/** @fileoverview Header for sorting */

#ifndef SORT_H_
#define SORT_H_

#include<fts.h>
#include<stdbool.h>
#include"structures.h"

int sort(const FTSENT** one, const FTSENT** two, FLAGS* sort_flags);

#endif
