#include "stdafx.h"
#include "Controller.h"

LPDIRECTINPUT8 directInputDevice;
HRESULT hResult;
LPDIRECTINPUTDEVICE8 joystick_g;
GUID ownGuid_g;

BOOL CALLBACK enumCallback(const DIDEVICEINSTANCE* instance, VOID* context)
{
	HRESULT hr;
	if (!Controller::connectedDevices.CheckPresence(instance->guidInstance)) {
		hr = directInputDevice->CreateDevice(instance->guidInstance, &joystick_g, NULL);
	}
	else {
		return DIENUM_CONTINUE;
	}

	if (FAILED(hr)) {
		return DIENUM_CONTINUE;
	}

	ownGuid_g = instance->guidInstance;
	Controller::connectedDevices.Add(instance->guidInstance);
	return DIENUM_STOP;
}

bool Controller::InitializeJoyCon()
{

	if (FAILED(hResult = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (VOID**)&directInputDevice, NULL))) {
		return hResult;
	}

	if (FAILED(hResult = directInputDevice->EnumDevices(DI8DEVCLASS_GAMECTRL, enumCallback,
		NULL, DIEDFL_ATTACHEDONLY))) {
		return hResult;
	}

	if (joystick_g == NULL) {
		return FALSE;
	}
	
	joystick = joystick_g;
	ownGuid = ownGuid_g;
	joystick_g = NULL;
	ownGuid_g = {0,0,0,0};

	JoyStickProp();
	
	IsConnected = true;
	Controller::TotalConnected++;
	ControllerID = Controller::TotalConnected;

	initializeSnake();
	return TRUE;
}

void Controller::ShowSnake(Graphics *graph)
{
	if (Snake) {
		Snake->Show(graph, ControllerID);
	}
}

void Controller::UpdateSnake()
{
	if (Snake) {
		Snake->Update();
	}
}

void Controller::MoveSnake()
{
	if (Snake) {
		Snake->Move();
	}
}

void Controller::CheckCollision(Controller otherController) {
	if (otherController.IsConnected) {
		if (Snake) {
			if (otherController.Snake) {
				Snake->CheckCollision(otherController.Snake);
			}
		}
	}
}

void Controller::SetMargins(int hm, int vm)
{
	hMargin = hm;
	vMargin = vm;
	p2Start = POINT{ hm - 48, vm - 48 };
	if (Snake) {
		Snake->SetMargins(hm, vm);
	}
	
}

bool Controller::ProcessControls()
{
	if (!FAILED(JoyStickPoll(&joystickState))) {
		processStick();
		processButtons();
		return true;
	}
	else {
		Controller::TotalConnected--;
		Controller::connectedDevices.Remove(ownGuid);
		ownGuid = { 0,0,0,0 };
		Snake = NULL;
		IsConnected = false;
		return false;
	}
}

void Controller::processStick()
{
	switch (joystickState.rgdwPOV[0]) {
	case 0: //UP
		if (Snake) {
			Snake->GoUp();
		}
		break;
	case 9000: //RIGHT
		if (Snake) {
			Snake->GoRight();
		}
		break;
	case 18000: //DOWN
		if (Snake) {
			Snake->GoDown();
		}
		break;
	case 27000: //LEFT
		if (Snake) {
			Snake->GoLeft();
		}
		break;

	}
}

void Controller::processButtons()
{
	if (joystickState.rgbButtons[0] & 0x80) // A pressed
	{
		if (Snake) {
			Snake->Accelerate();
		}
	}
	if (joystickState.rgbButtons[0] == 0)   // A released
	{
		if (Snake) {
			Snake->Deccelerate();
		}
	}

	if (joystickState.rgbButtons[2] & 0x80) // B pressed
	{

	}
	if (joystickState.rgbButtons[2] == 0)   // B released
	{
		if (!Snake->alive) {
			//RespawnSnake();
		}
	}

	if (joystickState.rgbButtons[11] & 0x80) // joystick clicked
	{

	}
	if (joystickState.rgbButtons[11] == 0)  // joystick released
	{

	}
}

bool Controller::SnakeIsAccelerted()
{
	if (Snake) {
		return Snake->accelerated;
	}
	else {
		return false;
	}
}

HRESULT Controller::JoyStickProp()
{
	if (FAILED(hResult = joystick->SetDataFormat(&c_dfDIJoystick2))) {
		return hResult;
	}
	if (FAILED(hResult = joystick->SetCooperativeLevel(NULL, DISCL_EXCLUSIVE | DISCL_FOREGROUND))) {
		return hResult;
	}
	capabilities.dwSize = sizeof(DIDEVCAPS);
	if (FAILED(hResult = joystick->GetCapabilities(&capabilities))) {
		return hResult;
	}
	return S_OK;
}

HRESULT Controller::JoyStickPoll(DIJOYSTATE2 *js)
{
	HRESULT hr;

	if (joystick == NULL) {
		return E_FAIL;
	}

	// Poll the device to read the current state
	hr = joystick->Poll();
	if (FAILED(hr)) {
		hr = joystick->Acquire();
		while (hr == DIERR_INPUTLOST) {
			hr = joystick->Acquire();
		}

		if ((hr == DIERR_INVALIDPARAM) || (hr == DIERR_NOTINITIALIZED)) {
			return E_FAIL;
		}

		// If another application has control of this device, return successfully.
		// We'll just have to wait our turn to use the joystick.
		if (hr == DIERR_OTHERAPPHASPRIO) {
			return S_OK;
		}
	}

	// Get the input's device state
	if (FAILED(hr = joystick->GetDeviceState(sizeof(DIJOYSTATE2), js))) {
		return hr; // The device should have been acquired during the Poll()
	}

	return S_OK;
}

void Controller::initializeSnake()
{
	switch (ControllerID)
	{
	case 1:
		Snake = new SnakeHead(p1Start, 24, 2);
		break;

	case 2:
		Snake = new SnakeHead(p2Start, 24, 0);
		break;
	default:
		break;
	}

	Snake->SetMargins(hMargin, vMargin);
	
}



