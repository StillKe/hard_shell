#include "shell.h"
/**
 * main-function is the entry point of the program.
 * StartBuffer - Initializes a ClearBuffer struct.
 * custom_getline - Reads a line from a stream.
 * ongezaBuffer - Appends data to the ClearBuffer struct.
 * @buffer: Pointer to the ClearBuffer struct.
 * @StartChrome: Initial size for the buffer.
 * executeCommand - Executes a command in a child process.
 * Return: Number of characters read, or -1 on error or end of file.
 * main-function is the entry point of the program.
 */
void StartBuffer(ClearBuffer *buffer, size_t StartChrome)
{
buffer->words = (char *)malloc(StartChrome * sizeof(char));
if (buffer->words == NULL)
{
perror("malloc");
_exit(1);
}
buffer->size = 0;
buffer->chrome = StartChrome;
}

void ongezaBuffer(ClearBuffer *buffer, const char *words, size_t length)
{
if (buffer->size + length > buffer->chrome)
{
size_t newChrome = buffer->chrome * 2;
buffer->words = (char *)realloc(buffer->words, newChrome * sizeof(char));
if (buffer->words == NULL)
{
perror("realloc");
_exit(1);
}
buffer->chrome = newChrome;
buffer->chrome = newChrome;
}

memcpy(buffer->words + buffer->size, words, length);
buffer->size += length;
}
void freeBuffer(ClearBuffer *buffer)
{
free(buffer->words);
buffer->words = NULL;
buffer->size = 0;
buffer->chrome = 0;
}

int custom_getline(char *input, size_t size, FILE *stream)
{
int c;
size_t i = 0;
while (i < size - 1)
{
c = fgetc(stream);
if (c == EOF || c == '\n')
{
break;
}
input[i] = (char)c;
i++;
}
input[i] = '\0';
if (i == 0 && c == EOF)
return (-1);

return (i);
}

void executeCommand(const char *command)
{
pid_t child_id = fork();
if (child_id == -1)
{
perror("fork");
_exit(1);
}
else if (child_id == 0)
{
char **args = (char **)malloc(2 * sizeof(char *));
args[0] = (char *)command;
args[1] = NULL;
execve(command, args, NULL);
perror("execve");
_exit(1);
}
else
{
int status;
waitpid(child_id, &status, 0);
}
}
int main(void)
{
const char *prompt = "cisfun$\n";
char input[128];
int chars_read;

ClearBuffer inputBuffer;
StartBuffer(&inputBuffer, 128);

while (1)
{
write(STDOUT_FILENO, prompt, strlen(prompt));

chars_read = custom_getline(input, sizeof(input), stdin);

if (chars_read == -1)
{
perror("custom_getline");
break;
}

ongezaBuffer(&inputBuffer, input, chars_read);

executeCommand(inputBuffer.words);

freeBuffer(&inputBuffer);
StartBuffer(&inputBuffer, 128);
}

freeBuffer(&inputBuffer);
return (0);
}
