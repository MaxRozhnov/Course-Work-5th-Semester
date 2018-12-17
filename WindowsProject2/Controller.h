#pragma once
#include "stdafx.h"
#include <dinput.h>
#include <windows.h>
#include "GuidList.h"
#include "SnakeHead.h"



class Controller {
public:
	Controller() {
		Snake = NULL;
		IsConnected = false;
		hMargin = 0;
		vMargin = 0;
	};
	~Controller() {};

	int ControllerID;

	bool InitializeJoyCon();
	bool ProcessControls();
	bool IsConnected;
	bool SnakeIsAccelerted();
	bool SnakeIsALive() { return Snake ? Snake->alive : false; }

	SnakeHead *Snake;

	void ShowSnake(Graphics*);
	void UpdateSnake();
	void MoveSnake();
	void CheckCollision(Controller);
	void RespawnSnake() { initializeSnake(); }
	void SetMargins(int, int);
	void Restart() { delete Snake; initializeSnake(); }
	
	

	static GuidList connectedDevices;	
	static int TotalConnected;
	
	
private:
	LPDIRECTINPUTDEVICE8 joystick;
	DIDEVCAPS capabilities;
	DIJOYSTATE2 joystickState;
	

	HRESULT             JoyStickProp();
	HRESULT             JoyStickPoll(DIJOYSTATE2 *js);

	void processStick();
	void processButtons();
	void initializeSnake();

	POINT p1Start{ 24,24 };
	POINT p2Start{ 792-48, 576-48 };

	int hMargin;
	int vMargin;
	

	GUID ownGuid;
};

