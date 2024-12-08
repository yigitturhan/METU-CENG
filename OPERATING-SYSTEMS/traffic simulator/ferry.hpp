#ifndef OS_HW2_FERRY_HPP
#define OS_HW2_FERRY_HPP

#include "monitor.h"
#include <queue>

class Ferry : public Monitor {
public:
    int travel_time, maximum_wait_time, capacity, carsWaiting;
    Condition readyToDepart;
    Ferry() : readyToDepart(this) {
        carsWaiting = 0;
    }

    void pass(int carId, int id);
};

#endif // OS_HW2_FERRY_HPP