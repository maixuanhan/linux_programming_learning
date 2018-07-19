#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#include <sys/types.h>
#include <signal.h>

#include "common.h"

#define SHARED_OBJ_NAME "test_speed_shared_mem"
#define SHARED_OBJ_SIZE (sizeof(int) * ELEM_COUNT)

int main(int argc, char** argv)
{
    pid_t pid = 0;
    if (argc < 2)
    {
        fprintf(stderr, "Missing receiver's PID.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        pid = (pid_t)atoi(argv[1]);
    }

    print_time();
    printf("Create share mem object and write data\n");

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

    /* Write data */
    for (int i = 0; i < ELEM_COUNT; ++i)
    {
        ((int*)address)[i] = i;
    }

    /* give signal to the receiver */
    if (kill(pid, SIGUSR1) == -1)
    {
        fprintf(stderr, "kill FAILED!\n");
        exit(EXIT_FAILURE);
    }

    print_time();
    printf("Done\n");

    return EXIT_SUCCESS;
}