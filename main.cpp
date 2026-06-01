#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>
#include <cstdlib>

constexpr int MIN_FLOOR = 1;
constexpr int MAX_FLOOR = 10;
constexpr int MOVE_DELAY_MS = 1000;

void clear_screen()
{
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

class Elevator;
void draw_shafts(const Elevator &e1, const Elevator &e2);

class Elevator
{
public:
    int current_floor;
    int target_floor;
    std::string name;

    Elevator(std::string n, int floor)
    {
        name = n;
        current_floor = floor;
        target_floor = 0;
    } // construct

    void display_floor() const
    {
        std::cout << "[" << name << "] Current floor: " << current_floor << "\n";
    } // display_floor

    void move(int current, int floor, const Elevator &e1, const Elevator &e2)
    {
        current_floor = current;
        target_floor = floor;

        if (current == floor)
        {
            target_floor = 0;
            clear_screen();
            draw_shafts(e1, e2);
            display_floor();
            return;
        } // if

        int direction = (floor > current) ? 1 : -1;
        int dist = std::abs(floor - current);

        std::this_thread::sleep_for(std::chrono::milliseconds(600));

        for (int f = current + direction;; f += direction)
        {
            current_floor = f;
            if (f == floor)
                target_floor = 0;

            clear_screen();
            draw_shafts(e1, e2);
            display_floor();

            if (f == floor)
                break;

            std::this_thread::sleep_for(std::chrono::milliseconds(MOVE_DELAY_MS));
        } // for

        std::cout << "[" << name << "] Arrived at floor " << floor << "\n\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
    } // move
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

int get_int(const std::string &prompt)
{
    int val;
    while (true)
    {
        std::cout << prompt;
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);
        iss >> val;
        if (val == 0)
            return 0;
        if (val >= MIN_FLOOR && val <= MAX_FLOOR)
            return val;
        std::cout << "  Please enter a number between "
                  << MIN_FLOOR << " and " << MAX_FLOOR << ".\n";
    } // while
} // get_int

void run()
{
    Elevator elevator1("Elevator1", 1);
    Elevator elevator2("Elevator2", 1);

    while (true)
    {
        clear_screen();
        draw_shafts(elevator1, elevator2);

        std::cout << "Enter 0 to exit.\n";
        int from = get_int("  Your current floor (0-10): ");

        if (from == 0)
        {
            std::cout << "\nFinish!\n\n";
            break;
        } // if

        int dist1 = std::abs(elevator1.current_floor - from);
        int dist2 = std::abs(elevator2.current_floor - from);

        Elevator &elev = (dist2 < dist1) ? elevator2 : elevator1;

        std::this_thread::sleep_for(std::chrono::milliseconds(800));

        elev.move(elev.current_floor, from, elevator1, elevator2);

        int to = get_int("  Destination floor (1-10): ");
        std::cout << "\n";

        elev.move(from, to, elevator1, elevator2);
    } // while
} // run

int main()
{
    run();
    return 0;
} // main