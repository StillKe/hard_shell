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
#define MAX_ALIAS_COMMAND 256

pid_t pid;
int i;
char *args[2];

/* Structure to store alias mappings*/
struct Alias {
    char name[MAX_ALIAS_NAME];
    char command[MAX_ALIAS_COMMAND];
};

/*Array to store aliases*/
struct Alias aliases[MAX_ALIASES];
int alias_count = 0;

/**
 * Function to add an alias.
 */
void add_alias(const char *name, const char *command) {
    if (alias_count < MAX_ALIASES) {
        strcpy(aliases[alias_count].name, name);
        strcpy(aliases[alias_count].command, command);
        alias_count++;
        printf("Alias '%s' added: %s\n", name, command);
    } else {
        printf("Max alias limit reached.\n");
    }
}

/**
 * Function to execute a command or alias.
 */
void execute_command(char *input) {
    /*Check if it's an alias*/
    for (i = 0; i < alias_count; i++) {
        if (strcmp(input, aliases[i].name) == 0) {
            input = aliases[i].command;
            break;
        }
    }

    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        /**
         * This code is executed by the child process
         */

        /*Allocate memory dynamically for the args array*/
        
        args[0] = (char*)input;
        args[1] = NULL;
        
        execve(input, args, environ);

        /**
         * If exec fails, print the error message with the command
         */
        fprintf(stderr, "MyShell: %s: No such file or directory\n", input);
        exit(EXIT_FAILURE);
    } else {
        /**
         * This code is executed by the parent process
         * Wait for the child process to complete
         */
        waitpid(pid, NULL, 0);
    }
}


int main() {
    char fil[MAX_COMMAND_LENGTH];

    while (1) {
        /**
         * Display the prompt
         */
        printf("MyShell> ");
        fflush(stdout);

        /**
         * Read the user's command
         */
        if (fgets(fil, sizeof(fil), stdin) == NULL) {
            printf("\n");
            break;
        }

        /**
         * Remove the newline character from the command
         */
        fil[strcspn(fil, "\n")] = '\0';

        /**
         * Check if the input is an alias command
         */
        if (strncmp(fil, "alias", 5) == 0) {
            char name[MAX_ALIAS_NAME];
            char command[MAX_ALIAS_COMMAND];

            /*Parse the alias command*/
            if (sscanf(fil, "alias %s %[^\n]", name, command) == 2) {
                add_alias(name, command);
            } else {
                printf("Invalid alias syntax. Usage: alias <name> <command>\n");
            }
        } else {
            /**
             * Execute the command or alias
             */
            execute_command(fil);
        }
    }

    return 0;
}

