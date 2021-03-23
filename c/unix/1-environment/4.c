#include <unistd.h> /* POSIX API */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>  // isspace()
#define MAXLINE 256
#define MAXARGS 20

int main(void) {
        char buf[MAXLINE];
        char *args[MAXARGS];
        char * cmd;
        pid_t pid;
        int status;
        printf("%% "); /* tulosta kehoite: %%, jotta tulostuu % */
        while (fgets(buf, MAXLINE, stdin) != NULL) {
				buf[buf_len] = '\0'; /* korvaa rivinvaihtomerkki */
                if ((pid = fork()) < 0) {
                        perror("fork error");
                        exit(EXIT_FAILURE);
                }
                if (pid == 0) { /* lapsiprosessi jatkaa tästä */
						int buf_len = strlen(buf) - 1;
						char tmp[buf_len + 1];
						strncpy(tmp, buf, buf_len + 1);
						char *pos = tmp;
						char *start;
						int count = 0;

						while (1) {
								// - 1 to leave space for NULL
								if (count >= MAXARGS - 1 )
										break;
								// always first clear preceding whitespace
								while (isspace(*pos)) {
										++pos;
								}
								if (*pos == '\0')
										break;
								// mark starting position and move through the word
								start = pos;
								while (!isspace(*pos) && *pos != '\0') {
										++pos;
								}
								if (*pos != '\0') {
								// we hit whitespace so mark it with null terminator,
								// now from variable start to this null we have a
								// word that contains no whitespace
										*pos = '\0';
										++pos;
								}

								args[count] = start;
								++count;
						}
						
						if (count == 0) cmd = buf;
						else 			cmd = args[0];
						args[count] = NULL;
						// printf("cmd=\"%s\"\n", cmd);
						// for (int i = 0; i < count; ++i) {
						//         printf("arg[%d]=\"%s\"\n", i, args[i]);
						// }
                        execvp(cmd, args);
                        fprintf(stderr,
                                "couldn't execute execlp: %s - %s\n",
                                buf, strerror(errno));
                        exit(EXIT_FAILURE);
                }
                /* mammaprosessi jatkaa tästä */
                if ((pid = waitpid(pid, &status, 0)) < 0) {
                        perror("waitpid error");
                        exit(EXIT_FAILURE);
                }
                printf("%% ");
        }
        exit(EXIT_SUCCESS);
}
