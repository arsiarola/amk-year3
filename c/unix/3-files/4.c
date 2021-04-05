#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#define TRUE 1
#define FALSE 0

void err_exit(const char *errmsg);
int is_dot_or_dotdot(const char *str);
int main(int argc, char *argv[]) {
        DIR *dirp1, *dirp2;
        struct dirent *dent1, *dent2;
        if (argc < 3)
                err_exit("Usage: a.out [DIR1] [DIR2]\n");

        if ((dirp1=opendir(argv[1])) == NULL)
                err_exit("opendir1 failed");

        if ((dirp2=opendir(argv[2])) == NULL)
                err_exit("opendir2 failed");

        while(dent1=readdir(dirp1)) {
                if (is_dot_or_dotdot(dent1->d_name))
                        continue;
                while(dent2=readdir(dirp2)) {
                        if (is_dot_or_dotdot(dent2->d_name))
                                continue;
                        if (strcmp(dent1->d_name, dent2->d_name) == 0)
                                printf("Tiedosto %s löytyy molemmista hakemistoista.\n",
                                       dent1->d_name);
                }
                rewinddir(dirp2);
        }
        closedir(dirp1);
        closedir(dirp2);
        exit(EXIT_SUCCESS);
}

int is_dot_or_dotdot(const char *str) {
        if (strcmp(str, ".") == 0)
                return TRUE;
        if (strcmp(str, "..") == 0)
                return TRUE;
        return FALSE;
}

void err_exit(const char *errmsg) {
        fprintf(stderr, "%s", errmsg);
        exit(EXIT_FAILURE);
}
