#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#define MAXBUF 256

int try_open_file(const char *fname, int flags);
int main(int argc, char *argv[]) {
        int fd1, fd2;
        int count;
        char buf1[MAXBUF], buf2[MAXBUF];
        if (argc != 3) {
                fprintf(stderr,
                        "Program takes two arguments, files to be compared\n"
                        "Usage: %s [FILE1] [FILE2]\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        fd1 = try_open_file(argv[1], O_RDONLY);
        fd2 = try_open_file(argv[2], O_RDONLY);

        if ((count = pread(fd1, buf1, MAXBUF, 0)) < 0) {
                fprintf(stderr,  "Error in reading file %s : ", argv[1]);
                perror(NULL);
                exit(EXIT_FAILURE);
        }
        if ((count = pread(fd2, buf2, MAXBUF, 0)) < 0) {
                fprintf(stderr,  "Error in reading file %s : ", argv[2]);
                perror(NULL);
                exit(EXIT_FAILURE);
        }

        char *p1 = buf1, *p2 = buf2;
        for (count = 0 ; *p1 != '\0' && *p2 != '\0' ; ++p1, ++p2, ++count) {
                if (*p1 != *p2) break;
                printf("%c", *p1);
        }
        printf("\n");

        if (*p1 == '\0' && *p2 == '\0')
                printf("The contents of the files are same\n");
        else
                printf("Same amount of characters: %d\n", count);

        exit(EXIT_SUCCESS);
}

int try_open_file(const char *fname, int flags) {
        int fd = open(fname, flags);
        if (fd < 0) {
                fprintf(stderr,  "%s couldn't be opened for reading : ", fname);
                perror(NULL);
                exit(EXIT_FAILURE);
        }
        return fd;
}

