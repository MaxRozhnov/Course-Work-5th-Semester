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

void SnakeBody::Show(HDC hdc)
{
	if (next) {
		next->Show(hdc);
	}
	Graphics graphics(hdc);
	Pen      pen(Color(255, 0, 0, 255));
	graphics.DrawRectangle(&pen, position.x, position.y, size, size);
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
