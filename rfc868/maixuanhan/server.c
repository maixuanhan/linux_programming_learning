#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"

int main()
{
    int fd;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV; /* Wildcard IP address; service name is numeric */

    if (getaddrinfo(NULL, PORT_NUM, &hints, &result) != 0)
    {
        perror("getaddrinfo FAILED");
        exit(EXIT_FAILURE);
    }

    /* Walk through returned list until we find an address structure that can be used to successfully connect a socket */
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (fd == -1)
            continue; /* On error, try next address */

        if (bind(fd, rp->ai_addr, rp->ai_addrlen) == 0)
            break; /* Success */

        /* bind failed: close this socket and try next address */
        close(fd);
    }

    if (rp == NULL)
    {
        fprintf(stderr, "Could not bind any address!\n");
        exit(EXIT_FAILURE);
    }

    printf("TIME SERVER START, waiting for time request...\n");

    freeaddrinfo(result);

    struct sockaddr claddr;
    socklen_t len = sizeof(struct sockaddr);
    char buf[DATA_SIZE];

    while (1)
    {
        if (recvfrom(fd, buf, DATA_SIZE, 0, (struct sockaddr *)&claddr, &len) == 0)
        {
            /* Response to empty packet only */
            time_t current_time = time(NULL);
            int data = htonl(FromPosixTime((int)current_time));
            if (sendto(fd, &data, DATA_SIZE, 0, (struct sockaddr *)&claddr, len) == -1)
            {
                perror("sendto FAILED");
                exit(EXIT_FAILURE);
            }

            printf("Local time is sent : %s", LocalTimeToStr(current_time));
        }
    }

    close(fd);
    exit(EXIT_SUCCESS);
}
