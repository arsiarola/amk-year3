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

        printf("%d: Sleeping for 3 seconds\n", getpid());
        sleep(3);

        printf("%d: sends SIGUSR1 to parent\n", getpid());
        kill(getppid(), SIGUSR1);

        printf("%d: Stopping at pause(), waiting for SIGUSR1\n", getpid());
        pause();

        printf("%d: Exiting program\n", getpid());
        sleep(1);
        return 0;
}

void sig_handler(int sig) {
        char msg[128];
        snprintf(msg, 128, "%d: Tuli signaali SIGUSR1\n", getpid());
        write(STDOUT_FILENO, msg, strlen(msg));
}

