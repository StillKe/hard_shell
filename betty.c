#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/**
 * main - runs the shell program
 *
 * Return: 0 on success
 */
int main(void)
{
	char *buffer = NULL;
	size_t buffersize = 0;
	ssize_t p_chars;

	while (1)
	{
		p_chars = getline(&buffer, &buffersize, stdin);
		if (p_chars == -1)
		{
			free(buffer);
			exit(0);
		}
	write(STDOUT_FILENO, buffer, p_chars);
	}
	free(buffer);
	return (0);
}
