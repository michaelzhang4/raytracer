# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
EXTERNAL_DIR = external

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17 -O3 -fopenmp -I$(INCLUDE_DIR) -I$(EXTERNAL_DIR)

# Target executable
TARGET = $(BUILD_DIR)/main

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Build target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule for building object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target to remove build files
clean:
ifeq ($(OS),Windows_NT)
	cmd /c del /f /q $(BUILD_DIR)\*.o $(TARGET).exe
else
	rm -f $(BUILD_DIR)/*.o $(TARGET)
endif
