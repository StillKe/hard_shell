#ifndef SIMP_H
#define SIMP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

#define MAX_COMMAND_LENGTH 256
#define MAX_ALIASES 50
#define MAX_ALIAS_NAME 50

struct Alias
{
    char name[MAX_ALIAS_NAME];
    char command[MAX_COMMAND_LENGTH];
};

void add_alias(struct Alias *aliases, int *alias_count, const char *name, const char *command);
int sub_alias(char *input, struct Alias *aliases, int alias_count);
void start_child(char *input);
void wait_pid();

#endif
