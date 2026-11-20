CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude

indexArbreB: src/main.cpp include/arbreB.h
	$(CXX) $(CXXFLAGS) src/main.cpp -o indexArbreB

clean:
	rm -f indexArbreB