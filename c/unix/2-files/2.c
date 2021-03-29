#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#define MAXBUF 256
#define ERRSIZE 256

int try_open_file(const char *fname, int flags, char *errmsg, size_t size);

int main(int argc, char *argv[]) {
        int fd1, fd2;
        int count;
        char buf1[MAXBUF], buf2[MAXBUF]; // lets go for static buffers this time
        char errmsg[ERRSIZE];
        int ret = 0;
        if (argc != 3) {
                ret = -1;
                snprintf(errmsg, ERRSIZE,
                         "Program takes two arguments, files to be compared\n"
                         "Usage: %s [FILE1] [FILE2]\n", argv[0]);
                goto done;
        }

        if ((fd1 = try_open_file(argv[1], O_RDONLY, errmsg, ERRSIZE)) < 0)
                goto done;
        if ((fd2 = try_open_file(argv[2], O_RDONLY, errmsg, ERRSIZE)) < 0)
                goto close_file1;

        if ((count = pread(fd1, buf1, MAXBUF, 0)) < 0) {
                snprintf(errmsg, ERRSIZE, "Error in reading file %s : ", argv[1]);
                goto close_file2;
        }
        if ((count = pread(fd2, buf2, MAXBUF, 0)) < 0) {
                snprintf(errmsg, ERRSIZE, "Error in reading file %s : ", argv[2]);
                goto close_file2;
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

close_file2:
        close(fd2);
close_file1:
        close(fd1);
done:
        if (ret == 0) {
                exit(EXIT_SUCCESS);
        }
        else {
                fprintf(stderr, "%s", errmsg);
                exit(EXIT_FAILURE);
        }
}

int try_open_file(const char *fname, int flags, char *errmsg, size_t size) {
        int fd = open(fname, flags);
        if (fd < 0) {
                snprintf(errmsg, size,
                         "%s couldn't be opened for reading : ",
                         fname);
        }
        return fd;
}

