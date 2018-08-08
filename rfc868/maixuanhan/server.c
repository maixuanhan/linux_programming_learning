#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"

#define BACKLOG 50
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)

const char* GetAddressStr(struct sockaddr* addr, socklen_t addrlen)
{
    static char addrStr[ADDRSTRLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    if (getnameinfo(addr, addrlen, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        snprintf(addrStr, ADDRSTRLEN, "(%s, %s)", host, service);
    }
    else
    {
        snprintf(addrStr, ADDRSTRLEN, "(?UNKNOWN?)");
    }

    return addrStr;
}

int BindSocket(RunningMode_t mode)
{
    int fd;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC; /* Allows Ipv4 or IPv6 */
    hints.ai_socktype = mode == TCP_MODE ? SOCK_STREAM : SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV; /* Wildcard IP address; service name is numeric */

    if (getaddrinfo(NULL, PORT_NUM, &hints, &result) != 0)
    {
        perror("getaddrinfo FAILED");
        return -1;
    }

    /* Walk through returned list until we find an address structure that can be used to successfully connect a socket */
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (fd == -1) continue; /* On error, try next address */

        if (mode == TCP_MODE)
        {
            int optval = 1;
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
            {
                perror("setsockopt FAILED");
                close(fd);
                continue;
            }
        }

        if (bind(fd, rp->ai_addr, rp->ai_addrlen) == 0) break; /* Success */

        /* bind failed: close this socket and try next address */
        close(fd);
    }

    if (rp == NULL)
    {
        fprintf(stderr, "Could not bind any address!\n");
        return -1;
    }

    printf("SERVER started on %s\n", GetAddressStr(rp->ai_addr, rp->ai_addrlen));

    freeaddrinfo(result);

    return fd;
}

int main()
{
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        fprintf(stderr, "signal() : Ignore SIGPIPE failed!\n");
        exit(EXIT_FAILURE);
    }

    int ufd = BindSocket(UDP_MODE);
    if (ufd == -1)
    {
        fprintf(stderr, "Could not bind UDP port!\n");
        exit(EXIT_FAILURE);
    }

    int tfd = BindSocket(TCP_MODE);
    if (tfd == -1)
    {
        fprintf(stderr, "Could not bind TCP port!\n");
        exit(EXIT_FAILURE);
    }

    if (listen(tfd, BACKLOG) == -1)
    {
        perror("listen FAILED");
        exit(EXIT_FAILURE);
    }

    fd_set readfds;
    int nfds;

    struct sockaddr claddr;
    socklen_t len = sizeof(struct sockaddr);

    while (1)
    {
        nfds = 0;
        FD_ZERO(&readfds);

        FD_SET(ufd, &readfds);
        if (ufd >= nfds) nfds = ufd + 1;

        FD_SET(tfd, &readfds);
        if (tfd >= nfds) nfds = tfd + 1;

        if (select(nfds, &readfds, NULL, NULL, NULL) == -1)
        {
            perror("select FAILED");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(ufd, &readfds))
        {
            /* UDP handle */
            if (recvfrom(ufd, NULL, DATA_SIZE, 0, (struct sockaddr*)&claddr, &len) == 0)
            {
                /* Response to empty packet only */
                printf("UDP: RECEIVE request from %s\n", GetAddressStr(&claddr, len));

                time_t current_time = time(NULL);
                int data = htonl(FromPosixTime((int)current_time));
                if (sendto(ufd, &data, DATA_SIZE, 0, (struct sockaddr*)&claddr, len) == -1)
                {
                    perror("sendto FAILED");
                    continue; /* ignore error */
                }

                printf("     Local time is sent : %s", LocalTimeToStr(current_time));
            }
        }

        if (FD_ISSET(tfd, &readfds))
        {
            /* TCP handle */
            int fd = accept(tfd, (struct sockaddr*)&claddr, &len);
            if (fd == -1)
            {
                perror("accept FAILED");
                continue; /* ignore error */
            }

            printf("TCP: RECEIVE request from %s\n", GetAddressStr(&claddr, len));

            time_t current_time = time(NULL);
            int data = htonl(FromPosixTime((int)current_time));
            if (write(fd, &data, DATA_SIZE) == -1)
            {
                perror("write FAILED");
            }
            else
            {
                printf("     Local time is sent : %s", LocalTimeToStr(current_time));
            }

            close(fd);
        }
    }

    close(tfd);
    close(ufd);
    exit(EXIT_SUCCESS);
}
