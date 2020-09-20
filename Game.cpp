//The main game file
#include <iostream>
#include <fstream>
#include <vector>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "WindowAbstraction.h"
#include "WindowsAndMenus.h"

/**
 * Store the coordinates of the mouse pointer
 */
int mouseX = 0;
int mouseY = 0;
/**
 * Default window dimensions
 */
int const SCREEN_WIDTH = 1280;
int const SCREEN_HEIGHT = 720;
/**
 * The default window title
 */
std::string WINDOW_TITLE = "Game Window";

/**
 * Windowed resolutions to toggle through
 */
int const X_RESOLUTIONS[2][3] = { { 640, 1024, 1600 }, { 720, 1280, 1600 } };
int const Y_RESOLUTIONS[2][3] = { { 480, 768, 1200 }, { 480, 720, 900 } };
int ratio = 1;
int res = 1;

class GameWindow : public Window {
	public:
	GameWindow(SDL_Renderer *renderer, SDL_Window *window) {
		this->window = window;
		SDL_GetWindowSize(window,&SCREEN_WIDTH,&SCREEN_HEIGHT);
		quit = false;
		this->renderer = renderer;
		activeVisual = nullptr;
		this->activeTitle = "Main Menu";
		
		build();
		
	}
	~GameWindow() {
		destroy();
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
		Menu *mainMenu = new Menu(renderer, "Main Menu", "Assets/Image/space bg 3.bmp",3, buttons, 1, SCREEN_WIDTH, SCREEN_HEIGHT);
		visuals.push_back(mainMenu);
		std::string buttons2[4] = {"Fullscreen","Switch Resolution","Switch Ratio", "Go Back"};
		Menu *optionsMenu = new Menu(renderer, "Options", "Assets/Image/space bg 3.bmp",4, buttons2, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		visuals.push_back(optionsMenu);
		
		std::string buttons3[3] = {"New Game","Load Game","Go Back"};
		Menu *fileMenu = new Menu(renderer, "Play Game", "Assets/Image/space bg 3.bmp",3, buttons3, -1, SCREEN_WIDTH, SCREEN_HEIGHT);
		visuals.push_back(fileMenu);
		
		changeVisual(activeTitle);
	}
	
	void resize(int width, int height) {
		SCREEN_HEIGHT = height;
		SCREEN_WIDTH = width;
		SDL_SetWindowSize(window, width, height);
		build();
	}
	
	void changeVisual(std::string title) {
		for(unsigned int i = 0; i < visuals.size(); i++) {
			if(visuals.at(i)->getTitle() == title) {
				activeVisual = visuals.at(i);
				activeTitle = title;
				break;
			}
		}
	}
	
	void handleEvent(SDL_Event event) {
		activeVisual->hover(mouseX, mouseY);
		
		//Only care about left clicks
		if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
			int clicked = activeVisual->click(mouseX, mouseY);
			if(activeVisual->getTitle() == "Main Menu") {
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
						changeVisual("Main Menu");
						break;
				}
			}
			else if(activeVisual->getTitle() == "Play Game") {
				switch (clicked) {
					case 0:
						//New game
						break;
					case 1:
						//Load game
						break;
					case 2:
						changeVisual("Main Menu");
						break;
				}
			}
		}
		//Keys matter
		else if(event.type == SDL_KEYDOWN) {
			
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
	SDL_Init(0);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	SDL_Window *window = SDL_CreateWindow(WINDOW_TITLE.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,0);
	SDL_SetWindowResizable(window,SDL_TRUE);
	SDL_Renderer *renderer  = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
	SDL_Event event;
	
	GameWindow *gameWindow = new GameWindow(renderer, window);
	
	//main loop
	while(!gameWindow->shouldQuit()) {
		while(SDL_PollEvent(&event)) {
			SDL_GetMouseState(&mouseX, &mouseY);
			gameWindow->handleEvent(event);
		}
		gameWindow->draw();
		SDL_RenderPresent(renderer);
	}

	//quit SDL
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	//garbage collect
	delete(gameWindow);
	
	//and done
	return 0;
}
