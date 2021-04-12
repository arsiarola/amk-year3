#define _POSIX_C_SOURCE  200809L
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

static pid_t child;

extern const char * const sys_siglist[];
void sig_handler(int sig);
void err_exit(const char *errmsg);

int main(int argc, char *argv[]) {
        struct sigaction act;
        act.sa_handler = sig_handler;
        act.sa_flags = 0;
        int fd[2];

        if (pipe(fd) < 0)
                err_exit("creating pipe failed");

        sigemptyset(&act.sa_mask);
        sigaction(SIGUSR1,  &act, NULL);

        if ((child = fork()) < 0)
                err_exit("Failed to fork");


        if (child == 0) { // child
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
                close(fd[1]);
                if (execl("./2a.out", "", NULL) < 0)
                        err_exit("Error in launching child with execl");
        }
        // parent
        printf("program=%d, childprogram=%d\n", getpid(), child);
        close(fd[1]);

        printf("%d: Waiting for SIGUSR1 signal\n", getpid());
        pause();

        printf("%d: Exiting program\n", getpid());
        sleep(1);
        close(fd[0]);
        return 0;
}

void sig_handler(int sig) {
        char msg[128];
        snprintf(msg, 128,
                        "%d: Signaali SIGUSR1 vastaanotettu\n"
                        "%d: Lähetetään SIGUSR1 lapselle\n",
                        getpid(), getpid());
        write(STDOUT_FILENO, msg, strlen(msg));
        kill(child, SIGUSR1);
}


void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
