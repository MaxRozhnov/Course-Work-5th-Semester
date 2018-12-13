#include "stdafx.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <sstream>
#include <dinput.h>
#include "SnakeHead.h"
#include "Controller.h"

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MILLISECONDS 1000;
#define FRAMERATE 1;
#define TIMER_ID 1

int x = 0;
int y = 0;
int finished = 0;
int pause = 0;

int boxWidth = 20;
int windowWidth = 800;
int windowHeight = 600;
int horizontalSpeed = 4;
int verticalSpeed = 4;

bool oddFrame = false;


LPDWORD             updateLoopThreadID = NULL;

HWND                hWndGlobal = NULL;

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI        UpdateLoop(LPVOID);

VOID                OnPaint(HDC);
VOID                StartThreads();
VOID                OnKeyPressed(LPARAM, WPARAM, HWND);
VOID                TryConnectControllers();
VOID                TryProcessControllerInput();
VOID                TryMoveSnakes(bool);
VOID                TryShowSnakes(HDC);
VOID                OnTimer(HWND);
VOID                CheckCollisions();

GuidList Controller::connectedDevices = GuidList();
int Controller::TotalConnected = 0;

Controller PlayerOneController;
Controller PlayerTwoController;



INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
	HWND                hWnd;
	MSG                 msg;
	WNDCLASS            wndClass;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("GettingStarted");

	RegisterClass(&wndClass);

	hWnd = CreateWindow(
		TEXT("GettingStarted"),   // window class name
		TEXT("Getting Started"),  // window caption
		WS_OVERLAPPEDWINDOW,      // window style
		CW_USEDEFAULT,            // initial x position
		CW_USEDEFAULT,            // initial y position
		CW_USEDEFAULT,            // initial x size
		CW_USEDEFAULT,            // initial y size
		NULL,                     // parent window handle
		NULL,                     // window menu handle
		hInstance,                // program instance handle
		NULL);                    // creation parameters

	hWndGlobal = hWnd;
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	windowHeight = clientRect.bottom - clientRect.top;
	windowWidth = clientRect.right - clientRect.left;

	StartThreads();
	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);


	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	GdiplusShutdown(gdiplusToken);
	return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC          hdc;
	PAINTSTRUCT  ps;
	
	switch (message)
	{
	case WM_CREATE:

		PlayerOneController = Controller();
		PlayerTwoController = Controller();

		SetTimer(hWnd, TIMER_ID, 10, NULL);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(/*hWnd,*/ hdc);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		finished = 1;
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		OnKeyPressed(lParam, wParam, hWnd);
		break;
	case WM_TIMER:
		OnTimer(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
} // WndProc



DWORD WINAPI updateLoop(LPVOID lpParameter) {

	unsigned int& _pause = *((unsigned int*)lpParameter);
	while (!finished) {
		TryMoveSnakes(oddFrame);
		CheckCollisions();
		InvalidateRect(hWndGlobal, 0, true);
		oddFrame = !oddFrame;
		Sleep(125);
	}
	return 0;
}

VOID StartThreads() {
	HANDLE updateLoopHandle = CreateThread(0, 0, updateLoop, &pause, 0, updateLoopThreadID);
	//HANDLE paintRequestLoopHandle = CreateThread(0, 0, paintRequestLoop, &finished, 0, paintRequestLoopThreadID);
}

VOID OnPaint(/*HWND hWnd,*/ HDC hdc)
{
	TryShowSnakes(hdc);
}

VOID OnKeyPressed(LPARAM keyData, WPARAM virtualKeyCode, HWND hWnd) {

	if (virtualKeyCode == VK_ESCAPE) {
		pause = 1 - pause;
		if (pause) {
			OutputDebugString(L"\tPaused\n");
		}
		else {
			OutputDebugString(L"\tUnpaused\n");
		}

	}

	if (HIBYTE(GetKeyState(VK_UP))) {
		//testificate.GoUp();
	}
	if (HIBYTE(GetKeyState(VK_DOWN))) {
		//testificate.GoDown();
	}
	if (HIBYTE(GetKeyState(VK_LEFT))) {
		//testificate.GoLeft();
	}
	if (HIBYTE(GetKeyState(VK_RIGHT))) {
		//testificate.GoRight();
	}
	if (HIBYTE(GetKeyState(0x51))) { //Q
		//testificate.Grow();
	}
	if (HIBYTE(GetKeyState(0x45))) { //E
	}
}

VOID TryConnectControllers() {
	if (!PlayerOneController.IsConnected) {
		PlayerOneController.InitializeJoyCon();
	}
	else if (!PlayerTwoController.IsConnected) {
		PlayerTwoController.InitializeJoyCon();
	}
}

VOID OnTimer(HWND hWnd) {
	TryConnectControllers();
	TryProcessControllerInput();
}

VOID TryProcessControllerInput() {
	if (PlayerOneController.IsConnected) {
		PlayerOneController.ProcessControls();
	}
	if (PlayerTwoController.IsConnected) {
		PlayerTwoController.ProcessControls();
	}
}

VOID TryMoveSnakes(bool oddFrame) {
	if (!oddFrame) {
		if (PlayerOneController.IsConnected) {
			PlayerOneController.UpdateSnake();
			PlayerOneController.MoveSnake();
		}
		if (PlayerTwoController.IsConnected) {
			PlayerTwoController.UpdateSnake();
			PlayerTwoController.MoveSnake();
		}
	}
	if (oddFrame) {
		if (PlayerOneController.IsConnected) {
			if (PlayerOneController.SnakeIsAccelerted()) {
				PlayerOneController.UpdateSnake();
				PlayerOneController.MoveSnake();
			}
		}
		if (PlayerTwoController.IsConnected) {
			if (PlayerTwoController.SnakeIsAccelerted()) {
				PlayerTwoController.UpdateSnake();
				PlayerTwoController.MoveSnake();
			}
		}
	}
}

VOID TryShowSnakes(HDC hdc) {
	if (PlayerOneController.IsConnected) {
		PlayerOneController.ShowSnake(hdc);
	}
	if (PlayerTwoController.IsConnected) {
		PlayerTwoController.ShowSnake(hdc);
	}
}

VOID CheckCollisions() {
	if (PlayerOneController.IsConnected && PlayerTwoController.IsConnected) {
		PlayerOneController.CheckCollision(PlayerTwoController);
		PlayerTwoController.CheckCollision(PlayerOneController);
	}
}
