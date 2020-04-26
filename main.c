#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <sys/statfs.h>
#include <pthread.h>
#include <stdatomic.h>

// Declare number of threads to create
#define THREADS 2

// Structure that is create shared variables
typedef struct Threadparameter {

    int *optimalbuffersize;

    FILE *file;

    // atomic long pointer for thread safety
    atomic_long *counter;

} Threadparameter;


// Function spwaned as thread
void *readbuffer(void *parameters) {

    Threadparameter *params = (Threadparameter*) parameters;

    FILE *file = params->file;

    int *optimalbuffersize = params->optimalbuffersize;

    atomic_long *counter = params->counter;

    char *buffer = (char*) malloc(*optimalbuffersize * sizeof(char));

    int nread;

    long local_thread_counter = 0;

    while(nread = fread(buffer, sizeof(char), *optimalbuffersize, file))
    {

        nread++;

        while(nread--) 

            if(buffer[nread] == '\n')

                local_thread_counter++; 


    }

    // After counting using the local counter, outside the loop, increment the atomic counter
    atomic_fetch_add(counter, local_thread_counter);

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

    // Predefine optimal disk block read size
    optimalbuffersize = 1024 * 4;


    // If optimal found using fstat
    if(fstatfs(fd, &fsinfo) != -1)

        optimalbuffersize = fsinfo.f_bsize; // then overwrite the default one with the system optimal one


    if(!file) {

        printf("Couldn't open file\n");

        return -1;
    }

    atomic_long counter = 0; // Atomic counter initialized and this would be used by all threads

    Threadparameter *params = (Threadparameter*) malloc(sizeof(Threadparameter)); // Variable initialized using Threadparameter Struct

    params->file = file;

    params->counter = &counter;

    params->optimalbuffersize = &optimalbuffersize;

    pthread_t *threads = (pthread_t*)malloc(sizeof(pthread_t) * THREADS); // Number of threads initialized

    for(unsigned char loop = 0; loop < THREADS; loop++)

        pthread_create(&threads[loop], NULL, readbuffer, (void*) params); // Creating threads

    for(unsigned char loop = 0; loop < THREADS; loop++)

        pthread_join(threads[loop], NULL); // Joining threads to avoid program exiting before the threads complete


    printf("%ld\n",counter); // Printing the total count of lines

    fclose(file);

    free(threads);

    free(params);

    return 0;
}



