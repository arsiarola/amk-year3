#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

        int etsi(int fd, char *mjono);
int main(int argc, char *argv[]) {
        int fd;
        char *fname;
        if (argc < 2) {
                fprintf(stderr,
                        "Not enough arguments\n"
                        "Usage: %s [STR] [...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
        printf("Etsittävä sana on %s\n", argv[1]);
        for (int i = 2; i < argc; i++) {
                fname = argv[i];
                printf("%s  ", fname);
                if ((fd = open(fname, O_RDONLY)) < 0) {
                        perror(NULL);
                }
                else {
                        printf("%s\n", etsi(fd, "test") ? "KYLLÄ" : "ei");
                }
        }
}

int etsi(int fd, char *mjono) {
        off_t size = lseek(fd, 0, SEEK_END);
        char *buf = malloc(size + 1);
        char *found;
        buf[size] = '\0';
        if (buf == NULL) exit(EXIT_FAILURE);
        lseek(fd, 0, SEEK_SET);
        read(fd, buf, size);
        found = strstr(buf, mjono);
        free(buf);
        return found ? TRUE : FALSE;
}
