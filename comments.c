#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
  pid_t pid;
  pid_t child_id;

/*Function to remove comments from a line*/
void removeComments(char *input) {
    /*Find the position of the first '#' character*/
    char *commentPos = strchr(input, '#');
    if (commentPos != NULL) {
        /*Replace '#' with a null terminator to truncate the line at the comment*/
        *commentPos = '\0';
    }
}

/*Function to replace variable placeholders in a string*/
void replaceVariables(char *input, int status) {
    char *search = "$?";
    char replace[10]; /*Placeholder for the replacement value*/

    /*Check if the variable $? is present*/
    char *varPosition = strstr(input, search);
    if (varPosition != NULL) {
        /* $? is present, replace it with the exit status*/
        snprintf(replace, sizeof(replace), "%d", status);
        /*Remove the $? variable*/
        memmove(varPosition + strlen(replace), varPosition + strlen(search), strlen(varPosition) - strlen(search) + 1);
        /*Copy the replacement value*/
        strncpy(varPosition, replace, strlen(replace));
    }

    /*Handle $$ variable replacement*/
    search = "$$";
    pid = getpid();
    snprintf(replace, sizeof(replace), "%d", pid);

    varPosition = strstr(input, search);
    while (varPosition != NULL) {
        /*$$ is present, replace it with the process ID*/
        /*Remove the $$ variable*/
        memmove(varPosition + strlen(replace), varPosition + strlen(search), strlen(varPosition) - strlen(search) + 1);
        /*Copy the replacement value*/
        strncpy(varPosition, replace, strlen(replace));
        /*Find the next occurrence*/
        varPosition = strstr(varPosition + strlen(replace), search);
    }
}

int executeCommand(const char *command, int *lastExitStatus) {
    char *command_copy = strdup(command);
    removeComments(command_copy); /*Remove comments from the command*/
    replaceVariables(command_copy, *lastExitStatus); /*Replace variables in the command*/

    child_id = fork();
    if (child_id == -1) {
        perror("fork");
        _exit(1);
    } else if (child_id == 0) {
        /*Child process*/
        execl("/bin/sh", "/bin/sh", "-c", command_copy, (char *)0);
        perror("execl");
        _exit(1);
    } else {
        /*Parent process*/
        int status;
        waitpid(child_id, &status, 0);
        *lastExitStatus = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }

    free(command_copy);
return -1; 
}


int main(void) {
    const char *prompt = "$ ";
    char input[MAX_INPUT_SIZE];
    int lastExitStatus = 0;

    while (1) {
        write(STDOUT_FILENO, prompt, strlen(prompt));

        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("fgets");
            break;
        }

        /*Remove the newline character at the end of the input*/
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            break;
        }

        executeCommand(input, &lastExitStatus);
    }

    return 0;
}

