#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

void err_exit(const char *errmsg);
int main(void) {
        pid_t pid;
        printf("before fork\n");
        if ((pid = fork()) < 0)
                err_exit("fork error");

        printf("ppid=%d, pid=%d\n",getppid(), getpid());
        if (pid != 0) {
                sleep(10);
                if (waitpid(pid, NULL, 0) < 0) err_exit("wait error");
        }

        exit(EXIT_SUCCESS);
}

void err_exit(const char *errmsg) {
        fprintf(stderr, "%s", errmsg);
        exit(EXIT_FAILURE);
}
