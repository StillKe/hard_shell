#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
char buffer[1024];
pid_t child_id;
ssize_t bytesRead;

void executeCommand(const char *command) {
    char *command_copy = strdup(command);
    char **args = (char **)malloc(128 * sizeof(char *));
    int argCount = 0;
    int tokenLength = 0;
    int i, j;
    pid_t child_id;

    if (command_copy == NULL || args == NULL) {
        perror("Memory allocation error");
        free(command_copy);
        free(args);
        _exit(1);
    }

    for (i = 0; command_copy[i] != '\0'; i++) {
        if (command_copy[i] == ' ' || command_copy[i] == '\t' || command_copy[i] == '\n') {
            if (tokenLength > 0) {
                args[argCount] = (char *)malloc((tokenLength + 1) * sizeof(char));
                if (args[argCount] == NULL) {
                    perror("Memory allocation error");
                    free(command_copy);
                    for (j = 0; j < argCount; j++) {
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
        } else {
            tokenLength++;
        }
    }

    if (tokenLength > 0) {
        args[argCount] = (char *)malloc((tokenLength + 1) * sizeof(char));
        if (args[argCount] == NULL) {
            perror("Memory allocation error");
            free(command_copy);
            for (j = 0; j < argCount; j++) {
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

    child_id = fork();
    if (child_id == -1) {
        perror("fork");
        _exit(1);
    } else if (child_id == 0) {
        execvp(args[0], args);
        perror("execvp");
        _exit(1);
    } else {
        int status;
        waitpid(child_id, &status, 0);
    }

    free(command_copy);
    for (i = 0; i < argCount; i++) {
        free(args[i]);
    }
    free(args);
}

void handleSingleCommand(const char *command) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    child_id = fork();
    if (child_id == -1) {
        perror("fork");
        return;
    }

    if (child_id == 0) {
        
        close(pipefd[0]); 
        dup2(pipefd[1], STDOUT_FILENO); 
        close(pipefd[1]); 

        executeCommand(command); 

        _exit(1); 
    } else {
       
        close(pipefd[1]); 
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            write(STDOUT_FILENO, buffer, bytesRead);
        }
        close(pipefd[0]); 
    }
}

void handleCommandsSeparatedBySemicolon(const char *input) {
    const char *delimiter = ";";
    char *inputCopy = strdup(input);
    char *token = strtok(inputCopy, delimiter);

    while (token != NULL) {
        handleSingleCommand(token);
        token = strtok(NULL, delimiter);
    }

    free(inputCopy);
}

int main(void) {
    const char *prompt = "SimpleShell$ ";
    char input[MAX_INPUT_SIZE];
    

    while (1) {
        write(STDOUT_FILENO, prompt, strlen(prompt));

        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("fgets");
            break;
        }

        
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            break;
        }

        handleCommandsSeparatedBySemicolon(input);
    }

    return 0;
}

