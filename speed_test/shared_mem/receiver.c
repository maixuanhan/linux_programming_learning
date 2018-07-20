#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#include "common.h"

static volatile int is_ready;

int main(int argc, char** argv)
{
    /* Create a named semaphore */
    sem_t* p_sem = sem_open(SEMAPHORE_NAME, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (p_sem == SEM_FAILED)
    {
        fprintf(stderr, "sem_open FAILED!\n");
        exit(EXIT_FAILURE);
    }

    /* Open shared mem object */
    int fd = shm_open(SHARED_OBJ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        fprintf(stderr, "shm_open FAILED!\n");
        exit(EXIT_FAILURE);
    }

    /* Set the shared mem object with the desired size */
    if (ftruncate(fd, SHARED_OBJ_SIZE) == -1)
    {
        fprintf(stderr, "ftruncate FAILED!\n");
        exit(EXIT_FAILURE);
    }

    /* Create a new mapping which is shared */
    void* address = mmap(NULL, SHARED_OBJ_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
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

    printf("RECEIVER starts. Waiting for data from SENDER...\n");

    int sum = 0;
    int validated_sum = 0;

    int is_first_time = 1;

    for (int k = 0; k < NUMBER_OF_TIME; ++k)
    {
        is_ready = 0;

        while (!is_ready)
        {
            if (sem_wait(p_sem) == -1)
            {
                fprintf(stderr, "sem_wait FAILED!\n");
                exit(EXIT_FAILURE);
            }

            is_ready = ((int*)address)[LAST_ELEM_IDX];

            if (sem_post(p_sem) == -1)
            {
                fprintf(stderr, "sem_post FAILED!\n");
                exit(EXIT_FAILURE);
            }
        }

        if (is_first_time)
        {
            is_first_time = 0;
            print_time();
            printf("Start receiving data\n");
        }

        /* Read shared data */
        for (int i = 0; i < ELEM_COUNT; ++i)
        {
            sum = (sum + ((int*)address)[i]);   // let the buffer overflow, we don't care
            validated_sum = (validated_sum + i * k);
        }

        if (sem_wait(p_sem) == -1)
        {
            fprintf(stderr, "sem_wait FAILED!\n");
            exit(EXIT_FAILURE);
        }

        ((int*)address)[LAST_ELEM_IDX] = 0; // signal to sender

        if (sem_post(p_sem) == -1)
        {
            fprintf(stderr, "sem_post FAILED!\n");
            exit(EXIT_FAILURE);
        }

        // Debug:
        // printf("Get block [%d] of data\n", k + 1);
    }

    if (sum != validated_sum)
    {
        fprintf(stderr, "DATA is corrupted!\nSum: %d\nValidated_sum: %d\n", sum, validated_sum);
    }

    /* Unlink the shared memory object */
    if (shm_unlink(SHARED_OBJ_NAME) == -1)
    {
        fprintf(stderr, "shm_unlink FAILED!\n");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(SEMAPHORE_NAME) == -1)
    {
        fprintf(stderr, "sem_unlink FAILED!\n");
        exit(EXIT_FAILURE);
    }

    print_time();
    printf("Done\n");

    return EXIT_SUCCESS;
}