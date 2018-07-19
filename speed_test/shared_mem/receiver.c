#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#include <sys/types.h>      /* For pid_t */
#include <signal.h>         /* For kill() */


#include "common.h"

volatile sig_atomic_t write_done = 0;
static void sigusr_handler(int sig)
{
    write_done = 1;
}

int main(int argc, char** argv)
{
    pid_t pid = getpid();
    printf("Receiver is running with PID=%d\n", (int)pid);

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigusr_handler;
    if (sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        fprintf(stderr, "sigaction FAILED!\n");
        exit(EXIT_FAILURE);
    }

    while (!write_done); /* wait for signal from sender */

    print_time();
    printf("Receiving data...\n");

    /* Open shared mem object */
    int fd = shm_open(SHARED_OBJ_NAME, O_RDONLY, 0);
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
    void* address = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
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

    int sum = 0;
    int validated_sum = 0;
    for (int i = 0; i < ELEM_COUNT; ++i)
    {
        sum = (sum + ((int*)address)[i]);   // let the buffer overflow, we don't care
        validated_sum = (validated_sum + i);
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

    print_time();
    printf("Done\n");

    return EXIT_SUCCESS;
}