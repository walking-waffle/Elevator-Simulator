#include "elevator.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <limits>

static const int TOTAL_FLOORS = 10;
static const int MOVE_DELAY_MS = 1000;

// Render both elevators on a console-based building view.
void render_elevator(Elevator &e1, Elevator &e2)
{
    // Clear Screen (ANSI escape sequence)
    std::cout << "\033[2J\033[H";

    std::cout << "  +------------+------------+\n";
    std::cout << "  | Elevator 1 | Elevator 2 |\n";
    std::cout << "  +------------+------------+\n";

    for (int f = TOTAL_FLOORS; f >= 1; --f)
    {
        std::string slot1 = "        ";
        std::string slot2 = "        ";

        if (e1.current_floor == f)
            slot1 = "  [=]   ";
        if (e2.current_floor == f)
            slot2 = "  [=]   ";

        std::string fnum = std::to_string(f);
        if (f < 10)
            fnum = " " + fnum;

        std::cout << "  |  " << fnum << slot1 << "|  " << fnum << slot2 << "|\n";
    } // for

    std::cout << "  +------------+------------+\n";
    std::cout << "----------------------------------\n";
    e1.display_floor();
    e2.display_floor();
    std::cout << "----------------------------------\n";
} // render_elevator

// Simulate elevator movement to target floor and update display.
void animate_move(Elevator &elevator, Elevator &e1, Elevator &e2, int target)
{
    if (elevator.current_floor == target)
    {
        render_elevator(e1, e2);
        return;
    } // if

    int direction = (target > elevator.current_floor) ? 1 : -1;
    elevator.status = (direction == 1) ? "Going UP" : "Going DOWN";

    while (elevator.current_floor != target)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(MOVE_DELAY_MS));
        elevator.current_floor += direction;
        render_elevator(e1, e2);
    } // while

    elevator.status = "";
} // animate_move

// input within [lo, hi], enters 0 → program exits
// If input is non-numeric, input buffer is cleared
int read_int(const std::string &prompt, int lo, int hi)
{
    int val;

    while (true)
    {
        std::cout << prompt << " (or 0 to exit): ";

        if (std::cin >> val)
        {
            if (val == 0)
            {
                std::cout << "\nFinish!\n";
                std::exit(0);
            } // if

            if (val >= lo && val <= hi)
                return val;
        } // if
        else
        {
            // clear invalid input
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } // else

        std::cout << "  Please enter a valid NUMBER between "
                  << lo << " and " << hi << ", or 0 to exit.\n";
    } // while
} // read_int

int main()
{
    Elevator elevator1("Elevator 1", 1);
    Elevator elevator2("Elevator 2", 1);

    render_elevator(elevator1, elevator2);

    while (true)
    {
        int from = read_int("Enter your current floor (1-10)", 1, TOTAL_FLOORS);

        // selects nearest elevator
        int dist1 = std::abs(elevator1.current_floor - from);
        int dist2 = std::abs(elevator2.current_floor - from);
        Elevator &selected = (dist1 <= dist2) ? elevator1 : elevator2;
        std::cout << selected.name << " is coming.\n";

        animate_move(selected, elevator1, elevator2, from);

        int dest = read_int("Enter destination floor  (1-10)", 1, TOTAL_FLOORS);

        animate_move(selected, elevator1, elevator2, dest);
        selected.move(selected.current_floor, dest);
        std::cout << "** Arrived at floor " << selected.current_floor << ". **\n";
    } // while

    return 0;
} // main