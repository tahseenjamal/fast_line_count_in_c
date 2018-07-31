#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <sys/statfs.h>
#include <pthread.h>
#include <stdatomic.h>

#define THREADS 10

typedef struct Threadparameter {

    int *optimalbuffersize;

    FILE *file;

    atomic_long *counter;

} Threadparameter;

int read_data(char *buffer, int optimalbuffersize, FILE *file) {

    int nread;

    /*flockfile(file);*/

    nread = fread(buffer, sizeof(char), optimalbuffersize, file);

    /*funlockfile(file);*/

    return nread;

}


void *readbuffer(void *parameters) {

    Threadparameter *params = (Threadparameter*) parameters;

    FILE *file = params->file;

    int *optimalbuffersize = params->optimalbuffersize;

    atomic_long *count = params->counter;

    char *buffer = (char*) malloc(*optimalbuffersize * sizeof(char));

    int nread;

    while(nread = fread(buffer, sizeof(char), *optimalbuffersize, file))
    {

        nread++;

        while(nread--) {

            if(buffer[nread] == '\n')
            {
                atomic_fetch_add(count, 1);
                
            }

        }

    }

    free(buffer);

}

int main(int argc, char const *argv[]) {

    if(argc < 2) {

        printf("Pass file name as argument\n");

        return -1;
    }

    FILE *file;

    file = fopen(argv[1],"rb");

    struct statfs fsinfo = {0};

    int fd = fileno(file);

    int optimalbuffersize;

    optimalbuffersize = 1024 * 1024;

    if(fstatfs(fd, &fsinfo) != -1)
    {

        optimalbuffersize = fsinfo.f_bsize;

    }

    if(!file) {

        printf("Couldn't open file\n");

        return -1;
    }

    atomic_long count = 0;

    Threadparameter *params = (Threadparameter*) malloc(sizeof(Threadparameter));

    params->file = file;

    params->counter = &count;

    params->optimalbuffersize = &optimalbuffersize;

    pthread_t thread1, thread2, thread3, thread4;

    pthread_t *threads = (pthread_t*)malloc(sizeof(pthread_t) * THREADS);

    for(unsigned char loop = 0; loop < THREADS; loop++)
    {
        pthread_create(&threads[loop], NULL, readbuffer, (void*) params);

        pthread_join(threads[loop], NULL);
    }


    printf("%ld\n",count);

    fclose(file);

    free(params);

    return 0;
}



