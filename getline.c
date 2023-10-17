#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 256

unsigned int nreadUnsigned;
int i, newline_pos;
char *newline;

void process_input();

int main(void)
{
    while (1)
    {
        write(STDOUT_FILENO, "Type anything (type 'exit' to quit): ", 36);
        process_input();
    }

    return 0;
}

void process_input()
{
    static char input_buffer[BUFFER_SIZE];
    static int input_index = 0;

    ssize_t nread = read(STDIN_FILENO, input_buffer + input_index, BUFFER_SIZE - input_index);

    if (nread == -1)
    {
        perror("Error reading input");
        return;
    }

    input_index += nread;

    for (i = 0; i < input_index; i++)
    {
        char current_char = input_buffer[i];
        if (current_char == '\n')
        {
            write(STDOUT_FILENO, "Enter pressed\n", 14);
        }
        else if (current_char == ' ')
        {
            write(STDOUT_FILENO, "Space pressed\n", 14);
        }
        else if (current_char == '\t')
        {
            write(STDOUT_FILENO, "Tab pressed\n", 12);
        }
    }

    newline = memchr(input_buffer, '\n', input_index);
    if (newline != NULL)
    {
        newline_pos = newline - input_buffer;

        nreadUnsigned = (unsigned int)newline_pos;
        write(STDOUT_FILENO, "Keystroke is: (", 16);
        write(STDOUT_FILENO, &nreadUnsigned, sizeof(nreadUnsigned));
        write(STDOUT_FILENO, " characters): ", 13);
        write(STDOUT_FILENO, input_buffer, newline_pos);
        write(STDOUT_FILENO, "\n", 1);

        if (strncmp(input_buffer, "exit", 4) == 0)
        {
            write(STDOUT_FILENO, "Program exited...\n", 18);
            _exit(0);
        }

        input_index -= (newline_pos + 1);
        memmove(input_buffer, input_buffer + newline_pos + 1, input_index);
    }
}

