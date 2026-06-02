#include "elevator.h"

#include <iostream>
#include <cmath>
#include <string>

void assert_eq(int a, int b, const std::string &test_name)
{
    if (a == b)
        std::cout << "[PASS] " << test_name << "\n";
    else
        std::cout << "[FAIL] " << test_name << " expected "
                  << b << " got " << a << "\n";
}

void assert_eq_str(const std::string &a, const std::string &b, const std::string &test_name)
{
    if (a == b)
        std::cout << "[PASS] " << test_name << "\n";
    else
        std::cout << "[FAIL] " << test_name << "\n";
}

// ===== tests =====
void test_constructor()
{
    Elevator e("E1", 3);

    assert_eq(e.current_floor, 3, "constructor floor");
    assert_eq_str(e.name, "E1", "constructor name");
}

void test_E1_move_up()
{
    Elevator e("E1", 1);
    e.move(1, 5);

    assert_eq(e.current_floor, 5, "move up");
}

void test_E1_move_down()
{
    Elevator e("E1", 8);
    e.move(8, 2);

    assert_eq(e.current_floor, 2, "move down");
}

void test_E2_move_up()
{
    Elevator e("E2", 1);
    e.move(6, 10);

    assert_eq(e.current_floor, 10, "move up");
}

void test_E2_move_down()
{
    Elevator e("E2", 10);
    e.move(8, 6);

    assert_eq(e.current_floor, 6, "move down");
}

int main()
{
    std::cout << "Running tests...\n\n";

    test_constructor();
    test_E1_move_up();
    test_E1_move_down();
    test_E2_move_up();
    test_E2_move_down();

    return 0;
}