#define _XOPEN_SOURCE 500
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

#define MSG "line1\nline2\nlast line of the file"

extern const char * const sys_siglist[];

void err_exit(const char *errmsg);
void print_sig(int sig);


/*
    man 2 unlink, second paragraph from description, explains why the file can be closed
    and unlinked in the child process while the mother process still uses it afterwards

   "If  the  name  was  the  last link to a file but any processes still have the file
   open, the file will remain in existence until the last file  descriptor  referring
   to it is closed."
*/

int main(void) {
        pid_t c;
        int fd, status;
        char buf[64];
        char fname[] = "tmpXXXXXX";

        if ((fd = mkstemp(fname)) < 0)
                err_exit("mkstmp failed : ");

        if ((c = fork()) < 0)
                err_exit("fork error");

        if (c == 0) {
                if (write(fd, MSG, strlen(MSG)) != strlen(MSG))
                        err_exit("Error in writing to file");
                close(fd);
                unlink(fname);
                exit(EXIT_SUCCESS);
        }

        else { // parent
                waitpid(c, &status, 0);
                print_sig(status);
                if (status)
                        err_exit("child returned error");
                lseek(fd, 0, SEEK_SET);
                int count;
                while ((count = read(fd, buf, 63)) > 0) {
                        buf[count] = '\0';
                        printf("%s", buf);
                }
                printf("\n");
        }

        close(fd);
        unlink(fname);
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
