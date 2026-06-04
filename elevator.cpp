#include "elevator.h"
#include <iostream>
#include <chrono>

static const int MOVE_DELAY_MS = 1000;

Elevator::Elevator(const std::string &n, int start_floor)
    : current_floor(start_floor), name(n), status(""), stop_flag(false)
{
    // 電梯物件出來後，將電梯的 run 丟入 thread worker
    worker = std::thread([this]
                         { this->run(); });
}

Elevator::~Elevator()
{
    stop_flag = true;
    // 解構時，解開所有worker
    cv.notify_all();
    if (worker.joinable())
        worker.join();
}

// ----------------------
// getter
// ----------------------
int Elevator::get_current_floor()
{
    return current_floor.load();
}

std::string Elevator::get_name()
{
    return name;
}

std::string Elevator::get_status()
{
    return status;
}

// ----------------------
// setter
// ----------------------
void Elevator::set_render_callback(std::function<void()> cb)
{
    render_cb = cb;
}

void Elevator::display_floor()
{
    std::cout << "[" << name << "] Current floor: " << current_floor;
    if (!status.empty())
        std::cout << " [" << status << "]";
    std::cout << "\n";
}

// productor
void Elevator::move(int current_floor, int floor)
{
    {
        std::lock_guard<std::mutex> lock(mu);
        task_queue.push({current_floor, floor});
    }
    cv.notify_one();
}

void Elevator::run()
{
    while (!stop_flag)
    {
        int target = 0;
        // 用 mu 鎖住，去 queue 取東西
        {
            std::unique_lock<std::mutex> lock(mu);
            cv.wait(lock, [this]
                    { return !task_queue.empty() || stop_flag; });

            if (stop_flag && task_queue.empty())
                return;

            target = task_queue.front().second;
            task_queue.pop();
        }

        // 電梯move動作
        while (current_floor != target)
        {
            int direction = (target > current_floor) ? 1 : -1;
            status = (direction == 1) ? "Going UP" : "Going DOWN";

            // render update
            if (render_cb)
                render_cb();

            std::this_thread::sleep_for(std::chrono::milliseconds(MOVE_DELAY_MS));
            current_floor += direction;
        }

        status = "";
        if (render_cb)
            render_cb();
    }
}