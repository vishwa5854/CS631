//
// Created by z on 12/17/22.
//

#ifndef SISH_DATA_STRUCTURES_H
#define SISH_DATA_STRUCTURES_H

#include <stdbool.h>
// TODO: Uncomment this on NetBSD
//#include<sys/syslimits.h>

#ifndef LOGIN_NAME_MAX
#define LOGIN_NAME_MAX 256
#endif

#define MAX_COMMAND_SIZE 4096

#define PROGRAM_NAME "SISH"

int exit_status_of_last_command = 0;

int current_process_id = 0;

char current_user[LOGIN_NAME_MAX];

typedef struct FLAGS_STRUCT {
    bool c;
    char command[MAX_COMMAND_SIZE];
    bool x;
} FLAGS;

#endif //SISH_DATA_STRUCTURES_H
