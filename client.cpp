#include <iostream>
#include <string>
#include <limits>

// sindows socket
#include <winsock2.h>
#include <ws2tcpip.h>

static const int TOTAL_FLOORS = 10;
static const int PORT = 8080;

// check input
int read_int(const std::string &prompt, int lo, int hi)
{
    int val;
    while (true)
    {
        std::cout << prompt << " (or 0 to exit): " << std::flush;
        if (std::cin >> val)
        {
            if (val == 0)
            {
                std::cout << "\nFinish!\n";
                return 0;
            }
            if (val >= lo && val <= hi)
                return val;
        }
        // 輸入糟糕的東西要清除
        else
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cout << "  Please enter a valid NUMBER between "
                  << lo << " and " << hi << ", or 0 to exit." << std::endl;
    }
}

// socket + send (from to)
int main()
{
    // init
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Startup failed.\n";
        return 1;
    }

    std::cout << "Connecting to Elevator Server...\n";

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        std::cout << "\n Socket creation error \n";
        WSACleanup();
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // IPv4 to binary
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        std::cout << "\nInvalid Address!\n";
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
    {
        std::cout << "\nConnection Failed!\n";
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    std::cout << "Connected Successfully!\n\n";

    // 開始可以操控電梯 loop send
    while (true)
    {
        int from = read_int("Enter your current floor (1-10)", 1, TOTAL_FLOORS);
        if (from == 0)
            break;

        int to = read_int("Enter destination floor  (1-10)", 1, TOTAL_FLOORS);
        if (to == 0)
            break;

        std::string command = std::to_string(from) + " " + std::to_string(to);
        send(sock, command.c_str(), command.length(), 0);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}