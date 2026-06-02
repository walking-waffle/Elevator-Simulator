#include "elevator.h"

#include <iostream>
#include <thread>
#include <chrono>

static const int MOVE_DELAY_MS = 1000;

Elevator::Elevator(const std::string &n, int start_floor)
    : current_floor(start_floor), name(n), status("")
{
}
void Elevator::display_floor()
{
    std::cout << "[" << name << "] Current floor: " << current_floor;
    if (!status.empty())
        std::cout << " [" << status << "]";
    std::cout << "\n";
} // display_floor

void Elevator::move(int current, int floor)
{
    current_floor = current;
    if (current == floor)
        return;
    int direction = (floor > current) ? 1 : -1;
    while (current_floor != floor)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(MOVE_DELAY_MS));
        current_floor += direction;
        display_floor();
    } // while
}