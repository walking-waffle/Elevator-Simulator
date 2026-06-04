#include "elevator.h"

#include <iostream>
#include <string>
#include <limits>
#include <cmath>
#include <mutex>
#include <sstream>
#include <thread>

// windows socket
#include <winsock2.h>
#include <ws2tcpip.h>

static const int TOTAL_FLOORS = 10;
static const int PORT = 8080;
std::mutex mu;

void render_elevator(Elevator &e1, Elevator &e2)
{
    std::lock_guard<std::mutex> lock(mu);
    std::cout << "\033[2J\033[H";
    std::cout << "  +------------+------------+\n";
    std::cout << "  | Elevator 1 | Elevator 2 |\n";
    std::cout << "  +------------+------------+\n";

    for (int f = TOTAL_FLOORS; f >= 1; --f)
    {
        std::string slot1 = "        ";
        std::string slot2 = "        ";

        if (e1.get_current_floor() == f)
            slot1 = "  [=]   ";
        if (e2.get_current_floor() == f)
            slot2 = "  [=]   ";

        std::string fnum = std::to_string(f);
        if (f < 10)
            fnum = " " + fnum;

        std::cout << "  |  " << fnum << slot1 << "|  " << fnum << slot2 << "|\n";
    }

    std::cout << "  +------------+------------+\n";
    std::cout << "----------------------------------\n";
    e1.display_floor();
    e2.display_floor();
    std::cout << "----------------------------------\n";
}

// 用四種 case 計算電梯對乘客的運送成本
int calculate_dispatch_cost(Elevator &e, int from, int dest)
{
    int e_floor = e.get_current_floor();
    std::string e_status = e.get_status();

    int dist = std::abs(e_floor - from);

    // 乘客想去的方向
    int req_dir = (dest > from) ? 1 : -1;

    // case 1: 成本是距離
    if (e_status.empty() || e_status == "")
        return dist;

    // 電梯目前的移動方向
    int e_dir = (e_status == "Going UP") ? 1 : -1;

    // 判斷電梯是否靠近乘客
    bool closer = (e_dir == 1 && e_floor <= from) ||
                  (e_dir == -1 && e_floor >= from);

    if (closer)
    {
        if (e_dir == req_dir)
            // case2: 超級順路
            return dist;
        else
            // case 3: 電梯得先送完當前方向的客人才會回頭
            return dist + TOTAL_FLOORS;
    }
    else
        // case 4: 超級不順路
        return dist + (TOTAL_FLOORS * 2);
}

// 處理來自 Client 的資料
void handle_client(SOCKET client_socket, Elevator &e1, Elevator &e2)
{
    char buffer[1024] = {0};
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int valread = recv(client_socket, buffer, 1024, 0);

        // 關閉 socket
        if (valread <= 0)
        {
            std::lock_guard<std::mutex> lock(mu);
            std::cout << "\n[System] Client disconnected.\n";
            closesocket(client_socket);
            break;
        }

        // 電梯調度
        std::stringstream ss(buffer);
        int from, dest;
        if (ss >> from >> dest)
        {
            // 分別計算兩台電梯的派遣成本
            int cost1 = calculate_dispatch_cost(e1, from, dest);
            int cost2 = calculate_dispatch_cost(e2, from, dest);

            // 選擇成本較低的電梯
            Elevator &selected = (cost1 <= cost2) ? e1 : e2;
            selected.move(from, dest);
        }
    }
}

// socket
// render elevator
int main()
{
    // init
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    Elevator elevator1("Elevator 1", 1);
    Elevator elevator2("Elevator 2", 1);

    // 呼叫 render call back後，可以一次對兩台電梯 render update
    auto render_cb = [&]()
    { render_elevator(elevator1, elevator2); };
    elevator1.set_render_callback(render_cb);
    elevator2.set_render_callback(render_cb);

    render_cb();

    // socket creation
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed.\n";
        WSACleanup();
        return 1;
    }

    struct sockaddr_in address;
    // 指定通訊協定 IPv4
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed.\n";
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // listen
    if (listen(server_fd, 3) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed.\n";
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    {
        std::lock_guard<std::mutex> lock(mu);
        std::cout << "[System] Server listening on port " << PORT << "\n";
    }

    // accept
    SOCKET client_socket;
    int addrlen = sizeof(address);
    while ((client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) != INVALID_SOCKET)
    {
        {
            std::lock_guard<std::mutex> lock(mu);
            std::cout << "[System] Input Client Connected!\n";
        }
        std::thread client_thread(handle_client, client_socket, std::ref(elevator1), std::ref(elevator2));
        client_thread.detach();
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}