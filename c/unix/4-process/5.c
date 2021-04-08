#define _XOPEN_SOURCE 500
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

#define MSG "line1\nline2\nlast line of the file"
#define SIZE 2

extern const char * const sys_siglist[];

void err_exit(const char *errmsg);
void print_sig(int sig);

int main(void) {
        pid_t c;
        int fd[2];
        int status;
        char buf[SIZE];

        pipe(fd);
        if ((c = fork()) < 0)
                err_exit("fork error");

        if (c == 0) {
                close(fd[0]); // doesnt read the pipe
                if (write(fd[1], MSG, strlen(MSG)) != strlen(MSG))
                        err_exit("Error in writing to file");
                close(fd[1]);
                exit(EXIT_SUCCESS);
        }

        // parent
        close(fd[1]); // doesnt write to pipe
        waitpid(c, &status, 0);
        print_sig(status);
        if (status)
                err_exit("child returned error");

        int count;
        while ((count = read(fd[0], buf, SIZE-1)) > 0) {
                buf[count] = '\0';
                printf("%s", buf);
        }
        printf("\n");

        close(fd[0]);
        exit(EXIT_SUCCESS);
}

void print_sig(int sig) {
        if (sig == 0)
                printf("0 - Command succeeded\n");
        else if (WIFEXITED(sig)) {
                printf("%d - %s\n",
                       WEXITSTATUS(sig),
                       sys_siglist[WEXITSTATUS(sig)]);
        }
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
