
#include "GameObjects.h"

//Character-CLass-Functions---------------------------------------------

void Character::keyRotateMove(char actionSent)
{	//angle is in degrees, Trig uses radians ( degrees x PI/180)
	switch (actionSent)
	{
	case 'l':
		rotationRate = -torque;
		keyRotateMove('f');
		break;
	case 'r':
		rotationRate = torque;
		keyRotateMove('f');
		break;

	case 'f':
		if (xVel == 0 && yVel == 0) // Set Speed if still
		{
			xVel += sin(angle * M_PI / 180) * speed;
			yVel -= cos(angle * M_PI / 180) * speed;
		}
		else if (abs(xVel) < speed && abs(yVel) < speed)
		{
			xVel += sin(angle * M_PI / 180) * force;
			yVel -= cos(angle * M_PI / 180) * force;
		} break;

	case'b':
		if (abs(xVel) < speed && abs(yVel) < speed)
		{
			xVel -= sin(angle * M_PI / 180) * force;
			yVel += cos(angle * M_PI / 180) * force;
		} break;
	}
}

void Character::applyDrag()
{
	// apply linear drag - set vel to zero once clearly less than 1 pixel speed 
	if (abs(xVel) > 0.1f) xVel *= friction; else xVel = 0;
	if (abs(yVel) > 0.1f) yVel *= friction; else yVel = 0;
}

void Character::updatePosition(float frameTimeSent)
{
	// Add the rotation rate to the angle and apply drag
	if (abs(rotationRate) > 10)	rotationRate *= friction;
	else rotationRate = 0;
	angle += rotationRate * frameTimeSent;

	// Add the current velocity to current position
	// round up /down to nearest pixel depending on direction
	if (xVel > 0) x += floor(xVel * frameTimeSent);
	else if (xVel < 0) x += ceil(xVel * frameTimeSent);
	if (yVel > 0) y += floor(yVel * frameTimeSent);
	else if (yVel < 0) y += ceil(yVel * frameTimeSent);
}

void Character::screenLimit()
{	// Limit sprite to screen 
	if (x > SCREEN_WIDTH - SPRITE_SIZE)
	{
		xVel = 0;
		x = SCREEN_WIDTH - SPRITE_SIZE;
	}
	if (x < 0)
	{
		xVel = 0;
		x = 0;
	}
	if (y > SCREEN_HEIGHT - SPRITE_SIZE)
	{
		y = SCREEN_HEIGHT - SPRITE_SIZE;
		yVel = 0;
	}
	if (y < 0)
	{
		y = 0;
		yVel = 0;
	}
}

void Character::setVelocity(float velSent)
{
	xVel = sin(angle * M_PI / 180) * velSent;
	yVel = -cos(angle * M_PI / 180) * velSent;
}

//NPC-CLass-Functions----------------------------------------------------

void NPC::patrol(int w1x, int w1y, int w2x, int w2y, int w3x, int w3y, int w4x, int w4y, int w5x, int w5y,
					int w6x, int w6y, int w7x, int w7y, int w8x, int w8y, int w9x, int w9y, int w10x, int w10y, int w11x, int w11y)
{
	wpx[0] = w1x;
	wpx[1] = w2x;
	wpx[2] = w3x;
	wpx[3] = w4x;
	wpx[4] = w5x;
	wpx[5] = w6x;
	wpx[6] = w7x;
	wpx[7] = w8x;
	wpx[8] = w9x;
	wpx[9] = w10x;
	wpx[10] = w11x;

	wpy[0] = w1y;
	wpy[1] = w2y;
	wpy[2] = w3y;
	wpy[3] = w4y;
	wpy[4] = w5y;
	wpy[5] = w6y;
	wpy[6] = w7y;
	wpy[7] = w8y;
	wpy[8] = w9y;
	wpy[9] = w10y;
	wpy[10] = w11y;

	currentWP = 1;
}
void NPC::wpAngleUpdate(int wp1, int wp2, int wp3, int wp4, int wp5, int wp6, int wp7, int wp8, int wp9, int wp10, int wp11)
{
	wpa[0] = wp1;
	wpa[1] = wp2;
	wpa[2] = wp3;
	wpa[3] = wp4;
	wpa[4] = wp5;
	wpa[5] = wp6;
	wpa[6] = wp7;
	wpa[7] = wp8;
	wpa[8] = wp9;
	wpa[9] = wp10;
	wpa[10] = wp11;
}

void NPC::atWP(int num)
{
	/*if (wpx[num] - x > 5 || x - wpx[num] < -5)
	{
		if (wpy[num] - y > -5 || y - wpy[num] < 5)
		{
			speed = 1;
		}
	}*/
}
void NPC::update()
{

	if (currentWP == 1)
	{
		if (x > wpx[0]) x = x - speed;
		if (x < wpx[0]) x = x + speed;
		if (y > wpy[0]) y = y - speed;
		if (y < wpy[0]) y = y + speed;
		if (x == wpx[0] && y == wpy[0])  currentWP = 2; angle = wpa[0];
	}
	if (currentWP == 2)
	{
		if (x > wpx[1]) x = x - speed;
		if (x < wpx[1]) x = x + speed;
		if (y > wpy[1]) y = y - speed;
		if (y < wpy[1]) y = y + speed;
		if (x == wpx[1] && y == wpy[1])  currentWP = 3; angle = wpa[1];
	}
	if (currentWP == 3)
	{
		if (x > wpx[2]) x = x - speed;
		if (x < wpx[2]) x = x + speed;
		if (y > wpy[2]) y = y - speed;
		if (y < wpy[2]) y = y + speed;
		if (x == wpx[2] && y == wpy[2])  currentWP = 4; angle = wpa[2];
	}
	if (currentWP == 4)
	{
		if (x > wpx[3]) x = x - speed;
		if (x < wpx[3]) x = x + speed;
		if (y > wpy[3]) y = y - speed;
		if (y < wpy[3]) y = y + speed;
		if (x == wpx[3] && y == wpy[3])  currentWP = 5; angle = wpa[3];
	}
	if (currentWP == 5)
	{
		if (x > wpx[4]) x = x - speed;
		if (x < wpx[4]) x = x + speed;
		if (y > wpy[4]) y = y - speed;
		if (y < wpy[4]) y = y + speed;
		if (x == wpx[4] && y == wpy[4])  currentWP = 6; angle = wpa[4];
	}
	if (currentWP == 6)
	{
		if (x > wpx[5]) x = x - speed;
		if (x < wpx[5]) x = x + speed;
		if (y > wpy[5]) y = y - speed;
		if (y < wpy[5]) y = y + speed;
		if (x == wpx[5] && y == wpy[5])  currentWP = 7; angle = wpa[5];
	}
	if (currentWP == 7)
	{
		if (x > wpx[6]) x = x - speed;
		if (x < wpx[6]) x = x + speed;
		if (y > wpy[6]) y = y - speed;
		if (y < wpy[6]) y = y + speed;
		if (x == wpx[6] && y == wpy[6])  currentWP = 8; angle = wpa[6];
	}
	if (currentWP == 8)
	{
		if (x > wpx[7]) x = x - speed;
		if (x < wpx[7]) x = x + speed;
		if (y > wpy[7]) y = y - speed;
		if (y < wpy[7]) y = y + speed;
		if (x == wpx[7] && y == wpy[7])  currentWP = 9; angle = wpa[7];
	}
	if (currentWP == 9)
	{
		if (x > wpx[8]) x = x - speed;
		if (x < wpx[8]) x = x + speed;
		if (y > wpy[8]) y = y - speed;
		if (y < wpy[8]) y = y + speed;
		if (x == wpx[8] && y == wpy[8])  currentWP = 10; angle = wpa[8];
	}
	if (currentWP == 10)
	{
		if (x > wpx[9]) x = x - speed;
		if (x < wpx[9]) x = x + speed;
		if (y > wpy[9]) y = y - speed;
		if (y < wpy[9]) y = y + speed;
		if (x == wpx[9] && y == wpy[9])  currentWP = 11; angle = wpa[9];
	}
	if (currentWP == 11)
	{
		if (x > wpx[10]) x = x - speed;
		if (x < wpx[10]) x = x + speed;
		if (y > wpy[10]) y = y - speed;
		if (y < wpy[10]) y = y + speed;
		if (x == wpx[10] && y == wpy[10])  currentWP = 1; angle = wpa[10];
	}
}