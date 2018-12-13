#include "stdafx.h"
#include "SnakeBody.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

void SnakeBody::Move(POINT pos)
{
	if (next) {
		next->Move(position);
	}

	position = pos;
}

void SnakeBody::Grow()
{
	if (next) {
		next->Grow();
	}
	else
	{
		SnakeBody *temp = new SnakeBody(position, size);
		temp->next = NULL;
		next = temp;
		temp = NULL;
	}
}

void SnakeBody::Show(Graphics* graph, int colorPalette)
{
	if (next) {
		next->Show(graph, colorPalette);
	}
	//Graphics graphics(hdc);
	Color orange = Color(255, 255, 153, 0);
	Color cyan = Color(255, 0, 255, 255);
	//Graphics graphics(hdc);
	if (colorPalette == 1) {
		Pen pen(orange,2);
		graph->DrawRectangle(&pen, position.x, position.y, size, size);
	}
	if (colorPalette == 2) {
		Pen pen(cyan,2);
		graph->DrawRectangle(&pen, position.x, position.y, size, size);
	}
}

bool SnakeBody::CheckDead(POINT head)
{
	if (next) {
		return ((head.x == position.x && head.y == position.y) || next->CheckDead(head)) ;
	}
	else {
		return (head.x == position.x && head.y == position.y);
	}
}
