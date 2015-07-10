#!/bin/sh

# g++ gothic.cpp -lSDL2 -ansi -pedantic -Wall -Wextra -O3 -o gothic

# /System/Library/Frameworks/OpenGL.framework/Versions/A/Headers/gl.h
# /opt/X11/include/GL/gl.h

g++ source/*.cpp -Wall -I source -I source/include -I /opt/X11/include -I /users/jbeck/src/include -o longshot
