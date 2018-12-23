#pragma once
#include "stdafx.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "SnakeHead.h"
#include "SnakePart.h"
#include <stdlib.h> 
#include <time.h> 

using namespace Gdiplus;

class Apple {
public:
	Apple(POINT pos, int lWidth, int lHeight) {
		srand(time(NULL));
		frame = 0;
		position = pos;
		size = 24;
		logicalHeight = lHeight;
		logicalWidth = lWidth;
		respawn();
	}
	
	void Show(Graphics*);
	void TryEat(SnakeHead*, SnakeHead*);
	
	
private:
	void  respawn();
	POINT position;
	bool  overlaps(SnakeHead*, SnakeHead*);
	int frame;
	int size;
	int logicalWidth;
	int logicalHeight;
};