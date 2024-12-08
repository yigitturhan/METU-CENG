#include "WriteOutput.h"
#include "narrowBridge.hpp"
#include "helper.h"
#include <iostream>
#include <ctime>
void NarrowBridge::pass(int direction, int id, int carId) {
    start:
    pthread_mutex_lock(&mut);
    if(direction == currentDirection){
        if((direction == 0 && carsPassing1 > 0) || (direction == 1 &&  carsPassing0 > 0)){
            waitForOppositeDirectionToPass(direction, carId);
            pthread_mutex_unlock(&mut);
            goto start;
        }
        else if (carsWaiting[direction].empty() || carsWaiting[direction].front() == carId){
            if(!carsWaiting[direction].empty()) carsWaiting[direction].erase(carsWaiting[direction].begin());
            
            if(carsPassing0 != 0 && direction == 0) sleep_milli(PASS_DELAY);
            if(carsPassing1 != 0 && direction == 1) sleep_milli(PASS_DELAY);
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
        notifyTheCarsOnTheOppositeDirection(1-direction);
        pthread_mutex_unlock(&mut);
        goto start;
    }
    else if (carsPassing0 + carsPassing1 == 0){
        currentDirection = direction;
        notifyTheCarsOnTheOppositeDirection(1-direction);
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
void NarrowBridge::exit(int direction, int id, int carId){
    updateCarpassing(direction,false);
    WriteOutput(carId,'N',id,FINISH_PASSING);
    if(direction == 0 && carsPassing0 == 0) notifyTheCarsOnTheOppositeDirection(direction);
    if(direction == 1 && carsPassing1 == 0)  notifyTheCarsOnTheOppositeDirection(direction);
}

void NarrowBridge::waitForOppositeDirectionToPass(int direction, int carId){
    carsWaiting[direction].push_back(carId);
    if (direction == 0) pthread_cond_wait(&c1.cond,&mut);
    else pthread_cond_wait(&c2.cond,&mut);
    carsWaiting[direction].pop_back();
}
void NarrowBridge::waitInTheLine(int direction){
    if(direction == 0){
        pthread_cond_signal(&c1.cond);
        pthread_cond_wait(&c1.cond,&mut);}
    else{
        pthread_cond_signal(&c2.cond);
        pthread_cond_wait(&c2.cond,&mut);} 
}
void NarrowBridge::notifyTheNextWaitingCar(int direction){
    if(direction == 0) pthread_cond_signal(&c1.cond);
    else pthread_cond_signal(&c2.cond);
}
void NarrowBridge::waitForDirectionChange(int direction, int carId){
    struct timespec t = get_timespec(maximum_wait_time);
    int res;
    if(direction == 0) res = pthread_cond_timedwait(&c1.cond,&mut,&t);
    else res = pthread_cond_timedwait(&c2.cond,&mut,&t);
    if (res != 0) maximum_wait_limit_reached[direction] = true;
}
void NarrowBridge::notifyTheCarsOnTheOppositeDirection(int direction){
    if(direction == 0) pthread_cond_signal(&c2.cond);
    else pthread_cond_signal(&c1.cond); 
}
void NarrowBridge::updateCarpassing(int direction, bool increase){
    if(direction == 0 && increase) carsPassing0++;
    else if (direction == 0) carsPassing0--;
    else if(increase) carsPassing1++;
    else carsPassing1--;
}

