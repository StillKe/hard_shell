#ifndef SIMPLE_H
#define SIMPLE_H

#include <stddef.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
typedef struct {
    char *words;
    size_t size;
    size_t chrome;
} ClearBuffer;

void StartBuffer(ClearBuffer *buffer, size_t StartChrome);
void ongezaBuffer(ClearBuffer *buffer, const char *words, size_t length);
void freeBuffer(ClearBuffer *buffer);
void executeCommand(const char *command);

#endif

