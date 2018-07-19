#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// RACE CONDITION on signal handling
#define MAX_ULL 0xffffffffffffffff
#define TERM_ULL 0x1010101010101010

// sig_atomic_t a = 0;
static volatile unsigned long long a = 0;

static void alarm_handler(int sig)
{
    /* reset */
    // printf("reset a\n");
    a = TERM_ULL;
}

int main()
{
    volatile unsigned long long b = 0;
    struct itimerval itv;
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = alarm_handler;
    if (sigaction(SIGALRM, &sa, NULL) == -1)
    {
        printf("sigaction failed!\n");
        exit(1);
    }

    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;
    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = 1; /* small interval to increase the chance of race condition */

    if (setitimer(ITIMER_REAL, &itv, 0) == -1)
    {
        printf("setitimer failed!\n");
        exit(1);
    }

    while (1)
    {
        a = MAX_ULL;

        b = a; /* if the SIGALRM comes while the variable is updating, its value will be messed up and the value is printed */

        if (b != TERM_ULL && b != MAX_ULL)
        {
            printf("b=%llx\n", b);
        }
    }

    return 0;
}
