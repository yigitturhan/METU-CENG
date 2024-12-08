#include "helper.h"
void sleep_milli(int milliseconds) {
    long seconds = milliseconds/1000;
    long microseconds = (milliseconds%1000)*1000;
    if ( seconds > 0 ) sleep(seconds);
    if ( microseconds > 0 ) usleep(microseconds);
}
struct timespec get_timespec(long maximum_wait_time){
    struct timespec time_spec;
    clock_gettime(CLOCK_REALTIME, &time_spec);
    long additional_nanos = 1000000 * maximum_wait_time;
    time_spec.tv_sec += additional_nanos / 1000000000;
    time_spec.tv_nsec += additional_nanos % 1000000000;
    if (time_spec.tv_nsec >= 1000000000) {
        time_spec.tv_sec++;
        time_spec.tv_nsec -= 1000000000;
    }
    return time_spec;
}