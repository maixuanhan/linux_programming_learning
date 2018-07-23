#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ctype.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "common.h"

static volatile int is_not_ready;

int main(int argc, char** argv)
{
    int buffer[ELEM_COUNT];
    print_time();
    printf("SENDER starts.\n");

    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
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

    /* Send data */
    for (int k = 0; k < NUMBER_OF_TIMES; ++k)
    {
        for (int i = 0; i < ELEM_COUNT; ++i)
        {
            buffer[i] = i * k;
        }

        if (sendto(sfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&address, sizeof(struct sockaddr_un)) == -1)
        {
            fprintf(stderr, "sendto FAILED!\n");
            fprintf(stderr, "Details: %s\n", strerror(errno));
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
