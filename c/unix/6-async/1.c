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



void err_exit(const char *errmsg);

int main(int argc, char *argv[]) {
        pid_t child;
        int flags;
        int fd[2];

        pipe(fd);
        if (fd < 0)
                err_exit("pipe failed");

        flags = fcntl(fd[0], F_GETFL); 
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
        flags = fcntl(STDIN_FILENO, F_GETFL); 
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

        if ((child = fork()) < 0)
                err_exit("Failed to fork");


        if (child == 0) { // child
                close(fd[0]);
                char msg[32];
                for (int i = 0; i < 20; ++i) {
                        snprintf(msg, 32, "line%d\n", i);
                        write(fd[1], msg, strlen(msg));
                        sleep(1);
                }
                close(fd[1]);
                return 0;
        }
        // parent
        char buf[32];
        close(fd[1]);
        int n;
        while (1) {
                if ((n = read(fd[0], buf, 10)) > 0 ) {
                        buf[n] = '\0';
                        printf("%s", buf);
                } 
                else if (n == - 1 && errno != EAGAIN) {
                        err_exit("Error reading child pipe");
                } 

                if ((n = read(STDIN_FILENO, buf, 10)) > 0 ) {
                        buf[n] = '\0';
                        printf("%s", buf);
                }
                else if (n == - 1 && errno != EAGAIN) {
                        err_exit("Error reading child pipe");
                } 
        }

        close(fd[0]);
        exit(EXIT_SUCCESS);
        return 0;
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}

void sig_handler(int sig) {
        char msg[128];
        snprintf(msg, 128, "SIGCHLD vastaanotettu\n");
        exit(EXIT_SUCCESS);
}
