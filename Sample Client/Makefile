# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -pthread -Wall

# Source files
SRCS = SampleClient.cpp ../MapReduceFramework.cpp ../Context.cpp ../WarpContext.cpp ../Barrier.cpp ../JobHandler.cpp

# Header files
HEADERS = ../MapReduceFramework.h ../Context.h ../WarpContext.h ../Barrier.h ../JobHandler.h

# Output executable
TARGET = sample_client

# Default target
all: $(TARGET)

# Compile and link the target executable
$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

# Clean up the build
clean:
	rm -f $(TARGET)

# Run the client
run: $(TARGET)
	./$(TARGET)
