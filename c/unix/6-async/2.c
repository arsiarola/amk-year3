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

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

void err_exit(const char *errmsg);
void sig_handler(int sig);

int main(int argc, char *argv[]) {
        pid_t child;
        int fd[2];

        pipe(fd);
        if (fd < 0)
                err_exit("pipe failed");

        if ((child = fork()) < 0)
                err_exit("Failed to fork");


        if (child == 0) { // child
                // allow only fd[1]
                close(fd[0]);
                close(STDOUT_FILENO);
                close(STDIN_FILENO);
                char msg[32];
                for (int i = 0; i < 5; ++i) {
                        snprintf(msg, 32, "line%d\n", i);
                        write(fd[1], msg, strlen(msg));
                        sleep(1);
                }
                close(fd[1]);
                exit(EXIT_SUCCESS);
        }
        // parent
        close(fd[1]);

        struct sigaction act;
        act.sa_handler = sig_handler;
        act.sa_flags = 0;
        sigemptyset(&act.sa_mask);
        sigaction(SIGCHLD,  &act, NULL);

        fd_set fd_sel;

        char buf[32];
        int n, count;
        while (1) {
                // select() will modify bits, reset them back
                FD_ZERO(&fd_sel);
                FD_SET(STDIN_FILENO, &fd_sel);
                FD_SET(fd[0], &fd_sel);

                count = select(4, &fd_sel, NULL, NULL, NULL);
                /* count = select(MAX(STDIN_FILENO, fd[0]), &fd_sel, NULL, NULL, NULL); */
                if (count == -1)
                        err_exit("select() error");

                if (FD_ISSET(fd[0], &fd_sel)) {
                        if ((n = read(fd[0], buf, 10)) > 0 ) {
                                write(STDOUT_FILENO, buf, strlen(buf));
                        } 
                        else if (n == -1 && errno != EAGAIN) {
                                err_exit("Error reading child pipe");
                        } 
                }

                if (FD_ISSET(STDIN_FILENO, &fd_sel)) {
                        if ((n = read(STDIN_FILENO, buf, 10)) > 0 ) {
                                write(STDOUT_FILENO, buf, strlen(buf));
                        }
                        else if (n == -1 && errno != EAGAIN) {
                                err_exit("Error reading stdin");
                        } 
                }
        }

        close(fd[0]);
        // shouldn't come to this part of code
        exit(EXIT_FAILURE);
        return 0;
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}

void sig_handler(int sig) {
        char msg[128];
        snprintf(msg, 128, "SIGCHLD vastaanotettu, exiting\n");
        fprintf(stderr, msg, strlen(msg));
        exit(EXIT_SUCCESS);
}
