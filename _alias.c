#include "simp.h"
void start_alias(char *input, struct Alias *aliases, int *alias_count)
{
if (sub_alias(input, aliases, *alias_count))
{
start_child(input);
}
else
{
wait_pid();
}
}
void add_alias(struct Alias *aliases, int *alias_count, const char *name, const char *command)
{
const char *alias_added_msg = "Alias added: ";
const char *alias_limit_msg;
if (*alias_count < MAX_ALIASES)
{
strcpy(aliases[*alias_count].name, name);
strcpy(aliases[*alias_count].command, command);
(*alias_count)++;

write(STDOUT_FILENO, alias_added_msg, strlen(alias_added_msg));
write(STDOUT_FILENO, name, strlen(name));
write(STDOUT_FILENO, ": ", 2);
write(STDOUT_FILENO, command, strlen(command));
write(STDOUT_FILENO, "\n", 1);
}
else
{
alias_limit_msg = "Max alias limit reached.\n";
write(STDOUT_FILENO, alias_limit_msg, strlen(alias_limit_msg));
}
}


int sub_alias(char *input, struct Alias *aliases, int alias_count)
{
int k;
for (k = 0; k < alias_count; k++)
{
if (strcmp(input, aliases[k].name) == 0)
{
strcpy(input, aliases[k].command);
return (1);
}
}
return (0);
}

void start_child(char *input)
{
const char *error_message;
pid_t pid = fork();

if (pid == -1)
{
perror("fork");
exit(EXIT_FAILURE);
}
if (pid == 0)
{

char *args[2];
args[0] = input;
args[1] = NULL;

execve(input, args, environ);

error_message = "No such file or directory\n";
write(STDERR_FILENO, error_message, strlen(error_message));

exit(EXIT_FAILURE);
}
}

void wait_pid()
{

waitpid(0, NULL, 0);
}

int main(void)
{
const char *invalid_alias_msg;
char fil[MAX_COMMAND_LENGTH];
struct Alias aliases[MAX_ALIASES];
int alias_count = 0;
 while (1)
{

const char *prompt = "alias$";
write(STDOUT_FILENO, prompt, strlen(prompt));
fflush(stdout);

if (fgets(fil, sizeof(fil), stdin) == NULL)
{
char newline = '\n';
write(STDOUT_FILENO, &newline, 1);
}

fil[strcspn(fil, "\n")] = '\0';

if (strncmp(fil, "alias", 5) == 0)
{
char name[MAX_ALIAS_NAME];
char command[MAX_COMMAND_LENGTH];

if (sscanf(fil, "alias %s %[^\n]", name, command) == 2)
{
add_alias(aliases, &alias_count, name, command);
}
else
{
invalid_alias_msg = "Invalid alias syntax. Usage: alias <name> <command>\n";
write(STDOUT_FILENO, invalid_alias_msg, strlen(invalid_alias_msg));
}
}
else
{
start_alias(fil, aliases, &alias_count);
}
}

return (0);
}

