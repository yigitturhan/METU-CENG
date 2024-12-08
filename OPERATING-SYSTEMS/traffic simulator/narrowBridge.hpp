#ifndef OS_HW2_NARROWBRIDGE_HPP
#define OS_HW2_NARROWBRIDGE_HPP

#include "monitor.h"
#include <vector>

class NarrowBridge : public Monitor {
public:
    long maximum_wait_time;
    int currentDirection, carsPassing0, carsPassing1, travel_time;
    std::vector<std::vector<int> > carsWaiting;
    std::vector<bool> maximum_wait_limit_reached;
    Condition c1, c2;
    NarrowBridge() : c1(this), c2(this) {
        currentDirection = 2, carsPassing0 = 0, carsPassing1 = 0;
        for (int i  = 0; i < 2; i++){
            maximum_wait_limit_reached.push_back(false);
            std::vector<int> v1;
            carsWaiting.push_back(v1);
        }

    }

    void pass(int direction, int id, int carId);
    void exit(int direction, int id, int carId);
    void waitForOppositeDirectionToPass(int direction, int carId);
    void notifyTheNextWaitingCar(int direction);
    void waitForDirectionChange(int direction, int carId);
    void notifyTheCarsOnTheOppositeDirection(int direction);
    void waitInTheLine(int direction);
    void updateCarpassing(int direction, bool increase);
};

#endif // OS_HW2_NARROWBRIDGE_HPP