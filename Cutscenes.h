//Cutscenes to tie the whole thing together
#include <iostream>
#include <fstream>
#include <vector>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"
#include "WindowAbstraction.h"

#ifndef CUTSCENES_H
#define CUTSCENES_H

void startCutscene(SDL_Renderer *renderer) {
	std::string const filename = "Assets/Image/startCutscene.png";
	int const length = 5000;
	
	SDL_Surface *sur = IMG_Load(filename.c_str());
	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, sur);
	SDL_FreeSurface(sur);
	SDL_RenderCopy(renderer, tex, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_Delay(length);
	SDL_DestroyTexture(tex);
}

void endCutscene(SDL_Renderer *renderer) {
	std::string const filename = "Assets/Image/endCutscene.png";
	int const length = 5000;
	
	SDL_Surface *sur = IMG_Load(filename.c_str());
	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, sur);
	SDL_FreeSurface(sur);
	SDL_RenderCopy(renderer, tex, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_Delay(length);
	SDL_DestroyTexture(tex);
}

#endif
