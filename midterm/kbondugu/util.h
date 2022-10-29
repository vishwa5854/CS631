#ifndef UTIL_H_
#define UTIL_H_

#include"structures.h"

char* which_month(int month);

long int get_number_of_digits(long int number);

long int max_of_two(long int a, long int b);

long int just_max(long int a, long int b);

void swap(char** x, char** y);

void report_errors(int N, char** argv);

void convert_bytes_to_human_readable(double size, char* buff);

int move_args_and_non_existent_files_to_top(int N, char ** paths);

void create_paths(int N, char ** paths, int start, char** required_paths);

int init_flags_from_args(FLAGS* flags, char* args);

double effective_number_of_blocks(int set_block_size, int env_block_size, blkcnt_t st_blocks);

#endif
