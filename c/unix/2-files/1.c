#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#define ERRSIZE 256

int main(int argc, char *argv[]) {
        int fd;
        int count, size;
        char *buf, *fname;
        char errmsg[ERRSIZE];
        int ret = 0;
        if (argc != 2) {
                snprintf(errmsg, ERRSIZE,
                         "Program takes one argument, file to be read backwards\n"
                         "Usage: %s [FILE]\n", argv[0]);
                ret = -1;
                goto done;
        }
        fname = argv[1];
        fd = open(fname, O_RDONLY);
        if (fd < 0) {
                ret = errno;
                snprintf(errmsg, ERRSIZE,
                         "%s couldn't be opened for reading : %s",
                         fname, strerror(errno));
                goto done;
        }


        size = lseek(fd, 0, SEEK_END);;
        printf("filesize=%d\n", size);
        lseek(fd, 0, SEEK_SET);
        buf = malloc(sizeof(char) * size + 1);
        if (buf == NULL) {
                ret = errno;
                snprintf(errmsg, ERRSIZE,
                         "Error in allocating memory : %s\n",
                         strerror(errno));
                goto close_file;
        }

        if ((count = pread(fd, buf, size, 0)) < 0) {
                ret = errno;
                snprintf(errmsg, ERRSIZE,
                         "Error in reading file %s : %s",
                         fname, strerror(errno));
                goto free_buf;
        }

        // no strrev() for gcc =(
        // could do function for reversing but we can just loop the str backwards
        // and print one char at a time
        for (int i = count-1; i >= 0; --i) {
                printf("%c", buf[i]);
        }

free_buf:
        free(buf);
close_file:
        close(fd);
done:
        if (ret == 0) {
                exit(EXIT_SUCCESS);
        }
        else {
                fprintf(stderr, "%s", errmsg);
                exit(EXIT_FAILURE);
        }

}
