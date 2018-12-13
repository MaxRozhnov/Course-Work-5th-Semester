#pragma once
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

class SnakePart {
public:
	POINT position;
	SnakePart(POINT pos) : position(pos){};
	~SnakePart() {};

	virtual void Grow() = 0;
	virtual void Show(HDC) = 0;
};