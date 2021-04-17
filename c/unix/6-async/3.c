
#define _POSIX_C_SOURCE  200809L
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>

#define IN  0
#define OUT 1
void err_exit(const char *errmsg);
void sig_handler(int sig);

int main(int argc, char *argv[]) {
        pid_t c1, c2, c3;
        int p1[2], p2[2];


        if (pipe(p1) < 0)
                err_exit("pipe1 failed");
        if ((c1 = fork()) < 0)
                err_exit("Failed to fork");

        if (c1 == 0) {
                dup2(p1[OUT],STDOUT_FILENO);
                close(p1[OUT]);
                close(p1[IN]);
                close(STDIN_FILENO);
                execlp("ps", "ps", "-A", NULL);
                exit(EXIT_FAILURE);
        }


        if (pipe(p2) < 0)
                err_exit("pipe2 failed");
        if ((c2 = fork()) < 0)
                err_exit("Failed to fork child2");
        if (c2 == 0) {
                dup2(p1[IN], STDIN_FILENO);
                dup2(p2[OUT], STDOUT_FILENO);
                close(p1[IN]);
                close(p1[OUT]);
                close(p2[IN]);
                close(p2[OUT]);
                execlp("grep", "grep", "bash", NULL);
                exit(EXIT_FAILURE);
        }

        /*
              child1 and 2 have been handled and since
              pipe1 is just used between those, it can be closed

              pipe1         pipe2         stdout
              child1 ------ child2 ------ child3 ----->
                       ^
        */
        close(p1[IN]);
        close(p1[OUT]);

        if ((c3 = fork()) < 0)
                err_exit("Failed to fork child3");
        if (c3 == 0) {
                dup2(p2[IN], STDIN_FILENO);
                close(p2[IN]);
                close(p2[OUT]);
                /* execlp("wc", "wc", "-l", NULL); */
                execlp("wc", "wc", "-l", NULL);
                exit(EXIT_FAILURE);
        }

        /*
             child3 writes directly to stdout, and uses pipe2 to
             communicate with child2, so pipe2 can be closed

                    pipe1         pipe2         stdout
             child1 ------ child2 ------ child3 ----->
                                    ^
        */
        close(p2[IN]);
        close(p2[OUT]);


        // not checking the status of of child so we can pass NULL to wstatus parameter
        waitpid(c1, NULL, 0);
        waitpid(c2, NULL, 0);
        waitpid(c3, NULL, 0);
        exit(EXIT_SUCCESS);
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
