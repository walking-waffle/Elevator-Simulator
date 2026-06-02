#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <string>

class Elevator
{
public:
    int current_floor;
    std::string name;
    std::string status;

    Elevator(const std::string &n, int start_floor = 1);

    void display_floor();
    void move(int current, int floor);
};

#endif