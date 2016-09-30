// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------
// CDStart.cpp

#include "stdafx.h"
#include "CDStart.h"
#include "Resource.h"

#include "ButtonMenu.h"
// #include "RebootHandler.h"

// ptr to the Button Menu interface
static CButtonMenu *g_pMenu = NULL;

HINSTANCE g_hInstance = NULL;
HCURSOR hCursorHand = NULL;
HCURSOR hCursorWait = NULL;


// Function prototypes
LONG APIENTRY WndProc(HWND hWnd, UINT message, UINT wParam, LONG lParam);
void OnCreate(HWND hWnd);
void OnPaint(HWND hWnd);
void OnMouseMove(HWND hWnd, int X, int Y);
void SelectNewButton(HDC hdc, int nNewButton);
void OnLButtonDown(HWND hWnd, int X, int Y);
void OnLButtonUp(HWND hWnd, int X, int Y);
void OnQueryNewPalette(HWND hWnd);

// Frame window style
#define FRAME_STYLE                WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX



void DisplayError (HWND hwndParent, UINT uErrorString) 
{
    const UINT BUFFER_SIZE = 1024;
    TCHAR szTemp[BUFFER_SIZE];
    TCHAR szTitle[BUFFER_SIZE];

    LoadString(g_hInstance, uErrorString, szTemp, BUFFER_SIZE);
    LoadString(g_hInstance, IDS_ERROR_TITLE, szTitle, BUFFER_SIZE);

    MessageBox(hwndParent, szTemp , szTitle, MB_OK);
}


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpszCmdLine*/, int nCmdShow )
{
    // Save application instance handle
    g_hInstance = hInstance;

    OSVERSIONINFO osVerInfo;
    memset( (LPVOID)&osVerInfo, 0, sizeof(OSVERSIONINFO) );
    osVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx( &osVerInfo );
    if( osVerInfo.dwPlatformId != VER_PLATFORM_WIN32_NT 
        || osVerInfo.dwMajorVersion < 5) // win 2k or greater
    {
        DisplayError (NULL, IDS_ERROR_OS);
        return 0;
    }
    // End version compliance checking.

    TCHAR szAppName[128];
    // Get application title from string table
    UINT uAppNameID = CButtonMenu::GetAppNameID();
    LoadString(hInstance, uAppNameID, szAppName, sizeof(szAppName)/sizeof(TCHAR));

    // Check if there's another copy of CD Start running
	HANDLE hMutex = ::CreateMutex (NULL, FALSE, _T("SYMC-CDSTART"));
	if (hMutex != NULL)
	{
		if (::GetLastError () == ERROR_ALREADY_EXISTS)
		{
            HWND hWndTemp = FindWindow(NULL, szAppName);
            if( hWndTemp )
            {
                ShowWindow(hWndTemp, SW_SHOWNORMAL);
                SetForegroundWindow(hWndTemp);
                return 0;
            }
		}
	}

    // initialize Button Menu Obj
    g_pMenu = new CButtonMenu;

    // Load the hand cursor
    hCursorHand = LoadCursor( NULL, IDC_HAND );
    hCursorWait = LoadCursor( NULL, IDC_WAIT );

    // Register window class
  	WNDCLASS wc;
    wc.style = CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc = (WNDPROC) WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = hCursorHand;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = szAppName;
    RegisterClass(&wc);

    // Create frame window
    HWND hWnd = CreateWindow(szAppName, szAppName, FRAME_STYLE, CW_USEDEFAULT,
       CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Main message loop
  	MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(hWnd, g_pMenu->GetCurrentAccelerator(), &msg))
        {
            TranslateMessage(&msg); 
            DispatchMessage(&msg); 
        }
    }

    // cleanup
    delete g_pMenu;
	::CloseHandle(hMutex);

    return msg.wParam;
}

LONG APIENTRY WndProc(HWND hWnd, UINT nMsg, UINT wParam, LONG lParam)
{
    switch(nMsg)
    {
    case WM_CREATE:
        OnCreate(hWnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        OnPaint(hWnd);
        return 0;
    case WM_MOUSEMOVE:
        OnMouseMove(hWnd, LOWORD(lParam), HIWORD(lParam));
        return 0;
    case WM_LBUTTONDOWN:
        OnLButtonDown(hWnd, LOWORD(lParam), HIWORD(lParam));
        return 0;
    case WM_LBUTTONUP:
        OnLButtonUp(hWnd, LOWORD(lParam), HIWORD(lParam));
        return 0;
    case WM_QUERYNEWPALETTE:
        OnQueryNewPalette(hWnd);
        return 1;
    case WM_PALETTECHANGED:
        if (hWnd != (HWND) wParam)
            OnQueryNewPalette(hWnd);
        return 0;
    case WM_COMMAND:
        // accel handlers
        switch (LOWORD(wParam))
        {
        case ID_NEXT_BUTTON:
            {
            int nButton = g_pMenu->GetCurrentButton() + 1;
            if (nButton >= g_pMenu->GetButtonCount())
                nButton = 0;

            HDC hdc = GetDC(hWnd);
            SelectNewButton (hdc, nButton);
            ReleaseDC(hWnd, hdc);
            }
            break;

        case ID_PREV_BUTTON:
            {
            int nButton = g_pMenu->GetCurrentButton();
            if (nButton == 0 || nButton == CButtonMenu::NO_BUTTON)
                nButton = g_pMenu->GetButtonCount();

            HDC hdc = GetDC(hWnd);
            SelectNewButton (hdc, nButton - 1);
            ReleaseDC(hWnd, hdc);
            }
            break;

        case ID_PUSH_BUTTON:
            if (g_pMenu->GetCurrentButton() != CButtonMenu::NO_BUTTON)
            {
                g_pMenu->ProcessButtonCommand(hWnd, g_pMenu->GetCurrentButton());
            }
            break;

        default:
            // must be a menu item accel
            g_pMenu->ProcessButtonCommand(hWnd, g_pMenu->GetButtonFromID(LOWORD(wParam)));
        }
        return 0;
    }

    return DefWindowProc(hWnd, nMsg, wParam, lParam);
}



void OnCreate(HWND hWnd)
{
    // Resize & center window on the desktop
    RECT Rect;
    Rect.top = 0;
    Rect.left = 0;
    Rect.right = g_pMenu->GetBackgroundBitmap()->GetWidth();
    Rect.bottom = g_pMenu->GetBackgroundBitmap()->GetHeight();
    
    UINT dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    AdjustWindowRect(&Rect, dwStyle, FALSE);

    RECT DesktopRect;
    SystemParametersInfo(SPI_GETWORKAREA, NULL, &DesktopRect, NULL);

    int X = ((DesktopRect.right - DesktopRect.left) - (Rect.right - Rect.left)) / 2;
    int Y = ((DesktopRect.bottom - DesktopRect.top) - (Rect.bottom - Rect.top)) / 2;

    MoveWindow(hWnd, X, Y, (Rect.right-Rect.left), (Rect.bottom-Rect.top), TRUE);

    // Point cursor to the default button
    g_pMenu->SetDefaultButton(hWnd, 0);

    // Set the application icon
    HICON hAppIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_APP));
    PostMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM) hAppIcon);
    PostMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM) hAppIcon);
}

void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);

    // Set the background bitmap's palette
    SelectPalette(ps.hdc, g_pMenu->GetBackgroundBitmap()->GetPalette(), FALSE);
    RealizePalette(ps.hdc);

    // Draw the background bitmap
    g_pMenu->GetBackgroundBitmap()->Draw(ps.hdc, 0, 0);

    // Now draw each button 
    for(int X=0; X < g_pMenu->GetButtonCount(); X++)
    {
        g_pMenu->DrawButton(ps.hdc, X);
    }

    // Clean-up
    EndPaint(hWnd, &ps);
}

void OnMouseMove(HWND hWnd, int X, int Y)
{
    HDC hdc = GetDC(hWnd);

    // First figure out which button we're now on
    int nNewButton = g_pMenu->GetButtonFromPosition (X,Y);

    SelectNewButton(hdc, nNewButton);
    ReleaseDC(hWnd, hdc);
}

void SelectNewButton (HDC hdc, int nNewButton)
{
    // If we're still at the same button, just exit
    if (nNewButton == g_pMenu->GetCurrentButton())
        return;

    int nOldButton = g_pMenu->GetCurrentButton();
    g_pMenu->SetCurrentButton(nNewButton);

    SelectPalette(hdc, g_pMenu->GetBackgroundBitmap()->GetPalette(), FALSE);
    RealizePalette(hdc);

    // If another button was selected, redraw the previous button & text (if applicable)
    if (nOldButton != CButtonMenu::NO_BUTTON)
    {
        g_pMenu->DrawButton(hdc, nOldButton);
    }

    // If not button is currently selected, we're done!
    if (nNewButton == CButtonMenu::NO_BUTTON)
    {
        return;
    }

    // Otherwise, draw the selected button and text
    g_pMenu->DrawButton(hdc, nNewButton);

    // Since someone has moved the mouse over a new button,
    // let's hear some sound...
    PlaySound(MAKEINTRESOURCE(IDR_WAVE_BTNHIGHLIGHT), g_hInstance,
        SND_RESOURCE | SND_ASYNC);

    return;
}

void OnLButtonDown(HWND hWnd, int X, int Y)
{
    _ASSERT(g_pMenu->m_bMouseDown == false);
    g_pMenu->m_bMouseDown = true;
    ::SetCapture(hWnd);

    // Find out which button was clicked (if any)
    int nButton = g_pMenu->GetButtonFromPosition (X,Y);
    if (nButton != g_pMenu->GetCurrentButton())
    {
        // probably a keyboard command has changed current button
        OnMouseMove(hWnd, X, Y);
    }

    // If no button was selected, we're done!
    if (nButton == CButtonMenu::NO_BUTTON)
        return;

    // Display the button that was clicked in its depressed state
    HDC hdc = GetDC(hWnd);

    SelectPalette(hdc, g_pMenu->GetBackgroundBitmap()->GetPalette(), FALSE);
    RealizePalette(hdc);

    g_pMenu->DrawButton(hdc, nButton);

    ReleaseDC(hWnd, hdc);
}

void OnLButtonUp(HWND hWnd, int X, int Y)
{
    _ASSERT(g_pMenu->m_bMouseDown == true);
    g_pMenu->m_bMouseDown = false;
    ::ReleaseCapture();

    // Find out which button was clicked (if any)
    int nButton = g_pMenu->GetButtonFromPosition (X,Y);

    // If no button was selected, we're done!
    if (nButton == CButtonMenu::NO_BUTTON)
        return;

    // Display the button that was clicked in its selected state
    HDC hdc = GetDC(hWnd);

    SelectPalette(hdc, g_pMenu->GetBackgroundBitmap()->GetPalette(), FALSE);
    RealizePalette(hdc);

    g_pMenu->DrawButton(hdc, nButton);

    ReleaseDC(hWnd, hdc);

    g_pMenu->ProcessButtonCommand(hWnd, nButton);
}


void OnQueryNewPalette(HWND hWnd)
{
    HDC hdc = GetDC(hWnd);

    SelectPalette(hdc, g_pMenu->GetBackgroundBitmap()->GetPalette(), FALSE);
    if (RealizePalette(hdc) > 0)
        InvalidateRect(hWnd, NULL, TRUE);

    ReleaseDC(hWnd, hdc);
}

