#define _POSIX_C_SOURCE  200809L
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

extern const char * const sys_siglist[];
void sig_handler(int sig);

int main(int argc, char *argv[]) {
        struct sigaction act;
        act.sa_handler = sig_handler;
        act.sa_flags = 0;

        sigemptyset(&act.sa_mask); // yhtään ei estetä, kaikki signaalit kelpuutetaan
        sigaction(SIGUSR1,  &act, NULL);
        printf("ppid=%d, pid=%d\n", getppid(), getpid());
        /* kill(getppid(), SIGUSR1); */
        pause();
        return 0;
}

void sig_handler(int sig) {
        char msg[] = "Tuli signaali SIGUSR1\n";
        write(STDOUT_FILENO, msg, strlen(msg));
}

