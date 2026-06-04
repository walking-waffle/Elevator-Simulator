#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <functional>

class Elevator
{
public:
    Elevator(const std::string &n, int start_floor = 1);
    ~Elevator();

    void display_floor();
    void move(int current_floor, int floor);
    void set_render_callback(std::function<void()> cb);

    // Getters
    int get_current_floor();
    std::string get_name();
    std::string get_status();

private:
    std::atomic<int> current_floor;
    std::string name;
    std::string status;

    // for multithread
    std::queue<std::pair<int, int>> task_queue;
    std::mutex mu;
    std::condition_variable cv;
    std::atomic<bool> stop_flag;
    std::thread worker;
    std::function<void()> render_cb;

    void run();
};

#endif