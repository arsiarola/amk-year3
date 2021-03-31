#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
extern const char * const sys_siglist[];

void err_exit(const char *errmsg);
int main(void) {
        pid_t c1, c2;
        int status1, status2;
        printf("before fork\n");
        (c1 = fork()) && (c2 = fork());
        if (c1 == 0) {
                printf("First child ready\n");
                exit(1);
        }

        else if (c2 == 0) {
                sleep(3);
                printf("Second child ready\n");
                exit(2);
        }
        else { // parent
                waitpid(c2, &status2, 0);
                printf("child2 waitstatus: ");
                print_sig(status2);

                waitpid(c1, &status1, 0);
                printf("child1 waitstatus: ");
                print_sig(status1);
        }

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
        fprintf(stderr, "%s", errmsg);
        exit(EXIT_FAILURE);
}
