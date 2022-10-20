#include"util.h"
#include<fts.h>
#include<grp.h>
#include<pwd.h>
#include<stdio.h>
#include<bsd/string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<time.h>
#include<stdbool.h>
#include"flags.h"

void print(FLAGS *flags2, FTSENT* node);