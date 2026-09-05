CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wno-unused-function -Wextra -g -O0

program: main.cpp CircBuf.cpp
	$(CXX) $(CXXFLAGS) main.cpp CircBuf.cpp -o program

clean:
	rm -f program