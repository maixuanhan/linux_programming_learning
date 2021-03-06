#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"

int main(int argc, char **argv)
{
    RunningMode_t mode = UDP_MODE;
    if (argc < 2)
    {
        fprintf(stderr, "Missing address!\n");
        exit(EXIT_FAILURE);
    }
    else if (argc > 2)
    {
        mode = GetRunningMode(argv[2]);
    }

    int fd;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    /* Call getaddrinfo() to obtain a list of addresses that we can try connecting to */

    memset(&hints, 0, sizeof(hints));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC; /* Allows Ipv4 or IPv6 */
    hints.ai_socktype = mode == TCP_MODE ? SOCK_STREAM : SOCK_DGRAM;
    hints.ai_flags = AI_NUMERICSERV;

    if (getaddrinfo(argv[1], PORT_NUM, &hints, &result) != 0)
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

        if (mode == TCP_MODE)
        {
            if (connect(fd, rp->ai_addr, rp->ai_addrlen) != -1)
                break; /* Success */

            /* Connect failed: close this socket and try next address */
            close(fd);
        }
        else
        {
            break;
        }
    }

    if (rp == NULL)
    {
        fprintf(stderr, "Could not connect to the server!\n");
        exit(EXIT_FAILURE);
    }

    char buf[DATA_SIZE];

    if (mode == TCP_MODE)
    {
        freeaddrinfo(result);

        if (read(fd, buf, sizeof(buf)) == -1)
        {
            perror("read FAILED");
            exit(EXIT_FAILURE); /* Automatically close fd */
        }
    }
    else
    {
        if (sendto(fd, NULL, 0, 0, (struct sockaddr *)rp->ai_addr, rp->ai_addrlen) != 0)
        {
            perror("sendto FAILED");
            exit(EXIT_FAILURE);
        }

        freeaddrinfo(result);

        if (recvfrom(fd, buf, DATA_SIZE, 0, NULL, NULL) != DATA_SIZE)
        {
            perror("recvfrom FAILED");
            exit(EXIT_FAILURE);
        }
    }

    if (close(fd) == -1)
    {
        perror("close FAILED");
        exit(EXIT_FAILURE);
    }

    // printf("RECEIVED BUF: ");
    // for (int i = 0; i < sizeof(buf); ++i)
    // {
    //     printf("%.2x ", (unsigned char)buf[i]);
    // }
    // printf("\n");

    int *p = (int *)buf;
    time_t rawtime = ToPosixTime(ntohl(*p));
    printf("Received time is: %s\n", LocalTimeToStr(rawtime));

    return EXIT_SUCCESS;
}
