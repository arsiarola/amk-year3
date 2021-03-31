#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

void err_exit();
int sameFile(int fd1, int fd2);
int main(int argc, char *argv[]) {
        int fd1, fd2;
        char *fname1, *fname2;
        if (argc != 3) {
                fprintf(stderr,
                        "Program takes two arguments, files to be compared\n"
                        "Usage: %s [FILE1] [FILE2]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
        fname1 = argv[1];
        fname2 = argv[2];
        if ((fd1 = open(fname1, O_RDONLY)) < 0)
                err_exit();
        if ((fd2 = open(fname2 , O_RDONLY)) < 0) {
                close(fd1);
                err_exit();
        }

        printf("Files same = %s\n", sameFile(fd1, fd2) ? "TRUE" : "FALSE");
        close(fd1);
        close(fd2);
        exit(EXIT_SUCCESS);
}

int sameFile(int fd1, int fd2) {
        struct stat stat1, stat2;
        if(fstat(fd1 ,&stat1) < 0)
                err_exit();
        if(fstat(fd2 ,&stat2) < 0)
                err_exit();
        // for debugging
        // printf("File1 inode:    %lu\n", stat1.st_ino);
        // printf("File2 inode:    %lu\n", stat2.st_ino);
        // printf("File1 dev:      %lu\n", stat1.st_dev);
        // printf("File2 dev:      %lu\n", stat2.st_dev);
        return (stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino);
}

void err_exit() {
        perror(NULL);
        exit(EXIT_FAILURE);
}
