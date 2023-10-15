#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
void executeCommand(char **args);

int main()
{
char var_name[4096];
char **args = (char **)malloc(128 * sizeof(char *));
char *token;
int argCount;

if (args == NULL)
{
perror("malloc");
return (1);
}

while (1) 
{
write(STDOUT_FILENO, "#cisfun$ ", 9); 
if (fgets(var_name, sizeof(var_name), stdin) == NULL)
{
perror("fgets");
break;
}

if (var_name[0] == '\n')
{
continue;
}

if (var_name[strlen(var_name) - 1] == '\n')
{
var_name[strlen(var_name) - 1] = '\0';
}

token = var_name;
argCount = 0;

while (*token != '\0')
{
while (*token == ' ') {
token++;  /* Skip leading spaces */
}
args[argCount] = token;

while (*token != ' ' && *token != '\0') {
token++;  /* Move to the next space or null terminator */
}

if (*token == ' ')
{
*token = '\0';  /* Null-terminate the argument */
}

argCount++;
}

args[argCount] = NULL;

if (strcmp(args[0], "cd") == 0) {
if (args[1] == NULL) {
chdir(getenv("HOME"));
}
else if (chdir(args[1]) == -1)
{
write(STDOUT_FILENO, args[1], strlen(args[1]));
write(STDOUT_FILENO, ": No such file or directory\n", 28);
}
}
else if (strcmp(args[0], "exit") == 0)
{
free(args);
break;
}
else
{
executeCommand(args);
}
}

free(args);
    return (0);
}

void executeCommand(char **args)
{
pid_t child_id = fork();
if (child_id == 0)
{
        /* Child process */
if (execvp(args[0], args) == -1)
{
perror("execvp");
_exit(1);
}
} else if (child_id < 0)
{
perror("fork");
} else {
/* Parent process */
int status;
waitpid(child_id, &status, 0);
}
}

