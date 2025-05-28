#!/bin/bash
g++ -fdiagnostics-color=always -g *.cpp \
    -lsfml-graphics -lsfml-window -lsfml-system \
    -o "main"
export DISPLAY=:0
./main