#include "stdafx.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <sstream>
#include <dinput.h>
#include "SnakeHead.h"
#include "Controller.h"
#include "Apple.h"

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MILLISECONDS 1000
#define FRAMERATE 1
#define TIMER_ID 1

#define TILE_WIDTH 24
#define H_EXCESS 9
#define V_EXCESS 32

#define LOGICAL_WIDTH 45
#define LOGICAL_HEIGHT 35

#define MESSAGE_HEIGHT 50
#define MESSAGE_WIDTH 400

#define ACTUAL_WIDTH ((LOGICAL_WIDTH * TILE_WIDTH) + H_EXCESS)
#define ACTUAL_HEIGHT ((LOGICAL_HEIGHT * TILE_WIDTH) + V_EXCESS)

int x = 0;
int y = 0;
int finished = 0;
int pause = 0;
int displayingLostMessage = 0;

int boxWidth = 20;
int windowWidth = TILE_WIDTH * LOGICAL_WIDTH + H_EXCESS;
int windowHeight = TILE_WIDTH * LOGICAL_HEIGHT + V_EXCESS;

bool SnakeOneDied = false;
bool SnakeTwoDied = false;

Apple apple = Apple(POINT{0,0}, LOGICAL_WIDTH, LOGICAL_HEIGHT);



int horizontalSpeed = 4;
int verticalSpeed = 4;

bool oddFrame = false;


LPDWORD             updateLoopThreadID = NULL;
LPDWORD             controllerLoopThreadID = NULL;

HWND                hWndGlobal = NULL;

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI        updateLoop(LPVOID);
DWORD WINAPI        controllerLoop(LPVOID);

VOID                OnPaint(HWND, HDC);
VOID                StartThreads();
VOID                OnKeyPressed(LPARAM, WPARAM, HWND);
VOID                TryConnectControllers();
VOID                TryProcessControllerInput();
VOID                TryMoveSnakes(bool);
VOID                TryShowSnakes(Graphics*);
VOID                OnTimer(HWND);
VOID                CheckCollisions();
VOID                TryRestart();






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
	wndClass.lpszClassName = TEXT("Snakes_2");

	RECT windowClientArea{ 0,0, windowWidth, windowHeight };
	AdjustWindowRect(&windowClientArea, WS_OVERLAPPEDWINDOW, FALSE);

	RegisterClass(&wndClass);

	hWnd = CreateWindow(
		TEXT("Snakes_2"),   // window class name
		TEXT("Snakes 2"),  // window caption
		WS_OVERLAPPEDWINDOW,      // window style
		CW_USEDEFAULT,            // initial x position
		CW_USEDEFAULT,            // initial y position
		windowClientArea.right,            // initial x size
		windowClientArea.bottom,            // initial y size
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
		PlayerOneController.SetMargins(LOGICAL_WIDTH * TILE_WIDTH, LOGICAL_HEIGHT * TILE_WIDTH);
		PlayerTwoController.SetMargins(LOGICAL_WIDTH * TILE_WIDTH, LOGICAL_HEIGHT * TILE_WIDTH);


		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hWnd, hdc);
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
		TryRestart();
		if(!displayingLostMessage) {
			TryMoveSnakes(oddFrame);
			CheckCollisions();
			SnakeOneDied = false;
			SnakeTwoDied = false;
		}
		else {
			displayingLostMessage--;
		}

		InvalidateRect(hWndGlobal, 0, false);
		oddFrame = !oddFrame;
		if (PlayerOneController.Snake) {
			apple.TryEat(PlayerOneController.Snake, PlayerTwoController.Snake);
		}
		if (PlayerTwoController.Snake) {
			apple.TryEat(PlayerOneController.Snake, PlayerTwoController.Snake);
		}

		Sleep(75);
	}
	return 0;
}

DWORD WINAPI controllerLoop(LPVOID lpParameter) {

	while (!finished) {
		TryConnectControllers();
		TryProcessControllerInput();
		Sleep(10);
	}
	return 0;
}

VOID StartThreads() {
	HANDLE updateLoopHandle = CreateThread(0, 0, updateLoop, &pause, 0, updateLoopThreadID);
	HANDLE controllerLoopHandle = CreateThread(0, 0, controllerLoop, &pause, 0, controllerLoopThreadID);
}

VOID OnPaint(HWND hWnd, HDC hdc)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	Bitmap buffer(rc.right, rc.bottom);
	Graphics graphicsbuf(&buffer);
	Graphics graphics(hdc);
	graphicsbuf.Clear(Color(255, 0, 0, 0));
	Graphics *temp = &graphicsbuf;

	if ((PlayerOneController.IsConnected || PlayerTwoController.IsConnected) && !displayingLostMessage) {
		apple.Show(temp);
	}

	TryShowSnakes(temp);
	

	graphics.DrawImage(&buffer, 0, 0);
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
		//forTest.GoUp();
	}
	if (HIBYTE(GetKeyState(VK_DOWN))) {
		//forTest.GoDown();
	}
	if (HIBYTE(GetKeyState(VK_LEFT))) {
		//forTest.GoLeft();
	}
	if (HIBYTE(GetKeyState(VK_RIGHT))) {
		//forTest.GoRight();
	}
	if (HIBYTE(GetKeyState(0x51))) { //Q
		//forTest.Grow();
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

VOID TryShowSnakes(Graphics *graph) {
	if (!displayingLostMessage) {
		if (PlayerOneController.IsConnected) {
			PlayerOneController.ShowSnake(graph);
		}
		if (PlayerTwoController.IsConnected) {
			PlayerTwoController.ShowSnake(graph);
		}
	}
	else {

		RectF temp;
		Font font(&FontFamily(L"Arial"), 30);
		PointF screenCenter(ACTUAL_WIDTH/2,ACTUAL_HEIGHT/2);
		RectF messageRect(0, 0, MESSAGE_WIDTH, MESSAGE_HEIGHT);
		LinearGradientBrush brush(Rect(0, 0, 100, 100), Color::White, Color::White, LinearGradientModeHorizontal);

		if (PlayerOneController.IsConnected && PlayerTwoController.IsConnected) {
			if (SnakeOneDied && SnakeTwoDied) {
				graph->MeasureString(L"It's a tie!", -1, &font, messageRect, &temp);
				PointF ActualPosition(screenCenter.X - temp.GetRight() / 2, screenCenter.Y - temp.GetBottom() / 2);
				Status st = graph->DrawString(L"It's a tie!", -1, &font, PointF(ActualPosition), &brush);
			}
			else if (SnakeTwoDied) {
				graph->MeasureString(L"Player one wins!", -1, &font, messageRect, &temp);
				PointF ActualPosition(screenCenter.X - temp.GetRight() / 2, screenCenter.Y - temp.GetBottom() / 2);
				Status st = graph->DrawString(L"Player one wins!", -1, &font, PointF(ActualPosition), &brush);
			}
			else if (SnakeOneDied) {
				graph->MeasureString(L"Player two wins!", -1, &font, messageRect, &temp);
				PointF ActualPosition(screenCenter.X - temp.GetRight() / 2, screenCenter.Y - temp.GetBottom() / 2);
				Status st = graph->DrawString(L"Player two wins!", -1, &font, PointF(ActualPosition), &brush);
			}
		}
		else {
			graph->MeasureString(L"You Lost!", -1, &font, messageRect, &temp);
			PointF ActualPosition(screenCenter.X - temp.GetRight() / 2, screenCenter.Y - temp.GetBottom() / 2);
			Status st = graph->DrawString(L"You Lost!", -1, &font, ActualPosition, &brush);
		}

	}

}

VOID CheckCollisions() {
	if (PlayerOneController.IsConnected && PlayerTwoController.IsConnected) {
		PlayerOneController.CheckCollision(PlayerTwoController);
		PlayerTwoController.CheckCollision(PlayerOneController);
	}
}

VOID TryRestart() {
	if (PlayerOneController.IsConnected) {
		if (!PlayerOneController.SnakeIsALive()) {
			displayingLostMessage = 30;
			SnakeOneDied = true;
			PlayerOneController.Restart();
			if (PlayerTwoController.IsConnected) {
				if (!PlayerTwoController.SnakeIsALive()) {
					SnakeTwoDied = true;
				}
				PlayerTwoController.Restart();
			}
		}
	}
	if (PlayerTwoController.IsConnected) {
		if (!PlayerTwoController.SnakeIsALive()) {
			displayingLostMessage = 30;
			SnakeTwoDied = true;
			PlayerTwoController.Restart();
			if (PlayerOneController.IsConnected) {
				if (!PlayerOneController.SnakeIsALive()) {
					SnakeOneDied = true;
				}
				PlayerOneController.Restart();
				
			}
		}
	}

}
