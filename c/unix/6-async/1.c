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
        sigaction(SIGINT,  &act, NULL);
        sigaction(SIGTERM, &act, NULL);
        while (1) {
                printf("pid=%d\n", getpid());
                sleep(1);
        }
        return 0;
}

void sig_handler(int sig) {
        char msg[64];
        snprintf(
                 msg, 64, "%s%s%s",
                 "Tuli signaali ",
                 strsignal(sig),
                 "-signaali, suoritus jatkuu\n"
                 );
        write(STDOUT_FILENO, msg, strlen(msg));
}

