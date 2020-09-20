//Game data things
#include <iostream>
#include <fstream>
#include <vector>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#ifndef GAMEDATA_H
#define GAMEDATA_H

/**
 * Basic wrapper for 2D int array
 */
class MapData {
	private:
	int w;
	int h;
	int **data;
	
	public:
	MapData(int w, int h) {
		this->w = w;
		this->h = h;
		int **arr1 = (int**)calloc(h,sizeof(int*));
		for(int i = 0; i < h; i++) {
			int *arr2 = (int*)calloc(w,sizeof(int));
			arr1[i] = arr2;
		}
		data = arr1;
	}
	~MapData() {
		for(int i = 0; i < h; i++) {
			free(data[i]);
		}
		free(data);
	}
	
	int **getData() {
		return data;
	}
	
	int getW() {
		return w;
	}
	int getH() {
		return h;
	}
};

MapData *readFile(std::string filename) {
	FILE *fp = fopen(filename.c_str(), "r");
	if(!fp) {
		throw;
	}
	int w = getw(fp);
	int h = getw(fp);
	//printf("Loading file: %d x %d\n",w, h);
	MapData *data = new MapData(w, h);
	int **theData = data->getData();
	for(int y = 0; y < h; y++) {
		for(int x = 0; x < w; x++) {
			theData[y][x] = getw(fp);
		}
	}
	
	return data;
}

#endif
