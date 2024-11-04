# Makefile

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17 -O3 -fopenmp

# Target executable
TARGET = raytracer

# Source files
SRCS = main.cpp raytracer.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Build target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# < means dependency (cpp) and @ means target (o)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	# Use cmd /c to execute del in Windows shell
	cmd /c del /f /q raytracer.exe main.o raytracer.o
else
	# Linux/macOS command
	rm -f $(TARGET) $(OBJS)
endif
