#pragma once
#include "SnakePart.h"
#include "SnakeBody.h"

class SnakeHead : public SnakePart {
public:
	SnakeHead(POINT pos, int nsize, int ndirection) : SnakePart(pos), 
													  size(nsize), 
		direction(ndirection) 
	{
		moveLock = false;
		accelerated = false;
		alive = true;
		nextMove = -1;
		Grow();
		Grow();
		Move();
		Move();
	};

	bool alive;
	bool accelerated;

	void Accelerate() { accelerated = true; }
	void Deccelerate() { accelerated = false; }

	void Move();
	void Grow() override;
	void Show(HDC) override;

	void GoUp();
	void GoRight();
	void GoDown();
	void GoLeft();

	void Update();

	void Die() { alive = false; }

	void CheckCollision(SnakeHead *otherSnake) {
		if ((otherSnake->position.x == position.x) && (otherSnake->position.y == position.y)) {
			otherSnake->Die();
			Die();
		}
		else {
			SnakeBody *otherSnakeTail = otherSnake->GetTail();
			if (otherSnakeTail->CheckDead(position)) {
				Die();
			}
		}
	}

	void Respawn() {
		tail = NULL;
		moveLock = false;
		accelerated = false;
		alive = true;
		nextMove = -1;
		Grow();
		Grow();
		Move();
		Move();

	}

	SnakeBody *GetTail() { return tail; }

private:
	int direction; //0 - UP
				   //1 - RIGHT
				   //2 - DOWN
				   //3 - LEFT
	int nextMove;
	int size;
	bool moveLock;
	
	SnakeBody *tail;
};