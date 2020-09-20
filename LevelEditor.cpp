//Basic tools for a window for a level editor
#include <iostream>
#include <fstream>
#include <vector>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

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
std::string WINDOW_TITLE = "Level Editor";
/**
 * The font to use
 */
std::string const FONT_NAME = "Assets/TTF/Ubuntu-R.ttf";
/**
 * The default folder to save levels
 */
 std::string const DEFAULT_DIRECTORY = "Maps/";

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
		TTF_Font *font = TTF_OpenFont(FONT_NAME.c_str(), 32);
		if(!font)
			throw;
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
	}
	
	int tileCount() {
		int columns = w/squareSide;
		int rows = h/squareSide;
		return columns * rows;
	}
	
	int tileSize() {
		return squareSide;
	}
};

//Data management
//-------------------------------------------------------------------------
void setWindowTitle(std::string filename) {
	if(filename == "") {
		WINDOW_TITLE += " : <new file>";
	}
	else {
		WINDOW_TITLE += " : " + filename;
	}
}

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
	
	bool click() {
		return (mouseX >= rect.x && mouseX <= rect.x + rect.w && mouseY >= rect.y && mouseY <= rect.y + rect.h);
	}
	
	void draw() {
		element->draw(rect);
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
	
	bool click() {
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

void floodFill(MapData *mapData, int x, int y, int newValue) {
	printf("Fill called with value=%d\n",newValue);
	if(x < 0 || y < 0 || x>=mapData->getW() || y>=mapData->getH()) {
		return;
	}
	
	int w = mapData->getW();
	int h = mapData->getH();
	int oldValue = mapData->getData()[y][x];
	int **data = mapData->getData();
	data[y][x] = -2;
	
	bool changed = true;
	while(changed) {
		changed = false;
		for(int y = 0; y < h; y++) {
			for(int x = 0; x < w; x++) {
				if(data[y][x] == oldValue) {
					if(x>0 && data[y][x-1] == -2) {
						data[y][x] = -2;
						changed = true;
						continue;
					}
					if(x<w-1 && data[y][x+1] == -2) {
						data[y][x] = -2;
						changed = true;
						continue;
					}
					if(y>0 && data[y-1][x] == -2) {
						data[y][x] = -2;
						changed = true;
						continue;
					}
					if(y<h-1 && data[y+1][x] == -2) {
						data[y][x] = -2;
						changed = true;
						continue;
					}
				}
			}
		}
	}
	for(int y = 0; y < h; y++) {
		for(int x = 0; x < w; x++) {
			if(data[y][x] == -2) {
				data[y][x] = newValue;
			}
		}
	}
}

/**
 * Most of the program
 */
class WindowManager {
	private:
	std::vector<SpecificTile*> tilesetTiles;
	std::vector<SpecificTile*> mapTiles;
	SpecificElement *sidePanel;
	SpecificElement *background;
	SpecificElement *activeText;
	SpecificTile *activeTile;
	SDL_Renderer *renderer;
	int activeIndex;
	TilesetDrawer *tileset;
	int tileSize;
	int mapTileSize;
	int mapW;
	MapData *data;
	bool lmbDown;
	
	public:
	WindowManager(SDL_Renderer *renderer, std::string tilesetName, int tilesize, MapData *data) {
		this->renderer = renderer;
		sidePanel = NULL;
		background = NULL;
		activeIndex = -1;
		this->tileset = new TilesetDrawer(tilesetName, renderer, tilesize);
		tileSize = tileset->tileSize();
		mapTileSize = tileSize;
		this->data = data;
		mapW = 1;
		lmbDown = false;
		build();
	}
	
	~WindowManager() {
		destroy();
	}
	
	void destroy() {
		while(tilesetTiles.size()) {
			if(tilesetTiles.back()) delete(tilesetTiles.back());
			tilesetTiles.pop_back();
		}
		while(mapTiles.size()) {
			if(mapTiles.back()) delete(mapTiles.back());
			mapTiles.pop_back();
		}
		if(sidePanel) delete(sidePanel);
		if(background) delete(background);
		if(activeText) delete(activeText);
	}
	
	/**
	 * Assemble all the elements into the vector
	 * Call again every time window is resized
	 */
	void build() {
		destroy();
		//build the backgrounds
		background = new SpecificElement(new ColorTile({200, 200, 200, 255}, renderer), {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
		sidePanel = new SpecificElement(new ColorTile({121, 121, 121, 255}, renderer), {0, 0, SCREEN_WIDTH/4, SCREEN_HEIGHT});
		//build the tile palette
		int columns = (sidePanel->getW()-1) / (tileSize+1);
		int x = 0;
		int y = 0;
		for(int i = 0; i < tileset->tileCount(); i++) {
			tilesetTiles.push_back(new SpecificTile(tileset, { (x*(tileSize+1))+1, (y*(tileSize+1))+1, tileSize, tileSize }, i));
			x++;
			if(x >= columns) {
				x = 0;
				y++;
			}
		}
		//build the active text/tile
		activeText = new SpecificElement(new TextTile("Active Element:", renderer), {0,(int)14.5*SCREEN_HEIGHT/16,SCREEN_WIDTH/8,SCREEN_WIDTH/24});
		activeTile = new SpecificTile(tileset,{SCREEN_WIDTH/8 + (SCREEN_WIDTH/8-SCREEN_HEIGHT/16)/2,7*SCREEN_HEIGHT/8, SCREEN_HEIGHT/16,SCREEN_HEIGHT/16},activeIndex);
		//construct the map
		int mapH = data->getH();
		mapW = data->getW();
		int wFit = (3*(SCREEN_WIDTH/4) - 2) / mapW;
		int hFit = (SCREEN_HEIGHT - 2) / mapH;
		mapTileSize = wFit > hFit ? hFit : wFit;
		int vertOffset = ((SCREEN_HEIGHT - 2) - mapH * mapTileSize) / 2;
		int horiOffset = (((3*(SCREEN_WIDTH/4) - 2) - mapW * mapTileSize) / 2) + SCREEN_WIDTH/4;
		int **data = this->data->getData();
		for(int y = 0; y < mapH; y++) {
			for(int x = 0; x < mapW; x++) {
				mapTiles.push_back(new SpecificTile(tileset, { (x*(mapTileSize))+horiOffset, (y*(mapTileSize))+vertOffset, mapTileSize, mapTileSize }, data[y][x]));
			}
		}
	}
	
	/**
	 * Draw everything
	 */
	void draw() {
		//draw background
		background->draw();
		//draw side panel background
		sidePanel->draw();
		//then draw tileset vector
		for(unsigned int i = 0; i < tilesetTiles.size(); i++) {
			tilesetTiles.at(i)->draw();
		}
		//then draw active tile
		activeText->draw();
		if(activeIndex >= 0) {
			activeTile->draw();
		}
		//then draw map
		for(unsigned int i = 0; i < mapTiles.size(); i++) {
			mapTiles.at(i)->draw();
			//then draw grid over top
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_Rect outline = mapTiles.at(i)->getRect();
			SDL_RenderDrawRect(renderer, &outline);
		}
	}
	
	/**
	 * Handle logic
	 */
	bool update(SDL_Event event) {
		bool returnValue = true;
		
		//first handle explicit SDL events
		/**
		 * Clicking logic
		 */
		if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
			//if clicked side panel, check tileset vector
			if(sidePanel->click()) {
				bool wasTile = false;
				for(unsigned int i = 0; i < tilesetTiles.size(); i++) {
					if(tilesetTiles.at(i)->click()) {
						activeIndex = tilesetTiles.at(i)->getIndex();
						wasTile = true;
						break;
					}
				}
				if(!wasTile) activeIndex = -1;
				activeTile->updateValue(activeIndex);
			}
			lmbDown = true;
		}
		if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
			lmbDown = false;
		}
		/**
		 * Flood fill when press F
		 */
		if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_f) {
			if(!sidePanel->click() && activeIndex >= -1) {
				int i = 0;
				for(; (unsigned int)i < mapTiles.size(); i++) {
					if(mapTiles.at(i)->click()) {
						break;
					}
				}
				floodFill(data,i%mapW,i/mapW,activeIndex);
				build();
			}
		}
		/**
		 * Quit if window is closed
		 */
		if(event.type == SDL_WINDOWEVENT) {
			if(event.window.event == SDL_WINDOWEVENT_CLOSE) {
				returnValue = false;
			}
			else if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
				SCREEN_WIDTH = event.window.data1;
				SCREEN_HEIGHT = event.window.data2;
				build();
			}
		}
		
		//then other cases
		if(lmbDown) {
			//check map array if mouse is not in side panel
			if(!sidePanel->click()) {
				for(unsigned int i = 0; i < mapTiles.size(); i++) {
					if(mapTiles.at(i)->click()) {
						mapTiles.at(i)->updateValue(activeIndex);
						data->getData()[i/mapW][i%mapW] = activeIndex;
						break;
					}
				}
			}
		}

		SDL_PumpEvents();
		SDL_FlushEvents(SDL_FIRSTEVENT,SDL_APP_DIDENTERFOREGROUND);
		SDL_FlushEvents(SDL_TEXTEDITING,SDL_MOUSEMOTION);
		SDL_FlushEvents(SDL_MOUSEWHEEL,SDL_LASTEVENT);
		return returnValue;
	}
};

//Console IO
//-------------------------------------------------------------------------
/**
 * Clear the buffer
 */
void clear() {
	while(getchar() != '\n');
}

/**
 * Use CLI to prompt user for data
 */
void getInfo(std::string *input, int *width, int *height, std::string *tileset, int *tilesize) {
	printf("New file? (y/n): ");
	char newFile[2] = { 0 };
	int num = scanf("%1[ynYN]", newFile);
	clear();
	if(num && (newFile[0] == 'y' || newFile[0] == 'Y')) {
		*input = "";
		printf("Map width?: ");
		if(!scanf("%d",width) || *width < 1) {
			printf("Invalid input, aborting...\n");
			exit(EXIT_FAILURE);
		}
		clear();
		printf("Map height?: ");
		if(!scanf("%d",height) || *height < 1) {
			printf("Invalid input, aborting...\n");
			exit(EXIT_FAILURE);
		}
		clear();
	}
	else if(num && (newFile[0] == 'n' || newFile[0] == 'N')) {
		printf("Input file name?: ");
		char filename[256] = { 0 };
		scanf("%255s", filename);
		clear();
		FILE *fp = fopen(filename, "r");
		if(!fp) {
			printf("Cannot open file, aborting...\n");
			exit(EXIT_FAILURE);
		}
		fclose(fp);
		std::string newfilename(filename);
		*input = newfilename;
		//these will be set when we load the file
		*width = -1;
		*height = -1;
	}
	else {
		printf("Invalid input, aborting...\n");
		exit(EXIT_FAILURE);
	}
	//get tileset info
	printf("Input tileset name?: ");
	char filename[256] = { 0 };
	scanf("%255s", filename);
	clear();
	FILE *fp = fopen(filename, "r");
	if(!fp) {
		printf("Cannot open tileset, aborting...\n");
		exit(EXIT_FAILURE);
	}
	fclose(fp);
	/*if(strlen(filename) < 5 || strcmp((char*)filename+strlen(filename)-4,".bmp")) {
		printf("Tileset must be .bmp format, aborting...\n");
		exit(EXIT_FAILURE);
	}*/
	std::string newfilename(filename);
	*tileset = newfilename;
	
	printf("Tile size? (for 32x32 enter 32, etc): ");
	if(!scanf("%d",tilesize) || *tilesize < 1) {
		printf("Invalid input, aborting...\n");
		exit(EXIT_FAILURE);
	}
	clear();
}

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

/**
 * Write the file as a bunch of ints. Width, then height, then each row left to right
 */
void writeFile(std::string filename, MapData *data) {
	FILE *fp = fopen(filename.c_str(), "w");
	if(!fp) {
		throw new std::exception;
	}
	putw(data->getW(), fp);
	putw(data->getH(), fp);
	int **theData = data->getData();
	for(int y = 0; y < data->getH(); y++) {
		for(int x = 0; x < data->getW(); x++) {
			putw(theData[y][x], fp);
		}
	}
}

/**
 * Use CLI to prompt user for output filename, then save the map data
 */
void saveFile(MapData *data) {
	std::string outputFilename = "";
	while(outputFilename == "") {
		printf("Output file name?: ");
		char filename[256] = { 0 };
		scanf("%255s", filename);
		clear();
		if(!strcmp(filename, "")) {
			printf("Please enter a filename.\n");
			continue;
		}
		FILE *fp = fopen(filename, "r");
		if(fp) {
			fclose(fp);
			printf("File already exists, overwrite? (y,n): ");
			char newFile[2] = { 0 };
			fflush(stdin);
			int num = scanf("%1[ynYN]", newFile);
			clear();
			if(num && (newFile[0] == 'y' || newFile[0] == 'Y')) {
				std::string newfilename(filename);
				outputFilename = newfilename;
			}
			if(num && (newFile[0] == 'n' || newFile[0] == 'N')) {
				continue;
			}
			else {
				printf("Invalid input.\n");
				continue;
			}
		}
		std::string newfilename(filename);
		outputFilename = newfilename;
		try {
			writeFile(outputFilename, data);
		} catch (std::exception e) {
			outputFilename = "";
			printf("Failed to write to file.");
		}
	}
}



//Real Code
//-------------------------------------------------------------------------
int main() {
	//get information: input file name, output file name, map size, tileset, tile size
	std::string input;
	int width;
	int height;
	std::string tileset;
	int tilesize;
	getInfo(&input, &width, &height, &tileset, &tilesize);
	setWindowTitle(input);
	MapData *mapData = NULL;
	if(input == "") {
		mapData = new MapData(width, height);
	}
	else {
		try {
			mapData = readFile(input);
		} catch(std::exception e) {
			printf("Could not read input file, aborting...");
			delete(mapData);
			exit(EXIT_FAILURE);
		}
	}
	//printf("Loaded file: %d x %d\n",mapData->getW(), mapData->getH());
	
	//start SDL
	SDL_Init(0);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	SDL_Window *window = SDL_CreateWindow(WINDOW_TITLE.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,0);
	SDL_SetWindowResizable(window,SDL_TRUE);
	SDL_Renderer *renderer  = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
	SDL_Event event;
	WindowManager *windowManager = new WindowManager(renderer, tileset, tilesize, mapData);
	
	//main loop
	bool run = true;
	while(run) {
		while(SDL_PollEvent(&event)) {
			SDL_GetMouseState(&mouseX, &mouseY);
			run = windowManager->update(event);
		}
		windowManager->draw();
		SDL_RenderPresent(renderer);
	}
	
	//quit SDL
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	//save file
	saveFile(mapData);
	
	//garbage collect
	delete(windowManager);
	delete(mapData);
	
	//and done
	return 0;
}
