#pragma once
#include <stdio.h>
#include<iostream>
#include <SDL.h>
#include<string>

// Constants
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define SPRITE_SIZE 32
#define FPS 50

//Classes-----------------------------------------------------

class RaceTrack
{
public:
	bool isActive = false;
	int x, y;
	SDL_Texture* spriteTexture = nullptr;
};//-----

class Object
{
public:
	bool isActive = false;
	int x, y;
	SDL_Texture* spriteTexture = nullptr;
};//-----

class Character
{
public:
	bool isActive = false;
	int x = 0, y = 0;
	int laps = 0;
	float xVel = 0, yVel = 0;
	float friction = 0.95f;
	float angle = 90;
	float speed = 125;
	float force = 125;
	float torque = 50; // rotation force
	float rotationRate = 0;
	SDL_Texture* spriteTexture = nullptr;

	// -----------------------

	void keyRotateMove(char actionSent);
	void applyDrag();
	void updatePosition(float frameTimeSent);
	void screenLimit();
	void setVelocity(float velSent);
};// End Class -----

class NPC
{
public:
	//string state;
	SDL_Texture* spriteTexture = nullptr;

	bool isActive = false;
	bool inStart = false;
	int racesWon = 0;
	int laps = 0;
	int x = 0, y = 0;
	float angle = 0;
	int speed = 1;
	//float xVel = 0, yVel = 0;

	//int wp1x, wp2x, wp3x, wp4x, wp5x;
	//int wp1y, wp2y, wp3y, wp4y, wp5y;
	//int wp1a, wp2a, wp3a, wp4a, wp5a;
	int wpx[11];
	int wpy[11];
	int wpa[11];

	int currentWP;

	// -----------------------

	void patrol(int w1x, int w1y, int w2x, int w2y, int w3x, int w3y, int w4x, int w4y, int w5x, int w5y,
				int w6x, int w6y, int w7x, int w7y, int w8x, int w8y, int w9x, int w9y, int w10x, int w10y, int w11x, int w11y);
	void wpAngleUpdate(int wp1, int wp2, int wp3, int wp4, int wp5, int wp6, int wp7, int wp8, int wp9, int wp10, int wp11);
	void update();
	void atWP(int num);

};// End Class -----

