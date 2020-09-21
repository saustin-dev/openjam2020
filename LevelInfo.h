//Information for levels
#include <iostream>
#include <fstream>
#include <vector>

#ifndef LEVELINFO_H
#define LEVELINFO_H

int const LEVEL_COUNT = 9;
int const TILESIZE = 16;
std::string FILENAMES[LEVEL_COUNT] = { "Data/Maps/Level1.map", "Data/Maps/Level2.map", "Data/Maps/Level3.map", "Data/Maps/Level4.map", "Data/Maps/Level5.map", "Data/Maps/Level6.map", "Data/Maps/Level7.map", "Data/Maps/Level8.map", "Data/Maps/Level9.map"  };
std::string BACKGROUNDS[LEVEL_COUNT] = { "Assets/Image/Clouds 3.png", "Assets/Image/Clouds 3.png", "Assets/Image/Clouds 3.png", "Assets/Image/Clouds 3.png", "Assets/Image/Clouds 3.png", "Assets/Image/Clouds 3 Sunset.png", "Assets/Image/Clouds 3 Sunset.png", "Assets/Image/Clouds 3 Sunset.png", "Assets/Image/Clouds 3 Sunset.png" };
std::string TILESET = "Assets/Image/metroidvania.png";
std::string MUSIC_NAMES[LEVEL_COUNT] = { "Assets/Sound/JourneyAhead.ogg", "Assets/Sound/JourneyAhead.ogg", "Assets/Sound/JourneyAhead.ogg", "Assets/Sound/JourneyAhead.ogg", "Assets/Sound/JourneyAhead.ogg", "Assets/Sound/TowardsTheSummit.ogg", "Assets/Sound/TowardsTheSummit.ogg", "Assets/Sound/TowardsTheSummit.ogg", "Assets/Sound/TowardsTheSummit.ogg" };
int START_COORDS[LEVEL_COUNT][4][2] = { { { 3, 10 }, { 48, 21 }, { 21, 0 }, { 0, 0 } }, 
										{ { 1, 52 }, { 0, 0 }, { 21, 0 }, { 0, 0 } }, 
										{ { 1, 7 }, { 28, 10 }, { 0, 0 }, { 15, 13 } },
										{ { 0, 0 }, { 38, 18 }, { 0, 0 }, { 0, 0 } },
										{ { 0, 0 }, { 0, 0 }, { 19, 0 }, { 0, 0 } },
										{ { 0, 29 }, { 0, 0 }, { 63, 0 }, { 30, 29 } },
										{ { 0, 0 }, { 0, 0 }, { 10, 0 }, { 0, 0 } },
										{ { 0, 0 }, { 68, 4 }, { 0, 0 }, { 8, 26 } },
										{ { 0, 48 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
std::string ADJACENT_MAPS[LEVEL_COUNT][4] = { { "", "Data/Maps/Level2.map", "", "" }, 
											{ "Data/Maps/Level1.map", "", "Data/Maps/Level3.map", "" }, 
											{ "Data/Maps/Level4.map", "Data/Maps/Level6.map", "", "Data/Maps/Level2.map" },
											{ "", "Data/Maps/Level3.map", "", "Data/Maps/Level5.map" },
											{ "", "", "", "Data/Maps/Level1.map" },
											{ "Data/Maps/Level3.map", "", "Data/Maps/Level8.map", "Data/Maps/Level7.map" },
											{ "", "", "Data/Maps/Level6.map", "" },
											{ "", "Data/Maps/Level9.map", "", "Data/Maps/Level6.map" },
											{ "", "", "win", "" }  };

#endif
