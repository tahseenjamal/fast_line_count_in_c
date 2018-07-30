#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <sys/statfs.h>

int main(int argc, char const *argv[]) {

    if(argc < 2) {

        printf("Pass file name as argument\n");

        return -1;
    }

    FILE *file;

    file = fopen(argv[1],"rb");

    struct statfs fsinfo = {0};

    int fd = fileno(file);

    long optimalbuffersize;

    optimalbuffersize = 1024 * 1024;

    if(fstatfs(fd, &fsinfo) != -1)
    {

        optimalbuffersize = fsinfo.f_bsize;

    }

    char *buffer = (char*) malloc(sizeof(char) * optimalbuffersize);

    int nread;

    if(!file) {

        printf("Couldn't open file\n");

        return -1;
    }

    long count = 0;

    while(nread = fread(buffer, 1, optimalbuffersize, file))
    {
        nread++;

        while(nread--) {

            if(buffer[nread] == '\n')
    
                count++;

        }

    }

    printf("%ld\n",count);

    fclose(file);

    free(buffer);

    return 0;
}



