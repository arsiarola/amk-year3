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
        int count;
        if (argc < 2) {
                fprintf(stderr,
                        "Not enough arguments\n"
                        "Usage: %s [STR] [...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
        printf("Etsittävä sana on %s\n", argv[1]);
        count = 0;
        for (int i = 2; i < argc; i++) {
                fname = argv[i];
                printf("%s  ", fname);
                if ((fd = open(fname, O_RDONLY)) < 0) {
                        perror(NULL);
                        exit(EXIT_FAILURE);
                }
                else {
                        int found = etsi(fd, argv[1]);
                        if (found < 0) {
                                close(fd);
                                exit(EXIT_FAILURE);
                        }
                        printf("%s\n", found ? "KYLLÄ" : "ei");
                        if (found) count++;
                        close(fd);
                }
        }
        printf("Tiedostoja %d kpl, esiintymä %d:ssä tiedostossa.\n", argc - 2, count);
        exit(EXIT_SUCCESS);
}

int etsi(int fd, char *mjono) {
        off_t size;
        char *buf, *found;
        size = lseek(fd, 0, SEEK_END);
        buf = (char *) malloc(sizeof(char) * size + 1);
        if (buf == NULL) {
                perror("Memory allocation fail : ");
                return -1;
        }
        buf[size] = '\0';
        lseek(fd, 0, SEEK_SET);
        if (read(fd, buf, size) != size) {
                perror("File reading failed");
                free(buf);
                return -2;
        }
        found = strstr(buf, mjono);
        free(buf);
        return found ? TRUE : FALSE;
}
