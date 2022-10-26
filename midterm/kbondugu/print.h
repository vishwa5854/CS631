#include<fts.h>
#include<stdbool.h>
#include<sys/param.h>
#include"structures.h"

/** I couldn't find and compiler couldn't compile*/
// #include<syslimits.h>

#define MAX_BYTES_SIZE 10
#define STRMODE_LENGTH 11
#define MAX_MONTH_LENGTH 4

/** I couldn't find the system limits in time, so declaring constants instead. */
#define MAX_FILE_NAME_LENGTH 255
#define MAX_MONTH_NAME_LENGTH 3

#ifndef LOGIN_NAME_MAX
#define LOGIN_NAME_MAX 256
#endif

void print(struct FLAGS_STRUCT* flags, FTSENT* node, PF* print_buffer, MP* max_map);

void flush(PF* print_buffer, MP* max_map, struct FLAGS_STRUCT* flags);

MP* init_max_map(MP* max_map);