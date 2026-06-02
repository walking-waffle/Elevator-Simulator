CXX = g++
CXXFLAGS = -std=c++17 -Wall

ifeq ($(OS),Windows_NT)
    EXE = .exe
    RM = del /Q
else
    EXE =
    RM = rm -f
endif

# exe
MAIN_TARGET = main$(EXE)
TEST_TARGET = test$(EXE)

# obj
MAIN_OBJS = main.o elevator.o
TEST_OBJS = test.o elevator.o

all: $(MAIN_TARGET) $(TEST_TARGET)

# main
$(MAIN_TARGET): $(MAIN_OBJS)
	$(CXX) $(MAIN_OBJS) -o $(MAIN_TARGET) $(CXXFLAGS)

# test
$(TEST_TARGET): $(TEST_OBJS)
	$(CXX) $(TEST_OBJS) -o $(TEST_TARGET) $(CXXFLAGS)

# main.cpp → main.o
# elevator.cpp → elevator.o
%.o: %.cpp elevator.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) *.o *.obj

.PHONY: all clean