#include "crossRoad.hpp"
#include "helper.h"
#include "WriteOutput.h"

void CrossRoad::pass(int direction, int id, int carId){
    start:
    pthread_mutex_lock(&mut);
    if(direction == currentDirection){
        if(check(direction)){
            waitForOtherDirectionToPass(direction, carId);
            pthread_mutex_unlock(&mut);
            goto start;
        }
        else if (carsWaiting[direction].empty() || carsWaiting[direction].front() == carId){
            if(!carsWaiting[direction].empty()) carsWaiting[direction].erase(carsWaiting[direction].begin());
            if(shouldSleep(direction)) sleep_milli(PASS_DELAY);
            updateCarpassing(direction,true);
            WriteOutput(carId,'N',id,START_PASSING);
            notifyTheNextWaitingCar(direction);
            pthread_mutex_unlock(&mut);
        }
        else{
            carsWaiting[direction].push_back(carId);
            waitInTheLine(direction);
            if(!carsWaiting[direction].empty()) carsWaiting[direction].pop_back();
            pthread_mutex_unlock(&mut);
            goto start;
        }
    }
    else if(maximum_wait_limit_reached[direction]){
        maximum_wait_limit_reached[direction] = false;
        currentDirection = direction;
        notifyTheCarsOnTheNextDirection(direction);
        pthread_mutex_unlock(&mut);
        goto start;
    }
    else if (carsPassing0 + carsPassing1 + carsPassing2 + carsPassing3 == 0){
        currentDirection = direction;
        notifyTheCarsOnTheNextDirection(direction);
        pthread_mutex_unlock(&mut);
        goto start;
    }
    else{
        carsWaiting[direction].push_back(carId);
        waitForDirectionChange(direction, carId);
        if(!carsWaiting[direction].empty()) carsWaiting[direction].pop_back();
        currentDirection = direction;
        pthread_mutex_unlock(&mut);
        goto start;
    }
}
void CrossRoad::exit(int direction, int id, int carId){
    updateCarpassing(direction,false);
    WriteOutput(carId,'N',id,FINISH_PASSING);
    if(direction == 0 && carsPassing0 == 0) notifyTheCarsOnTheNextDirection(direction);
    if(direction == 1 && carsPassing1 == 0)  notifyTheCarsOnTheNextDirection(direction);
    if(direction == 2 && carsPassing2 == 0)  notifyTheCarsOnTheNextDirection(direction);
    if(direction == 3 && carsPassing3 == 0)  notifyTheCarsOnTheNextDirection(direction);
}
void CrossRoad::waitForDirectionChange(int direction, int carId){
    struct timespec t = get_timespec(maximum_wait_time);
    int res;
    switch(direction){
    case 0:
        res = pthread_cond_timedwait(&c1.cond,&mut,&t);
        break;
    case 1:
        res = pthread_cond_timedwait(&c2.cond,&mut,&t);
        break;
    case 2:
        res = pthread_cond_timedwait(&c3.cond,&mut,&t);
        break;
    case 3:
        res = pthread_cond_timedwait(&c4.cond,&mut,&t);
        break;            
    }
    if (res != 0) maximum_wait_limit_reached[direction] = true;
}
void CrossRoad::notifyTheCarsOnTheNextDirection(int direction){
    switch(direction){
    case 0:
        if(!carsWaiting[1].empty()) c2.notify();
        else if (!carsWaiting[2].empty()) c3.notify();
        else c4.notify();
        return;
    case 1:
        if(!carsWaiting[2].empty()) c3.notify();
        else if (!carsWaiting[3].empty()) c4.notify();
        else c1.notify();
        return;
    case 2:
        if(!carsWaiting[3].empty()) c4.notify();
        else if (!carsWaiting[4].empty()) c1.notify();
        else c2.notify();
        return;
    case 3:
        if(!carsWaiting[0].empty()) c1.notify();
        else if (!carsWaiting[1].empty()) c2.notify();
        else c3.notify();
        return;
    }
}

void CrossRoad::waitInTheLine(int direction){
    switch(direction){
    case 0:
        pthread_cond_signal(&c1.cond);
        pthread_cond_wait(&c1.cond,&mut);
        return;
    case 1:
        pthread_cond_signal(&c2.cond);
        pthread_cond_wait(&c2.cond,&mut);
        return;
    case 2:
        pthread_cond_signal(&c3.cond);
        pthread_cond_wait(&c3.cond,&mut);
        return;
    case 3:
        pthread_cond_signal(&c4.cond);
        pthread_cond_wait(&c4.cond,&mut);
        return;
    }
}
bool CrossRoad::check(int direction){
    return (((direction == 0) && (carsPassing1 + carsPassing2 + carsPassing3 > 0)) ||
    ((direction == 1) && (carsPassing0 + carsPassing2 + carsPassing3 > 0)) || 
    ((direction == 2) && (carsPassing1 + carsPassing3 + carsPassing0 > 0)) ||
    ((direction == 3) && (carsPassing1 + carsPassing2 + carsPassing0 > 0)));
}
void CrossRoad::updateCarpassing(int direction, bool increase){
    switch(direction){
    case 0:
        if(increase) carsPassing0++;
        else carsPassing0--;
        return;
    case 1:
        if(increase) carsPassing1++;
        else carsPassing1--;
        return;
    case 2:
        if(increase) carsPassing2++;
        else carsPassing2--;
        return;
    case 3:
        if (increase) carsPassing3++;
        else carsPassing3--;
        return;
    }
}
void CrossRoad::waitForOtherDirectionToPass(int direction, int carId){
    carsWaiting[direction].push_back(carId);
    switch(direction){
    case 0:
        pthread_cond_wait(&c1.cond,&mut);
        break;
    case 1:
        pthread_cond_wait(&c2.cond,&mut);
        break;
    case 2:
        pthread_cond_wait(&c3.cond,&mut);
        break;
    case 3:
        pthread_cond_wait(&c4.cond,&mut);
        break;
    }
    carsWaiting[direction].pop_back();
}
bool CrossRoad::shouldSleep(int direction){
    return ((direction == 0 && carsPassing0 > 0) || (direction == 1 && carsPassing1 > 0) ||
        (direction == 2 && carsPassing2 > 0) || (direction == 3 && carsPassing3 > 0));
}
void CrossRoad::notifyTheNextWaitingCar(int direction){
    switch(direction){
    case 0:
        pthread_cond_signal(&c1.cond);
        return;
    case 1:
        pthread_cond_signal(&c2.cond);
        return;
    case 2:
        pthread_cond_signal(&c3.cond);
        return;
    case 3:
        pthread_cond_signal(&c4.cond);
        return;
    }
}
