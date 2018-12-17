#pragma once
#include "SnakePart.h"

class SnakeBody : public SnakePart {
public:
	SnakeBody(POINT pos, int nsize) : SnakePart(pos),
		size(nsize) {
		next = NULL;
	};

	void Move(POINT);
	void Grow() override;
	void Show(Graphics*, int) override;
	bool CheckDead(POINT);
	
private:
	int size;
	SnakeBody *next;

};