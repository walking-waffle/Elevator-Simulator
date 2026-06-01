#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>
#include <cstdlib>

const int MIN_FLOOR = 1;
const int MAX_FLOOR = 10;
const int MOVE_DELAY_MS = 1000;

void clear_screen()
{
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
} // clear_screen

class ElevatorSystem;
class Elevator;
void draw_shafts(const Elevator &e1, const Elevator &e2);

class Elevator
{
public:
    int current_floor;
    int target_floor;
    std::string name;
    ElevatorSystem *system;

    Elevator(std::string n, int floor, ElevatorSystem *sys)
    {
        name = n;
        current_floor = floor;
        target_floor = 0;
        system = sys;
    } // construct

    void display_floor() const
    {
        std::cout << "[" << name << "] Current floor: " << current_floor << "\n";
    } // display_floor

    void move(int current, int floor);
};

std::string cell(const Elevator &e, int f)
{
    bool is_cur = (f == e.current_floor);
    bool is_tgt = (e.target_floor != 0 && f == e.target_floor);
    std::ostringstream ss;
    ss << "  " << (f < 10 ? " " : "") << f;
    if (is_cur && is_tgt)
        ss << "  [=]*  ";
    else if (is_cur)
        ss << "  [=]   ";
    else if (is_tgt)
        ss << "   *    ";
    else
        ss << "        ";
    return ss.str();
} // cell

void draw_shafts(const Elevator &e1, const Elevator &e2)
{
    std::cout << "\n";
    std::cout << "  +------------+------------+\n";
    std::cout << "  | Elevator 1 | Elevator 2 |\n";
    std::cout << "  +------------+------------+\n";

    for (int f = MAX_FLOOR; f >= MIN_FLOOR; --f)
        std::cout << "  |" << cell(e1, f) << "|" << cell(e2, f) << "|\n";

    std::cout << "  +------------+------------+\n";
} // draw_shafts

int get_int(const std::string &prompt, bool allow_zero = false)
{
    int val;

    while (true)
    {
        std::cout << prompt;
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);
        if (!(iss >> val))
        {
            std::cout << "Please enter a valid number.\n";
            continue;
        }
        if (allow_zero && val == 0)
            return 0;
        if (val >= MIN_FLOOR && val <= MAX_FLOOR)
            return val;
        std::cout << "Please enter a number between "
                  << MIN_FLOOR << " and "
                  << MAX_FLOOR << ".\n";
    } // while
}

class ElevatorSystem
{
    Elevator elevator1;
    Elevator elevator2;

public:
    ElevatorSystem()
        : elevator1("Elevator1", 1, this), elevator2("Elevator2", 1, this) {}

    void draw() const
    {
        clear_screen();
        draw_shafts(elevator1, elevator2);
    } // draw

    void dispatch(int from)
    {
        int dist1 = std::abs(elevator1.current_floor - from);
        int dist2 = std::abs(elevator2.current_floor - from);
        Elevator &elev = (dist2 < dist1) ? elevator2 : elevator1;

        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        elev.move(elev.current_floor, from);

        int to = get_int("  Destination floor (1-10): ");
        std::cout << "\n";

        elev.move(from, to);
    } // dispatch

    void run()
    {
        while (true)
        {
            draw();

            std::cout << "Enter 0 to exit.\n";
            int from = get_int("Your current floor (0-10): ", true);

            if (from == 0)
            {
                std::cout << "\nFinish!\n\n";
                break;
            } // if

            dispatch(from);
        } // while
    } // run
};

void Elevator::move(int current, int floor)
{
    current_floor = current;
    target_floor = floor;

    if (current == floor)
    {
        target_floor = 0;
        system->draw();
        display_floor();
        return;
    } // if

    int direction = (floor > current) ? 1 : -1;

    std::this_thread::sleep_for(std::chrono::milliseconds(600));

    for (int f = current + direction;; f += direction)
    {
        current_floor = f;
        if (f == floor)
            target_floor = 0;

        system->draw();
        display_floor();

        if (f == floor)
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(MOVE_DELAY_MS));
    } // for

    std::cout << "[" << name << "] Arrived at floor " << floor << "\n\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
} // move

int main()
{
    ElevatorSystem system;
    system.run();
    return 0;
} // main