CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude
SOURCES  = src/main.cpp src/film.cpp src/imdb.cpp
EN_TETES = include/arbreB.h include/film.h include/imdb.h

indexArbreB: $(SOURCES) $(EN_TETES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o indexArbreB

clean:
	rm -f indexArbreB