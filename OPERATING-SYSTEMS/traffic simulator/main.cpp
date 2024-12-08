#include "narrowBridge.hpp"
#include "ferry.hpp"
#include "crossRoad.hpp"
#include "car.hpp"
#include "helper.h"
#include <vector>
#include <pthread.h>
#include "WriteOutput.h"
#include <iostream>
std::vector<NarrowBridge> narrow_bridges;
std::vector<std::vector<Ferry> > ferries;
std::vector<CrossRoad> crossroads;
std::vector<Car> cars;
void parser(){
    int number_of_narrow_bridge, number_of_ferry, number_of_crossroad, number_of_cars;
    std::cin >> number_of_narrow_bridge;
    for (int i = 0; i < number_of_narrow_bridge; i++){
        NarrowBridge *narrowBridge = new NarrowBridge();
        std::cin >> narrowBridge->travel_time >> narrowBridge->maximum_wait_time;
        narrow_bridges.push_back(*narrowBridge);
    }
    std::cin >> number_of_ferry;
    std::vector<Ferry> v1, v2;
    ferries.push_back(v1);
    ferries.push_back(v2);
    for (int i = 0; i < number_of_ferry; i++){
        Ferry *ferry = new Ferry();
        std::cin >> ferry->travel_time >> ferry->maximum_wait_time >> ferry->capacity;
        Ferry *ferry2 = new Ferry();
        ferry2->travel_time = ferry->travel_time;
        ferry2->maximum_wait_time = ferry->maximum_wait_time;
        ferry2->capacity = ferry->capacity;
        ferries[0].push_back(*ferry);
        ferries[1].push_back(*ferry2);
    }
    std::cin >> number_of_crossroad;
    for (int i = 0; i < number_of_crossroad; i++){
        CrossRoad *crossRoad = new CrossRoad();
        std::cin >> crossRoad->travel_time >> crossRoad->maximum_wait_time;
        crossroads.push_back(*crossRoad);
    }
    std::cin >> number_of_cars;
    for (int i = 0; i < number_of_cars; i++){
        Car car;
        std::cin >> car.travel_time >> car.path_length;
        for (int j = 0; j < car.path_length; j++){
            Path path;
            std::cin >> path.connector >> path.from >> path.to;
            car.path_list.push_back(path);
        }
        cars.push_back(car);
    }
}

void travelThroughNarrowBridge(const std::string& connector, int carIndex, int pathId) {
    int id = std::stoi(connector.substr(1));
    int direction = cars[carIndex].path_list[pathId].from;
    NarrowBridge& bridge = narrow_bridges[id];
    bridge.pass(direction, id,carIndex);
    sleep_milli(bridge.travel_time);
    bridge.exit(direction, id,carIndex);
}
void travelThroughFerry(const std::string& connector, int carIndex, int pathId) {
    int id = std::stoi(connector.substr(1));
    int direction = cars[carIndex].path_list[pathId].from;
    Ferry& ferry3 = ferries[direction][id];
    ferry3.pass(carIndex, id);
    sleep_milli(ferry3.travel_time);
    WriteOutput(carIndex, 'F', id, FINISH_PASSING);
}

void travelThroughCrossRoad(const std::string& connector, int carIndex, int pathId) {
    int id = std::stoi(connector.substr(1));
    int direction = cars[carIndex].path_list[pathId].from;
    CrossRoad& crossRoad = crossroads[id];
    crossRoad.pass(direction, id, carIndex);
    sleep_milli(crossRoad.travel_time);
    crossRoad.exit(direction, id ,carIndex);
}
void* carThreadFunction(void *p) {
    int carIndex = *((int*)p);
    Car& car = cars[carIndex];
    for (int i = 0; i < car.path_length; i++) {
        Path path = car.path_list[i];
        WriteOutput(carIndex, path.connector[0],std::stoi(path.connector.substr(1)),TRAVEL);
        sleep_milli(car.travel_time);
        switch (path.connector[0]) {
            case 'N':
                WriteOutput(carIndex, path.connector[0],std::stoi(path.connector.substr(1)),ARRIVE);
                travelThroughNarrowBridge(path.connector, carIndex, i);
                break;
            case 'F':
                travelThroughFerry(path.connector, carIndex, i);
                break;
            case 'C':
                WriteOutput(carIndex, path.connector[0],std::stoi(path.connector.substr(1)),ARRIVE);
                travelThroughCrossRoad(path.connector, carIndex, i);
                break;
        }
    }
    return nullptr;
}
int main() {
    parser();
    InitWriteOutput();
    std::vector<pthread_t> threads;
    threads.reserve(cars.size());
    std::vector<int> thread_args(cars.size());
    for (int i = 0; i < cars.size(); i++) {
        thread_args[i] = i;
        pthread_t thread;
        pthread_create(&thread, nullptr, carThreadFunction, (void*)&thread_args[i]);
        threads.push_back(thread);
    }
    for (auto & thread : threads) pthread_join(thread, nullptr);
    return 0;
}