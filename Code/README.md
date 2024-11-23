# C++ Raytracer/Pathtracer

## Overview
This repository contains a project for rendering scenes using textures and JSON-based configurations. Follow the instructions below to build, run, and clean the project.

## Requirements
- A C++ compiler (e.g., GCC or Clang)
- Make utility
- JSON configuration files (placed in the `jsons` directory)

## Directory Structure
- **Code/**: Contains the source code, textures, and build scripts.
- **src/**: Contains source cpp files.
- **include/**: Contains source header files.
- **external/**: Contains external libraries.
- **build**: Contains object and executable files.
- **jsons/**: Directory where JSON configuration files should be placed.
- **textures/**: Folder for storing texture files (e.g., PPM format).
- **TestSuite/**: Output PPM scenes.
- **Report/**: Coursework report.

## How to Build and Run
1. **Navigate to the `Code` directory:**
   ```bash
   cd Code
   make
   ./build/main.exe
    ```
2. **Cleaning `Code` Repository**
    ```bash
    make clean
    ```