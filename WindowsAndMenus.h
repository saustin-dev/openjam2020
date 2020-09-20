//More objects and stuff
#include <iostream>
#include <fstream>
#include <vector>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "WindowAbstraction.h"

#ifndef WINDOWSANDMENUS_H
#define WINDOWSANDMENUS_H

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
	virtual std::string getTitle() = 0;
	virtual void draw() {
	};
	virtual void hover(int mouseX, int mouseY) {
	};
	virtual int click(int mouseX, int mouseY) = 0;
	virtual std::string onActive() = 0;
	virtual void update() {
	}
	virtual void handleInput(SDL_Event event) {
	}
};
/**
 * Simple class for quick and easy menus
 */
class Menu : public Visual {
	protected:
	std::vector<SpecificElement*> elements;
	std::vector<Button*> buttonVector;
	SDL_Renderer *renderer;
	std::string title;
	std::string background;
	int buttons;
	int side;
	std::vector<std::string> buttonLabels;
	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;
	std::string activeCommand;
	
	public:
	Menu(SDL_Renderer *renderer, std::string title, std::string background, std::string activeCommand, int buttons, std::string buttonLabels[], int side, int screenWidth, int screenHeight) {
		this->renderer = renderer;
		this->title = title;
		this->background = background;
		this->buttons = buttons;
		for(int i = 0; i < buttons; i++) {
			this->buttonLabels.push_back(buttonLabels[i]);
		}
		this->side = side;
		this->SCREEN_WIDTH = screenWidth;
		this->SCREEN_HEIGHT = screenHeight;
		this->activeCommand = activeCommand;
		
		build();
	}
	~Menu() {
		destroy();
	}
	
	std::string getTitle() {
		return title;
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
	
	void hover(int mouseX, int mouseY) {
		for(int i = 0; i < buttons; i++) {
			buttonVector.at(i)->hover(mouseX, mouseY);
		}
	}
	
	void resize(int width, int height) {
		SCREEN_WIDTH = width;
		SCREEN_HEIGHT = height;
		build();
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
	
	std::string onActive() {
		return activeCommand;
	}
	
	void update() {
	}
	
	void handleEvent(SDL_Event event) {
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
	std::string activeTitle;
	std::vector<Logic*> logics;
	std::vector<Visual*> visuals;
	bool quit;
	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;
	SDL_Renderer *renderer;
	SDL_Window *window;
	
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

#endif
