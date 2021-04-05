/* isottele.c */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(void) {
	char c; int n;
	while ( (n = read(STDIN_FILENO, &c, 1)) != 0) {
		if (islower(c))
			c = toupper(c);
		if (write(STDOUT_FILENO, &c, n) != n) {
			perror("can't write to STDOUT_FILENO");
			exit(EXIT_FAILURE);
		}
	}
	exit(EXIT_SUCCESS);
}
