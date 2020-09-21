//Putting some of the reusable stuff from the editor in a header file
#include <iostream>
#include <fstream>
#include <vector>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#ifndef WINDOWABSTRACTION_H
#define WINDOWABSTRACTION_H

/**
 * The font to use
 */
std::string const FONT_NAME = "Assets/TTF/Ubuntu-R.ttf";
int const FONT_SIZE = 64;

//Visual output
//-------------------------------------------------------------------------
/**
 * A thing that can be drawn
 * MapTile wasn't a very good name for this
 */
class MapTile {
	protected:
	SDL_Texture *texture;
	SDL_Renderer *renderer;
	
	public:
	MapTile() {
	}
	virtual ~MapTile() {
	};
	virtual void draw(SDL_Rect rect) {
	};
};

/**
 * Just a box filled with color
 */
class ColorTile : public MapTile {
	protected:
	SDL_Color color;
	
	public:
	ColorTile(SDL_Color color, SDL_Renderer *renderer) {
		this->renderer = renderer;
		this->color = color;
	}
	
	void updateColor(SDL_Color color) {
		this->color = color;
	}
	
	void draw(SDL_Rect rect) {
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(renderer, &rect);
	}
};

/**
 * Draw some text
 */
class TextTile : public MapTile {
	protected:
	std::string text;
	
	public:
	TextTile(std::string text, SDL_Renderer *renderer) {
		this->renderer = renderer;
		setText(text);
	}
	
	void setText(std::string text) {
		this->text = text;
		TTF_Font *font = TTF_OpenFont(FONT_NAME.c_str(), FONT_SIZE);
		SDL_Surface *surface = TTF_RenderText_Solid(font,text.c_str(), {0, 0, 0});
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		TTF_CloseFont(font);
	}
	
	void draw(SDL_Rect rect) {
		SDL_RenderCopy(renderer, texture, NULL, &rect);
	}
};

/**
 * Basic abstraction of a single image
 */
class ImageTile : public MapTile {
	public:
	ImageTile(std::string filename, SDL_Renderer *renderer) {
		SDL_Surface *surface =  IMG_Load(filename.c_str());
		if(!surface) 
			throw;
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		this->renderer = renderer;
	}
	~ImageTile() {
		SDL_DestroyTexture(texture);
	}
	
	void draw(SDL_Rect rect) {
		SDL_RenderCopy(renderer, texture, NULL, &rect);
	}
};

/**
 * This draws tilesets
 */
class TilesetDrawer {
	protected:
	SDL_Texture *texture;
	SDL_Renderer *renderer;
	int w;
	int h;
	int squareSide;
	
	public:
	TilesetDrawer(std::string filename, SDL_Renderer *renderer, int squareSide) {
		SDL_Surface *surface =  IMG_Load(filename.c_str());
		if(!surface) 
			throw;
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		this->renderer = renderer;
		SDL_QueryTexture(texture, NULL, NULL, &w, &h);
		this->squareSide = squareSide;
	}
	~TilesetDrawer() {
		SDL_DestroyTexture(texture);
	}
	
	void draw(SDL_Rect rect, int index) {
		if(index < 0)
			return;
		SDL_Rect src;
		src.w = squareSide;
		src.h = squareSide;
		int columns = w/squareSide;
		int x = 0;
		int y = 0;
		if(index > 0) {
			x = index%columns;
			y = index/columns;
		}
		src.x = x*squareSide;
		src.y = y*squareSide;
		SDL_RenderCopy(renderer,texture,&src,&rect);
		//printf("drawing %d at %d,%d (%d x %d)\n",index,rect.x,rect.y,rect.w,rect.h);
	}
	
	int tileCount() {
		int columns = w/squareSide;
		int rows = h/squareSide;
		return columns * rows;
	}
	
	int tileSize() {
		return squareSide;
	}
	
	void setTileSize(int tileSize) {
		squareSide = tileSize;
	}
};

//Window Elements
//-------------------------------------------------------------------------
/**
 * This has a MapTile but also stores its location and can tell if it 
 * is clicked on
 */
class SpecificElement {
	private:
	SDL_Rect rect;
	MapTile *element;
	
	
	public:
	SpecificElement(MapTile *element, SDL_Rect rect) {
		this->element = element;
		this->rect = rect;
	}
	~SpecificElement() {
		delete(element);
	}
	
	bool click(int mouseX, int mouseY) {
		return (mouseX >= rect.x && mouseX <= rect.x + rect.w && mouseY >= rect.y && mouseY <= rect.y + rect.h);
	}
	
	void draw() {
		element->draw(rect);
	}
	
	void updateColor(SDL_Color color) {
		ColorTile *cElement = dynamic_cast<ColorTile*>(element);
		if(cElement) {
			cElement->updateColor(color);
		}
	}
	
	SDL_Rect getRect() {
		return rect;
	}
	int getW() {
		return rect.w;
	}
	int getH() {
		return rect.h;
	}
	int getX() {
		return rect.x;
	}
	int getY() {
		return rect.y;
	}
	
};

/**
 * Like the former but for tilesets
 */
class SpecificTile {
	private:
	TilesetDrawer *tileset;
	SDL_Rect rect;
	int index;
	
	public:
	SpecificTile(TilesetDrawer *tileset, SDL_Rect rect, int index) {
		this->tileset = tileset;
		this->rect = rect;
		this->index = index;
	}
	SpecificTile(TilesetDrawer *tileset, SDL_Rect rect) : SpecificTile(tileset, rect, 0) {
	}
	
	void updateValue(int index) {
		this->index = index;
	}
	
	bool click(int mouseX, int mouseY) {
		return (mouseX >= rect.x && mouseX <= rect.x + rect.w && mouseY >= rect.y && mouseY <= rect.y + rect.h);
	}
	
	void draw() {
		tileset->draw(rect,index);
	}
	
	int getIndex() {
		return index;
	}
	
	SDL_Rect getRect() {
		return rect;
	}
	int getW() {
		return rect.w;
	}
	int getH() {
		return rect.h;
	}
	int getX() {
		return rect.x;
	}
	int getY() {
		return rect.y;
	}
};

#endif
