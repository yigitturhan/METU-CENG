
#ifndef OS_HW2_CROSSROAD_HPP
#define OS_HW2_CROSSROAD_HPP

#include "monitor.h"
#include <vector>

class CrossRoad : public Monitor {
public:
    int travel_time;
    int maximum_wait_time;
    std::vector<bool> maximum_wait_limit_reached;
    int carsPassing0, carsPassing1, carsPassing2, carsPassing3;
    int currentDirection;
    Condition c1, c2, c3, c4;
    std::vector<std::vector<int> > carsWaiting;

    CrossRoad() : c1(this), c2(this), c3(this), c4(this){
        carsPassing0 = 0, carsPassing1 = 0, carsPassing2 = 0, carsPassing3 = 0;
        for (int i = 0; i < 4; i++){
            std::vector<int> v1;
            maximum_wait_limit_reached.push_back(false);
            carsWaiting.push_back(v1);
        }
    }

    void pass(int direction, int id, int carId);
    void exit(int direction, int carId, int id);
    void waitForDirectionChange(int direction, int carId);
    void notifyTheCarsOnTheNextDirection(int direction);
    void waitInTheLine(int direction);
    bool check(int direction);
    void updateCarpassing(int direction, bool increase);
    void waitForOtherDirectionToPass(int direction, int carId);
    bool shouldSleep(int direction);
    void notifyTheNextWaitingCar(int direction);

};

#endif // OS_HW2_CROSSROAD_HPP
