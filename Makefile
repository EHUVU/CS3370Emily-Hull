CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -g -O0

program: main.cpp NPC.cpp StockOrder.cpp Pool.h NPC.h StockOrder.h
	$(CXX) $(CXXFLAGS) main.cpp NPC.cpp StockOrder.cpp -o program

clean:
	rm -f program