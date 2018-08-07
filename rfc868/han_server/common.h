/* common.h */
/* Han Mai */

#ifndef TIME_PROTO_UTILS_FUNCS
#define TIME_PROTO_UTILS_FUNCS

#include <string.h>
#include <time.h>

#define PORT_NUM "37"

const int JAN_1_1970 = 2208988800;
const int DATA_SIZE = 4;

time_t ToPosixTime(int t)
{
    return (time_t)(t - JAN_1_1970);
}

int FromPosixTime(time_t t)
{
    return (int)(t + JAN_1_1970);
}

char *LocalTimeToStr(time_t t)
{
    struct tm* timeinfo;
    timeinfo = localtime(&t);
    return asctime(timeinfo);
}

typedef enum
{
    UDP_MODE,
    TCP_MODE
} RunningMode_t;

RunningMode_t GetRunningMode(const char* m)
{
    if (strncmp(m, "udp", 3) == 0)
    {
        return UDP_MODE;
    }

    return TCP_MODE;
}

#endif /* TIME_PROTO_UTILS_FUNCS */
