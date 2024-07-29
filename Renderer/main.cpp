// HelloWindowsDesktop.cpp
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <iostream>
#include <conio.h>
#include "renderer.h"

// Global variables
#pragma region GlobalVariables

//the accumulated values sent to the update function
float AccumulatedX = 0.0f;
float AccumulatedY = 0.0f;

//the delta x and y that are calculated each message and added to the accumulated values
float dx = 0.0f;
float dy = 0.0f;

//variables for saving the current and previous x and y values
float X = 0.0f;
float Y = 0.0f;
float prevX = 0.0f;
float prevY = 0.0f;

//bool to determine if the view matrix should be changed
bool MouseLook = false;

//boolean variables for movement
bool MoveForward = false;
bool MoveBackward = false;
bool MoveLeft = false;
bool MoveRight = false;
bool MoveUp = false;
bool MoveDown = false;

#pragma endregion

//when message is recieved calculate delta x and y

//set previous to recieved 

//add delta to accumulated delta

//when update is called it should take in accumulated delta x and delta y

//when sent delta x and y should be zeroed at the end of main loop 

//end::MouseLook CameraController;

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Engine Development");

HINSTANCE hInst;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void create_console();
void destroy_console(int exit_code);
MSG begin_main_loop();

#pragma region WindowCreationStuff

namespace
{
	HWND  main_hwnd = NULL;
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;// CHANGED FROM (HBRUSH)(COLOR_WINDOW + 1) TO REMOVE FLICKER
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Engine Development"),
			NULL);

		return 1;
	}

	// Store instance handle in our global variable
	hInst = hInstance;

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// 500, 100: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	main_hwnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_POPUPWINDOW | WS_CAPTION, // CHANGED FROM WS_OVERLAPPEDWINDOW TO DISABLE SIZING
		CW_USEDEFAULT, CW_USEDEFAULT,
		1280, 720,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!main_hwnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Engine Development"),
			NULL);

		return 1;
	}

	create_console();

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(main_hwnd, nCmdShow);
	UpdateWindow(main_hwnd);

	MSG msg = begin_main_loop();

	destroy_console((int)msg.wParam);

	return (int)msg.wParam;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam & GetKeyState('W'))
		{
			MoveForward = true;
		}
		if ( GetKeyState('S'))
		{
			MoveBackward = true;
		}
		if (wParam & GetKeyState('A'))
		{
			MoveLeft = true;
		}
		if ( GetKeyState('D'))
		{
			MoveRight = true;
		}
	

		break;

	case WM_KEYUP:
		if (MoveForward == true)
		{
			MoveForward = false;
		}
		if (MoveBackward == true)
		{
			MoveBackward = false;
		}
		if (MoveLeft == true)
		{
			MoveLeft = false;
		}
		if (MoveRight == true)
		{
			MoveRight = false;
		}


		break;
	case WM_MOUSEMOVE:

		if (wParam & MK_LBUTTON)
		{
			//set mouse look to true to tell update that the view matrix should be changed
			 MouseLook = true;
			 //set the previous x and y positions
			 prevX = X;
			 prevY = Y;
			 //capture the current x and y positions of the mouse 
			 X = GET_X_LPARAM(lParam);
			 Y = GET_Y_LPARAM(lParam);

			 
			 //calculate the delta x and delta y values
			 dx = X - prevX;
			 dy = Y - prevY;

			 if (dx <= 10.0f && dx >= -10.0f)
			 {
				 AccumulatedX += dx;
			 }
			 if (dy <= 10.0f && dy >= -10.0f)
			 {
				 AccumulatedY += dy;
			 }
			 //add values to accumulated x and y values
			 //AccumulatedX += dx;
			 //AccumulatedY += dy;


		}
		else
		{
			MouseLook = false;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

void create_console()
{
#ifndef NDEBUG
	AllocConsole();
	FILE* new_std_in_out;
	freopen_s(&new_std_in_out, "CONOUT$", "w", stdout);
	freopen_s(&new_std_in_out, "CONIN$", "r", stdin);
	std::cout << "Debug Console Opened.\n"; // Don’t forget to include <iostream>
#endif
}

void destroy_console(int exit_code)
{
#ifndef NDEBUG

	if (exit_code)
	{
		std::cout << "Exit Code: " << exit_code << "\n";
		std::cout << "Press any key to continue...";
		_getch();
	}

	FreeConsole();
#endif
}
#pragma endregion

MSG begin_main_loop()
{
	MSG msg;

	end::renderer_t renderer(main_hwnd);
	//Create points and lines
	//SetCameraMatrix(renderer);
	// Main application loop:
	while (true)
	{
		// Process all messages, stop on WM_QUIT
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// WM_QUIT does not need to be // translated or dispatched
			if (msg.message == WM_QUIT)
				break;
			// Translates messages and sends them to WndProc
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// In the future, do per frame/tick updates here...
			renderer.draw(AccumulatedX, AccumulatedY, MouseLook);
			AccumulatedX = 0.0f;
			AccumulatedY = 0.0f;
		}
	}

	return msg;
}

