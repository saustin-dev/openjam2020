# openjam2020

This is my repo for my entry to Open Jam 2020.

## Open Source Tools Used:
- gcc/g++
- gdb
- SDL2
- Geany
- Ubuntu
- Musescore
- Audacity
- Pinta
- GIMP

## How to Play
### Linux
You need SDL2 as well as image, mixer, and TTF to play, so download them with (on Ubuntu):
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev

On Linux, build with the command: 
g++ -o "Game" "Game.cpp" -lm -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf
or for the level editor:
g++ -o "LevelEditor" "LevelEditor.cpp" -lm -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf

Then run with ./LevelEditor or ./Game

### Windows
An executable and .dlls will be provided so just run the .exe and it should hopefully work.

## How to Actually Play
The controls are:
A/D: move
S: crouch, slide (while running), cancel glide (while gliding)
Space: jump, enter glide (while airborne)
