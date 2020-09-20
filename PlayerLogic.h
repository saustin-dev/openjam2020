//Code for the player
#include <iostream>
#include <fstream>
#include <vector>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"

#ifndef PLAYERLOGIC_H
#define PLAYERLOGIC_H

/**
 * Size of player in pixels
 */
int const WIDTH = 100;
int const HEIGHT = 100;
/**
 * Constants for speed and such
 */
int const GRAVITY = 100;
int const RUN_SPEED = 300;
int const MAX_YVEL = 1000;
int const JUMP_YVEL = -500;

class Player;

class PlayerCollider {
	private:
	SDL_Rect rect;
	int xvel;
	int yvel;
	int gravity;
	Player *parent;
	unsigned int lastTime;
	bool collidedLeft;
	bool collidedRight;
	bool collidedTop;
	bool collidedBottom;
	
	public:
	PlayerCollider(int xpos, int ypos, Player *parent) {
		rect = { xpos, ypos, WIDTH, HEIGHT };
		xvel = 0;
		yvel = 0;
		gravity = 0;
		collidedLeft = false;
		collidedRight = false;
		collidedTop = false;
		collidedBottom = false;
	}
	~PlayerCollider() {
	}
	
	void setGravity(bool grav) {
		if(grav) {
			gravity = GRAVITY;
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
		//handle movement on x and y axis, then add gravity to yvel as well
		double secs = elapsedTime/1000;
		int xMov = secs*xvel;
		int yMov = secs*yvel;
		bool xRight = xMov > 0;
		while(xMov) {
			if(xRight) {
				rect.x++;
				xMov--;
				if(checkCollision()) {
					rect.x--;
					collidedRight = true;
					break;
				}
			}
			else {
				rect.x--;
				xMov++;
				if(checkCollision()) {
					rect.x++;
					collidedLeft = true;
					break;
				}
			}
		}
		bool yDown = yMov > 0;
		while(yMov) {
			if(yDown) {
				rect.y++;
				yMov--;
				if(checkCollision()) {
					rect.y--;
					collidedBottom = true;
					break;
				}
			}
			else {
				rect.y--;
				yMov++;
				if(checkCollision()) {
					rect.y++;
					collidedTop = true;
					break;
				}
			}
		}
		
		yvel += secs*gravity;
	}
	
	bool checkCollision() {
		return false;
	}
	
	void clearYVel() {
		yvel = 0;
	}
	
	void stop() {
		xvel = 0;
	}
	
	bool didCollideLeft() {
		bool output = collidedLeft;
		collidedLeft = false;
		return output;
	}
	bool didCollideRight() {
		bool output = collidedRight;
		collidedRight = false;
		return output;
	}
	bool didCollideTop() {
		bool output = collidedLeft;
		collidedLeft = false;
		return output;
	}
	bool didCollideBottom() {
		bool output = collidedLeft;
		collidedLeft = false;
		return output;
	}
};

class Player {
	private:
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
		virtual ~PlayerState() = 0;
		
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
		virtual void onCollideBottom() {
		}
		virtual void onNoCollideBottom() {
		}
		virtual void onLeftDown() {
		}
		virtual void onRightDown() {
		}
		virtual void onDownDown() {
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
				SDL_Surface *surface = IMG_Load((filename + std::to_string(i) + extension).c_str());
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
		}
		void onActive() {
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
				SDL_Surface *surface = IMG_Load((filename + std::to_string(i) + extension).c_str());
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
		}
		void onActive() {
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
				SDL_Surface *surface = IMG_Load((filename + std::to_string(i) + extension).c_str());
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
		}
		void onRightDown() {
			parent->setFacing(1);
		}
		//go to sliding
		void onDownDown() {
			parent->setState("sliding");
		}
		//go to jumping and set yvel
		void onJump() {
			parent->setState("jumping");
		}
		//if facing is left, go to standing
		void onLeftUp() {
		}
		//if facing is right, go to standing
		void onRightUp() {
		}
		//do nothing
		void onDownUp() {
		}
		
		//move
		void onUpdate() {
		}
		void onActive() {
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
		//is the slide button held down?
		bool holdingSButton;
		
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
				SDL_Surface *surface = IMG_Load((filename + std::to_string(i) + extension).c_str());
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
			if(holdingSButton)
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
		}
		//if facing is right, set holdingDButton to true
		void onRightDown() {
		}
		//set holdingSButton to true
		void onDownDown() {
		}
		//do nothing
		void onJump() {
		}
		//if facing is left, set holdingDButton to false
		void onLeftUp() {
		}
		//if facing is right, set holdingDButton to false
		void onRightUp() {
		}
		//set holdingSButton to false
		void onDownUp() {
		}
		
		//move, check if slide time has elapsed and if so switch to running, standing, or crouching
		void onUpdate() {
			if(SDL_GetTicks() > slideBeginTime + slideDuration) {
				if(!holdingDButton) {
					if(holdingSButton) {
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
			holdingDButton = true;
			holdingSButton = true;
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
				SDL_Surface *surface = IMG_Load((filename + std::to_string(i) + extension).c_str());
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
		}
		//remove y velocity
		void onCollideTop() {
		}
		//go to standing or running
		void onCollideBottom() {
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
			
		}
		//do nothing
		void onActive() {
		}
	};
	class GlidingState : public PlayerState {
		private:
		bool leftDown;
		bool rightDown;
		
		public:
		GlidingState(SDL_Renderer *renderer, Player *parent) {
			filename = "gliding";
			extension = ".png";
			frames = 1;
			framerate = 0;
			this->renderer = renderer;
			this->parent = parent;
			for(int i = 0; i < frames; i++) {
				SDL_Surface *surface = IMG_Load((filename + std::to_string(i) + extension).c_str());
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
		}
		//remove y velocity
		void onCollideTop() {
		}
		//go to standing or running
		void onCollideBottom() {
			if(rightDown && !leftDown) {
				parent->setState("running");
				parent->setFacing(1);
			}
			else if(!rightDown && leftDown) {
				parent->setState("running");
				parent->setFacing(0);
			}
			else {
				parent->setState("standing");
			}
		}
		//do nothing
		void onNoCollideBottom() {
		}
		void onLeftDown() {
			leftDown = true;
		}
		void onRightDown() {
			rightDown = true;
		}
		void onDownDown() {
		}
		void onJump() {
		}
		//do nothing
		void onLeftUp() {
			leftDown = false;
		}
		void onRightUp() {
			rightDown = false;
		}
		void onDownUp() {
		}
		
		//apply gravity, change x-vel based on down keys
		void onUpdate() {
			
		}
		//get what direction keys are down
		void onActive() {
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
	Player(SDL_Renderer *renderer, int x, int y) {
		standing = new StandingState(renderer, this);
		crouching = new CrouchingState(renderer, this);
		running = new RunningState(renderer, this);
		sliding = new SlidingState(renderer, this);
		jumping = new JumpingState(renderer, this);
		gliding = new GlidingState(renderer, this);
		currentState = standing;
		
		collision = new PlayerCollider(x, y, this);
	}
	~Player() {
		delete(standing);
		delete(crouching);
		delete(sliding);
		delete(running);
		delete(jumping);
		delete(gliding);
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
	
	void draw() {
	}
};

#endif
