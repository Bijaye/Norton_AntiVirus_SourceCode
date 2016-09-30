// Copyright 1996 Symantec Corporation
//****************************************************************************
//
// $Header:   S:/CDSTART/VCS/CDSTART.h_v   1.7   29 Jul 1998 14:58:08   DBuches  $
//
//****************************************************************************
// $Log:   S:/CDSTART/VCS/CDSTART.h_v  $
// 
//    Rev 1.7   29 Jul 1998 14:58:08   DBuches
// Added code to support new splash screen.
//
//    Rev 1.6   15 Jul 1998 14:29:18   dshanno
// Added IDS_BUTTON_FONT_NAME for localisation
//
//    Rev 1.5   09 Jul 1998 16:55:00   dshanno
// Added Font Style Defines for localisation
//
//    Rev 1.4   08 Jul 1998 23:20:06   dshanno
// Changed size/placement of bitmap constants
//
//    Rev 1.1   20 Jul 1997 15:42:46   GWESTER
// Changes for Roswell & Tesla.
//
//    Rev 1.0   18 Apr 1997 13:45:12   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:08:00   RFULLER
// Initial revision
//
//    Rev 1.2   14 Jan 1997 19:12:36   TSmith
// Adjusted button bitmap coordinates to better align them with the main bitmap.
//
//    Rev 1.1   08 Jan 1997 21:28:52   TSmith
// Added handling for fifth button.
//
//    Rev 1.0   31 Dec 1996 15:46:54   TSmith
// Initial revision.
//****************************************************************************
#ifndef _CDSTART_H_
#define _CDSTART_H_

///////////////////////////////////////////////////////////////////////////
//  Misc program defines
///////////////////////////////////////////////////////////////////////////

#define NUM_BUTTONS         4           // Number of buttons on the main screen
#define NUM_BITMAPS         3           // Number of bitmaps for each button

#define MAX_STRING                      50	// Longest string allowed to be displayed
#define BUTTON_TEXT_OFFSET_LEFT			15  // Offset from bitmap's left edge to display text
#define BUTTON_TEXT_OFFSET_TOP			10   // Offset from bitmap's top edge to display text

#define CLIENT_WIDTH        500         // Dimensions the client area must be
#define CLIENT_HEIGHT       350         //   to display the main bitmap

#define VIDEO_WIDTH         320         // Width of startup AVI
#define VIDEO_HEIGHT        240         // Height of startup AVI file

///////////////////////////////////////////////////////////////////////////
//  Button location/information defines
///////////////////////////////////////////////////////////////////////////

#define BUTTON1_SIZE_X          456                     // Size of Bitmaps
#define BUTTON2_SIZE_X          456
#define BUTTON3_SIZE_X          456
#define BUTTON4_SIZE_X          456
//#define BUTTON5_SIZE_X      70

#define BUTTON1_SIZE_Y          46
#define BUTTON2_SIZE_Y          46
#define BUTTON3_SIZE_Y          46
#define BUTTON4_SIZE_Y          46
//#define BUTTON5_SIZE_Y      31

#define STATE_NORMAL        0           // Button states
#define STATE_ACTIVE        1
#define STATE_DEPRESSED     2

#define BUTTON_INDEX_1      0           // These defines are used to index
#define BUTTON_INDEX_2      1           // and draw the small bitmaps and to
#define BUTTON_INDEX_3      2           // identify buttons
#define BUTTON_INDEX_4      3
//#define BUTTON_INDEX_5      4
#define BUTTON_ALL          0xFFFF      // BUTTON_ALL is also used to specify
					//   none of the buttons in some cases


///////////////////////////////////////////////////////////////////////////
//  Small bitmap location/information
///////////////////////////////////////////////////////////////////////////

#define BTN_BMP1_XLOC       26           // These defines determine where
#define BTN_BMP2_XLOC       26           // the small bitmaps will be drawn
#define BTN_BMP3_XLOC       26           // in relation to the main background
#define BTN_BMP4_XLOC       26
//#define BTN_BMP5_XLOC       523

#define BTN_BMP1_YLOC       143
#define BTN_BMP2_YLOC       190
#define BTN_BMP3_YLOC       236
#define BTN_BMP4_YLOC       284
//#define BTN_BMP5_YLOC       7

#define BTN_BMP1_SIZE_X     456          // Dimensions of small bitmaps used to
#define BTN_BMP2_SIZE_X     456          // show button state.
#define BTN_BMP3_SIZE_X     456
#define BTN_BMP4_SIZE_X     456
//#define BTN_BMP5_SIZE_X     70

#define BTN_BMP1_SIZE_Y     46
#define BTN_BMP2_SIZE_Y     46
#define BTN_BMP3_SIZE_Y     46
#define BTN_BMP4_SIZE_Y     46
//#define BTN_BMP5_SIZE_Y     31

///////////////////////////////////////////////////////////////////////////
//  Resource IDs
///////////////////////////////////////////////////////////////////////////

#define IDI_CDSTART         100         // Program icon

#define IDB_MAIN            12000       // Bitmaps

#define IDB_BUT1_A          12001
#define IDB_BUT1_B          12002
#define IDB_BUT1_C          12003

#define IDB_BUT2_A          12011
#define IDB_BUT2_B          12012
#define IDB_BUT2_C          12013

#define IDB_BUT3_A          12021
#define IDB_BUT3_B          12022
#define IDB_BUT3_C          12023

#define IDB_BUT4_A          12031
#define IDB_BUT4_B          12032
#define IDB_BUT4_C          12033

//#define IDB_BUT5_A          12041
//#define IDB_BUT5_B          12042
//#define IDB_BUT5_C          12043

#define IDB_SPLASH16        12100
#define IDB_SPLASH256       12101

#define IDS_WINDOW_TITLE        13000   // String defines
#define IDS_INSTALL_CAPTION     13001
#define IDS_NOT_INSTALLED       13002
#define IDS_ERROR_CAP           13003
#define IDS_ERROR_CANTLAUNCH    13004
#define IDS_ERROR_NOTWINNT      13005

#define IDS_BUTTON1_TEXT                13006
#define IDS_BUTTON2_TEXT                13007
#define IDS_BUTTON3_TEXT                13008
#define IDS_BUTTON4_TEXT                13009

					// Launch messages
#define IDM_BUTTON_1            14001
#define IDM_BUTTON_2            14002
#define IDM_BUTTON_3            14003
#define IDM_BUTTON_4            14004
//#define IDM_BUTTON_5            14005 // Re-enable this one if you need a fifth button

#define IDM_START               14006
#define IDM_VIDEO_END           14007
#define IDM_SHOWMAINWND         14008

#define IDR_ACCEL				15000

#define IDS_BUTTON_FONT_NAME	16000

///////////////////////////////////////////////////////////////////////////
//  Launch command info defines
///////////////////////////////////////////////////////////////////////////

#define SETUP_PATH      "\\NAVNT"
#define SETUP_COMMAND   "\\SETUP.EXE"
#define HELP_PATH       "\\NAVNT"
#define HELP_CDSTART    "\\INFODESK.HLP"
#define AUTOSTRT_VIDEO  "\\CDSTART.AVI"
#define SARC_VIDEO      "\\SARC\\SARC97.AVI"
#define TRIAL_PRODUCTS  "\\TRIAL"
#define NAV_SYSTEM_PATH "\\SYSTEM"

///////////////////////////////////////////////////////////////////////////
//  Registry entry defines
///////////////////////////////////////////////////////////////////////////

#define KEY_SHOWSTARTUP "SOFTWARE\\Symantec\\Norton AntiVirus NT"
#define KEY_NAVDIR "SOFTWARE\\Symantec\\Norton AntiVirus NT\\Install\\4.0"
#define VAL_LOCATION  "InstallDir"
#define VAL_SHOWSTARTUP "DisableSplashScreen"

///////////////////////////////////////////////////////////////////////////
//  Structure definitions
///////////////////////////////////////////////////////////////////////////

typedef struct
    {
    HBITMAP         hBitmap[NUM_BITMAPS];
    HPALETTE        hPalette[NUM_BITMAPS];
    POINT           ptLoc;
    SIZE            size;
    UINT            uState;
	TCHAR                   szText[MAX_STRING];
    }   NBUTTONINFO, *LPNBUTTONINFO;


///////////////////////////////////////////////////////////////////////////
//  Inline functions
///////////////////////////////////////////////////////////////////////////

VOID _inline ClientToScreenRect(HWND hWnd, LPRECT lpRect)
{
    POINT       pnt;

    pnt.x = lpRect->left;
    pnt.y = lpRect->top;
    ClientToScreen(hWnd, &pnt);
    lpRect->left = pnt.x;
    lpRect->top = pnt.y;

    pnt.x = lpRect->right;
    pnt.x = lpRect->bottom;
    ClientToScreen(hWnd, &pnt);
    lpRect->right = pnt.x;
    lpRect->bottom = pnt.y;

    return;
}



VOID _inline ScreenToClientRect(HWND hWnd, LPRECT lpRect)
{
    POINT       pnt;

    pnt.x = lpRect->left;
    pnt.y = lpRect->top;
    ScreenToClient(hWnd, &pnt);
    lpRect->left = pnt.x;
    lpRect->top = pnt.y;

    pnt.x = lpRect->right;
    pnt.x = lpRect->bottom;
    ScreenToClient(hWnd, &pnt);
    lpRect->right = pnt.x;
    lpRect->bottom = pnt.y;

    return;
}


VOID _inline NRealizeThePalette(HDC hdc)
{
    UINT        uNumColors = 0;
    DWORD       dwError = 0;

    uNumColors = RealizePalette(hdc);

    if (uNumColors == GDI_ERROR)
	dwError = GetLastError();

#ifdef SYM_DEBUG
    char        szErrBuf[128];

    wsprintf((LPSTR)szErrBuf, "RealizePalette return %08X, Error %08X",
	     uNumColors, dwError);

    //lstrcat(szErrBuf, "\r\n");
    //OutputDebugString(szErrBuf);
#endif

    return;
}


VOID _inline LaunchError(LPSTR lpCmdLine)
{
    extern char         g_szErrorCap[];
    extern char         g_szErrorText[];
    extern char         g_szBuffer[];
    extern HWND         g_hWnd;
    extern HINSTANCE    g_hInst;

    LoadString(g_hInst, IDS_ERROR_CAP, g_szErrorCap, SYM_MAX_PATH - 1);
    LoadString(g_hInst, IDS_ERROR_CANTLAUNCH, g_szBuffer, SYM_MAX_PATH - 1);

    wsprintf((LPSTR)g_szErrorText, g_szBuffer, lpCmdLine);

    MessageBox(g_hWnd, g_szErrorText, g_szErrorCap, MB_OK | MB_ICONSTOP);

    return;
}


BOOL _inline IsWinNT(VOID)
{
    BOOL                bRet = FALSE;
    OSVERSIONINFO       osVerInfo;

    memset((LPVOID)&osVerInfo, 0, sizeof(OSVERSIONINFO));
    osVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if (GetVersionEx(&osVerInfo))
	{
	if (osVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	    bRet = TRUE;
	}
    return(bRet);
}



///////////////////////////////////////////////////////////////////////////
//  Function prototypes
///////////////////////////////////////////////////////////////////////////

BOOL InitApplication(HINSTANCE hInstance);

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

LRESULT SYM_EXPORT WINAPI MainWndProc(HWND hWnd, UINT message,
				      WPARAM wParam, LPARAM lParam);

UINT WINAPI IsMouseOnButton(HWND hWnd, LPPOINT lpPt);

BOOL WINAPI DrawButtons(HWND hWnd, UINT uButton, HDC hdc);

BOOL WINAPI NCD_DrawBitmap(HDC hDC, LPRECT lpRect, HBITMAP hBitmap,                 // (INPUT) Bitmap to use
			   HPALETTE hPalette, WORD wFlags);

HBITMAP WINAPI LoadResourceBitmap(HINSTANCE hInstance, LPSTR lpString,
				  HPALETTE FAR *lphPalette);

HPALETTE WINAPI CreateDIBPalette(LPBITMAPINFO lpbmi, LPINT lpiNumColors);

HWND WINAPI NCDS_CreateVideoWnd(HWND hWnd, HINSTANCE hInst);

LRESULT SYM_EXPORT WINAPI VideoWndProc(HWND hWnd, UINT uMessage,
				       WPARAM wParam, LPARAM lParam);

#endif
