#ifndef HOMEWORK2_HELPER_H
#define HOMEWORK2_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <time.h>
    #include <unistd.h>

    #define PASS_DELAY 10

    void sleep_milli(int milliseconds);

    struct timespec get_timespec(long maximum_wait_time);

#ifdef __cplusplus
};
#endif

#endif //HOMEWORK2_HELPER_H
