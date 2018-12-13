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
		break;
	case 1: //RIGHT
		position.x += size;
		break;
	case 2:  //DOWN
		position.y += size;
		break;
	case 3: //LEFT
		position.x -= size;
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

void SnakeHead::Show(HDC hdc)
{
	if (alive) {
		if (tail) {
			tail->Show(hdc);
		}
		Graphics graphics(hdc);
		Pen      pen(Color(255, 255, 0, 0));
		graphics.DrawRectangle(&pen, position.x, position.y, size, size);
	}
}

void SnakeHead::GoUp()
{
	if (moveLock) {
		nextMove = 0;
	}
	if (direction != 2 && !moveLock) {
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
	if (direction != 3 && !moveLock) {
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
	if (direction != 0 && !moveLock) {
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
	if (direction != 1 && !moveLock) {
		direction = 3;
		nextMove = -1;
		moveLock = true;
	}
}

void SnakeHead::Update()
{
	//Move();
	if (tail->CheckDead(position)) {
		alive = false;
	}
}

