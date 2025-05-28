#!/bin/bash

# Compiler and flags
CXX=x86_64-w64-mingw32-g++
CXXFLAGS="-std=c++17 -Wall -Wextra -O2"
LDFLAGS="-L/usr/local/lib -Wl,--no-whole-archive /usr/x86_64-w64-mingw32/lib/libmingw32.a -lsfml-graphics -lsfml-window -lsfml-system"

# Include directories
INCLUDES="-I/usr/local/include"

# Output directory
OUTPUT_DIR="./build/windows"

# Source and target files
SOURCE_FILES="*.cpp"
TARGET="$OUTPUT_DIR/game.exe"

# Create output directory if not exists
mkdir -p $OUTPUT_DIR

# Compile the project
$CXX $CXXFLAGS $SOURCE_FILES -o $TARGET $INCLUDES $LDFLAGS

# Check for errors
if [ $? -eq 0 ]; then
  echo "Build successful!"
else
  echo "Build failed!"
fi
