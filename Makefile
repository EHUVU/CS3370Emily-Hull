CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -g -O0

program: pulse.cpp pulse_config.cpp pulse_config.h

	$(CXX) $(CXXFLAGS) pulse.cpp pulse_config.cpp -o tests

clean:
	rm -f program