# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17 -O3 -fopenmp -I.

# Target executable
TARGET = raytracer

# Source files
SRCS = main.cpp camera.cpp scene.cpp parser.cpp shapes.cpp helpers.cpp utilities.cpp material.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Build target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule for building object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target to remove build files
clean:
ifeq ($(OS),Windows_NT)
	# Use cmd /c to execute del in Windows shell
	cmd /c del /f /q $(TARGET).exe $(OBJS)
else
	# Linux/macOS command
	rm -f $(TARGET) $(OBJS)
endif
