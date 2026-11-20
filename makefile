CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude
LDLIBS   = -lsqlite3
SOURCES  = src/main.cpp src/film.cpp src/imdb.cpp src/baseFilms.cpp
EN_TETES = include/arbreB.h include/film.h include/imdb.h include/baseFilms.h include/entree.h

indexArbreB: $(SOURCES) $(EN_TETES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o indexArbreB $(LDLIBS)

clean:
	rm -f indexArbreB