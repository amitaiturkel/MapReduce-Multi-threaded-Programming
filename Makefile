# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11

# Source files and object files
SOURCES = MapReduceFramework.cpp Context.cpp WarpContext.cpp Barrier.cpp JobHandler.cpp
HEADERS = MapReduceFramework.h Context.h WarpContext.h Barrier.h JobHandler.h
OBJECTS = $(SOURCES:.cpp=.o)
LIBRARY = libMapReduceFramework.a

# Default target
all: $(LIBRARY)

# Rule to create the static library
$(LIBRARY): $(OBJECTS)
	ar rcs $@ $^

# Rule to compile each source file into object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Tar target
tar: clean
	tar -cvf ex3.tar $(SOURCES) $(HEADERS) Makefile  README

# Clean up the build files
clean:
	rm -f $(OBJECTS) $(LIBRARY) ex3.tar

.PHONY: all clean tar
