#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/sysmacros.h>
#include <errno.h>
#include <string.h>


#define BUFFSIZE 2048

int main(int argc, char *argv[]) {

    int file;
    char buf[BUFFSIZE];
    char *buf2, num[128];
    int n, lines, i, sum;
    off_t size;
    char **arr;

    char *str = "File you wanted to create already exists, would you like to 1 to overwrite it or 2 to count the sum of numbers in it? type 1 or 2\n";


    if ((file = open(argv[1], O_RDWR | O_CREAT | O_EXCL, S_IRWXG | S_IRWXU | S_IRWXO)) < 0) {

        if (errno == EEXIST) {

            write(STDOUT_FILENO, str, 130*sizeof(char));

            while((n=read(STDIN_FILENO, buf, BUFFSIZE))>0) {
                if(buf[0] == '1') {

                    if ((file = open(argv[1], O_WRONLY | O_TRUNC)) < 0) {
                        perror("Tiedoston avaaminen ja resetointi ei onnistununt.");
                        exit(EXIT_FAILURE);
                    }

                    while((n=read(STDIN_FILENO, buf, BUFFSIZE))>0) {
                        if(write(file,buf, n) != n) {
                        perror("could not write all chars");
                        exit(EXIT_FAILURE);
                        }
                     }


                }
                else if(buf[0] == '2') {
                    if ((file = open(argv[1], O_RDONLY)) < 0) {
                        perror("Tiedoston avaaminen ei onnistununt.");
                        exit(EXIT_FAILURE);
                    }

                    size = lseek(file, 0, SEEK_END);
                    lseek(file, 0, SEEK_SET);

                    if ((buf2 = (char *)malloc(size)) == NULL) {
                        perror("Malloc ei onnistunut.");
                        exit(EXIT_FAILURE);
                    }

                    if (read(file, buf2, size) != size) {
                        perror("error reading the file");
                        exit(EXIT_FAILURE);
                    }

                   write(STDOUT_FILENO, buf2, size);

                    lines = 0;
                    for(i = 0; buf2[i] != '\0'; i++) if (buf2[i] == '\n') lines++;
                    arr = (char **)malloc(lines*sizeof(buf2));

                    char *p = strtok (buf2, "\n");
                    i=0;

                    while (p != NULL)
                    {
                        arr[i++] = p;
                        p = strtok (NULL, "\n");
                    }

                    sum=0;
                    for (i=0;i<lines;i++) {
                        sum += atoi(arr[i]);
                    }

                    sprintf(num,"\nSum of the numbers in the file is %d\n",sum);

                    write(STDOUT_FILENO,num,128*sizeof(char));

                    break;

                }

            }
        }

        else {
            perror("Tiedoston luominen ei onnistununt.");
            exit(EXIT_FAILURE);
        }
    }
    else {
        while((n=read(STDIN_FILENO, buf, BUFFSIZE))>0) {
            if(write(file,buf, n) != n) {
                perror("could not write all chars");
                exit(EXIT_FAILURE);
            }
        }
    }

    free(arr);
    free(buf2);
    close(file);
    exit(EXIT_SUCCESS);
    return 1;
}
