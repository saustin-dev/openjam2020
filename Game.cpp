//The main game file
#include <iostream>
#include <fstream>
#include <vector>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "WindowAbstraction.h"

/**
 * Store the coordinates of the mouse pointer
 */
int mouseX = 0;
int mouseY = 0;
/**
 * Window dimensions
 */
int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 1024;
/**
 * The default window title
 */
std::string WINDOW_TITLE = "Game Window";

//Building blocks for menus
//-------------------------------------------------------------------------
/**
 * A button that changes color when hovered over
 */
class Button {
	protected:
	SpecificElement *bg;
	SpecificElement *bg2;
	SpecificElement *text;
	bool hovered;
	
	public:
	Button(SDL_Renderer *renderer, std::string buttonText, SDL_Color color1, SDL_Color color2, SDL_Rect rect) {
		bg = new SpecificElement(new ColorTile(color1, renderer), rect);
		bg2 = new SpecificElement(new ColorTile(color2, renderer), rect);
		int offX = rect.w/10;
		int offY = rect.h/10;
		text = new SpecificElement(new TextTile(buttonText, renderer), { rect.x+offX, rect.y+offY, rect.w-2*offX, rect.h-2*offY });
		hovered = false;
	}
	~Button() {
		if(bg) delete(bg);
		if(bg2) delete(bg2);
		if(text) delete(text);
	}
	
	void hover(int mouseX, int mouseY) {
		if(click(mouseX, mouseY)) {
			hovered = true;
		}
		else {
			hovered = false;
		}
	}
	
	bool click(int mouseX, int mouseY) {
		return bg->click(mouseX, mouseY);
	}
	
	void draw() {
		if(hovered) {
			bg2->draw();
		}
		else {
			bg->draw();
		}
		text->draw();
	}
};

/**
 * Generic class for things that can be rendered in a window
 */
class Visual {
	public:
	virtual ~Visual() {
	};
	
	virtual void draw() {
	};
	virtual void hover() {
	};
	virtual int click() {
		return -1;
	};
};
/**
 * Simple class for quick and easy menus
 */
class Menu : public Visual{
	protected:
	std::vector<SpecificElement*> elements;
	std::vector<Button*> buttonVector;
	SDL_Renderer *renderer;
	std::string title;
	std::string background;
	int buttons;
	int side;
	std::vector<std::string> buttonLabels;
	
	public:
	Menu(SDL_Renderer *renderer, std::string title, std::string background, int buttons, std::string buttonLabels[], int side) {
		this->renderer = renderer;
		this->title = title;
		this->background = background;
		this->buttons = buttons;
		for(int i = 0; i < buttons; i++) {
			this->buttonLabels.push_back(buttonLabels[i]);
		}
		this->side = side;
		
		build();
	}
	~Menu() {
		destroy();
	}
	
	void destroy() {
		while(elements.size()) {
			if(elements.back()) delete(elements.back());
			elements.pop_back();
		}
		while(buttonVector.size()) {
			if(buttonVector.back()) delete(buttonVector.back());
			elements.pop_back();
		}
	}
	
	void build() {
		destroy();
		
		//add the background
		elements.push_back(new SpecificElement(new ImageTile(background, renderer), { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }));
		
		//establish which part of the screen the menu buttons will occupy
		SDL_Rect rect;
		if(side < 0) {
			rect = { 0, 0, SCREEN_WIDTH/3, SCREEN_HEIGHT };
		}
		else if(side == 0) {
			rect = { SCREEN_WIDTH/3, 0, SCREEN_WIDTH/3, SCREEN_HEIGHT };
		}
		else {
			rect = { 2*SCREEN_WIDTH/3, 0, SCREEN_WIDTH/3, SCREEN_HEIGHT };
		}
		
		//get each part of the menu its space rectangle
		int verticalUnits = buttons + 1;
		SDL_Rect subrect = rect;
		subrect.h /= verticalUnits;
		//add the title
		int offX = subrect.w/10;
		int offY = subrect.h/7;
		elements.push_back(new SpecificElement(new TextTile(title, renderer), { subrect.x+offX, subrect.y+offY, subrect.w-2*offX, subrect.h-2*offY }));
		//add the buttons
		for(int i = 0; i < buttons; i++) {
			subrect.y += subrect.h;
			buttonVector.push_back(new Button(renderer, buttonLabels.at(i), { 150, 150, 150, 255 }, { 200, 200, 200, 255 }, { subrect.x+offX, subrect.y+offY, subrect.w-2*offX, subrect.h-2*offY } ));
		}	
	}
	
	void hover() {
		hover(mouseX, mouseY);
	}
	
	void hover(int mouseX, int mouseY) {
		for(int i = 0; i < buttons; i++) {
			buttonVector.at(i)->hover(mouseX, mouseY);
		}
	}
	
	int click() {
		return click(mouseX, mouseY);
	}
	
	int click(int mouseX, int mouseY) {
		int clicked = -1;
		for(int i = 0; i < buttons; i++) {
			if(buttonVector.at(i)->click(mouseX, mouseY)) {
				clicked = i;
				break;
			}
		}
		return clicked;
	}
	
	void draw() {
		for(unsigned int i = 0; i < elements.size(); i++) {
			elements.at(i)->draw();
		}
		for(int i = 0; i < buttons; i++) {
			buttonVector.at(i)->draw();
		}
	}
		
};

/**
 * The logic for each view will be handled separately
 */
class Logic {
	public:
	virtual ~Logic() {
	}
	
	virtual void handleEvent(SDL_Event event, int clickButton) {
	};
};

/**
 * The highest level which will switch between Visuals such as menus or the game
 */
class Window {
	protected:
	Logic *activeLogic;
	Visual *activeVisual;
	std::vector<Logic*> logics;
	std::vector<Visual> visuals;
	bool quit;
	
	public:
	virtual ~Window() {
	};
	void draw(){
		activeVisual->draw();
	}
	virtual void handleEvent(SDL_Event event){
	};
	bool shouldQuit(){
		return quit;
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
	SDL_Renderer *renderer  = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
	SDL_Event event;
	
	std::string buttons[5] = {"button 1","button 2","button 3","button 4","button 5"};
	Menu *testMenu = new Menu(renderer, "test menu", "space bg 3.bmp",5, buttons, 1);
	
	//main loop
	bool run = true;
	while(run) {
		while(SDL_PollEvent(&event)) {
			SDL_GetMouseState(&mouseX, &mouseY);
			testMenu->hover(mouseX, mouseY);
		}
		testMenu->draw();
		SDL_RenderPresent(renderer);
	}
	
	//quit SDL
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	//garbage collect
	delete(testMenu);
	
	//and done
	return 0;
}
