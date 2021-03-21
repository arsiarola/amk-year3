#include <unistd.h> /* POSIX API */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAXLINE 256
#define MAXARGS 20

int main(void) {
        char buf[MAXLINE];
        pid_t pid;
        int status;
        printf("%% "); /* tulosta kehoite: %%, jotta tulostuu % */
        while (fgets(buf, MAXLINE, stdin) != NULL) {
                int buf_len = strlen(buf) - 1;
                buf[buf_len] = '\0'; /* korvaa rivinvaihtomerkki */
                char *args[MAXARGS];
                char tmp[buf_len + 1];
                char *pos = tmp;
                char *start;
                int count = 0;
                strncpy(tmp, buf, buf_len + 1);

                while (1) {
                        while (isspace(*pos)) {
                                ++pos;
                        }
                        if (*pos == '\0') break;
                        start = pos;
                        while (!isspace(*pos) && *pos != '\0') {
                                ++pos;
                        }
                        if (*pos != '\0') {
                                *pos = '\0';
                                ++pos;
                        }

                        printf("start=%s\n", start);
                        args[count] = start;
                        ++count;
                }

                for (int i = 0; i < count; ++i) {
                        printf("arg[%d]=\"%s\"\n", i, args[i]);
                }


                if ((pid = fork()) < 0) {
                        perror("fork error");
                        exit(EXIT_FAILURE);
                }
                if (pid == 0) { /* lapsiprosessi jatkaa tästä */
                        execlp(buf, buf, (char *)0);
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

/* int idx = 0; */
/* token = strtok(pos, " "); */
/* while (token != NULL) { */
/*         printf("str=%s\n", token); */
/*         token = strtok(NULL, " "); */
/*         ++idx; */
/* } */
/* printf("count=%d\n", count); */
/* int count = idx; */
/* char *args[count + 1]; // + 1 for NULL */
/* args[count] = NULL; */
/* pos = buf; */
/* count = 0; */
/* token = strtok(pos, " "); */
/* while (token != NULL) { */
/*         args[count] = token; */
/*         ++count; */
/*         token = strtok(NULL, " "); */
/* } */
/* for (int i = 0; i < args_len-1; ++i) { */
/*         printf("args[%d]=%s\n", i, args[i]); */
/* } */
