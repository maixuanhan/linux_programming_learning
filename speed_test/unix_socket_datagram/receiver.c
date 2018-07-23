#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ctype.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "common.h"

int main(int argc, char** argv)
{
    char buffer[BUFFER_SIZE];
    /* Create socket fd */
    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
    {
        fprintf(stderr, "socket FAILED!\n");
        exit(EXIT_FAILURE);
    }

    /* Try to remove existed sockket file (if it exists) */
    if (remove(SOCKET_NAME) == -1 && errno != ENOENT)
    {
        fprintf(stderr, "remove FAILED!\n");
        exit(EXIT_FAILURE);
    }

    /* Construct the sockaddr_un object */
    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_NAME, sizeof(address.sun_path) - 1);

    /* Bind the address to the socket fd */
    if (bind(sfd, (struct sockaddr*)&address, sizeof(address)) == -1)
    {
        fprintf(stderr, "bind FAILED!\n");
        exit(EXIT_FAILURE);
    }

    printf("RECEIVER starts. Waiting for data from SENDER...\n");

    int sum = 0;
    int validated_sum = 0;

    for (int k = 0; k < NUMBER_OF_TIMES; ++k)
    {
        for (int i = 0; i < ELEM_COUNT; ++i)
        {
            validated_sum = (validated_sum + i * k);
        }
    }

    /* Read data */
    int count;
    int is_first = 1;
    int c = 0;
    while (sum != validated_sum && (count = recvfrom(sfd, buffer, BUFFER_SIZE, 0, NULL, NULL)) > 0)
    {
        if (is_first)
        {
            is_first = 0;
            print_time();
            printf("Start receiving data\n");
        }

        if (count % sizeof(int) != 0)
        {
            printf("\033[31mTest result is not reliable\033[0m\n");
        }

        // printf("\033[31mCount = %d\033[0m\n", ++c);

        for (int i = 0; i < count / 4; ++i)
        {
            sum = (sum + ((int*)buffer)[i]); // let the buffer overflow, we don't care
        }
    }

    if (count == -1)
    {
        fprintf(stderr, "recvfrom FAILED!\n");
        exit(EXIT_FAILURE);
    }

    if (sum != validated_sum)
    {
        fprintf(stderr, "DATA is mismatched!\nSum:           %d\nValidated_sum: %d\n", sum, validated_sum);
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
