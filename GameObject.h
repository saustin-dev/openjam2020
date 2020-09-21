//The file which will handle game state
#include <iostream>
#include <fstream>
#include <vector>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"
#include "WindowAbstraction.h"
#include "WindowsAndMenus.h"
#include "GameData.h"
#include "PlayerLogic.h"
#include "LevelInfo.h"

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

class GameLevel {
	private:
	MapData *data;
	std::string filename;
	SDL_Renderer *renderer;
	SDL_Texture *bgTex;
	std::string bg;
	std::string musicCommand;
	std::string tileset;
	TilesetDrawer *tilesetDrawer;
	//command system will be used to go from one level to another
	std::string leftCommand;
	std::string rightCommand;
	std::string upCommand;
	std::string downCommand;
	//different coordinates to start at depending on where you enter from
	int leftCoords[2];
	int rightCoords[2];
	int upCoords[2];
	int downCoords[2];
	
	public:
	GameLevel(SDL_Renderer *renderer, std::string filename, std::string bg, std::string tileset, int tileSize, std::string musicCommand, int startCoords[4][2], 
	std::string leftCommand, std::string rightCommand, std::string upCommand, std::string downCommand) {
		this->filename = filename;
		this->renderer = renderer;
		this->bg = bg;
		this->musicCommand = musicCommand;
		this->leftCommand = leftCommand;
		this->rightCommand = rightCommand;
		this->upCommand = upCommand;
		this->downCommand = downCommand;
		this->tileset = tileset;
		this->tilesetDrawer = new TilesetDrawer(tileset, renderer, tileSize);
		leftCoords[0] = startCoords[0][0];
		leftCoords[1] = startCoords[0][1];
		rightCoords[0] = startCoords[1][0];
		rightCoords[1] = startCoords[1][1];
		upCoords[0] = startCoords[2][0];
		upCoords[1] = startCoords[2][1];
		downCoords[0] = startCoords[3][0];
		downCoords[1] = startCoords[3][1];
		data = readFile(filename);
		SDL_Surface *bgSurface = IMG_Load(bg.c_str());
		bgTex = SDL_CreateTextureFromSurface(renderer, bgSurface);
		SDL_FreeSurface(bgSurface);
	}
	~GameLevel() {
		SDL_DestroyTexture(bgTex);
		delete(tilesetDrawer);
	}
	
	std::string getMusicCommand() {
		return musicCommand;
	}
	
	std::string getLeft() {
		return this->leftCommand;
	}
	std::string getRight() {
		return this->rightCommand;
	}
	std::string getUp() {
		return this->upCommand;
	}
	std::string getDown() {
		return this->leftCommand;
	}
	
	int tileSize() {
		return tilesetDrawer->tileSize();
	}
	
	void setTileSize(int tileSize) {
		tilesetDrawer->setTileSize(tileSize);
	}
	
	MapData *getData() {
		return data;
	}
	
	std::string getFilename() {
		return filename;
	}
	
	/**
	 * Load the level by taking the player and setting them, then return music command
	 */
	std::string load(Player* player, int side) {
		if(side == 0) {
			player->changeMap(data,tileSize()*leftCoords[0],tileSize()*leftCoords[1],1);
		}
		else if(side == 1) {
			player->changeMap(data,tileSize()*rightCoords[0],tileSize()*rightCoords[1],0);
		}
		else if(side == 2) {
			player->changeMap(data,tileSize()*upCoords[0],tileSize()*rightCoords[1],0);
		}
		else if(side == 3) {
			player->changeMap(data,tileSize()*downCoords[0],tileSize()*rightCoords[1],1);
		}
		
		return musicCommand;
	}
	
	void draw(Player *player, int width, int height) {
		//first the background
		SDL_RenderCopy(renderer, bgTex, NULL, NULL);
		
		//then the map
		SDL_Rect playerRect = player->getRect();
		int centerX = playerRect.x + playerRect.w/2;
		int centerY = playerRect.y + playerRect.h/2;
		
		//then the player
	}
};

class GameObject : public Visual {
	private:
	int width;
	int height;
	//holds onto the current level as well as all loaded levels
	GameLevel *currentLevel;
	std::vector<GameLevel*> levels;
	//keeps the player object as well
	Player *player;
	//has a reference to the window command queue in order to push songs up
	CommandQueue *windowCommandQueue;
	
	public:
	GameObject(SDL_Renderer *renderer, CommandQueue *queuePtr, int tileSize, int width, int height, int levelIndex, int side) {
		//load up all the levels
		for(int i = 0; i < LEVEL_COUNT; i++) {
			levels.push_back(new GameLevel(renderer,FILENAMES[i],BACKGROUNDS[i],TILESET,tileSize,MUSIC_NAMES[i],START_COORDS[i],
								ADJACENT_MAPS[i][0],ADJACENT_MAPS[i][1],ADJACENT_MAPS[i][2],ADJACENT_MAPS[i][3]));
		}
		currentLevel = levels.at(levelIndex);
		//construct the player
		player = new Player(renderer, 0, 0, nullptr, tileSize);
		//load player into level
		currentLevel->load(player, side);
		windowCommandQueue = queuePtr;
	}
	GameObject(SDL_Renderer *renderer, CommandQueue *queuePtr, int tileSize, int width, int height) : GameObject { renderer, queuePtr, tileSize, width, height, 0, 0 } {
	};
	
	std::string getTitle() {
		return "Game";
	}
	
	bool switchLevel(std::string filename) {
		for(unsigned int i = 0; i<levels.size(); i++) {
			if(levels.at(i)->getFilename() == filename) {
				currentLevel = levels.at(i);
				return true;
			}
		}
		return false;
	}
	
	//see if the player has moved out of bounds and if so attempt to change level
	void checkBounds() {
		SDL_Rect playerRect = player->getRect();
		int w = currentLevel->getData()->getW() * currentLevel->tileSize();
		int h = currentLevel->getData()->getH() * currentLevel->tileSize();
		//out of bounds by left of screen
		if(playerRect.x + playerRect.w < 0) {
			if(switchLevel(currentLevel->getLeft())) {
				windowCommandQueue->add(currentLevel->load(player,1));
			}
			return;
		}
		//out of bounds by right of screen
		if(playerRect.x - playerRect.w > w) {
			if(switchLevel(currentLevel->getRight())) {
				windowCommandQueue->add(currentLevel->load(player,0));
			}
			return;
		}
		//out of bounds by top of screen
		if(playerRect.y + playerRect.h < 0) {
			if(switchLevel(currentLevel->getUp())) {
				windowCommandQueue->add(currentLevel->load(player,3));
			}
			return;
		}
		//out of bounds by bottom of screen
		if(playerRect.y - playerRect.h > h) {
			if(switchLevel(currentLevel->getDown())) {
				windowCommandQueue->add(currentLevel->load(player,2));
			}
			return;
		}
		
	}
	void update() {
		//update the player
		player->update();
		checkBounds();
	}
	void handleInput(SDL_Event event) {
		//pass input direct to player
		player->handleInput(event);
	}
	void draw() {
		//draw the current level, then draw the player
		currentLevel->draw(player, width, height);
	}
	
	std::string onActive() {
		return currentLevel->getMusicCommand();
	}
	
	//empty virtual functions from visual
	void hover(int mouseX, int mouseY) {
	}
	int click(int mouseX, int mouseY) {
		return -1;
	}
};

class GameDrawer : public Visual {
	private:
	std::string title;
	SDL_Renderer *renderer;
	TilesetDrawer *tilesetDrawer;
	SpecificElement *bg;
	MapData *currentMap;
	int tileRes;
	std::string activeCommand;
	Player *player;
	
	public:
	GameDrawer(SDL_Renderer *renderer, std::string title, std::string background, std::string activeCommand, SDL_Rect rect, MapData *map, int tileRes) {
		this->title = title;
		this->tilesetDrawer = new TilesetDrawer("Assets/Image/metroidvania.png",renderer,16);
		this->tileRes = tileRes;
		this->currentMap = map;
		this->bg = new SpecificElement(new ImageTile(background, renderer), rect);
		this->player = new Player(renderer, 0, 0, map, tileRes);
		this->activeCommand = activeCommand;
	}
	~GameDrawer() {
		delete(tilesetDrawer);
		delete(bg);
	}
	
	std::string getTitle() {
		return this->title;
	}
	
	void hover(int mouseX, int mouseY) {
	}
	int click(int mouseX, int mouseY) {
		return -1;
	}
	
	void draw() {
		bg->draw();
		int offX = 0;
		int offY = 0;
		int w = currentMap->getW();
		int h = currentMap->getH();
		int **data = currentMap->getData();
		for(int y = 0; y < h; y++) {
			for(int x = 0; x < w; x++) {
				tilesetDrawer->draw({offX+tileRes*x,offY+tileRes*y,tileRes,tileRes},data[y][x]);
			}
		}
		player->draw(player->getRect());
	}
	
	std::string onActive() {
		return activeCommand;
	}
	
	void update() {
		player->update();
	}
	
	void handleInput(SDL_Event event) {
		player->handleInput(event);
	}
};

#endif
