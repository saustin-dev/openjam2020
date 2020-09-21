//Information for levels
#include <iostream>
#include <fstream>
#include <vector>

#ifndef LEVELINFO_H
#define LEVELINFO_H

int const LEVEL_COUNT = 3;
int const TILESIZE = 16;
std::string FILENAMES[LEVEL_COUNT] = { "Data/Maps/Level1.map", "Data/Maps/Level2.map", "Data/Maps/Level3.map" };
std::string BACKGROUNDS[LEVEL_COUNT] = { "Assets/Image/Clouds 3.png", "Assets/Image/Clouds 3.png", "Assets/Image/Clouds 3.png" };
std::string TILESET = "Assets/Image/metroidvania.png";
std::string MUSIC_NAMES[LEVEL_COUNT] = { "Assets/Sound/JourneyAhead.ogg", "Assets/Sound/JourneyAhead.ogg", "Assets/Sound/JourneyAhead.ogg" };
int START_COORDS[LEVEL_COUNT][4][2] = { { { 3, 10 }, { 48, 21 }, { 0, 0 }, { 0, 0 } }, 
										{ { 1, 52 }, { 0, 0 }, { 21, 0 }, { 0, 0 } }, 
										{ { 1, 7 }, { 28, 10 }, { 0, 0 }, { 15, 13 } } };
std::string ADJACENT_MAPS[LEVEL_COUNT][4] = { { "", "Data/Maps/Level2.map", "", "" }, 
											{ "Data/Maps/Level1.map", "", "Data/Maps/Level3.map", "" }, 
											{ "", "", "", "Data/Maps/Level2.map" } };

#endif
