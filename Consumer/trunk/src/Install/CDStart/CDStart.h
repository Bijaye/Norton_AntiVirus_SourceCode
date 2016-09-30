////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CDSTART_H
#define CDSTART_H

// Frame window style
#define FRAME_STYLE				WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX

// Splash Screen
#define IDM_START				3000

// Color definitions
#define NORMAL_COLOR            RGB(214, 74, 49)
#define MOUSEOVER_COLOR         RGB(214, 74, 49)
#define MOUSEDOWN_COLOR         RGB(0, 0, 214)

// Button information
#define MAX_BUTTONS				6

#include <vector>

typedef struct _BUTTONINFO 
{
	int X;
	int Y;
	int Width;
	int Height;
	UINT nButtonTextID;
	CDibImage NormalBmp;
	CDibImage MouseOverBmp;
	CDibImage MouseDownBmp;
} BUTTONINFO;

typedef std::vector<BUTTONINFO*> ButtonInfoVector;

///////////////////////////////////////////////////////////////////////////////
//
// Function prototypes

LONG APIENTRY WndProc(HWND hWnd, UINT message, UINT wParam, LONG lParam);
BOOL InitializeBitmapInfo();
void OnCreate(HWND hWnd);
void OnPaint(HWND hWnd);
void OnEraseBackGround(HWND hWnd, HDC hdc);
BOOL DrawButtonText(HDC hDC, BUTTONINFO *pButton, COLORREF Color, HFONT *hFont, BOOL bShift = FALSE);
void OnMouseMove(HWND hWnd, int X, int Y);
BUTTONINFO* GetCurrentSelection(HDC hDC, int X, int Y);
void OnLButtonDown(HWND hWnd, int X, int Y);
void OnLButtonUp(HWND hWnd, int X, int Y);
void ProcessButtonCommand(HWND hWnd, int nButton);
BOOL GetAppDirectory(LPTSTR szDirectory);
void OnQueryNewPalette(HWND hWnd);
BOOL IsRunningAsAdministrator();
bool IsSystemHighContrastMode();
void PaintBkForHighContrastMode(HDC hDC);
VOID GetCDStartOptions();

#endif

