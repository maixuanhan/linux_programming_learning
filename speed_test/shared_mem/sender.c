#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#include "common.h"

static volatile int is_not_ready;

int main(int argc, char** argv)
{
    print_time();
    printf("SENDER starts.\n");

    /* Create a named semaphore */
    sem_t* p_sem = sem_open(SEMAPHORE_NAME, 0);
    if (p_sem == SEM_FAILED)
    {
        fprintf(stderr, "sem_open FAILED!\n");
        exit(EXIT_FAILURE);
    }

    /* Open shared mem object */
    int fd = shm_open(SHARED_OBJ_NAME, O_RDWR, 0);
    if (fd == -1)
    {
        fprintf(stderr, "shm_open FAILED!\n");
        exit(EXIT_FAILURE);
    }

    /* Get shared mem object information */
    struct stat sb;
    if (fstat(fd, &sb) == -1)
    {
        fprintf(stderr, "fstat FAILED!\n");
        exit(EXIT_FAILURE);
    }

    /* Create a new mapping which is shared */
    void* address = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (address == MAP_FAILED)
    {
        fprintf(stderr, "mmap FAILED!\n");
        exit(EXIT_FAILURE);
    }

    /* Close the file descriptor as we don't need it anymore */
    if (close(fd) == -1)
    {
        fprintf(stderr, "close FAILED!\n");
        exit(EXIT_FAILURE);
    }

    for (int k = 0; k < NUMBER_OF_TIMES; ++k)
    {
        /* Write data to shared memory */
        for (int i = 0; i < ELEM_COUNT; ++i)
        {
            ((int*)address)[i] = i * k;
        }

        if (sem_wait(p_sem) == -1)
        {
            fprintf(stderr, "sem_wait FAILED!\n");
            exit(EXIT_FAILURE);
        }

        ((int*)address)[LAST_ELEM_IDX] = 1; // signal to receiver

        if (sem_post(p_sem) == -1)
        {
            fprintf(stderr, "sem_post FAILED!\n");
            exit(EXIT_FAILURE);
        }

        // debug:
        // printf("Write block [%d] of data\n", k + 1);

        is_not_ready = 1;
        while (is_not_ready)
        {
            if (sem_wait(p_sem) == -1)
            {
                fprintf(stderr, "sem_wait FAILED!\n");
                exit(EXIT_FAILURE);
            }

            is_not_ready = ((int*)address)[LAST_ELEM_IDX];

            if (sem_post(p_sem) == -1)
            {
                fprintf(stderr, "sem_post FAILED!\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    print_time();
    printf("Done\n");

    return EXIT_SUCCESS;
}
