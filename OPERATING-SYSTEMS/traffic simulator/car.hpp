#ifndef OS_HW2_CAR_HPP
#define OS_HW2_CAR_HPP
#include <vector>
#include "path.hpp"
class Car{
public:
    int travel_time;
    int path_length;
    std::vector<Path> path_list;
};
#endif //OS_HW2_CAR_HPP
