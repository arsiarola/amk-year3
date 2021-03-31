#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


int malloc_fcontent(int fd, char *fcontent, int *fsize);
int read_file(char *fname, char *fcontent, int *fsize);

int main(int argc, char *argv[]) {
        char *buf1, *buf2;
        int  size1, size2;
        if (argc != 3) {
                fprintf(stderr,
                        "Program takes two arguments, files to be compared\n"
                        "Usage: %s [FILE1] [FILE2]\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        if (read_file(argv[1], buf1, &size1)) {
                fprintf(stderr,
                        "Error reading file\n");
                goto done;
        }
        if (read_file(argv[2], buf2, &size2)) {
                fprintf(stderr,
                        "Error reading file\n");
                goto free_buf1;
        }


        char *p1 = buf1, *p2 = buf2;
        int count = 0;
        for ( ; *p1 != '\0' && *p2 != '\0' ; ++p1, ++p2, ++count) {
                if (*p1 != *p2) break;
                printf("%c", *p1);
        }
        printf("\n");

        if (*p1 == '\0' && *p2 == '\0')
                printf("The contents of the files are same\n");
        else
                printf("Same amount of characters: %d\n", count);

free_buf2:
        free(buf2);
free_buf1:
        free(buf1);
done:
        if (errno) {
                perror(NULL);
                exit(EXIT_FAILURE);
        }
        else {
                exit(EXIT_SUCCESS);
        }
}

int malloc_fcontent(int fd, char *fcontent, int *fsize) {
        char *buf;
        int size, sum, ret;
        size = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);
        buf = malloc(sizeof(char) * size + 1);
        if (buf == NULL)
                return errno;
        buf[size] = '\0';
        fcontent = buf;
        *fsize = size;
        return 0;
}

int read_file(char *fname, char *fcontent, int *fsize) {
        char *content;
        int fd, size;
        if ((fd = open(fname, O_RDONLY)) < 0)
                return errno;

        if (malloc_fcontent(fd, content, &size)) {
                close(fd);
                return errno;
        }

        if ((pread(fd, content, size, 0)) != size) {
                close(fd);
                return errno;
        }

        close(fd);
        fcontent = content;
        *fsize = size;
        return 0;
}


