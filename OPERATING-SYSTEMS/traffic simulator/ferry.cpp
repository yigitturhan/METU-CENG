#include "ferry.hpp"
#include "WriteOutput.h"
#include "helper.h"
void Ferry::pass(int carId, int id) {
    pthread_mutex_lock(&mut);
    WriteOutput(carId,'F',id,ARRIVE);
    carsWaiting++;
    if (carsWaiting >= capacity){
        WriteOutput(carId, 'F',id,START_PASSING );
        pthread_cond_broadcast(&readyToDepart.cond);
        carsWaiting--;
        pthread_mutex_unlock(&mut);
    }
    else{
        timespec t = get_timespec(maximum_wait_time);
        int res = pthread_cond_timedwait(&readyToDepart.cond, &mut, &t);
        if(res != 0) pthread_cond_broadcast(&readyToDepart.cond);
        WriteOutput(carId, 'F',id,START_PASSING );
        carsWaiting--;
        pthread_mutex_unlock(&mut);
    }
}

