#!/bin/bash
g++ -fdiagnostics-color=always -g *.cpp \
    -lsfml-graphics -lsfml-window -lsfml-system \
    -lbox2d \
    -o main

export DISPLAY=:0
./main
