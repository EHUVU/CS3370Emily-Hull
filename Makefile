CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wno-unused-function -Wextra -g -O0

program: main.cpp bigint.cpp
	$(CXX) $(CXXFLAGS) main.cpp bigint.cpp -o program

clean:
	rm -f program