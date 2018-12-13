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
	};
	~Controller() {};

	int ControllerID;

	bool InitializeJoyCon();
	bool ProcessControls();
	bool IsConnected;
	bool SnakeIsAccelerted();

	SnakeHead *Snake;

	void ShowSnake(HDC);
	void UpdateSnake();
	void MoveSnake();
	void CheckCollision(Controller);
	void RespawnSnake() { initializeSnake(); }
	
	

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

	GUID ownGuid;
};

