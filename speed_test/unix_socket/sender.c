#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/un.h>
#include <sys/socket.h>

#include "common.h"

static volatile int is_not_ready;

int main(int argc, char** argv)
{
    int buffer[ELEM_COUNT];
    print_time();
    printf("SENDER starts.\n");

    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        fprintf(stderr, "socket FAILED!\n");
        exit(EXIT_FAILURE);
    }

    /* Construct the sockaddr_un object */
    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_NAME, sizeof(address.sun_path) - 1);

    /* Connect to the server */
    if (connect(sfd, (struct sockaddr*)&address, sizeof(address)) == -1)
    {
        fprintf(stderr, "connect FAILED!\n");
        exit(EXIT_FAILURE);
    }

    /* Send data */
    for (int k = 0; k < NUMBER_OF_TIMES; ++k)
    {
        for (int i = 0; i < ELEM_COUNT; ++i)
        {
            buffer[i] = i * k;
        }

        if (write(sfd, buffer, BUFFER_SIZE) == -1)
        {
            fprintf(stderr, "write FAILED!\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Close server socket */
    if (close(sfd) == -1)
    {
        fprintf(stderr, "close FAILED!\n");
        exit(EXIT_FAILURE);
    }

    print_time();
    printf("Done\n");

    return EXIT_SUCCESS;
}
