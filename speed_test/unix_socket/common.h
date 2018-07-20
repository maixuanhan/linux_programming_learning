/* common.h */
/* Han Mai */

#ifndef SPEED_TEST_COMMON_H
#define SPEED_TEST_COMMON_H

#include <stdio.h>
#include <time.h>

#define NUMBER_OF_TIMES 200
#define ELEM_COUNT 1000000
#define BUFFER_SIZE (sizeof(int) * ELEM_COUNT)
#define SOCKET_NAME "/tmp/un_sock_test"

static struct timespec g_last_time;

void print_time()
{
    struct timespec ts;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);

    struct timespec d;
    if (ts.tv_nsec >= g_last_time.tv_nsec)
    {
        d.tv_nsec = ts.tv_nsec - g_last_time.tv_nsec;
        d.tv_sec = ts.tv_sec - g_last_time.tv_sec;
    }
    else
    {
        d.tv_nsec = ts.tv_nsec + 1000000000 - g_last_time.tv_nsec;
        d.tv_sec = ts.tv_sec - g_last_time.tv_sec - 1;
    }

    printf("\033[32m[Time: %lld.%.9ld]", (long long)ts.tv_sec, ts.tv_nsec);     // time point
    printf("[Duration: %lld.%.9ld]\033[0m ", (long long)d.tv_sec, d.tv_nsec);   // duration

    g_last_time = ts;
}

#endif /* SPEED_TEST_COMMON_H */
