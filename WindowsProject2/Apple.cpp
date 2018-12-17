#include "stdafx.h"
#include "Apple.h"
#include <stdlib.h> 

void Apple::Show(Graphics *graph)
{
	Color green = Color(255, 0, 230, 0);
	Pen pen(green, 2);
	graph->DrawRectangle(&pen, position.x + frame, position.y + frame, 24-frame*2, 24-frame*2);
	if (++frame == 12) {
		frame = 0;
	}
}

void Apple::TryEat(SnakeHead *p1, SnakeHead *p2)
{
	if (!(p1 && (p1->position.x == position.x) && (p1->position.y == position.y))) {
		if (!(p2 && (p2->position.x == position.x) && (p2->position.y == position.y))) {
			return;
		}
		else {
			p2->Grow();
		}
	}
	else {
		p1->Grow();
	}

	bool success = false;
	while (!success) {
		respawn();
		if (!overlaps(p1, p2)) {
			success = true;
		}
	}
}

void Apple::respawn()
{
	int pX = rand() % (logicalWidth );
	int pY = rand() % (logicalHeight );
	POINT newPosition{ pX*size,pY*size };

	position = newPosition;
}

bool Apple::overlaps(SnakeHead *p1, SnakeHead *p2)
{

	if (p1&&(p1->position.x == position.x) && (p1->position.y == position.y)) {
		return true;
	}
	if (p2 && (p2->position.x == position.x) && (p2->position.y == position.y)) {
		return true;
	}

	if (p1) {
		SnakeBody *p1Tail = p1->GetTail();
		if (p1Tail->CheckDead(position)) {
			return true;
		}
	}

	if (p2) {
		SnakeBody *p2Tail = p2->GetTail();
		if (p2Tail->CheckDead(position)) {
			return true;
		}
	}

	return false;
}


