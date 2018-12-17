#include "stdafx.h"
#include "SnakeHead.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

void SnakeHead::Move()
{
	if (tail) {
		tail->Move(position);
	}
	if (!moveLock && (nextMove >= 0)) {
		direction = nextMove;
		nextMove = -1;
	}
	switch (direction)
	{
	case 0:  //UP
		position.y -= size;
		if (verticalMargin) {
			if (position.y < 0) {
				position.y = verticalMargin - size;
			}
		}
		break;
	case 1: //RIGHT
		position.x += size;
		if (horizontalMargin) {
			if (position.x > horizontalMargin - size) {
				position.x = 0;
			}
		}
		break;
	case 2:  //DOWN
		position.y += size;
		if (verticalMargin) {
			if (position.y > verticalMargin - size) {
				position.y = 0;
			}
		}
		break;
	case 3: //LEFT
		position.x -= size;
		if (horizontalMargin) {
			if (position.x < 0) {
				position.x = horizontalMargin - size;
			}
		}
		break;

	default:
		break;
	}
	moveLock = false;
}

void SnakeHead::Grow()
{
	if (tail) {
		tail->Grow();
	}
	else
	{
		SnakeBody *temp = new SnakeBody(position, size);
		//temp->next = NULL;
		tail = temp;
		temp = NULL;
	}
	
}

void SnakeHead::Show(Graphics* graph, int colorPalette)
{
	if (alive) {
		if (tail) {
			tail->Show(graph, colorPalette);
		}
		Color red = Color(255, 255, 0, 0);
		Color blue = Color(255, 0, 0, 255);
		//Graphics graphics(hdc);
		if (colorPalette == 1) {
			Pen pen(red,2);
			graph->DrawRectangle(&pen, position.x, position.y, size, size);
		}
		if (colorPalette == 2) {
			Pen pen(blue,2);
			graph->DrawRectangle(&pen, position.x, position.y, size, size);
		}
		
	}
}

void SnakeHead::GoUp()
{
	if (moveLock) {
		nextMove = 0;
	}
	if (direction != 2 && !moveLock && direction != 0) {
		direction = 0;
		nextMove = -1;
		moveLock = true;
	}
}

void SnakeHead::GoRight()
{
	if (moveLock) {
		nextMove = 1;
	}
	if (direction != 3 && !moveLock && direction != 1) {
		direction = 1;
		nextMove = -1;
		moveLock = true;
	}
}

void SnakeHead::GoDown()
{
	if (moveLock) {
		nextMove = 2;
	}
	if (direction != 0 && !moveLock && direction != 2) {
		direction = 2;
		nextMove = -1;
		moveLock = true;
	}
}

void SnakeHead::GoLeft()
{ 
	if (moveLock) {
		nextMove = 3;
	}
	if (direction != 1 && !moveLock && direction != 3) {
		direction = 3;
		nextMove = -1;
		moveLock = true;
	}
}

void SnakeHead::SetMargins(int hm, int vm)
{
	if (vm > 0 && hm >= 0) {
		horizontalMargin = hm;
		verticalMargin = vm;
	}
}

void SnakeHead::Update()
{
	//Move();
	if (tail->CheckDead(position)) {
		alive = false;
	}
}

