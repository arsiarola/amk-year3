#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#define MAXBUF 256

int main(int argc, char *argv[]) {
        int fd;
        int count;
        char buf[MAXBUF];
        if (argc != 2) {
                fprintf(stderr,
                        "Program takes one argument, file to be read backwards\n"
                        "Usage: %s [FILE]\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
                fprintf(stderr,  "%s couldn't be opened for reading : ", argv[1]);
                perror(NULL);
                exit(EXIT_FAILURE);
        }
        if ((count = pread(fd, buf, MAXBUF, 0)) < 0) {
                fprintf(stderr,  "Error in reading file %s : ", argv[1]);
                perror(NULL);
                exit(EXIT_FAILURE);
        }

        // no strrev() for gcc =(
        // could do function for reversing but we can just loop the str backwards
        // and print one char at a time
        for (int i = count-1; i >= 0; --i) {
                printf("%c", buf[i]);
        }

        exit(EXIT_SUCCESS);
}
