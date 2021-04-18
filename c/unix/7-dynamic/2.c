#define _POSIX_C_SOURCE  200809L
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>

#define MSIZE 4096
#define NAMELEN 80

struct henkilo {
        char nimi[NAMELEN];
        int ika;
};
struct henkilo henkilo;

void err_exit(const char *errmsg);

int main(int argc, char *argv[]) {
        pid_t child;
        int fd;
        char *m_name = "/test123123";

        if ((fd = shm_open("test", O_RDWR | O_CREAT, 0600)) < 0)
                err_exit("shm_open: opening shared memory failed");

        ftruncate(fd, MSIZE);
        void *p = mmap(NULL, MSIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        if (p == MAP_FAILED)
                err_exit("mmap: ongelma alueen liittämisessä osoiteavaruuteen");

        if ((child = fork()) < 0)
                err_exit("fork failed");

        if (child == 0) {
                write(STDOUT_FILENO, "test\n", 5);
                int sig;
                sigset_t set;
                sigemptyset(&set)

                do {
                        sigwait(&signal_set, &sig);
                } while (sig != SIGUSR1);
                write(STDOUT_FILENO, "Got sigusr1\n", 5);
                sleep(5);


                munmap(p, MSIZE); // poista omasta osoiteavaruudesta
                shm_unlink(m_name); // poista KJ:n kirjanpidosta
                exit(EXIT_SUCCESS);
        }

        // Parent
        // remember to free name
        char name[NAMELEN];
        int age, result;
        ssize_t len;
        printf("Enter a name: ");
        fgets(name, NAMELEN, stdin);
        printf("name=%s\n", name);

        // loop to make sure we get a number
        do {
                printf("Enter age: ");
                result = scanf("%d", &age);

                if (result == 0) {
                        printf("Unable to get a number, try again\n");
                        // clear stdin until a newline is found so we can use sscanf again
                        while (fgetc(stdin) != '\n') { }
                }
        } while (result  <= 0);
        printf("age=%d\n", age);

strcpy(henkilo.nimi, name);
        henkilo.ika = age;
        sleep(1);
        kill(child, SIGUSR1);
        munmap(p, MSIZE); // poista omasta osoiteavaruudesta
                shm_unlink(m_name); // poista KJ:n kirjanpidosta
                sleep(50);
        exit(EXIT_SUCCESS);
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
