#include "simp.h"
void toaComments(char *input)
{
    char *commentPosition = strchr(input, '#');
    if (commentPosition != NULL) {
        *commentPosition = '\0';
    }
}

void writeIntToString(char *str, int num)
{   
	int b;
    int digits = 1;
    int temp = num;
    while (temp / 10 > 0) {
        digits++;
        temp /= 10;
    }

    for (b = digits - 1; b >= 0; b--)
    {
        str[b] = '0' + (num % 10);
        num /= 10;
    }
    str[digits] = '\0';
}

void revisitVariables(char *input, int status)
{
	pid_t pid;
    char *search = "$?";
    char revisit[10];
    
    char *varPosition = strstr(input, search);
    if (varPosition != NULL) {
        writeIntToString(revisit, status);
        memmove(varPosition + strlen(revisit), varPosition + strlen(search), strlen(varPosition) - strlen(search) + 1);
        strncpy(varPosition, revisit, strlen(revisit));
    }

    search = "$$";
    pid = getpid();
    writeIntToString(revisit, pid);

    varPosition = strstr(input, search);
    while (varPosition != NULL) {
        memmove(varPosition + strlen(revisit), varPosition + strlen(search), strlen(varPosition) - strlen(search) + 1);
        strncpy(varPosition, revisit, strlen(revisit));
        varPosition = strstr(varPosition + strlen(revisit), search);
    }
}

int startCommand(const char *amri, int *lastExitStatus)
{
    pid_t child_id;
    char *command_dublicate = strdup(amri);
    toaComments(command_dublicate);
    revisitVariables(command_dublicate, *lastExitStatus);

    child_id = fork();
    if (child_id == -1)
    {
        perror("fork");
        _exit(1);
    } else if (child_id == 0)
    {
        char *cmd[4];  /*Array to hold the command and arguments*/
        cmd[0] = "/bin/sh";
        cmd[1] = "-c";
        cmd[2] = command_dublicate;
        cmd[3] = NULL;

        execve("/bin/sh", cmd, NULL);
        perror("execve");
        _exit(1);;
    }
    else
    {
        int status;
        waitpid(child_id, &status, 0);
        *lastExitStatus = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }

    free(command_dublicate);
    return (-1);
}

int main(int argc, char *argv[])
{
	ssize_t Readbytes;
	char mstari[MAX_INPUT_SIZE];
	char newline;
    const char *filename;
    int file_fd;

    int lastExitStatus = 0;

    if (argc != 2) {
        char usage[] = "Usage: ";
        write(STDERR_FILENO, usage, sizeof(usage) - 1);
        write(STDERR_FILENO, argv[0], strlen(argv[0]));
        newline = '\n';
        write(STDERR_FILENO, &newline, 1);
        return (1);
    }

    filename = argv[1];
    file_fd = open(filename, O_RDONLY);
    if (file_fd == -1) {
        perror("open");
        return 1;
    }

    
    

    while ((Readbytes = read(file_fd, mstari, sizeof(mstari))) > 0)
    {
        mstari[Readbytes] = '\0'; 
        startCommand(mstari, &lastExitStatus);
    }

    close(file_fd);

    return (0);
}

