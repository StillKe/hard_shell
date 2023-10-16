#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    char current_char;
    char input_buffer[256];
    int input_index = 0;
    unsigned int nread;
    int i;

    while (1)
    {
        write(STDOUT_FILENO, "Type anything (type 'exit' to quit): ", 36);

        while (1)
        {
            if (read(STDIN_FILENO, &current_char, 1) != 1)
            {
                perror("Error reading input");
                return 1;
            }

            if (current_char == '\n')
            {
                break;
            }

            if (input_index < 255)
            {
                input_buffer[input_index] = current_char;
                input_index++;
            }
        }

        input_buffer[input_index] = '\0';

        if (strcmp(input_buffer, "exit") == 0)
        {
            write(STDOUT_FILENO, "Program exited...\n", 18);
            return 0;
        }

        if (input_index == 0)
        {
            write(STDOUT_FILENO, "No characters entered.\n", 24);
        }
        else
        {
            for (i = 0; i < input_index; i++)
            {
                if (input_buffer[i] == '\n')
                {
                    write(STDOUT_FILENO, "Enter pressed\n", 14);
                }
                else if (input_buffer[i] == ' ')
                {
                    write(STDOUT_FILENO, "Space pressed\n", 14);
                }
                else if (input_buffer[i] == '\t')
                {
                    write(STDOUT_FILENO, "Tab pressed\n", 12);
                }
            }

            nread = (unsigned int)input_index;
            write(STDOUT_FILENO, "Keystroke is: (", 16);
            write(STDOUT_FILENO, &nread, sizeof(nread));
            write(STDOUT_FILENO, " characters): ", 13);
            write(STDOUT_FILENO, input_buffer, input_index);
            write(STDOUT_FILENO, "\n", 1);
        }

        input_index = 0;
    }

    return 0;
}

