//Code for the player
#include <iostream>
#include <fstream>
#include <vector>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"
#include "GameData.h"


#ifndef PLAYERLOGIC_H
#define PLAYERLOGIC_H

/**
 * Size of player in pixels
 */
int const WIDTH = 75;
int const HEIGHT = 100;
/**
 * Constants for speed and such
 */
int const GRAVITY = 300;
int const RUN_SPEED = 300;
int const MAX_YVEL = 1000;
int const JUMP_YVEL = -500;

bool leftDown;
bool rightDown;
bool downDown;

std::string const SPRITE_DIRECTORY = "Assets/Image/";

class Player {
	private:
	class PlayerCollider {
		private:
		SDL_Rect rect;
		int xvel;
		int yvel;
		int gravity;
		Player *parent;
		unsigned int lastTime;
		MapData *map;
		int tileSize;
		
		public:
		PlayerCollider(int xpos, int ypos, Player *parent, MapData *map, int tileSize) {
			changeMap(map, xpos, ypos);
			xvel = 0;
			yvel = 0;
			gravity = 0;
			this->tileSize = tileSize;
			this->parent = parent;
		}
		~PlayerCollider() {
		}
		
		SDL_Rect getRect() {
			return rect;
		}
		
		void changeMap(MapData *map, int x, int y) {
			rect = { x, y, WIDTH, HEIGHT };
			this->map = map;
		}
		
		void setGravity(int grav) {
			if(grav > 0) {
				gravity = GRAVITY;
			}
			else if(grav < 0) {
				gravity = GRAVITY/2;
			}
			else {
				gravity = 0;
			}
		}
		
		void move(bool direction) {
			if(!direction)
				xvel = -1*RUN_SPEED;
			else
				xvel = RUN_SPEED;
		}
		
		void update() {
			unsigned int elapsedTime = SDL_GetTicks() - lastTime;
			lastTime = SDL_GetTicks();
			//check collision, move if possible, and if collided tell parent state
			yvel += gravity*(double)elapsedTime/1000.0;
			
			int xMov = xvel*(double)elapsedTime/1000.0;
			//left:
			while(xMov < 0) {
				bool collision = false;
				int x = rect.x-1;
				for(int y = rect.y; y < rect.y+rect.h; y += tileSize) {
					if(checkCollision(x,y)) {
						collision = true;
						break;
					}
				}
				int y = rect.y+rect.h;
				if(checkCollision(x,y)) {
						collision = true;
				}
				if(collision) {
					parent->onCollideLeft();
					break;
				}
				else {
					rect.x--;
					xMov++;
				}
			}
			//right:
			while(xMov > 0) {
				bool collision = false;
				int x = rect.x+rect.w+1;
				for(int y = rect.y; y < rect.y+rect.h; y += tileSize) {
					if(checkCollision(x,y)) {
						collision = true;
						break;
					}
				}
				int y = rect.y+rect.h;
				if(checkCollision(x,y)) {
						collision = true;
				}
				if(collision) {
					parent->onCollideRight();
					break;
				}
				else {
					rect.x++;
					xMov--;
				}
			}
			
			int yMov = yvel*(double)elapsedTime/1000.0;
			//up:
			while(yMov < 0) {
				bool collision = false;
				int y = rect.y-1;
				for(int x = rect.x; x < rect.x+rect.w; x += tileSize) {
					if(checkCollision(x,y)) {
						collision = true;
						break;
					}
				}
				int x = rect.x+rect.w;
				if(checkCollision(x,y)) {
						collision = true;
				}
				if(collision) {
					parent->onCollideTop();
					break;
				}
				else {
					rect.y--;
					yMov++;
				}
			}
			//down:
			bool collidedBottom = false;
			while(yMov >= 0) {
				bool collision = false;
				int y = rect.y+rect.h+1;
				for(int x = rect.x; x < rect.x+rect.w; x += tileSize) {
					if(checkCollision(x,y)) {
						collision = true;
						break;
					}
				}
				int x = rect.x+rect.w;
				if(checkCollision(x,y)) {
						collision = true;
				}
				if(collision) {
					parent->onCollideBottom();
					collidedBottom = true;
					break;
				}
				else {
					rect.y++;
					yMov--;
				}
			}
			if(!collidedBottom) {
				parent->onNoCollideBottom();
			}
		}
		
		bool checkCollision(int x, int y) {
			return map->valueAtPoint(x, y, tileSize) != -1;
		}
		
		void clearYVel() {
			yvel = 0;
		}
		
		void stop() {
			xvel = 0;
		}
		
		void jump() {
			yvel = JUMP_YVEL;
		}
	};
	
	class PlayerState {
		protected:
		std::string filename;
		std::string extension;
		int frames;
		int framerate;
		std::vector<SDL_Texture*> textures;
		SDL_Renderer *renderer;
		Player *parent;
		
		public:
		PlayerState() {
		};
		virtual ~PlayerState() {
		};
		
		std::string getName() {
			return filename;
		}
		
		void draw(SDL_Rect rect) {
			//first find which frame of the animation to draw
			unsigned int index = 0;
			if(framerate) {
				unsigned int currentTime = SDL_GetTicks();
				unsigned int pointInCurrentSecond = currentTime % 1000;
				index = pointInCurrentSecond / (1000/framerate);
			}
			//cut down to the right amount of frames
			index = index % frames;
			//then verify that there are enough frames loaded
			index = index < textures.size() ? index : 0;
			
			//finally render the texture
			if(parent->getFacing())
				SDL_RenderCopyEx(renderer, textures.at(index), NULL, &rect, 0, NULL, SDL_FLIP_HORIZONTAL);
			else
				SDL_RenderCopy(renderer, textures.at(index), NULL, &rect);
		}
		
		virtual void onCollideFront() {
		}
		virtual void onCollideTop() {
		}
		virtual void onCollideBottom() = 0;
		virtual void onNoCollideBottom() {
		}
		virtual void onLeftDown() {
			leftDown = true;
		}
		virtual void onRightDown() {
			rightDown = true;
		}
		virtual void onDownDown() {
			downDown = true;
		}
		virtual void onLeftUp() {
			leftDown = false;
		}
		virtual void onRightUp() {
			rightDown = false;
		}
		virtual void onDownUp() {
			downDown = false;
		}
		virtual void onJump() {
		}
		
		virtual void onUpdate() {
		}
		virtual void onActive() {
		}
	};
	class StandingState : public PlayerState {
		public:
		StandingState(SDL_Renderer *renderer, Player *parent) {
			filename = "standing";
			extension = ".png";
			frames = 1;
			framerate = 0;
			this->renderer = renderer;
			this->parent = parent;
			for(int i = 0; i < frames; i++) {
				SDL_Surface *surface = IMG_Load((SPRITE_DIRECTORY + filename + std::to_string(i) + extension).c_str());
				textures.push_back(SDL_CreateTextureFromSurface(renderer,surface));
				SDL_FreeSurface(surface);
			}
		}
		~StandingState() {
			while(textures.size()) {
				if(textures.back()) SDL_DestroyTexture(textures.back());
				textures.pop_back();
			}
		}
		
		//should not happen in this state
		void onCollideFront() {
		}
		void onCollideTop() {
		}
		void onCollideBottom() {
		}
		void onNoCollideBottom() {
			parent->setState("jumping");
		}
		//change facing and move to running
		void onLeftDown() {
			parent->setFacing(0);
			parent->setState("running");
		}
		//change facing and move to running
		void onRightDown() {
			parent->setFacing(1);
			parent->setState("running");
		}
		//go to crouching
		void onDownDown() {
			parent->setState("crouching");
		}
		//go to jumping
		void onJump() {
			parent->setState("jumping");
			parent->getCollision()->jump();
		}
		//do nothing
		void onLeftUp() {
		}
		void onRightUp() {
		}
		void onDownUp() {
		}
		
		//do nothing
		void onUpdate() {
			parent->getCollision()->stop();
			parent->getCollision()->clearYVel();
		}
		void onActive() {
			parent->getCollision()->stop();
			parent->getCollision()->clearYVel();
		}
	};
	class CrouchingState : public PlayerState {
		public:
		CrouchingState(SDL_Renderer *renderer, Player *parent) {
			filename = "crouching";
			extension = ".png";
			frames = 1;
			framerate = 0;
			this->renderer = renderer;
			this->parent = parent;
			for(int i = 0; i < frames; i++) {
				SDL_Surface *surface = IMG_Load((SPRITE_DIRECTORY + filename + std::to_string(i) + extension).c_str());
				textures.push_back(SDL_CreateTextureFromSurface(renderer,surface));
				SDL_FreeSurface(surface);
			}
		}
		~CrouchingState() {
			while(textures.size()) {
				if(textures.back()) SDL_DestroyTexture(textures.back());
				textures.pop_back();
			}
		}
		
		//should not happen in this state
		void onCollideFront() {
		}
		void onCollideTop() {
		}
		void onCollideBottom() {
		}
		void onNoCollideBottom() {
		}
		//change facing
		void onLeftDown() {
			parent->setFacing(0);
		}
		void onRightDown() {
			parent->setFacing(1);
		}
		//do nothing
		void onDownDown() {
		}
		void onJump() {
		}
		//do nothing
		void onLeftUp() {
		}
		void onRightUp() {
		}
		//go to standing
		void onDownUp() {
			parent->setState("standing");
		}
		
		//do nothing
		void onUpdate() {
			parent->getCollision()->stop();
			parent->getCollision()->clearYVel();
		}
		void onActive() {
			parent->getCollision()->stop();
			parent->getCollision()->clearYVel();
		}
	};
	class RunningState : public PlayerState {
		public:
		RunningState(SDL_Renderer *renderer, Player *parent) {
			filename = "running";
			extension = ".png";
			frames = 2;
			framerate = 4;
			this->renderer = renderer;
			this->parent = parent;
			for(int i = 0; i < frames; i++) {
				SDL_Surface *surface = IMG_Load((SPRITE_DIRECTORY + filename + std::to_string(i) + extension).c_str());
				textures.push_back(SDL_CreateTextureFromSurface(renderer,surface));
				SDL_FreeSurface(surface);
			}
		}
		~RunningState() {
			while(textures.size()) {
				if(textures.back()) SDL_DestroyTexture(textures.back());
				textures.pop_back();
			}
		}
		
		//go to standing
		void onCollideFront() {
			parent->setState("standing");
		}
		//should not happen
		void onCollideTop() {
		}
		void onCollideBottom() {
		}
		//go to jumping
		void onNoCollideBottom() {
			parent->setState("jumping");
		}
		//change facing
		void onLeftDown() {
			parent->setFacing(0);
			parent->getCollision()->move(parent->getFacing());
		}
		void onRightDown() {
			parent->setFacing(1);
			parent->getCollision()->move(parent->getFacing());
		}
		//go to sliding
		void onDownDown() {
			parent->setState("sliding");
		}
		//go to jumping and set yvel
		void onJump() {
			parent->setState("jumping");
			parent->getCollision()->jump();
		}
		//if facing is left, go to standing
		void onLeftUp() {
			if(!parent->getFacing()) {
				parent->setState("standing");
				parent->getCollision()->stop();
			}
		}
		//if facing is right, go to standing
		void onRightUp() {
			if(parent->getFacing()) {
				parent->setState("standing");
				parent->getCollision()->stop();
			}
		}
		//do nothing
		void onDownUp() {
		}
		
		//move
		void onUpdate() {
			parent->getCollision()->move(parent->getFacing());
		}
		void onActive() {
			parent->getCollision()->move(parent->getFacing());
		}
	};
	class SlidingState : public PlayerState {
		private:
		//ticks when slide state was entered
		unsigned int slideBeginTime;
		//how long the slide lasts in ms
		unsigned int slideDuration;
		//is the directional button held down?
		bool holdingDButton;
		
		public:
		SlidingState(SDL_Renderer *renderer, Player *parent) {
			filename = "sliding";
			extension = ".png";
			slideDuration = 1000;
			frames = 1;
			framerate = 0;
			this->renderer = renderer;
			this->parent = parent;
			for(int i = 0; i < frames; i++) {
				SDL_Surface *surface = IMG_Load((SPRITE_DIRECTORY + filename + std::to_string(i) + extension).c_str());
				textures.push_back(SDL_CreateTextureFromSurface(renderer,surface));
				SDL_FreeSurface(surface);
			}
		}
		~SlidingState() {
			while(textures.size()) {
				if(textures.back()) SDL_DestroyTexture(textures.back());
				textures.pop_back();
			}
		}
		
		//go to crouching or standing
		void onCollideFront() {
			if(downDown)
				parent->setState("crouching");
			else
				parent->setState("standing");
		}
		//should not happen
		void onCollideTop() {
		}
		void onCollideBottom() {
		}
		//go to jumping
		void onNoCollideBottom() {
			parent->setState("jumping");
		}
		//if facing is left, set holdingDButton to true
		void onLeftDown() {
			if(!parent->getFacing())
				holdingDButton = true;
		}
		//if facing is right, set holdingDButton to true
		void onRightDown() {
			if(parent->getFacing())
				holdingDButton = true;
		}
		void onDownDown() {
		}
		//do nothing
		void onJump() {
		}
		//if facing is left, set holdingDButton to false
		void onLeftUp() {
			if(!parent->getFacing())
				holdingDButton = false;
		}
		//if facing is right, set holdingDButton to false
		void onRightUp() {
			if(parent->getFacing())
				holdingDButton = false;
		}
		void onDownUp() {
		}
		
		//move, check if slide time has elapsed and if so switch to running, standing, or crouching
		void onUpdate() {
			parent->getCollision()->move(parent->getFacing());
			if(SDL_GetTicks() > slideBeginTime + slideDuration) {
				if(!holdingDButton) {
					if(downDown) {
						parent->setState("crouching");
					}
					else {
						parent->setState("standing");
					}
				}
				else {
					parent->setState("running");
				}
			}
		}
		//record current time, set holdingD/SButton to true
		void onActive() {
			slideBeginTime = SDL_GetTicks();
			parent->getCollision()->move(parent->getFacing());
		}
	};
	class JumpingState : public PlayerState {
		public:
		JumpingState(SDL_Renderer *renderer, Player *parent) {
			filename = "jumping";
			extension = ".png";
			frames = 1;
			framerate = 0;
			this->renderer = renderer;
			this->parent = parent;
			for(int i = 0; i < frames; i++) {
				SDL_Surface *surface = IMG_Load((SPRITE_DIRECTORY + filename + std::to_string(i) + extension).c_str());
				if(surface == NULL) {
					printf("%s", SDL_GetError());
				}
				textures.push_back(SDL_CreateTextureFromSurface(renderer,surface));
				SDL_FreeSurface(surface);
			}
		}
		~JumpingState() {
			while(textures.size()) {
				if(textures.back()) SDL_DestroyTexture(textures.back());
				textures.pop_back();
			}
		}
		
		//remove x velocity
		void onCollideFront() {
			parent->getCollision()->stop();
		}
		//remove y velocity
		void onCollideTop() {
			parent->getCollision()->clearYVel();
		}
		//go to standing or running
		void onCollideBottom() {
			SDL_PumpEvents();
			Uint8 const *state = SDL_GetKeyboardState(NULL);
			if(state[SDL_SCANCODE_D]) {
				if(state[SDL_SCANCODE_A]) {
					//if both directions down go to standing
					parent->setState("standing");
				}
				else {
					//if just d down go to running and facing=right
					parent->setState("running");
					parent->setFacing(1);
				}
			}
			else if(state[SDL_SCANCODE_A]) {
				//if just a down go to running and facing=left
				parent->setState("running");
				parent->setFacing(0);
			}
			else {
				//if no directions down go to standing
				parent->setState("standing");
			}
		}
		//do nothing
		void onNoCollideBottom() {
		}
		void onLeftDown() {
		}
		void onRightDown() {
		}
		void onDownDown() {
		}
		//go to gliding
		void onJump() {
			parent->setState("gliding");
		}
		//do nothing
		void onLeftUp() {
		}
		void onRightUp() {
		}
		void onDownUp() {
		}
		
		//apply gravity
		void onUpdate() {
			parent->getCollision()->setGravity(1);
		}
		void onActive() {
			parent->getCollision()->setGravity(1);
		}
	};
	class GlidingState : public PlayerState {
		public:
		GlidingState(SDL_Renderer *renderer, Player *parent) {
			filename = "gliding";
			extension = ".png";
			frames = 1;
			framerate = 0;
			this->renderer = renderer;
			this->parent = parent;
			for(int i = 0; i < frames; i++) {
				SDL_Surface *surface = IMG_Load((SPRITE_DIRECTORY + filename + std::to_string(i) + extension).c_str());
				textures.push_back(SDL_CreateTextureFromSurface(renderer,surface));
				SDL_FreeSurface(surface);
			}
		}
		~GlidingState() {
			while(textures.size()) {
				if(textures.back()) SDL_DestroyTexture(textures.back());
				textures.pop_back();
			}
		}
		
		//remove x velocity
		void onCollideFront() {
			parent->getCollision()->stop();
		}
		//remove y velocity
		void onCollideTop() {
			parent->getCollision()->clearYVel();
		}
		//go to standing or running
		void onCollideBottom() {
			if(rightDown && !leftDown) {
				parent->setFacing(1);
				parent->setState("running");
			}
			else if(!rightDown && leftDown) {
				parent->setFacing(0);
				parent->setState("running");
			}
			else {
				parent->setState("standing");
			}
		}
		//do nothing
		void onNoCollideBottom() {
		}
		//strafe
		void onLeftDown() {
			if(!rightDown)
				parent->getCollision()->move(0);
		}
		void onRightDown() {
			if(!leftDown)
				parent->getCollision()->move(1);
		}
		void onDownDown() {
		}
		void onJump() {
		}
		//stop strafing
		void onLeftUp() {
			if(!rightDown)
				parent->getCollision()->stop();
		}
		void onRightUp() {
			if(!leftDown)
				parent->getCollision()->stop();
		}
		void onDownUp() {
		}
		
		//apply gravity, change x-vel based on down keys
		void onUpdate() {
			parent->getCollision()->setGravity(-1);
			if(rightDown && !leftDown)
				parent->getCollision()->move(1);
			else if(leftDown && !rightDown)
				parent->getCollision()->move(0);
			else
				parent->getCollision()->stop();
		}
		//get what direction keys are down
		void onActive() {
			parent->getCollision()->setGravity(-1);
			SDL_PumpEvents();
			Uint8 const *state = SDL_GetKeyboardState(NULL);
			rightDown = state[SDL_SCANCODE_D];
			leftDown = state[SDL_SCANCODE_A];
		}
	};
	
	bool rightFacing;
	PlayerState *currentState;
	StandingState *standing;
	CrouchingState *crouching;
	RunningState *running;
	SlidingState *sliding;
	JumpingState *jumping;
	GlidingState *gliding;
	PlayerCollider *collision;
	
	
	
	public:
	Player(SDL_Renderer *renderer, int x, int y, MapData *mapData, int tileSize) {
		standing = new StandingState(renderer, this);
		crouching = new CrouchingState(renderer, this);
		running = new RunningState(renderer, this);
		sliding = new SlidingState(renderer, this);
		jumping = new JumpingState(renderer, this);
		gliding = new GlidingState(renderer, this);
		currentState = standing;
		
		collision = new PlayerCollider(x, y, this, mapData, tileSize);
	}
	~Player() {
		delete(standing);
		delete(crouching);
		delete(sliding);
		delete(running);
		delete(jumping);
		delete(gliding);
		
	}
	
	void changeMap(MapData *newMap, int x, int y, bool facing) {
		collision->changeMap(newMap,x,y);
		this->rightFacing = facing;
	}
	
	void changeMap(MapData *newMap, int x, int y) {
		changeMap(newMap, x, y, 1);
	}
	
	void setFacing(bool newFacing) {
		rightFacing = newFacing;
	}
	
	bool getFacing() {
		return rightFacing;
	}
	
	void setState(std::string newState) {
		if(newState == "standing") {
			currentState = standing;
		}
		else if(newState == "crouching") {
			currentState = crouching;
		}
		else if(newState == "running") {
			currentState = running;
		}
		else if(newState == "sliding") {
			currentState = sliding;
		}
		else if(newState == "jumping") {
			currentState = jumping;
		}
		else if(newState == "gliding") {
			currentState = gliding;
		}
		
		currentState->onActive();
	}
	
	void draw(SDL_Rect rect) {
		currentState->draw(rect);
	}
	
	PlayerCollider *getCollision() {
		return collision;
	}
	
	void update() {
		collision->update();
	}
	
	SDL_Rect getRect() {
		return collision->getRect();
	}
	
	void onCollideLeft() {
		if(!rightFacing){}
			currentState->onCollideFront();
	}
	
	void onCollideRight() {
		if(rightFacing){}
			currentState->onCollideFront();
	}
	
	void onCollideTop() {
		currentState->onCollideTop();
	}
	
	void onCollideBottom() {
		//printf("%s + bottom\n", currentState->getName().c_str());
		currentState->onCollideBottom();
	}
	
	void onNoCollideBottom() {
		//printf("%s + nobottom\n", currentState->getName().c_str());
		currentState->onNoCollideBottom();
	}
	
	void handleInput(SDL_Event event) {
		if(event.type == SDL_KEYDOWN) {
			switch(event.key.keysym.sym) {
				case SDLK_a:
					currentState->onLeftDown();
					break;
				case SDLK_s:
					currentState->onDownDown();
					break;
				case SDLK_d:
					currentState->onRightDown();
					break;
				case SDLK_SPACE:
					currentState->onJump();
					break;
			}
		}
		else if(event.type == SDL_KEYUP) {
			switch(event.key.keysym.sym) {
				case SDLK_a:
					currentState->PlayerState::onLeftUp();
					currentState->onLeftUp();
					break;
				case SDLK_s:
					currentState->onDownUp();
					break;
				case SDLK_d:
					currentState->PlayerState::onRightUp();
					currentState->onRightUp();
					break;
			}
		}
	}
};

#endif
