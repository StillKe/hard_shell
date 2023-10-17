#include "simp.h"

/* Function to remove comments from a line */
void toaComments(char *input)
{
    /* Find the position of the first '#' character */
    char *commentPosition = strchr(input, '#');
    if (commentPosition != NULL)
    {
        /* Replace '#' with a null terminator to truncate the line at the comment */
        *commentPosition = '\0';
    }
}

/* Function to replace variable placeholders in a string */
void badilishaVariables(char *input, int status)
{
    pid_t pid;
    int temp, m, len, remaining;

    char *search = "$?";
    char badilisha[10]; /* Placeholder for the replacement value */

    /* Check if the variable $? is present */
    char *varPosition = strstr(input, search);
    if (varPosition != NULL)
    {
        /* $? is present, replace it with the exit status */
        len = 0;
        if (status < 0)
        {
            badilisha[len++] = '-';
            status = -status;
        }
        temp = status;
        do
        {
            badilisha[len++] = temp % 10 + '0';
            temp /= 10;
        } while (temp > 0);
        for (m = 0; m < len / 2; m++)
        {
            char temp = badilisha[m];
            badilisha[m] = badilisha[len - m - 1];
            badilisha[len - m - 1] = temp;
        }
        write(STDOUT_FILENO, badilisha, len); /* Write the replacement value */
        /* Calculate the remaining length to remove the $? variable */
        remaining = strlen(input) - (varPosition - input) - strlen(search);
        /* Move the rest of the string over the replaced part */
        memmove(varPosition, varPosition + strlen(search), remaining + 1);
    }

    /* Handle $$ variable replacement */
    search = "$$";
    pid = getpid();
    /* Calculate the length of the replacement value */
    len = 0;
    temp = pid;
    do
    {
        badilisha[len++] = temp % 10 + '0';
        temp /= 10;
    } while (temp > 0);
    for (m = 0; m < len / 2; m++)
    {
        char temp = badilisha[m];
        badilisha[m] = badilisha[len - m - 1];
        badilisha[len - m - 1] = temp;
    }

    varPosition = strstr(input, search);
    while (varPosition != NULL)
    {
        /* $$ is present, replace it with the process ID */
        write(STDOUT_FILENO, badilisha, len); /* Write the replacement value */
        /* Calculate the remaining length to remove the $$ variable */
        remaining = strlen(input) - (varPosition - input) - strlen(search);
        /* Move the rest of the string over the replaced part */
        memmove(varPosition, varPosition + strlen(search), remaining + 1);
        /* Find the next occurrence */
        varPosition = strstr(varPosition + len, search);
    }
}

int startCommand(const char *command, int *lastExitStatus)
{
    pid_t child_id;
    char *command_dublicate = strdup(command);
    toaComments(command_dublicate); /* Remove comments from the command */
    badilishaVariables(command_dublicate, *lastExitStatus); /* Replace variables in the command */

    child_id = fork();
    if (child_id == -1)
    {
        perror("fork");
        _exit(1);
    }
    else if (child_id == 0)
    {
        /* Child process */
        execl("/bin/sh", "/bin/sh", "-c", command_dublicate, (char *)0);
        perror("execl");
        _exit(1);
    }
    else
    {
        /* Parent process */
        int status;
        waitpid(child_id, &status, 0);
        *lastExitStatus = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }

    free(command_dublicate);
    return (-1);
}

int main(void)
{
    const char *prompt = "$ ";
    char input[MAX_INPUT_SIZE];
    int lastExitStatus = 0;

    while (1)
    {
        write(STDOUT_FILENO, prompt, strlen(prompt));

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            perror("fgets");
            break;
        }

        /* Remove the newline character at the end of the input */
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0)
        {
            break;
        }

        startCommand(input, &lastExitStatus);
    }
    return (0);
}

