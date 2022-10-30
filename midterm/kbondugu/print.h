#ifndef PRINT_H_
#define PRINT_H_

#include<stdbool.h>
#include<sys/param.h>
#include"structures.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<fts.h>

void print(FLAGS* flags, FTSENT* node, PF* print_buffer, MP* max_map, bool is_dir);

void flush(PF* print_buffer, MP* max_map, FLAGS* flags);

MP* init_max_map(MP* max_map);

#endif
