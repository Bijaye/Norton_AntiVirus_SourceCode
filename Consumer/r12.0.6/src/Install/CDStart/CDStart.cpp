
#include "stdafx.h"
#include "Dib.h"
#include "CDStart.h"
#include "cdstrhlp.h"
#include "Resource.h"
#include "..\CDStartRes\ResResource.h"
#include "ShellApi.h"
#include "Mmsystem.h"
#include "malloc.h"
#include <shlobj.h>
#include <crtdbg.h>
#include "NavOptHelper.h"
#include "InstOptsNames.h"
#include "FileAction.h"
#include "AVRES.h"
#include "osVerInfo.h"
#include "atlstr.h"
#include "NAVDetection.h"

///////////////////////////////////////////////////////////////////////////////
//
// Global variables

HICON hAppIcon = NULL;
HACCEL hAppAccel = NULL;
HINSTANCE hAppInstance = NULL;
HMODULE m_hResMod = NULL;
CDibImage           BackgroundBmp;
HCURSOR hCursorHand = NULL;
HFONT hFont= NULL;
HFONT hSelFont = NULL;
HWND hWnd = NULL;
HWND g_hPullDownWnd = NULL; 
bool                bIsNAVPro = false;
BOOL                bDirtyScreen = FALSE;
HDC                 g_hOffscreenDC = NULL;
HBITMAP             g_hOffscreenBMP = NULL;
HBITMAP             g_hOldOffscreenBMP = NULL;
LPTSTR g_lpszCmdLine = NULL;
BOOL                g_bInstallInProgress = FALSE;

RECT                g_scrnRect = {0};	
BOOL				g_bShowOffers = FALSE;
BOOL				g_bShowBrowse = TRUE;
HMODULE				g_hUIRES = NULL;

BUTTONINFO*         pLastButton = NULL;
ButtonInfoVector    AppButtons;
ButtonInfoVector::iterator iAppButtons = NULL;

CString g_csProductName;
CString g_csProductNameWithVersion;

TCHAR g_szFileLocation[MAX_PATH] = {0};

const TCHAR g_szExtraSectionName[]		= _T("extra");

const TCHAR g_szLayoutType[]				= _T("LAYOUTTYPE");
const TCHAR g_szLayoutTypeMultipleOS[]	= _T("MULTIPLEOS");

const TCHAR g_szWINNT_LAYOUT[]					= _T("WinNT");
const TCHAR g_szWIN9X_LAYOUT[]					= _T("Win9x");
BOOL				g_bMultiOS = FALSE;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	// FIRST CHECK : MinOS requirement
	InstallToolBox::COSVerInfo OSVI;
	if(!OSVI.CheckMinOS(InstallToolBox::COSVerInfo::ITB_OSI_WIN98, 
		InstallToolBox::COSVerInfo::ITB_OSI_WIN2K, 0, 0))
	{
		ATL::CString sMsg;
		sMsg.LoadString(m_hResMod, IDS_MIN_OS);
		MessageBox(NULL, sMsg, g_csProductName, MB_ICONEXCLAMATION | MB_OK);

		return 0;
	}

    // Get the Module Path
	TCHAR *pszBackSlash = NULL;
	GetModuleFileName(NULL, g_szFileLocation, sizeof(g_szFileLocation));
	pszBackSlash = _tcsrchr(g_szFileLocation, _T('\\'));
	*pszBackSlash = _T('\0');
	
	// Load the Resource DLL
	TCHAR szResDLL[MAX_PATH] = {0};
	wsprintf(szResDLL, _T("%s\\CDStart.loc"), g_szFileLocation);
	m_hResMod = LoadLibraryEx(szResDLL, NULL, LOAD_LIBRARY_AS_DATAFILE);

	// CDStart.loc may be named Autorun.loc (as cdstart.exe may be called autorun.exe) so 
	// if the LoadLibraryEx above fails ... we will try once more.
	if(!m_hResMod)
	{
		wsprintf(szResDLL, _T("%s\\Autorun.loc"), g_szFileLocation);
		m_hResMod = LoadLibraryEx(szResDLL, NULL, LOAD_LIBRARY_AS_DATAFILE);
		if(!m_hResMod)
			return 0;
	}

	// Check the AUTORUN.INF file to determine layout type
	TCHAR szLayoutCheck[MAX_PATH] = {0};
	TCHAR szAutoRun[MAX_PATH] = {0};
	wsprintf(szAutoRun, _T("%s\\AutoRun.inf"), g_szFileLocation);
	GetPrivateProfileString(g_szExtraSectionName, g_szLayoutType, _T(""), szLayoutCheck, sizeof(szLayoutCheck)/sizeof(szLayoutCheck[0]), szAutoRun);
	if ( _tcsicmp(szLayoutCheck, g_szLayoutTypeMultipleOS ) == 0 )
	{
		g_bMultiOS = TRUE;
		
        //Multiple OS layout, need to determine OS version for correct NAVSetup.exe
        //Since its passed the MinOS test, it must be 9x or NT
		if ( OSVI.Is9x() )
			wsprintf(g_szFileLocation, _T("%s\\%s"), g_szFileLocation, g_szWIN9X_LAYOUT);
		else
			wsprintf(g_szFileLocation, _T("%s\\%s"), g_szFileLocation, g_szWINNT_LAYOUT);
	}

	// Load the AVRES dll
	TCHAR szUIRES[MAX_PATH] = {0};
	wsprintf(szUIRES, _T("%s\\NAV\\External\\NORTON\\AVRES.dll"), g_szFileLocation);
	g_hUIRES = LoadLibraryEx(szUIRES, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if(!g_hUIRES)
		return 0;

	// Get product name and version from AVRES.DLL
	g_csProductName.LoadString(g_hUIRES, IDS_AV_PRODUCT_NAME);
	g_csProductNameWithVersion.LoadString(g_hUIRES, IDS_AV_PRODUCT_NAME_VERSION);	

	//check if we're navpro
	TCHAR szAdvTools[MAX_PATH] = {0};
	wsprintf(szAdvTools, _T("%s\\AdvTools"), g_szFileLocation);
	if (-1 != GetFileAttributes(szAdvTools))
		bIsNAVPro = true;

	MSG msg;
	WNDCLASS wc;
	TCHAR szWndClassName[256] = {0};

	_tcscpy(szWndClassName, _T("SYM_NAV_CDStartWnd"));

	// Allow only one instance of this app
	if (hWnd = FindWindow(szWndClassName, g_csProductNameWithVersion))
	{
		ShowWindow(hWnd, SW_SHOWNORMAL);
		SetForegroundWindow(hWnd);
		return 0;
	}

	// Save application instance handle
	hAppInstance = hInstance;

	// copy the commandline
	g_lpszCmdLine = lpszCmdLine;

	// Load the application's acclerators
	hAppAccel = LoadAccelerators(m_hResMod, MAKEINTRESOURCE(IDR_ACCEL));

	// Load the bitmaps
	if (InitializeBitmapInfo() == FALSE)
		return 0;

	// Load the hand cursor
	hCursorHand = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_HAND));

	// Create the font
	TCHAR szFont[MAX_PATH];
	LoadString(m_hResMod, IDS_FONT_SIZE, szFont, MAX_PATH);
	int nFontSize = atoi(szFont);
	LoadString(m_hResMod, IDS_FONT, szFont, MAX_PATH);

	hFont = CreateFont(-MulDiv(nFontSize, 96, 72), 0, 0, 0, FW_NORMAL, FALSE,
		FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, szFont);

	hSelFont = CreateFont(-MulDiv(nFontSize, 96, 72), 0, 0, 0, FW_NORMAL, FALSE,
		TRUE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, szFont);

	// Register window class
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = (WNDPROC) WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
	wc.lpszMenuName =  NULL;
	wc.lpszClassName = szWndClassName;
	RegisterClass(&wc);

	// Create frame window   
	hWnd = CreateWindowEx(WS_EX_CONTROLPARENT | WS_EX_APPWINDOW, szWndClassName, g_csProductNameWithVersion, FRAME_STYLE | WS_CLIPCHILDREN, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Main message loop
	while(GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hWnd, hAppAccel, &msg))
		{
			if(!IsDialogMessage(hWnd, &msg))
			{
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			}
		}
	}

	// Clean-up.

	for(iAppButtons = AppButtons.begin(); iAppButtons != AppButtons.end(); iAppButtons++)
	{
		delete (*iAppButtons);
	}

	if(g_hUIRES)
		FreeLibrary(g_hUIRES);

	// Delete the font object
	DeleteObject(hFont);
	DeleteObject(hSelFont);

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
		if (LOWORD(wParam) == ID_INSTALL)
			ProcessButtonCommand(hWnd, IDS_INSTALL_BUTTON);	
		if (LOWORD(wParam) == ID_BROWSE)	
		{
			if(g_bShowBrowse)
			{
				ProcessButtonCommand(hWnd, IDS_BROWSE_BUTTON);		
			}
		}

		//we dont have videos anymore...uncomment this and shift the numbers down to re-add them
		//	if (LOWORD(wParam) == ID_VIDEOS)
		//		ProcessButtonCommand(hWnd, 2);
		if (LOWORD(wParam) == ID_OFFERS)
		{
			if(g_bShowOffers)
			{
				ProcessButtonCommand(hWnd, IDS_OFFERS_BUTTON);
			}
		}
		if (LOWORD(wParam) == ID_EXIT)
			ProcessButtonCommand(hWnd, IDS_EXIT_BUTTON);
		return 0;

	case WM_NCDESTROY:
		if( g_hOffscreenDC )
		{
			if( g_hOldOffscreenBMP )
			{
				SelectObject( g_hOffscreenDC, g_hOldOffscreenBMP );
				DeleteObject( g_hOffscreenBMP );
			}
			DeleteDC( g_hOffscreenDC );
		}
		break;
	case WM_ERASEBKGND:
		OnEraseBackGround(hWnd, (HDC) wParam);
		return 1;
	}
	return DefWindowProc(hWnd, nMsg, wParam, lParam);
}

BOOL InitializeBitmapInfo()
{
	int nYPos = 151;
	int nXPos = 0;
	int nWidth = 300;
	int nHeight = 23;

	if (bIsNAVPro)
		nXPos = 52;
	else nXPos = 51;

	BUTTONINFO* pThisButton = NULL;

	if (BackgroundBmp.Load(IDB_CDSTARTBACKGROUND, g_hUIRES) == FALSE)
		return FALSE;

	//initalize all "constants" in buttons.
	//ThisButton.Y = nYPos;
	//ThisButton.X = nXPos;
	//ThisButton.Height = nHeight;
	//ThisButton.Width = nWidth;

	//initialize 5 buttons.  increase number of loops to increase number of
	//buttons and add more cases in interior swtich.
	//NOTE: Graphics only supports 6 buttons at this point.

	// Check all CDStart options such as 
	// whether we should show browse or options button...
	GetCDStartOptions();

	for(int x=1; x<=4; x++)
	{
		if (x==2 && !g_bShowBrowse)
			continue;

		if (x==3 && !g_bShowOffers)
			continue;

		pThisButton = new BUTTONINFO;
		pThisButton->X = nXPos;
		pThisButton->Y = nYPos;
		pThisButton->Width = nWidth;
		pThisButton->Height = nHeight;
        pThisButton->nButtonTextID = 0;

		//initialize each button's unique information...
		switch (x)
		{
		case 1:
			pThisButton->nButtonTextID = IDS_INSTALL_BUTTON;
			break;
		case 2:
			pThisButton->nButtonTextID = IDS_BROWSE_BUTTON;
			break;
			/*			case 3:
			pThisButton->nButtonTextID = IDS_VIEW_VIDEO_BUTTON;
			pThisButton->NormalBmp.Load(IDB_3UP);
			pThisButton->MouseOverBmp.Load(IDB_3HI);
			pThisButton->MouseDownBmp.Load(IDB_3DOWN);
			break;
			*/			
		case 3:
			pThisButton->nButtonTextID = IDS_OFFERS_BUTTON;
			break;
		case 4:
			pThisButton->nButtonTextID = IDS_EXIT_BUTTON;
			break;
		}

        // Prevent 'empty' buttons, which cause defect 1-4EPQSW to occur
        if(pThisButton->nButtonTextID > 0)
    		AppButtons.push_back(pThisButton);
        else    // Delete empty buttons to prevent memory leak
            delete pThisButton;

		nYPos += 30;
	}

	return TRUE;
}


void OnCreate(HWND hWnd)
{
	// Resize & center window on the desktop
	RECT Rect;
	Rect.top = 0;
	Rect.left = 0;
	Rect.right = BackgroundBmp.GetWidth() - 2;
	Rect.bottom = BackgroundBmp.GetHeight() - 2;
	AdjustWindowRect(&Rect, WS_OVERLAPPEDWINDOW, FALSE);

	RECT DesktopRect;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &DesktopRect, NULL);

	int X = ((DesktopRect.right - DesktopRect.left) - (Rect.right - Rect.left)) / 2;
	int Y = ((DesktopRect.bottom - DesktopRect.top) - (Rect.bottom - Rect.top)) / 2;

	MoveWindow(hWnd, X, Y, (Rect.right-Rect.left), (Rect.bottom-Rect.top), TRUE);

	// Set the application icon
	hAppIcon = LoadIcon(hAppInstance, MAKEINTRESOURCE(IDI_APP));
	PostMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM) hAppIcon);
	PostMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM) hAppIcon);
	PostMessage(hWnd, WM_COMMAND, IDM_START, 0L);

	// Initialize the current selection
	pLastButton = NULL;

	g_hOffscreenDC = CreateCompatibleDC(GetDC( hWnd ) );
	g_hOffscreenBMP = CreateCompatibleBitmap( GetDC( hWnd ), BackgroundBmp.GetWidth(), BackgroundBmp.GetHeight() );
	g_hOldOffscreenBMP = (HBITMAP) SelectObject( g_hOffscreenDC, g_hOffscreenBMP );

	// Draw the background bitmap
	BackgroundBmp.Draw(g_hOffscreenDC, 0, 0);

	ButtonInfoVector::iterator it = AppButtons.begin();

	while(it != AppButtons.end())
		DrawButtonText(g_hOffscreenDC, (*it++), NORMAL_COLOR, &hFont, FALSE);
}        

void OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	BeginPaint(hWnd, &ps);

	// Clean-up
	EndPaint(hWnd, &ps);
}

void OnEraseBackGround(HWND hWnd, HDC hdc)
{
	if(IsSystemHighContrastMode())
	{
		PaintBkForHighContrastMode(hdc);
	}
	else
	{
		// Set the background bitmap's palette
		SelectPalette(hdc, BackgroundBmp.GetPalette(), FALSE);
		RealizePalette(hdc);

		BitBlt(hdc,0,0,BackgroundBmp.GetWidth(), BackgroundBmp.GetHeight(), 
			g_hOffscreenDC, 0, 0, SRCCOPY);
	}
}



BOOL DrawButtonText(HDC hDC, BUTTONINFO *pButton, COLORREF Color, HFONT *hFont, BOOL bShift)
{

	// Set the text attributes
	SetBkMode(hDC, TRANSPARENT);

	if(IsSystemHighContrastMode())
	{
		COLORREF foreground = GetSysColor(COLOR_WINDOWTEXT);
		SetTextColor(hDC, foreground);
	}
	else
	{
		SetTextColor(hDC, Color | 0x02000000);
	}

	// Select the font
	HFONT hOldFont = (HFONT) SelectObject(hDC, *hFont);

	TCHAR szButtonText[256] = {0};
	WCHAR wszButtonText[256] = {0};

	if(pButton->nButtonTextID == IDS_INSTALL_BUTTON)
	{
		TCHAR szInstallText[MAX_PATH] = {0};

		// Setup "&Install Norton AntiVirus" text.
		LoadString(m_hResMod, pButton->nButtonTextID, szInstallText, sizeof(szInstallText));
		wsprintf(szButtonText, szInstallText, g_csProductNameWithVersion);
	}
	else
	{
		LoadString(m_hResMod, pButton->nButtonTextID, szButtonText, sizeof(szButtonText));
	}

	// Calculate the button text rectangle
	RECT Rect;
	Rect.left = pButton->X + 29;
	Rect.top = pButton->Y;
	Rect.right = pButton->X + pButton->Width;
	Rect.bottom = pButton->Y + pButton->Height;

	// Do we want to shift the text over by a pixel?
	if (bShift == TRUE)
	{
		Rect.top++;
		Rect.left++;
	}

	DrawText(hDC, szButtonText, -1, &Rect, DT_SINGLELINE | DT_VCENTER);

	// Select the old font again
	SelectObject(hDC, hOldFont);

	return TRUE;
}

void OnMouseMove(HWND hWnd, int X, int Y)
{
	HRGN    hrLogoRgn;
	RECT    rcClient;

	GetClientRect(hWnd,&rcClient);

	hrLogoRgn = CreateRectRgn(rcClient.left, rcClient.top, 
		rcClient.right, rcClient.bottom); 

	HDC hDC = GetDC(hWnd);
	SelectClipRgn(hDC, hrLogoRgn);

	BUTTONINFO *pButton;

	pButton = GetCurrentSelection(hDC, X, Y);

	// If we're still at the same button, just exit
	if (pButton != pLastButton)
	{
		if( pLastButton != NULL)
		{
			RECT rRect = { pLastButton->X, 
				pLastButton->Y, 
				pLastButton->X + pLastButton->Width, 
				pLastButton->Y + pLastButton->Height};

			InvalidateRect(hWnd, &rRect, TRUE );
		}

		if(pButton != NULL)
		{
			DrawButtonText(hDC, pButton, MOUSEOVER_COLOR, &hSelFont, FALSE);

			// Since someone has moved the mouse over a new button,
			// let's hear some sound...
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_BTNHIGHLIGHT), hAppInstance,	SND_RESOURCE | SND_ASYNC);
		}

	}

	pLastButton = pButton;

	DeleteObject(hrLogoRgn);
	ReleaseDC(hWnd, hDC);
}

BUTTONINFO* GetCurrentSelection(HDC hDC, int X, int Y)
{
	// First figure out which button we're now on
	ButtonInfoVector::iterator iButton;

	for(iButton = AppButtons.begin(); iButton != AppButtons.end(); iButton++)
	{
		if (X >= (*iButton)->X && (X <= (*iButton)->X + (*iButton)->Width) &&
			Y >= (*iButton)->Y && (Y <= (*iButton)->Y + (*iButton)->Height))
		{
			return (*iButton);
		}
	}
	return NULL;
}

void OnLButtonDown(HWND hWnd, int X, int Y)
{
	// Find out which button was clicked (if any)
	ButtonInfoVector::iterator iButton = NULL;

	for(iButton = AppButtons.begin(); iButton != AppButtons.end(); iButton++)
	{
		if (X >= (*iButton)->X && (X <= (*iButton)->X + (*iButton)->Width) &&
			Y >= (*iButton)->Y && (Y <= (*iButton)->Y + (*iButton)->Height))
		{
			break;
		}
	}

	// If no button was selected, we're done!
	if (iButton == AppButtons.end())
		return;

	// Display the button that was clicked in its depressed state
	HDC hDC = GetDC(hWnd);

	SelectPalette(hDC, BackgroundBmp.GetPalette(), FALSE);
	RealizePalette(hDC);

	(*iButton)->MouseDownBmp.Draw(hDC, (*iButton)->X,
		(*iButton)->Y);

	DrawButtonText(hDC, (*iButton), MOUSEDOWN_COLOR, &hSelFont, FALSE);

	ReleaseDC(hWnd, hDC);
}

void OnLButtonUp(HWND hWnd, int X, int Y)
{
	// Find out which button was clicked (if any)
	ButtonInfoVector::iterator iButton = NULL;

	for(iButton = AppButtons.begin(); iButton != AppButtons.end(); iButton++)
	{
		if (X >= (*iButton)->X && (X <= (*iButton)->X + (*iButton)->Width) &&
			Y >= (*iButton)->Y && (Y <= (*iButton)->Y + (*iButton)->Height))
		{
			break;
		}
	}

	// If no button was selected, we're done!
	if (iButton == AppButtons.end())
		return;

	// Display the button that was clicked in its selected state
	HDC hDC = GetDC(hWnd);

	SelectPalette(hDC, BackgroundBmp.GetPalette(), FALSE);
	RealizePalette(hDC);

	(*iButton)->MouseOverBmp.Draw(hDC, (*iButton)->X, (*iButton)->Y);

	DrawButtonText(hDC, (*iButton), MOUSEOVER_COLOR, &hFont, FALSE);

	ReleaseDC(hWnd, hDC);

	// Post the appropriate command message to the window

	ProcessButtonCommand(hWnd, (*iButton)->nButtonTextID);
}		

void ProcessButtonCommand(HWND hWnd, int iButtonID)
{
	TCHAR szDirectory[MAX_PATH];	
	GetAppDirectory(szDirectory);
	int nNAV = 0;

	// Play our button down sound
	PlaySound(MAKEINTRESOURCE(IDR_WAVE_BTNDOWN), hAppInstance, SND_RESOURCE | SND_SYNC);

	switch(iButtonID)
	{
	default:
		break;

	case IDS_BROWSE_BUTTON:
		{
			ShellExecute(hWnd, _T("open"), szDirectory, NULL, szDirectory, SW_SHOWNORMAL);
		}
		break;

	case IDS_OFFERS_BUTTON:
		break;

	case IDS_EXIT_BUTTON:
		{
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;

	case IDS_INSTALL_BUTTON:
		{
			if(g_bInstallInProgress)
				return;

			g_bInstallInProgress = TRUE;

			if (iButtonID == IDS_INSTALL_BUTTON)
				nNAV = 1;

			ShellExecute(NULL, NULL, _T("navsetup.exe"), NULL, g_szFileLocation, SW_SHOWNORMAL);

			g_bInstallInProgress = FALSE;
		}
		break;
	}
}  

BOOL GetAppDirectory(LPTSTR szDirectory)
{
	TCHAR szLongPath[MAX_PATH * 3] = {0};
	if(GetModuleFileName(NULL, szLongPath, MAX_PATH) == FALSE)
		return FALSE;

	GetShortPathName(szLongPath, szDirectory, MAX_PATH);
	LPTSTR pTrail = NULL;
	for (LPTSTR p = szDirectory; p && *p; p = CharNext(p))
	{
		if ( *p == '\\' )
		{
			pTrail = p;
		}
	}

	if (pTrail)
		*pTrail = '\0';

	return (pTrail ? TRUE : FALSE);
}

void OnQueryNewPalette(HWND hWnd)
{
	HDC hDC = GetDC(hWnd);

	SelectPalette(hDC, BackgroundBmp.GetPalette(), FALSE);
	if (RealizePalette(hDC) > 0)
		InvalidateRect(hWnd, NULL, TRUE);

	ReleaseDC(hWnd, hDC);
}


VOID GetCDStartOptions()
{
	// Get path to Instopts.dat.
	TCHAR szInstOpts[MAX_PATH] = {0};
	GetAppDirectory(szInstOpts);

	if(szInstOpts[(_tcslen(szInstOpts)) - 1] != '\\')
		_tcscat(szInstOpts, _T("\\"));
	
	// Setting the path for Instopts.dat depending on MultiOS or Regular layout.
	if (!g_bMultiOS)
        _tcscat(szInstOpts, _T("NAV\\InstOpts.dat"));
	else
		_tcscat(szInstOpts, _T("WINNT\\NAV\\InstOpts.dat"));

	DWORD dwNoBrowseBtn = 0;
	DWORD dwNoOptionsBtn = 0;
	CNAVOptFile InstOptsFile;

	if(InstOptsFile.Init(szInstOpts))
	{
		InstOptsFile.GetValue(InstallToolBox::CDSTART_NOBROWSEBUTTON, dwNoBrowseBtn, 0);
		g_bShowBrowse = (dwNoBrowseBtn == 1) ? FALSE : TRUE;

		if (!g_bShowBrowse)
		{
			g_bShowOffers = FALSE;
		}
		else
		{
			InstOptsFile.GetValue(InstallToolBox::CDSTART_NOOPTIONSBUTTON, dwNoOptionsBtn, 1);
			g_bShowOffers = (dwNoOptionsBtn == 1) ? FALSE : TRUE;
		}
	}
}


bool IsSystemHighContrastMode()
{
	HIGHCONTRAST hcInfo;
	memset(&hcInfo, 0, sizeof(HIGHCONTRAST));
	hcInfo.cbSize = sizeof(HIGHCONTRAST);

	SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hcInfo, 0);

	if(hcInfo.dwFlags & HCF_HIGHCONTRASTON)
		return true;
	else
		return false;
}

void PaintBkForHighContrastMode(HDC hDC)
{
	RECT rcClient = {0};
	GetClientRect(hWnd, &rcClient);
	FillRect(hDC, &rcClient, GetSysColorBrush(COLOR_WINDOW) );

	ButtonInfoVector::iterator it = AppButtons.begin();

	while(it != AppButtons.end())
		DrawButtonText(hDC, (*it++), NORMAL_COLOR, &hFont, FALSE);
}
