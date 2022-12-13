// =======================================================
// SDL Raceway
// Lynx Button CI411 Sem 2, April-May 2022
// =======================================================
// Libraries to include

#include "Levels.h"
#include "GameObjects.h"

#include <SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h> 
#include <SDL_mixer.h>
#include <stdio.h>
#include <iostream>
using namespace std;
#include<math.h>
#include<string>

// Function Prototypes
void startSDL();
void startScreen();
void createGameObjects();

void createLevel();
void LevelCompleteScreen();
void checkGameStates();
void resetLevel();
void SetWaypoints(int px1, int py1, int px2, int py2, int px3, int py3, int px4, int py4, int px5, int py5,
					int px6, int py6, int px7, int py7, int px8, int py8, int px9, int py9, int px10, int py10, int px11, int py11);
void raceResults();
void leaderboardResults();
void updateLeaderboard();

void gameLoop();
void limitFrameRate(Uint32);
void playerInputHandler();
void checkCollision();
void updateGameObjects();
void updateGUI();
void updateScreen();
void closeSDL();

// =======================================================
//Global Variables
float frameTime;
SDL_Window* gameWindow = NULL;
SDL_Surface* mainGameSurface = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* gameBackground = NULL;
SDL_Event playerInputEvent;

// Gobal Text Variables
TTF_Font* font = nullptr;
SDL_Color textColour = { 255, 255, 255 };
SDL_Surface* textSurface = nullptr;
SDL_Texture* textTexture = nullptr;
SDL_Rect textRect;

//// game Objects
Character pc;
NPC npcs[4];
Object waypoints[11];

//Game Play
bool gamePlaying = true;
bool replay = true;

int speedTracker = 0;

//Level vars
int currentLevel = 1;
int xStart = -16;
int yStart = -16;
int charaOffset = 80;

RaceTrack strAcross[20];
RaceTrack strUp[20];
RaceTrack tightUpRight[20];
RaceTrack tightUpLeft[20];
RaceTrack tightDownRight[20];
RaceTrack tightDownLeft[20];
RaceTrack startingPos;
RaceTrack trackers[3];

//Placement trackers
int inStartPos = 0;
int placeInRace = 5;
int places[3];
string leadTxt;
bool backingLoaded = false;

bool trackColl[3] = {false,false,false};
bool startColl = false;

//Audio
Mix_Music* music = NULL;
Mix_Chunk* pcCarNoise = NULL;
Mix_Chunk* npcCarNoise = NULL;

int main(int argc, char* args[])
{
	startSDL();
	startScreen();

	createGameObjects();

	gameLoop();

	closeSDL();
	return 0;
}
// =======================================================
// Starting the game
void startSDL()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) printf(" SDL init Error: %s\n", SDL_GetError());
	else
	{	//Create window
		gameWindow = SDL_CreateWindow("CI411 - Rocky Raceway", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		if (gameWindow == NULL) printf(" Window Error %s\n", SDL_GetError());
		//Get Game window surface
		mainGameSurface = SDL_GetWindowSurface(gameWindow);

		//Create a renderer for the window 
		renderer = SDL_CreateRenderer(gameWindow, -1, 0);

		//Set the draw color of renderer
		SDL_SetRenderDrawColor(renderer, 125, 125, 125, 0);

		// Initialise Fonts
		TTF_Init();
		font = TTF_OpenFont("fonts/Alatsi-Regular.ttf", 20);

		//Audio
		Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	}
}//-----

void startScreen()
{
	// Load Title Image
	const char* startImageFile = "images/TitleImage.png";
	SDL_Surface* startImage = IMG_Load(startImageFile);

	// merge image with surface
	SDL_BlitSurface(startImage, NULL, mainGameSurface, NULL);

	SDL_UpdateWindowSurface(gameWindow);
	SDL_Delay(1000);
	SDL_FreeSurface(startImage);
}//-----

// =======================================================
void gameLoop()
{
	Uint32 frameStartTime;
	//Main Game Loop  
	Mix_PlayMusic(music, -1);
	while (replay)
	{
		createLevel();

		while (gamePlaying)
		{
			frameStartTime = SDL_GetTicks(); // time since starting in milliseconds

			playerInputHandler();
			updateGameObjects();
			checkGameStates();
			updateScreen();

			limitFrameRate(frameStartTime);
		}

		LevelCompleteScreen();
	}
}//-----

// =======================================================
// Set up levels & game

void createGameObjects()
{
	// Background Image
	const char* spriteImageFile = "images/grassBackground.png";
	SDL_Surface* spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Background Image Error  - SDL Error: %s\n", SDL_GetError());
	gameBackground = SDL_CreateTextureFromSurface(renderer, spriteImage);

	//Load and Create Sprite Textures
	// PC Sprite 
	spriteImageFile = "images/RacingCarRed.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	pc.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	pc.x = SCREEN_WIDTH / 2 - SPRITE_SIZE / 2;
	pc.y = 512;
	pc.angle = 0;

	/*Waypoints ----------------------------------------------------------------------------------------------
	spriteImageFile = "images/square_cross_grey.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(waypoints) / sizeof(waypoints[0]); i++)
	{
		waypoints[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}*/

	spriteImageFile = "images/spaceBlock.png"; // change me to see tracking points
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(trackers) / sizeof(trackers[0]); i++)
	{
		trackers[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	// npcs
	spriteImageFile = "images/RacingCarBlue.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());

	for (int i = 0; i < sizeof(npcs) / sizeof(npcs[0]); i++)
	{
		npcs[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	// Set Terrain Blocks - straight across
	spriteImageFile = "images/track/TrackAcross.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	for (int i = 0; i < sizeof(strAcross) / sizeof(strAcross[0]); i++)
	{
		strAcross[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}
	// Set Terrain Blocks - straight up
	spriteImageFile = "images/track/TrackUp.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());

	for (int i = 0; i < sizeof(strUp) / sizeof(strUp[0]); i++)
	{
		strUp[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	// Set Terrain Blocks - tight up right
	spriteImageFile = "images/track/TrackTurnRightUp.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());

	for (int i = 0; i < sizeof(strUp) / sizeof(strUp[0]); i++)
	{
		tightUpRight[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}
	// Set Terrain Blocks - tight up left
	spriteImageFile = "images/track/TrackTurnLeftUp.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());

	for (int i = 0; i < sizeof(strUp) / sizeof(strUp[0]); i++)
	{
		tightUpLeft[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	// Set Terrain Blocks - tight down right
	spriteImageFile = "images/track/TrackTurnRightDown.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());

	for (int i = 0; i < sizeof(strUp) / sizeof(strUp[0]); i++)
	{
		tightDownRight[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}
	// Set Terrain Blocks - tight down left
	spriteImageFile = "images/track/TrackTurnLeftDown.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());

	for (int i = 0; i < sizeof(strUp) / sizeof(strUp[0]); i++)
	{
		tightDownLeft[i].spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);
	}

	// Set Terrain Blocks - tight down left
	spriteImageFile = "images/track/TrackStart.png";
	spriteImage = IMG_Load(spriteImageFile);
	if (spriteImage == NULL) printf(" Sprite Error  - SDL Error: %s\n", SDL_GetError());
	startingPos.spriteTexture = SDL_CreateTextureFromSurface(renderer, spriteImage);

	// Clear surface
	SDL_FreeSurface(spriteImage);

	//Load Audio
	music = Mix_LoadMUS("audio/verySuperbeep.wav");
	pcCarNoise = Mix_LoadWAV("audio/CarOnRoad.wav");
	npcCarNoise = Mix_LoadWAV("audio/carHorn.wav");
}//-----

void SetWaypoints(int px1,int py1, int px2, int py2, int px3, int py3, int px4, int py4, int px5, int py5,
					int px6, int py6, int px7, int py7, int px8, int py8, int px9, int py9, int px10, int py10, int px11, int py11)
{
	waypoints[0].x = px1;
	waypoints[0].y = py1;

	waypoints[1].x = px2;
	waypoints[1].y = py2;

	waypoints[2].x = px3;
	waypoints[2].y = py3;

	waypoints[3].x = px4;
	waypoints[3].y = py4;

	waypoints[4].x = px5;
	waypoints[4].y = py5;

	waypoints[5].x = px6;
	waypoints[5].y = py6;

	waypoints[6].x = px7;
	waypoints[6].y = py7;

	waypoints[7].x = px8;
	waypoints[7].y = py8;

	waypoints[8].x = px9;
	waypoints[8].y = py9;

	waypoints[9].x = px10;
	waypoints[9].y = py10;

	waypoints[10].x = px11;
	waypoints[10].y = py11;

	for (int i = 0; i < sizeof(npcs) / sizeof(npcs[0]); i++)
	{
		npcs[i].patrol(px1, py1, px2, py2, px3, py3, px4, py4, px5, py5, px6, py6, px7, py7, px8, py8, px9, py9, px10, py10, px11, py11);
	}
}

void createLevel()
{
	// Level current level
	int currentMap[9][13];

	if (currentLevel == 1)
	{
		for (int row = 0; row < 9; row++)
		{
			for (int col = 0; col < 13; col++)
			{
				currentMap[row][col] = level1[row][col];
			}
		}
		SetWaypoints(695, 130, 695, 260, 695, 400, 695, 510, 480, 510, 250, 510, 70, 510, 70, 320, 70, 130, 400, 125, 400, 125);
		for (int i = 0; i < sizeof(npcs) / sizeof(npcs[0]); i++)
		{
			npcs[i].wpAngleUpdate(90, 180, 180, 180, 270, 270, 270, 0, 0, 90, 90);
		}
	}
	if (currentLevel == 2)
	{
		for (int row = 0; row < 9; row++)
		{
			for (int col = 0; col < 13; col++)
			{
				currentMap[row][col] = level2[row][col];
			}
		}
		SetWaypoints(70, 510, 70, 130, 240, 130, 260, 190, 430, 200, 450, 260, 695, 260, 695, 440, 585, 460, 565, 510, 565, 510);
		for (int i = 0; i < sizeof(npcs) / sizeof(npcs[0]); i++)
		{
			npcs[i].wpAngleUpdate(270, 0, 90, 180, 90, 180, 90, 180, 270, 180, 180);
		}
	}
	if (currentLevel == 3)
	{
		for (int row = 0; row < 9; row++)
		{
			for (int col = 0; col < 13; col++)
			{
				currentMap[row][col] = level3[row][col];
			}
		}
		SetWaypoints(695, 130, 695, 310, 575, 340, 465, 382, 430, 450, 70, 510, 70, 400, 190, 390, 190, 260, 70, 260, 70, 130);
		for (int i = 0; i < sizeof(npcs) / sizeof(npcs[0]); i++)
		{
			npcs[i].wpAngleUpdate(90, 180, 270, 270, 180, 270, 0, 90, 0, 270, 0);
		}
	}

	if (backingLoaded == true)
	{
		// Background Image
		const char* spriteImageFile = "images/grassBackground.png";
		SDL_Surface* spriteImage = IMG_Load(spriteImageFile);
		if (spriteImage == NULL) printf(" Background Image Error  - SDL Error: %s\n", SDL_GetError());
		gameBackground = SDL_CreateTextureFromSurface(renderer, spriteImage);

		// Clear surface
		SDL_FreeSurface(spriteImage);
	}


	// Add Game Objects to the board ------------------------

	for (int row = 0; row < 9; row++)
	{
		for (int col = 0; col < 13; col++)
		{
			if (currentMap[row][col] == 0)
			{
				//Empty
			}

			if (currentMap[row][col] == 1)
			{
				if (pc.isActive == false)
				{
					// PC Start
					pc.x = xStart + col * SPRITE_SIZE * 2;
					pc.y = yStart + row * SPRITE_SIZE * 2 + charaOffset;

					if (currentLevel == 1 || currentLevel == 3)
					{
						pc.angle = 90;
					}
					else
					{
						pc.angle = 270;
					}
				}
			}

			if (currentMap[row][col] == 2) // across
			{
				// loop through terrin
				for (int i = 0; i < sizeof(strAcross) / sizeof(strAcross[0]); i++)
				{
					if (strAcross[i].isActive == false)
					{
						strAcross[i].x = xStart + col * (SPRITE_SIZE * 2);
						strAcross[i].y = yStart + row * (SPRITE_SIZE * 2);
						strAcross[i].isActive = true;
						break;
					}
				}
			}

			if (currentMap[row][col] == 3) // Up
			{
				for (int i = 0; i < sizeof(strUp) / sizeof(strUp[0]); i++)
				{
					if (strUp[i].isActive == false)
					{
						strUp[i].x = xStart + col * (SPRITE_SIZE * 2);
						strUp[i].y = yStart + row * (SPRITE_SIZE * 2);
						strUp[i].isActive = true;
						break;
					}
				}
			}

			if (currentMap[row][col] == 4)
			{
				for (int i = 0; i < sizeof(tightUpRight) / sizeof(tightUpRight[0]); i++)
				{
					if (tightUpRight[i].isActive == false)
					{
						tightUpRight[i].x = xStart + col * (SPRITE_SIZE * 2);
						tightUpRight[i].y = yStart + row * (SPRITE_SIZE * 2);
						tightUpRight[i].isActive = true;
						break;
					}
				}
			}
			if (currentMap[row][col] == 5)
			{
				for (int i = 0; i < sizeof(tightUpLeft) / sizeof(tightUpLeft[0]); i++)
				{
					if (tightUpLeft[i].isActive == false)
					{
						tightUpLeft[i].x = xStart + col * (SPRITE_SIZE * 2);
						tightUpLeft[i].y = yStart + row * (SPRITE_SIZE * 2);
						tightUpLeft[i].isActive = true;
						break;
					}
				}
			}

			if (currentMap[row][col] == 6)
			{
				for (int i = 0; i < sizeof(tightDownRight) / sizeof(tightDownRight[0]); i++)
				{
					if (tightDownRight[i].isActive == false)
					{
						tightDownRight[i].x = xStart + col * (SPRITE_SIZE * 2);
						tightDownRight[i].y = yStart + row * (SPRITE_SIZE * 2);
						tightDownRight[i].isActive = true;
						break;
					}
				}
			}
			if (currentMap[row][col] == 7) // across
			{
				// loop through terrin
				for (int i = 0; i < sizeof(tightDownLeft) / sizeof(tightDownLeft[0]); i++)
				{
					if (tightDownLeft[i].isActive == false)
					{
						tightDownLeft[i].x = xStart + col * (SPRITE_SIZE * 2);
						tightDownLeft[i].y = yStart + row * (SPRITE_SIZE * 2);
						tightDownLeft[i].isActive = true;
						break;
					}
				}
			}
			if (currentMap[row][col] == 8)
			{
				if (startingPos.isActive == false)
				{
					startingPos.x = xStart + col * (SPRITE_SIZE * 2);
					startingPos.y = yStart + row * (SPRITE_SIZE * 2);
					startingPos.isActive = true;
				}
			}
			if (currentMap[row][col] == 9)
			{

				for (int i = 0; i < sizeof(npcs) / sizeof(npcs[0]); i++)
				{
					if (npcs[i].isActive == false)
					{
						npcs[i].x = xStart + col * SPRITE_SIZE * 2;
						npcs[i].y = yStart + row * SPRITE_SIZE * 2 + charaOffset;
						npcs[i].isActive = true;
						break;
					}
				}
			}
			if (currentMap[row][col] == 10) // trackers
			{
				// loop through terrin
				for (int i = 0; i < sizeof(trackers) / sizeof(trackers[0]); i++)
				{
					if (trackers[i].isActive == false)
					{
						trackers[i].x = xStart + col * (SPRITE_SIZE * 2) + charaOffset;
						trackers[i].y = yStart + row * (SPRITE_SIZE * 2);
						trackers[i].isActive = true;
						break;
					}
				}
			}
		}
	}

}//-----

// =======================================================
// Check input

void playerInputHandler()
{
	// Set up the input event handler
	while (SDL_PollEvent(&playerInputEvent) != 0)
	{
		switch (playerInputEvent.type)
		{
		case SDL_QUIT:
			gamePlaying = false;
			replay = false;
			break;

		case SDL_KEYDOWN:

			switch (playerInputEvent.key.keysym.sym)
			{

			case SDLK_UP:  pc.keyRotateMove('f');	Mix_PlayChannel(1, pcCarNoise, 0); break;
			case SDLK_DOWN: pc.keyRotateMove('b');	Mix_PlayChannel(1, pcCarNoise, 0); break;
			case SDLK_LEFT:	pc.keyRotateMove('l');	Mix_PlayChannel(1, pcCarNoise, 0); break;
			case SDLK_RIGHT: pc.keyRotateMove('r');	Mix_PlayChannel(1, pcCarNoise, 0); break;

			case SDLK_w:  pc.keyRotateMove('f');	Mix_PlayChannel(1, pcCarNoise, 0); break;
			case SDLK_s:  pc.keyRotateMove('b');	Mix_PlayChannel(1, pcCarNoise, 0); break;
			case SDLK_a: pc.keyRotateMove('l');		Mix_PlayChannel(1, pcCarNoise, 0); break;
			case SDLK_d: pc.keyRotateMove('r');		Mix_PlayChannel(1, pcCarNoise, 0); break;
				
			case SDLK_2: currentLevel = 1; pc.laps = 4; break;
			case SDLK_3: currentLevel = 2; pc.laps = 4; break;
			} 	break;
		}
	}
}

// =======================================================
// Update Objects on screen

void updateGameObjects()
{
	// Collision Detection ---------------- 
	checkCollision();

	//--------------------------------------
	// Player Character
	if(currentLevel != 4)pc.screenLimit();
	pc.applyDrag();
	pc.updatePosition(frameTime);

	// npcs
	for (int i = 0; i < sizeof(npcs) / sizeof(npcs[0]); i++)
	{
		npcs[i].update();
	}
}//-----

void updateScreen()
{
	SDL_Rect spriteRect = { 0, 0, SCREEN_WIDTH , SCREEN_HEIGHT };
	// Clear & Update the screen
	SDL_RenderClear(renderer);
	// Set the Background Image
	SDL_RenderCopy(renderer, gameBackground, NULL, &spriteRect);

	// Terrain Blocks - Across
	for (int i = 0; i < sizeof(strAcross) / sizeof(strAcross[0]); i++)
	{
		if (strAcross[i].isActive) // exlcude the dead 
		{
			spriteRect = { strAcross[i].x, strAcross[i].y, SPRITE_SIZE * 2 , SPRITE_SIZE * 2 };
			SDL_RenderCopyEx(renderer, strAcross[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}
	// Terrain Blocks - Up
	for (int i = 0; i < sizeof(strUp) / sizeof(strUp[0]); i++)
	{
		if (strUp[i].isActive) // exlcude the dead 
		{
			spriteRect = { strUp[i].x, strUp[i].y, SPRITE_SIZE * 2 , SPRITE_SIZE * 2 };
			SDL_RenderCopyEx(renderer, strUp[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}
	// Terrain Blocks - Turn Up Right
	for (int i = 0; i < sizeof(tightUpRight) / sizeof(tightUpRight[0]); i++)
	{
		if (tightUpRight[i].isActive) // exlcude the dead 
		{
			spriteRect = { tightUpRight[i].x, tightUpRight[i].y, SPRITE_SIZE * 2 , SPRITE_SIZE * 2 };
			SDL_RenderCopyEx(renderer, tightUpRight[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}
	// Terrain Blocks - Turn Up Left
	for (int i = 0; i < sizeof(tightUpLeft) / sizeof(tightUpLeft[0]); i++)
	{
		if (tightUpLeft[i].isActive) // exlcude the dead 
		{
			spriteRect = { tightUpLeft[i].x, tightUpLeft[i].y, SPRITE_SIZE * 2 , SPRITE_SIZE * 2 };
			SDL_RenderCopyEx(renderer, tightUpLeft[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}
	// Terrain Blocks - Turn Down Right
	for (int i = 0; i < sizeof(tightDownRight) / sizeof(tightDownRight[0]); i++)
	{
		if (tightDownRight[i].isActive) // exlcude the dead 
		{
			spriteRect = { tightDownRight[i].x, tightDownRight[i].y, SPRITE_SIZE * 2 , SPRITE_SIZE * 2 };
			SDL_RenderCopyEx(renderer, tightDownRight[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}
	// Terrain Blocks - Turn Down Left
	for (int i = 0; i < sizeof(tightDownLeft) / sizeof(tightDownLeft[0]); i++)
	{
		if (tightDownLeft[i].isActive) // exlcude the dead 
		{
			spriteRect = { tightDownLeft[i].x, tightDownLeft[i].y, SPRITE_SIZE * 2 , SPRITE_SIZE * 2 };
			SDL_RenderCopyEx(renderer, tightDownLeft[i].spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
		}
	}

	if (startingPos.isActive) // Starting Point
	{
		spriteRect = { startingPos.x, startingPos.y, SPRITE_SIZE * 2 , SPRITE_SIZE * 2 };
		SDL_RenderCopyEx(renderer, startingPos.spriteTexture, NULL, &spriteRect, 0, NULL, SDL_FLIP_NONE);
	}

	// NPCS
	for (int i = 0; i < sizeof(npcs) / sizeof(npcs[0]); i++)
	{
		spriteRect = { npcs[i].x,  npcs[i].y, SPRITE_SIZE, SPRITE_SIZE };
		SDL_RenderCopyEx(renderer, npcs[i].spriteTexture, NULL, &spriteRect, npcs[i].angle, NULL, SDL_FLIP_NONE);
	}

	/* Waypoints - for debugging --------------------------------------------------------------------------------------------------
	for (int i = 0; i < sizeof(waypoints) / sizeof(waypoints[0]); i++)
	{
		spriteRect = { waypoints[i].x,  waypoints[i].y, SPRITE_SIZE, SPRITE_SIZE };
		SDL_RenderCopy(renderer, waypoints[i].spriteTexture, NULL, &spriteRect);
	}*/

	// trackers - for debugging --------------------------------------------------------------------------------------------------
	for (int i = 0; i < sizeof(trackers) / sizeof(trackers[0]); i++)
	{
		spriteRect = { trackers[i].x,  trackers[i].y, SPRITE_SIZE, SPRITE_SIZE };
		SDL_RenderCopy(renderer, trackers[i].spriteTexture, NULL, &spriteRect);
	}

	// PC
	spriteRect = { pc.x, pc.y, SPRITE_SIZE , SPRITE_SIZE };
	SDL_RenderCopyEx(renderer, pc.spriteTexture, NULL, &spriteRect, pc.angle, NULL, SDL_FLIP_NONE);

	updateGUI();  //******** USER INTERFACE  ***********

	//Update the Screen
	SDL_RenderPresent(renderer);

}//-----

void updateGUI()
{
	string  screenText;

	// PC lives
	if (currentLevel != 4) screenText = "Laps " + std::to_string(pc.laps) + "/3";
	else screenText = " ";

	textSurface = TTF_RenderText_Blended(font, screenText.c_str(), textColour);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	int textW = 0;
	int textH = 0;

	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 640, 30, textW, textH };
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);

	// PC Position  

	if (currentLevel != 4) screenText = "Race Place: " + std::to_string(placeInRace);
	else screenText = " ";
	textSurface = TTF_RenderText_Blended(font, screenText.c_str(), textColour);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 75, 30, textW, textH };
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
}//-----

// =======================================================
// Checking for winning and losing states

void checkCollision()
{
	SDL_Rect npcRect;
	SDL_Rect trackRect;
	SDL_Rect trackerRect;
	SDL_Rect resultRect;
	SDL_Rect pcRect = { pc.x, pc.y, SPRITE_SIZE, SPRITE_SIZE };

	bool fast = false;

	for (int i = 0; i < sizeof(npcs) / sizeof(npcs[0]); i++)
	{
		if (npcs[i].isActive)
		{
			npcRect.x = npcs[i].x;
			npcRect.y = npcs[i].y;
			npcRect.h = npcRect.w = SPRITE_SIZE * 2;

			if (SDL_HasIntersection(&npcRect, &pcRect)) // Hit by Bullet
			{

				// reverse vel on edge hit
				pc.xVel = -pc.xVel / 2;
				pc.yVel = -pc.yVel / 2;

				Mix_PlayChannel(1, npcCarNoise, 0);
				break;
			}
		}
	}

	if (startingPos.isActive)
	{
		trackRect.x = startingPos.x;
		trackRect.y = startingPos.y;
		trackRect.h = trackRect.w = SPRITE_SIZE * 2;

		if (SDL_HasIntersection(&pcRect, &trackRect)) // Hit by Bullet
		{
			if (startColl == false)
			{
				if (pc.laps == 0 && inStartPos == 3)
				{
					inStartPos = 4;
				}
				else if (pc.laps != 0)
				{
					inStartPos += 1;
					cout << "Touch Amount: " << inStartPos;
					startColl = true;
				}
			}
		}
		else
		{
			startColl = false;

			if (inStartPos >= 4 && pc.laps < 4)
			{
				inStartPos = 0;
				pc.laps += 1;
				raceResults();
				cout << "Lap Amount: " << pc.laps;
			}
		}

		for (int i = 0; i < sizeof(trackers) / sizeof(trackers[0]); i++)
		{
			if (trackers[i].isActive)
			{
				trackerRect.x = trackers[i].x;
				trackerRect.y = trackers[i].y;
				trackerRect.h = trackerRect.w = SPRITE_SIZE * 2;

				if (SDL_HasIntersection(&pcRect, &trackerRect)) // Hit by Bullet
				{
					if (trackColl[i] == false)
					{
						inStartPos += 1;
						cout << "Touch Amount: " << inStartPos;
						trackColl[i] = true;
						//break;
					}
				}
				else
				{
					trackColl[i] = false;
				}
			}
		}

		for (int i = 0; i < sizeof(npcs) / sizeof(npcs[0]); i++)
		{
			if (npcs[i].isActive)
			{
				npcRect.x = npcs[i].x;
				npcRect.y = npcs[i].y;
				npcRect.h = npcRect.w = SPRITE_SIZE * 2;

				if (SDL_HasIntersection(&npcRect, &trackRect)) // Hit by Bullet
				{
					npcs[i].inStart = true;
				}
				else
				{
					if (npcs[i].inStart == true)
					{
						npcs[i].inStart = false;
						npcs[i].laps += 1;
						raceResults();
						//cout << "NPC " << i << " Lap Amount : " << npcs[i].laps;
					}
					npcs[i].inStart = false;
				}
			}
		}
	}

	// Pc 'hits' track
	for (int i = 0; i < sizeof(strAcross) / sizeof(strAcross[0]); i++)
	{
		if (strAcross[i].isActive)
		{
			trackRect.x = strAcross[i].x;
			trackRect.y = strAcross[i].y;
			trackRect.h = trackRect.w = SPRITE_SIZE * 2;

			if (SDL_HasIntersection(&pcRect, &trackRect)) // Hit by Bullet
			{
				fast = true;
				break;
			}
			else
			{
				fast = false;
			}
		}
	}
	if (fast == false)
	{
		for (int i = 0; i < sizeof(strUp) / sizeof(strUp[0]); i++)
		{
			if (strUp[i].isActive)
			{
				trackRect.x = strUp[i].x;
				trackRect.y = strUp[i].y;
				trackRect.h = trackRect.w = SPRITE_SIZE * 2;

				if (SDL_HasIntersection(&pcRect, &trackRect)) // Hit by Bullet
				{
					fast = true;
					break;
				}
				else
				{
					fast = false;
				}
			}
		}
	}
	if (fast)
	{
		pc.speed = 200;
		pc.force = 150;
	}
	else
	{
		pc.speed = 100;
		pc.force = 75;
	}


}//----

void raceResults()
{
	for (int i = 0; i < sizeof(npcs) / sizeof(npcs[0]); i++)
	{
		if (npcs[i].laps >= 3 && pc.laps < 3)
		{
			npcs[i].racesWon += 1;
		}
		if (pc.laps > npcs[i].laps)
		{
			if (placeInRace <= 1) placeInRace = 1;
			else placeInRace -= 1;
			//cout << "I'm winning";
		}
		if (pc.laps <= npcs[i].laps)
		{
			if (placeInRace > 4) placeInRace = 5;
			else placeInRace += 1;
			//cout << "I'm losing";
		}
	}
}

void checkGameStates()
{
	if (pc.laps >= 3)
	{

		if (currentLevel < 4)
		{
			leaderboardResults();
			resetLevel();
			currentLevel += 1;
			createLevel();
			pc.laps = 0;
		}
		if (currentLevel == 4)
		{
			resetLevel();
			leaderboardResults();
			gamePlaying = false;
		}
		cout << "Current level: " << currentLevel;
	}
}

// =======================================================
// Leaderboard Control

void leaderboardResults()
{
	if (currentLevel == 1) places[0] = placeInRace;
	else if (currentLevel == 2) places[1] = placeInRace;
	else if (currentLevel == 3) places[2] = placeInRace;
	else if (currentLevel == 4)
	{
		//cout << "GUI has been ran";
		float mean = 0;
		for (int i = 0; i < sizeof(places) / sizeof(places[0]); i++)
		{
			mean = mean + places[i];
		}
		mean = mean / 3;
		//cout << "The mean is " << mean;
		//cout << "The places are " << places;

		string screenText;
		int textW = 0;
		int textH = 0;

		if (mean < 1.5f)leadTxt = "1st: PC\n\n2nd: NPC 3\n\n3rd: NPC 2\n\n4th: NPC 1\n\n5th: NPC 0";
		else if(mean < 2.5f)leadTxt = "1st: NPC 3\n\n2nd: PC\n\n3rd: NPC 2\n\n4th: NPC 1\n\n5th: NPC 0";
		else if(mean < 3.5f)leadTxt = "1st: NPC 3\n\n2nd: NPC 2\n\n3rd: PC\n\n4th: NPC 1\n\n5th: NPC 0";
		else if(mean < 4.5f)leadTxt = "1st: NPC 3\n\n2nd: NPC 2\n\n3rd: NPC 1\n\n4th: PC\n\n5th: NPC 0";
		else leadTxt = "1st: NPC 3\n\n2nd: NPC 2\n\n3rd: NPC 1\n\n4th: NPC 0\n\n5th: PC";

		updateLeaderboard();
	}
}

void updateLeaderboard()
{
	if (backingLoaded == false)
	{
		// Background Image
		const char* spriteImageFile = "images/leaderboard.png";
		SDL_Surface* spriteImage = IMG_Load(spriteImageFile);
		if (spriteImage == NULL) printf(" Background Image Error  - SDL Error: %s\n", SDL_GetError());
		gameBackground = SDL_CreateTextureFromSurface(renderer, spriteImage);
		backingLoaded = true;
	}

	// Leaderboard Pos  
	string screenText;
	int textW = 0;
	int textH = 0;

	screenText = "LeaderBoard: \n\n\n" + leadTxt + "\n\n\nPress Esc to Quit & \nEnter to play again.";
	textSurface = TTF_RenderText_Blended_Wrapped(font, screenText.c_str(), textColour, 0);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 300, 115, textW, textH };
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);

	//Update the Screen
	SDL_RenderPresent(renderer);
}

//----------------------------
// End game control

void resetLevel()
{
	startingPos.isActive = false;
	for (int i = 0; i < sizeof(trackers) / sizeof(trackers[0]); i++)
	{
		trackers[i].isActive = false;
	}
	for (int i = 0; i < sizeof(strAcross) / sizeof(strAcross[0]); i++)
	{
		strAcross[i].isActive = false;
	}
	for (int i = 0; i < sizeof(strUp) / sizeof(strUp[0]); i++)
	{
		strUp[i].isActive = false;
	}
	for (int i = 0; i < sizeof(tightUpRight) / sizeof(tightUpRight[0]); i++)
	{
		tightUpRight[i].isActive = false;
	}
	for (int i = 0; i < sizeof(tightUpLeft) / sizeof(tightUpLeft[0]); i++)
	{
		tightUpLeft[i].isActive = false;
	}
	for (int i = 0; i < sizeof(tightDownRight) / sizeof(tightDownRight[0]); i++)
	{
		tightDownRight[i].isActive = false;
	}
	for (int i = 0; i < sizeof(tightDownLeft) / sizeof(tightDownLeft[0]); i++)
	{
		tightDownLeft[i].isActive = false;
	}
	for (int i = 0; i < sizeof(npcs) / sizeof(npcs[0]); i++)
	{
		npcs[i].isActive = false;
	}
	//pc and npcs
	for (int i = 0; i < sizeof(npcs) / sizeof(npcs[0]); i++)
	{
		npcs[i].x = -30;
		npcs[i].y = -30;
		npcs[i].angle = 0;
	}
	pc.x = -30;
	pc.y = -30;
	pc.angle = 0;

	//GUI
	updateGUI();
}

void LevelCompleteScreen()
{
	// Set up the input event handler
	while (SDL_PollEvent(&playerInputEvent) != 0)
	{
		switch (playerInputEvent.type)
		{
		case SDL_QUIT:
			gamePlaying = false;
			replay = false;
			break;

		case SDL_KEYDOWN:

			switch (playerInputEvent.key.keysym.sym)
			{

			case SDLK_RETURN:
				gamePlaying = true;
				replay = true;
				resetLevel();
				currentLevel = 1;
				gameLoop();
				break;
			case SDLK_ESCAPE:
				gamePlaying = false;
				replay = false;
				break;
			} 	break;
		}
	}
	updateLeaderboard();
	// Load Title Image
	//const char* startImageFile = "images/bg_water_800.png";
	//SDL_Surface* startImage = IMG_Load(startImageFile);

	// merge image with surface
	//SDL_BlitSurface(startImage, NULL, mainGameSurface, NULL);

	//SDL_UpdateWindowSurface(gameWindow);

	//SDL_Delay(1000);
	//SDL_FreeSurface(startImage);
}//-----

//-----------------------------
void limitFrameRate(Uint32 startTick)
{
	Uint32 currentTick = SDL_GetTicks();

	if ((1000 / FPS) > currentTick - startTick) {
		SDL_Delay(1000 / FPS - (currentTick - startTick));
		frameTime = (float)1 / FPS;
	}
	else
	{
		frameTime = (float)(currentTick - startTick) / 1000;
	}
}//-----

void closeSDL()
{
	// Load Title Image
	const char* startImageFile = "images/TitleScreen.png";
	SDL_Surface* startImage = IMG_Load(startImageFile);

	// merge image with surface
	SDL_BlitSurface(startImage, NULL, mainGameSurface, NULL);

	SDL_UpdateWindowSurface(gameWindow);

	SDL_Delay(1000);
	SDL_FreeSurface(startImage);

	Mix_FreeChunk(pcCarNoise);
	Mix_FreeChunk(npcCarNoise);
	Mix_FreeMusic(music);
	Mix_CloseAudio();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(gameWindow);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();


}//-----
