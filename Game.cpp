//The main game file
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
#include "GameObject.h"
#include "Cutscenes.h"

/**
 * Store the coordinates of the mouse pointer
 */
int mouseX = 0;
int mouseY = 0;

/**
 * The default window title
 */
std::string WINDOW_TITLE = "Over The Clouds";

/**
 * Windowed resolutions to toggle through
 */
int const X_RESOLUTIONS[2][3] = { { 640, 1024, 1200 }, { 720, 1280, 1600 } };
int const Y_RESOLUTIONS[2][3] = { { 480, 768, 900 }, { 480, 720, 900 } };
int const TILE_SIZES[3] = {32,48,64};
int const FULLSCREEN_OPTIONS[3] = { 0, SDL_WINDOW_FULLSCREEN, SDL_WINDOW_FULLSCREEN_DESKTOP };
int ratio = 1;
int res = 1;
int fullscreen = 0;

/**
 * Store window dimensions
 */
int SCREEN_WIDTH = X_RESOLUTIONS[ratio][res];
int SCREEN_HEIGHT = Y_RESOLUTIONS[ratio][res];

/**
 * Framerate info
 */
int const FRAMERATE = 60;
int const MS_DELAY = 1000/FRAMERATE;

class MusicHandler {
	private:
	std::string currentSong;
	Mix_Music *currentMusic;
	
	public:
	MusicHandler() {
	}
	~MusicHandler() {
		stop();
	}
	
	void play(std::string arg) {
		//printf("Try to play song %s\n",arg.c_str());
		if(currentSong != arg) {
			currentSong = arg;
			currentMusic = Mix_LoadMUS(arg.c_str());
			if(Mix_PlayMusic(currentMusic, -1) == -1) {
				printf("Mix_PlayMusic: %s\n", Mix_GetError());
			}
		}
	}
	
	void stop() {
		//if(currentMusic) Mix_FreeMusic(currentMusic);
		Mix_HaltMusic();
		currentSong = "";
		
	}
};

class GameWindow : public Window {
	CommandQueue *queue;
	MusicHandler *music;
	GameObject *object;
	LevelState *levelState;
	std::string backTitle;
	
	public:
	GameWindow(SDL_Renderer *renderer, SDL_Window *window) {
		this->window = window;
		SDL_GetWindowSize(window,&SCREEN_WIDTH,&SCREEN_HEIGHT);
		quit = false;
		this->renderer = renderer;
		activeVisual = nullptr;
		this->activeTitle = WINDOW_TITLE;
		this->queue = new CommandQueue();
		this->music = new MusicHandler();
		backTitle = WINDOW_TITLE;
		levelState = new LevelState("Data/savedata.sav");
		object = new GameObject(renderer, queue, levelState, TILE_SIZES[res], SCREEN_WIDTH, SCREEN_HEIGHT);
		
		build();
		
	}
	~GameWindow() {
		destroy();
		delete(levelState);
	}
	
	void destroy() {
		while(visuals.size()) {
			//This warns about non-virtual destructor in Visual but if I add virtual deconstructor it segfaults...
			//if(visuals.back()) delete(visuals.back());
			visuals.pop_back();
		}
		while(logics.size()) {
			if(logics.back()) delete(logics.back());
			logics.pop_back();
		}
	}
	
	void build() {
		destroy();
		
		//Assemble all the different menus
		std::string buttons[3] = {"Start Game","Options","Quit"};
		Menu *mainMenu = new Menu(renderer, WINDOW_TITLE, "Assets/Image/Clouds 2.png", "play Assets/Sound/Interlude.ogg", 3, buttons, 1, SCREEN_WIDTH, SCREEN_HEIGHT);
		visuals.push_back(mainMenu);
		std::string buttons2[4] = {"Fullscreen","Switch Resolution","Switch Ratio", "Go Back"};
		Menu *optionsMenu = new Menu(renderer, "Options", "Assets/Image/Clouds 2.png", "play Assets/Sound/Interlude.ogg", 4, buttons2, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		visuals.push_back(optionsMenu);
		
		std::string buttons3[3] = {"New Game","Load Game","Go Back"};
		if(!levelState->doesFileExist())
			buttons3[1] = "<No Data>";
		Menu *fileMenu = new Menu(renderer, "Play Game", "Assets/Image/Clouds 2.png", "play Assets/Sound/Interlude.ogg", 3, buttons3, -1, SCREEN_WIDTH, SCREEN_HEIGHT);
		visuals.push_back(fileMenu);
		
		std::string buttons4[3] = {"Resume","Options","Main Menu"};
		Menu *pauseMenu = new Menu(renderer, "Pause", "Assets/Image/Clouds 2.png", "play Assets/Sound/Interlude.ogg", 3, buttons4, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		visuals.push_back(pauseMenu);
		
		visuals.push_back(object);
		object->resize(SCREEN_WIDTH, SCREEN_HEIGHT);
		
		changeVisual(activeTitle, 1);
	}
	
	void resize(int width, int height) {
		SCREEN_HEIGHT = height;
		SCREEN_WIDTH = width;
		SDL_SetWindowSize(window, width, height);
		SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		object->resize(width, height);
		object->changeTileSize(TILE_SIZES[res]);
		build();
	}
	
	void changeVisual(std::string title, bool building) {
		for(unsigned int i = 0; i < visuals.size(); i++) {
			if(visuals.at(i)->getTitle() == title) {
				activeVisual = visuals.at(i);
				queue->add(activeVisual->onActive());
				if(!building)
					backTitle = activeTitle;
				if(activeTitle == "Game")
					object->onInactive();
				activeTitle = title;
				break;
			}
		}
	}
	
	void changeVisual(std::string title) {
		changeVisual(title, 0);
	}
	
	void parseQueue() {
		while(!queue->isEmpty()) {
			std::string currentCommand = queue->remove();
			parseCommand(currentCommand);
		}
	}
	
	void parseCommand(std::string command) {
		std::string base = command.substr(0,command.find(' ',0));
		std::string arg = command.substr(command.find(' ',0)+1,command.length()-command.find(' ',0));
		if(base == "play") {
			music->play(arg);
		}
		else if(base == "stop") {
			music->stop();
		}
		else {
			printf("Unknown command '%s'\n",base.c_str());
		}
	}
	
	void update() {
		activeVisual->update();
	}
	
	void handleEvent(SDL_Event event) {
		activeVisual->hover(mouseX, mouseY);
		
		//Only care about left clicks
		if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
			int clicked = activeVisual->click(mouseX, mouseY);
			if(activeVisual->getTitle() == WINDOW_TITLE) {
				switch (clicked) {
					case 0:
						changeVisual("Play Game");
						break;
					case 1:
						changeVisual("Options");
						break;
					case 2:
						quit = true;
						break;
				}
			}
			else if(activeVisual->getTitle() == "Options") {
				switch (clicked) {
					case 0:
						//Fullscreen
						fullscreen++;
						if(fullscreen > 2) fullscreen = 0;
						SDL_SetWindowFullscreen(window,FULLSCREEN_OPTIONS[fullscreen]);
						break;
					case 1:
						//Change resolution
						res++;
						if(res > 2) res = 0;
						resize(X_RESOLUTIONS[ratio][res],Y_RESOLUTIONS[ratio][res]);
						break;
					case 2:
						//Change ratio
						ratio++;
						if(ratio > 1) ratio = 0;
						resize(X_RESOLUTIONS[ratio][res],Y_RESOLUTIONS[ratio][res]);
						break;
					case 3:
						changeVisual(backTitle);
						break;
				}
			}
			else if(activeVisual->getTitle() == "Play Game") {
				switch (clicked) {
					case 0:
						//New game
						levelState->deleteSave();
						object->reloadState();
						object->reset();
						startCutscene(renderer);
						changeVisual("Game");
						break;
					case 1:
						//Load game
						if(!levelState->doesFileExist())
							break;
						object->reloadState();
						object->reset();
						changeVisual("Game");
						break;
					case 2:
						changeVisual(WINDOW_TITLE);
						break;
				}
			}
			else if(activeVisual->getTitle() == "Pause") {
				switch (clicked) {
					case 0:
						//Resume game
						changeVisual("Game");
						break;
					case 1:
						//Go to options
						changeVisual("Options");
						break;
					case 2:
						//Save data first
						levelState->save();
						build();
						//Return to main menu
						changeVisual(WINDOW_TITLE);
						break;
				}
			}
		}
		//Keys matter
		else if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
			if(activeVisual->getTitle() == "Game") {
				if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
					//go to pause menu
					changeVisual("Pause");
				}
				else {
					activeVisual->handleInput(event);
				}
			}
		}
		if(event.type == SDL_WINDOWEVENT) {
			if(event.window.event == SDL_WINDOWEVENT_CLOSE) {
				quit = true;
			}
			else if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
				resize(event.window.data1, event.window.data2);
			}
		}
		
		SDL_PumpEvents();
		SDL_FlushEvents(SDL_FIRSTEVENT,SDL_APP_DIDENTERFOREGROUND);
		SDL_FlushEvents(SDL_TEXTEDITING,SDL_MOUSEMOTION);
		SDL_FlushEvents(SDL_MOUSEWHEEL,SDL_LASTEVENT);
	}
};

//Setup, loop, etc.
//-------------------------------------------------------------------------
int main() {
	//start SDL
	SDL_Init(SDL_INIT_AUDIO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	Mix_Init(MIX_INIT_MP3|MIX_INIT_OGG);
	Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);
	SDL_Window *window = SDL_CreateWindow(WINDOW_TITLE.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,0);
	SDL_Surface *icon = IMG_Load("Assets/Image/Character/icon.png");
	SDL_SetWindowIcon(window, icon);
	SDL_FreeSurface(icon);
	SDL_SetWindowResizable(window,SDL_TRUE);
	SDL_Renderer *renderer  = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
	SDL_Event event;
	
	GameWindow *gameWindow = new GameWindow(renderer, window);
	unsigned int lastTime = SDL_GetTicks();
	//main loop
	while(!gameWindow->shouldQuit()) {
		while(SDL_PollEvent(&event)) {
			SDL_GetMouseState(&mouseX, &mouseY);
			gameWindow->handleEvent(event);
		}
		gameWindow->update();
		gameWindow->parseQueue();
		gameWindow->draw();
		SDL_RenderPresent(renderer);
		unsigned int elapsedTime = SDL_GetTicks() - lastTime;
		lastTime = SDL_GetTicks();
		SDL_Delay(elapsedTime <= MS_DELAY ? MS_DELAY - elapsedTime : 0);
	}

	//quit SDL
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	TTF_Quit();
	Mix_CloseAudio();
	Mix_Quit();
	SDL_Quit();

	//garbage collect
	delete(gameWindow);
	
	//and done
	return 0;
}
