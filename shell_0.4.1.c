#include "shell.h"

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
        return -1;

    return i;
}

void executeCommand(const char *command)
{
    char *command_copy = strdup(command);
    char **args = (char **)malloc(128 * sizeof(char *));
    int argCount = 0;
    int tokenLength = 0;
    int i,j;
    pid_t child_id;

    if (command_copy == NULL || args == NULL)
    {
        perror("Memory allocation error");
        free(command_copy);
        free(args);
        _exit(1);
    }

    for (i = 0; command_copy[i] != '\0'; i++) {
        if (command_copy[i] == ' ' || command_copy[i] == '\t' || command_copy[i] == '\n')
        {
            if (tokenLength > 0) {
                args[argCount] = (char *)malloc((tokenLength + 1) * sizeof(char));
                if (args[argCount] == NULL) {
                    perror("Memory allocation error");
                    free(command_copy);
                    for (j = 0; j < argCount; j++)
                    {
                        free(args[j]);
                    }
                    free(args);
                    _exit(1);
                }
                strncpy(args[argCount], command_copy + i - tokenLength, tokenLength);
                args[argCount][tokenLength] = '\0';
                argCount++;
                tokenLength = 0;
            }
        }
        else
        {
            tokenLength++;
        }
    }

    if (tokenLength > 0) {
        args[argCount] = (char *)malloc((tokenLength + 1) * sizeof(char));
        if (args[argCount] == NULL) {
            perror("Memory allocation error");
            free(command_copy);
            for (j = 0; j < argCount; j++)
            {
                free(args[j]);
            }
            free(args);
            _exit(1);
        }
        strncpy(args[argCount], command_copy + strlen(command_copy) - tokenLength, tokenLength);
        args[argCount][tokenLength] = '\0';
        argCount++;
    }

    args[argCount] = NULL;

    if (strcmp(args[0], "exit") == 0)
    {
        int exit_status = 0;
        if (argCount > 1) {
            exit_status = atoi(args[1]);
        }
        free(command_copy);
        for (i = 0; i < argCount; i++)
        {
            free(args[i]);
        }
        free(args);
        exit(exit_status);
    }

    child_id = fork();
    if (child_id == -1)
    {
        perror("fork");
        _exit(1);
    } else if (child_id == 0)
    {
        execve(args[0], args, NULL);
        perror("execve");
        _exit(1);
    } else {
        int status;
        waitpid(child_id, &status, 0);
    }

    free(command_copy);
    for (i = 0; i < argCount; i++)
    {
        free(args[i]);
    }
    free(args);
}

int main(void)
{
    const char *prompt = "cisfun$\n";
    char input[128];
    int chars_read;

    ClearBuffer inputBuffer;
    StartBuffer(&inputBuffer, 128);

    while (1) {
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
    return 0;
}

