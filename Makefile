CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -g -O0

program: test_config.cpp pulse_config.cpp pulse_config.h

	$(CXX) $(CXXFLAGS) test_config.cpp pulse_config.cpp -o tests

clean:
	rm -f program