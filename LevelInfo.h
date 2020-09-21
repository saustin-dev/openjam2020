//Information for levels
#include <iostream>
#include <fstream>
#include <vector>

#ifndef LEVELINFO_H
#define LEVELINFO_H

int const LEVEL_COUNT = 1;

std::string FILENAMES[LEVEL_COUNT] = { "Data/Maps/Level1.map" };
std::string BACKGROUNDS[LEVEL_COUNT] = { "Assets/Image/Cloud 1.bmp" };
std::string TILESET = "Assets/Image/metroidvania.png";
std::string MUSIC_NAMES[LEVEL_COUNT] = { "Assets/Sound/JourneyAhead.ogg" };
int START_COORDS[LEVEL_COUNT][4][2] = { { { 3, 12 }, { 48, 23 }, { 0, 0 }, { 0, 0 } } };
std::string ADJACENT_MAPS[LEVEL_COUNT][4] = { { "", "Data/Maps/Level2.map", "", "" } };

#endif
