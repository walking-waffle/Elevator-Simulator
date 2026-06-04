CXX = g++
CXXFLAGS = -std=c++17
LDFLAGS = -lws2_32

SERVER = server.exe
CLIENT = client.exe

all: $(SERVER) $(CLIENT)

$(SERVER): server.cpp elevator.cpp elevator.h
	$(CXX) $(CXXFLAGS) server.cpp elevator.cpp -o $(SERVER) $(LDFLAGS)

$(CLIENT): client.cpp
	$(CXX) $(CXXFLAGS) client.cpp -o $(CLIENT) $(LDFLAGS)

clean:
	del $(SERVER) $(CLIENT)