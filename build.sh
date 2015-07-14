#!/bin/sh

# g++ gothic.cpp -lSDL2 -ansi -pedantic -Wall -Wextra -O3 -o gothic

# /System/Library/Frameworks/OpenGL.framework/Versions/A/Headers/gl.h
# /opt/X11/include/GL/gl.h

# 0    black     COLOR_BLACK     0,0,0
# 1    red       COLOR_RED       1,0,0
# 2    green     COLOR_GREEN     0,1,0
# 3    yellow    COLOR_YELLOW    1,1,0
# 4    blue      COLOR_BLUE      0,0,1
# 5    magenta   COLOR_MAGENTA   1,0,1
# 6    cyan      COLOR_CYAN      0,1,1
# 7    white     COLOR_WHITE     1,1,1

# g++ source/*.cpp -Wall -I source -I source/include -I /opt/X11/include -I /users/jbeck/src/include -o longshot
echo "$(tput setaf 7)$(tput setab 4)building $(tput setaf 0)#############################################################$(tput sgr 0)"

# MacOS (i386)
# g++ source/*.cpp source/dungeon/*.cpp source/feature/*.cpp -w -I source -I source/include -I /opt/X11/include -I /users/jbeck/src/include -l SDL2 -framework SDL2_mixer -framework SDL2_image -framework SDL2_ttf -framework OpenGL -arch i386 -o Release/longshot

# MacOS (default architecture)
# g++ source/*.cpp source/dungeon/*.cpp source/feature/*.cpp -w -I source -I source/include -I /opt/X11/include -I /users/jbeck/src/include -l SDL2 -framework SDL2_mixer -framework SDL2_image -framework SDL2_ttf -framework OpenGL -o Release/longshot
g++ source/*.cpp source/dungeon/*.cpp source/feature/*.cpp -Wall -I source -I source/include -I /opt/X11/include -I /users/jbeck/src/include -l SDL2 -framework SDL2_mixer -framework SDL2_image -framework SDL2_ttf -framework OpenGL -o Release/longshot

echo "$(tput setaf 7)$(tput setab 4)done $(tput setaf 0)#############################################################$(tput sgr 0)"
