#define _POSIX_C_SOURCE  200809L
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <smaphore.h>

#define MSIZE 4096
#define NAMELEN 80

struct henkilo {
        char nimi[NAMELEN];
        int ika;
};

void err_exit(const char *errmsg);

int main(int argc, char *argv[]) {
        pid_t child;
        int fd;
        struct henkilo henkilo;
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
                int sig;
                sigset_t set;
                sigemptyset(&set);
                sigaddset(&set, SIGUSR1);
                sigwait(&set, &sig);
                printf("CHILD received name: %s\n"
                                "CHILD received age:  %d\n",
                                ((struct henkilo *)p)->nimi, ((struct henkilo *)p)->ika);

                munmap(p, MSIZE); // poista omasta osoiteavaruudesta
                shm_unlink(m_name); // poista KJ:n kirjanpidosta
                sleep(5);
                exit(EXIT_SUCCESS);
        }

        // Parent
        // remember to free name
        char name[NAMELEN];
        int age, result;
        printf("Enter a name: ");
        fgets(name, NAMELEN, stdin);
        name[strcspn(name, "\n")] = '\0'; // replace newline with terminating null

        // loop to make sure we get a number
        while (1) {
                printf("Enter age: ");
                result = scanf("%d", &age); // result is number of successful conversions
                if (result == 1)
                        break;

                printf("Unable to get a number, try again\n");
                // clear stdin until a newline is found so we can use sscanf again
                while (fgetc(stdin) != '\n') { }
        }

        printf("name=%s\n", name);
        printf("age =%d\n", age);
        strcpy(henkilo.nimi, name);
        henkilo.ika = age;

        memcpy((struct henkilo *) p, &henkilo, sizeof(struct henkilo));
        kill(child, SIGUSR1);
        munmap(p, MSIZE); // poista omasta osoiteavaruudesta
        shm_unlink(m_name); // poista KJ:n kirjanpidosta
        exit(EXIT_SUCCESS);
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
