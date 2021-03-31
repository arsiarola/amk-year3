nclude <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/sysmacros.h>



int main(int argc, char *argv[]) {

    char *buf1, *buf2;
    off_t size1, size2;
    int file1, file2;

    struct stat sb1;
    struct stat sb2;


    if(argc < 2) {
        perror("Anna kaksi tiedosta argumentteinä");
        exit(EXIT_FAILURE);
    }
    else {

        if ((file1 = open(argv[1], O_RDONLY)) < 0) {
            perror("Tiedoston 1 avaaminen ei onnistununt tai sitä ei ole olemassa.");
            exit(EXIT_FAILURE);
        }
        if ((file2 = open(argv[2], O_RDONLY)) < 0) {
            perror("Tiedoston 2 avaaminen ei onnistununt tai sitä ei ole olemassa.");
            exit(EXIT_FAILURE);
        }

        size1 = lseek(file1, 0, SEEK_END);
        size2 = lseek(file2, 0, SEEK_END);
        lseek(file1, 0, SEEK_SET);
        lseek(file2, 0, SEEK_SET);

        if ((buf1 = (char *)malloc(size1)) == NULL) {
            perror("Malloc tiedostosta 1 ei onnistunut.");
            exit(EXIT_FAILURE);
        }
        if ((buf2 = (char *)malloc(size2)) == NULL) {
            perror("Malloc tiedostosta 2 ei onnistunut.");
            exit(EXIT_FAILURE);
        }

        if (lstat(argv[1], &sb1) == -1) {
               perror("lstat 1");
               exit(EXIT_FAILURE);
        }
        if (lstat(argv[2], &sb2) == -1) {
               perror("lstat 2");
               exit(EXIT_FAILURE);
        }

        if (read(file1, buf1, size1) != size1) {
            perror("read file 1");
            exit(EXIT_FAILURE);
        }

        if (read(file2, buf2, size2) != size2) {
            perror("read file 2");
            exit(EXIT_FAILURE);
        }

        printf("%s size:                %jd bytes\n", argv[1], (intmax_t) sb1.st_size);
        printf("%s size:                %jd bytes\n", argv[2], (intmax_t) sb2.st_size);

        int count = 1;

        while(1) {

            if(buf1[count-1] == '\0' || buf2[count-1] == '\0') break;
            if(buf1[count-1] != buf2[count-1]) break;
            count++;
        }
        if (count-1 == size1 && count-1 == size2) printf("Both files have same content.\n");
        else printf("%d same characted before a different one.\n", count+1);

        free(buf1);
        free(buf2);
        close(file1);
        close(file2);
        exit(EXIT_SUCCESS);
        return 1;
    }
}
