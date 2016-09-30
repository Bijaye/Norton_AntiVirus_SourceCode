// Copyright 1996 Symantec Corporation
//****************************************************************************
//
// $Header:   S:/CDSTART/VCS/cdstart.cpv   1.8   29 Jul 1998 14:58:08   DBuches  $
//
//****************************************************************************
// $Log:   S:/CDSTART/VCS/cdstart.cpv  $
// 
//    Rev 1.8   29 Jul 1998 14:58:08   DBuches
// Added code to support new splash screen.
//
//    Rev 1.7   15 Jul 1998 14:25:40   dshanno
// Fixed flicker by composing text in off-screen DC
// Added accelerator
// Moved font name to string resource IDS_BUTTON_FONT_NAME for localisation
//
//    Rev 1.6   09 Jul 1998 16:54:30   dshanno
// Changes to migrate font-dependent strings to resource file for localisation
//
//    Rev 1.5   08 Jul 1998 23:19:30   dshanno
// Now writes text over blank bmps, commented out playing of splash
// screen-video
//
//    Rev 1.4   15 Aug 1997 22:18:42   JBRENNA
// Corrected launched help file name from CDSTRHLP.HLP to CDSTART.HLP.
//
//    Rev 1.3   14 Aug 1997 23:38:34   GWESTER
// Added icon, System menu, and minimize button to the caption bar.
//
//    Rev 1.2   29 Jul 1997 11:18:02   GWESTER
// Added link to Win
// help file
//
//    Rev 1.1   20 Jul 1997 15:39:42   GWESTER
// Changes for Roswell & Tesla.
//
//    Rev 1.0   18 Apr 1997 13:45:10   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:08:00   RFULLER
// Initial revision
//
//    Rev 1.3   14 Jan 1997 18:49:42   TSmith
// Fixed #78572 - Added code to launch winfile.exe when running on NT 3.51
// instead of trying to launch explorer. Also added sounds for button
// highlights and presses.
//
//    Rev 1.2   09 Jan 1997 15:01:42   TSmith
// Disabled the annoying popup that covers the CDSTART screen when
// NAV is not currently installed on the machine.
//
//    Rev 1.1   08 Jan 1997 21:26:10   TSmith
// Added handling for fifth button.
//
//    Rev 1.0   31 Dec 1996 15:46:24   TSmith
// Initial revision.
//****************************************************************************
#include "platform.h"
#include <windowsx.h>
#include <shellapi.h>
#include <mmsystem.h>
#include <digitalv.h>
#include <vfw.h>
#include "autorun.h"
#include "cdstart.h"
#include "cdstrhlp.h"
#include "splash.h"


///////////////////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////////////////

NBUTTONINFO g_nButtons[NUM_BUTTONS];    // Button info structure array
HINSTANCE   g_hInst = NULL;             // Current instance
HPALETTE    g_hPalette = NULL;          // Handle to the background palette
HBITMAP     g_hMainBmp = NULL;          // Handle to the background bitmap
BOOL        g_bShowSplash = TRUE;       // Show the startup video
char        g_szCDDir[SYM_MAX_PATH];    // Holds path to CD-Rom
char        g_szNAVDir[SYM_MAX_PATH];   // Holds path to the NAV directory
char        g_szExecCmd[SYM_MAX_PATH];  // Command to execute
char        g_szExecDir[SYM_MAX_PATH];  // Startup dir for execution
char        g_szErrorCap[SYM_MAX_PATH]; // Error caption buffer
					// Error text buffer
char        g_szErrorText[SYM_MAX_PATH * 2];
char        g_szBuffer[SYM_MAX_PATH];   // Multi-purpose buffer
HWND        g_hWnd = NULL;              // global window handle
HWND        g_hMovie = NULL;            // window handle for avi file
HWND        g_hMCIWnd;
HFONT       g_hFont = NULL;

///////////////////////////////////////////////////////////////////////////
//
// WinMain
//
// Params:
//          HINSTANCE - Instance of the program
//          HINSTANCE - Previous instance
//          LPSTR - Command line
//          int - Show parameter
//
// Returns:
//
///////////////////////////////////////////////////////////////////////////
// 09/02/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
    MSG         msg;
    HWND        hPrev;

    if (hPrevInstance)
	return(FALSE);

    hPrev = FindWindow("Nort_CDStartClass", NULL);

    if (hPrev != NULL)
	{
	if (IsIconic(hPrev))
	    PostMessage(hPrev, WM_SYSCOMMAND, SC_RESTORE, 0L);

	SetForegroundWindow(hPrev);
	return(FALSE);
	}

    if (!QueryAutoRun())
	return(FALSE);

    memset((LPVOID)&g_nButtons, 0, sizeof(NBUTTONINFO) * NUM_BUTTONS);

    if (!InitApplication(hInstance))
	return (FALSE);

    if (!InitInstance(hInstance, nCmdShow))
	return (FALSE);

	HACCEL hAccelerators = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDR_ACCEL ) );

    while (GetMessage(&msg, NULL, NULL, NULL))
	{
	if( TranslateAccelerator( g_hWnd, hAccelerators, &msg ) == FALSE )
		TranslateMessage(&msg);
	DispatchMessage(&msg);
	}

    return(msg.wParam);
}


///////////////////////////////////////////////////////////////////////////
//
// InitApplication - initializes the app specific data
//
// Params:
//          HINSTANCE - instance of the program
//
// Returns: TRUE if app successfully initialized
//
///////////////////////////////////////////////////////////////////////////
// 09/02/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////
BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASS  wc;

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CDSTART));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "Nort_CDStartClass";

    return (RegisterClass(&wc));
}


///////////////////////////////////////////////////////////////////////////
//
// InitInstance - Initializes the instance specific data
//
// Params:
//          HINSTANCE - instance of the program
//          int       - nCmdShow parameter from WinMain
//
// Returns: TRUE if app successfully initialized
//
///////////////////////////////////////////////////////////////////////////
// 09/02/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND            hWnd;
    HKEY            hKey = NULL;
    DWORD           dwSize = SYM_MAX_PATH - 1;
    DWORD           dwType = 0;
    BITMAP          bitmap;

    g_hInst = hInstance;                // Set the global instance variable

					// Check to see if the startup
					// video needs to be shown

					// Dont bother checkin, we're not playing the startup vid

//    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_SHOWSTARTUP, 0, KEY_QUERY_VALUE,
//                   &hKey) == ERROR_SUCCESS && hKey != NULL)
//      {
//      dwSize = SYM_MAX_PATH - 1;

//      if (RegQueryValueEx(hKey, VAL_SHOWSTARTUP, NULL, &dwType,
//                          (LPBYTE)g_szBuffer, &dwSize) == ERROR_SUCCESS &&
//          !lstrcmpi(g_szBuffer, "1"))
//          {
//          g_bShowSplash = FALSE;
//          }

//      RegCloseKey(hKey);
//      hKey = NULL;
//      }



					// initialize the state information, etc.
    for (int nCtr = 0; nCtr < NUM_BUTTONS; nCtr++)
	{
	UINT            uState;
	UINT            uIdButtons[NUM_BITMAPS];
	POINT           point;
		UINT                    uIdText;

					// First get all the button-specific
					// info
	switch(nCtr)
	    {
	    case 0:
		uIdButtons[0] = IDB_BUT1_A;
		uIdButtons[1] = IDB_BUT1_B;
		uIdButtons[2] = IDB_BUT1_C;
		point.x = BTN_BMP1_XLOC;
		point.y = BTN_BMP1_YLOC;
		uState = STATE_NORMAL;
		uIdText = IDS_BUTTON1_TEXT;
		break;

	    case 1:
		uIdButtons[0] = IDB_BUT2_A;
		uIdButtons[1] = IDB_BUT2_B;
		uIdButtons[2] = IDB_BUT2_C;
		point.x = BTN_BMP2_XLOC;
		point.y = BTN_BMP2_YLOC;
		uState = STATE_NORMAL;
		uIdText = IDS_BUTTON2_TEXT;
		break;

	    case 2:
		uIdButtons[0] = IDB_BUT3_A;
		uIdButtons[1] = IDB_BUT3_B;
		uIdButtons[2] = IDB_BUT3_C;
		point.x = BTN_BMP3_XLOC;
		point.y = BTN_BMP3_YLOC;
		uState = STATE_NORMAL;
		uIdText = IDS_BUTTON3_TEXT;
		break;

	    case 3:
		uIdButtons[0] = IDB_BUT4_A;
		uIdButtons[1] = IDB_BUT4_B;
		uIdButtons[2] = IDB_BUT4_C;
		point.x = BTN_BMP4_XLOC;
		point.y = BTN_BMP4_YLOC;
		uState = STATE_NORMAL;
		uIdText = IDS_BUTTON4_TEXT;
		break;

					// Re-enable this if you want to
					// add a fifth button:
	    //case 4:
	    //    uIdButtons[0] = IDB_BUT5_A;
	    //    uIdButtons[1] = IDB_BUT5_B;
	    //    uIdButtons[2] = IDB_BUT5_C;
	    //    point.x = BTN_BMP5_XLOC;
	    //    point.y = BTN_BMP5_YLOC;
	    //    uState = STATE_NORMAL;
	    //    break;
	    }                           // end switch(nCtr)

					// Load the bitmaps and get the palette
					// of each one

	for (int nBCtr = 0; nBCtr < NUM_BITMAPS; nBCtr++)
	    {
	    g_nButtons[nCtr].hBitmap[nBCtr] =
			 LoadResourceBitmap(hInstance,
					    MAKEINTRESOURCE(uIdButtons[nBCtr]),
					    &g_nButtons[nCtr].hPalette[nBCtr]);
	    }

					// Set the location info and state for
					// this button

	g_nButtons[nCtr].ptLoc.x = point.x;
	g_nButtons[nCtr].ptLoc.y = point.y;
	g_nButtons[nCtr].uState = uState;

					// Load button text from resources
	LoadString(hInstance, uIdText, g_nButtons[nCtr].szText, MAX_STRING);

					// Get the bitmap dimensions

	GetObject(g_nButtons[nCtr].hBitmap[0], sizeof(BITMAP), (LPSTR)&bitmap);
	g_nButtons[nCtr].size.cx = bitmap.bmWidth;
	g_nButtons[nCtr].size.cy = bitmap.bmHeight;

	}                               // for (nCtr...)


					// Retrieve the window title from
					// the string table

    LoadString(hInstance, IDS_WINDOW_TITLE, g_szCDDir, SYM_MAX_PATH - 1);

					// Create the main window

    hWnd = CreateWindow("Nort_CDStartClass", g_szCDDir,
			WS_OVERLAPPED | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU | DS_3DLOOK,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, NULL, hInstance, NULL);

    if (!hWnd)
	return (FALSE);

    g_hWnd = hWnd;                      // Set global window handle

					// Now we parse out the path to the
					// CD-Rom to be used later when
					// launching things.

    GetModuleFileName(hInstance, g_szCDDir, SYM_MAX_PATH - 1);

    LPSTR       lpPtr = g_szCDDir;
    LPSTR       lpTemp;

    lpTemp = CharNext(lpPtr);

    while(lpTemp != lpPtr)
	{
	lpPtr = lpTemp;
	lpTemp = CharNext(lpPtr);
	}

    lpTemp = CharPrev(g_szCDDir, lpTemp);

    while(*lpTemp != '\\' && lpTemp != g_szCDDir)
	{
	*lpTemp = 0;
	lpTemp = CharPrev(g_szCDDir, lpTemp);
	}

    if (*lpTemp == '\\')
	*lpTemp = 0;


    return (TRUE);
}


///////////////////////////////////////////////////////////////////////////
//
// MainWndProc
//
// Params:
//          HWND - Window Handle
//          UINT - Window Message
//          WPARAM - WPARAM of the message
//          LPARAM - LPARAM of the message
//
// Returns:
//
///////////////////////////////////////////////////////////////////////////
// 09/02/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

LRESULT SYM_EXPORT WINAPI MainWndProc(HWND hWnd, UINT message,
				      WPARAM wParam, LPARAM lParam)
{
    static      BOOL        bProcessing = FALSE;
    static      UINT        uCurButton = BUTTON_ALL;
    static      DWORD       dwLength = 0;
    static      CSplashWindow* pSplash = NULL;

    switch (message)
	{
	case WM_CREATE:
	    {
	    int         nXDiff;
	    int         nYDiff;
	    RECT        WndPos;
	    RECT        Client;
		HINSTANCE	hInstance;

		HDC     hDC = GetDC(hWnd);

		hInstance = g_hInst;


							// Create font for buttons

		LOGFONT logFont;

		ZeroMemory(&logFont, sizeof(LOGFONT));

							// Load font name from resource file (for localisation)

		LoadString(hInstance, IDS_BUTTON_FONT_NAME, g_szBuffer, MAX_STRING);

		logFont.lfHeight            = PT_TO_PIXELS(hDC, 16);
		logFont.lfWeight            = FW_BOLD;
		logFont.lfCharSet           = GetTextCharset(hDC);
		logFont.lfOutPrecision      = OUT_TT_PRECIS;
		logFont.lfClipPrecision     = CLIP_DEFAULT_PRECIS;
		logFont.lfQuality           = DEFAULT_QUALITY;
		logFont.lfPitchAndFamily    = DEFAULT_PITCH | FF_DONTCARE;
		strcpy(logFont.lfFaceName, g_szBuffer);

		g_hFont = CreateFontIndirect(&logFont);

	    GetWindowRect(hWnd, &WndPos);
	    GetClientRect(hWnd, &Client);

	    nXDiff = CLIENT_WIDTH - Client.right;
	    nYDiff = CLIENT_HEIGHT - Client.bottom;

	    WndPos.right += nXDiff;
	    WndPos.bottom += nYDiff;

	    nXDiff = WndPos.right - WndPos.left;
	    nYDiff = WndPos.bottom - WndPos.top;

					// Size and center the window
        RECT rDesktop;
        GetClientRect( GetDesktopWindow(), &rDesktop );

	    SetWindowPos(hWnd, NULL,
             (rDesktop.right - nXDiff) / 2,
             (rDesktop.bottom - nYDiff) / 2,
			 nXDiff - 1, nYDiff - 1, SWP_NOACTIVATE);

					// Load the background bitmap and
					// it's palette

	    g_hMainBmp = LoadResourceBitmap(g_hInst, MAKEINTRESOURCE(IDB_MAIN),
					    &g_hPalette);

		DrawButtons(hWnd, BUTTON_ALL, NULL);

	    PostMessage(hWnd, WM_COMMAND, IDM_START, 0L);
	    return(TRUE);
	    }

    case WM_TIMER:
        if( wParam = 1 )
            {
            KillTimer( hWnd, 1 );
            if( pSplash != NULL )
                {
                delete pSplash;
                pSplash = NULL;
                }
            PostMessage(hWnd, WM_COMMAND, IDM_SHOWMAINWND, 0L);
            }
        break;


	case WM_COMMAND:
	    {
	    UINT    uCmd = GET_WM_COMMAND_ID(wParam, lParam);

	    switch(uCmd)
		{
        case IDM_START:

            //
            // Create splash screen
            //
            pSplash = new CSplashWindow( g_hInst, hWnd, IDB_SPLASH256, IDB_SPLASH16 );

            //
            // Set splash screen timer
            //
            SetTimer( hWnd, 1, 2000, NULL );

            return FALSE;

            // fall thru.

		case IDM_SHOWMAINWND:
		    ShowWindow(hWnd, SW_SHOWNORMAL);
		    UpdateWindow(hWnd);
		    return(FALSE);


		case IDM_VIDEO_END:
		    {
		    PostMessage(hWnd, WM_COMMAND, IDM_SHOWMAINWND, 0L);


					// Re-enable this code if you need to check
					// anything at startup:
#if 0
					// (DISABLED) Check to be sure we're under WinNT
					// and not Windows95 or something else
		    if (!IsWinNT())
			{
			LoadString(g_hInst, IDS_INSTALL_CAPTION, g_szErrorCap,
				   SYM_MAX_PATH - 1);

			LoadString(g_hInst, IDS_ERROR_NOTWINNT, g_szErrorText,
				   SYM_MAX_PATH - 1);

			MessageBox(hWnd, g_szErrorText, g_szErrorCap,
				   MB_OK | MB_ICONSTOP);

			PostMessage(hWnd, WM_CLOSE, 0, 0L);
			}
#endif

		    return(FALSE);
		    }

		//////////////////////////////////////////////////////////////
		// Handle 'Button 1'.
		case IDM_BUTTON_1:
					{
		    WinHelp( hWnd, _T("CDSTART.HLP>maintwo"), HELP_CONTEXT, IDH_CDSTART_INSTALL);
					return(FALSE);
					}
					// Disable this code. In the Maconi AutoRun
					// it was used to launch the setup program:
#if 0
		    {
		    HINSTANCE hInstance;

		    lstrcpy(g_szExecDir, g_szCDDir);
		    lstrcat(g_szExecDir, SETUP_PATH);
		    lstrcpy(g_szExecCmd, g_szExecDir);
		    lstrcat(g_szExecCmd, SETUP_COMMAND);

		    hInstance = ShellExecute(hWnd, "open", g_szExecCmd, "",
					     g_szExecDir, SW_SHOWNORMAL);

		    if (hInstance < (HINSTANCE)32)
			LaunchError(g_szExecCmd);

		    return(FALSE);
		    }
#endif

		//////////////////////////////////////////////////////////////
		// Handle 'Button 2'.
		case IDM_BUTTON_2:
					{
		    WinHelp( hWnd, _T("CDSTART.HLP>maintwo"), HELP_CONTEXT, IDH_CDSTART_VIDEOS);
					return(FALSE);
					}
					// Disable this code. In the Maconi AutoRun
					// it was used to play the SARC AVI on screen:
#if 0
		    {
		    ZeroMemory(&g_szExecCmd, sizeof(g_szExecCmd));
		    lstrcpy(g_szExecCmd, g_szCDDir);
		    lstrcat(g_szExecCmd, SARC_VIDEO);

		    g_hMCIWnd = MCIWndCreate( NULL,
					      g_hInst,
					      NULL,
					      g_szExecCmd );

		    if ( !g_hMCIWnd )
			{
			#if defined( SYM_DEBUG )
			MessageBox( hWnd, "Error playing SARC AVI",
				    "Debug Message", MB_OK | MB_ICONSTOP );
			#endif
			}
		    else
			{
			MCIWndPlay( g_hMCIWnd );
			}

		    return(FALSE);
		    }
#endif

		//////////////////////////////////////////////////////////////
		// Handle 'Button 3'.
		case IDM_BUTTON_3:
					{
		    WinHelp( hWnd, _T("CDSTART.HLP>maintwo"), HELP_CONTEXT, IDH_CDSTART_MORE);
					return(FALSE);
					}
					// Disable this code. In the Maconi AutoRun
					// it was used to browse the CD:
#if 0
		    {
		    HINSTANCE hInst;

		    //
		    // If the system tray is present then we
		    // must be on either NT4 or Win95.
		    //
		    if ( FindWindow( "Shell_TrayWnd", NULL ) )
			{
			//
			// We can use the explorer
			//
			hInst =
			ShellExecute( NULL, "open", "explorer.exe", g_szCDDir, NULL, SW_SHOWNORMAL );

			if ( hInst < ( HINSTANCE )32 )
			    {
			    LaunchError( g_szCDDir );
			    }
			}
		    else
			{
			//
			// We must be on NT3.x so we need to use File Manager
			//
			WinExec( "winfile.exe", SW_SHOW );
			}

		    return(FALSE);
		    }
#endif

		//////////////////////////////////////////////////////////////
		// Handle 'Button 4':
		case IDM_BUTTON_4:
					{
					PostMessage(hWnd, WM_DESTROY, 0, 0L);
					return(FALSE);
					}
					// Disable this code. In the Maconi AutoRun
					// it was used to browse the trial directory:
#if 0
		    {
		    HINSTANCE hInst;
		    //
		    // Compose path to the 'TRIAL' directory on CD
		    //
		    ZeroMemory(&g_szExecCmd, sizeof(g_szExecCmd));
		    lstrcpy(g_szExecCmd, g_szCDDir);
		    lstrcat(g_szExecCmd, TRIAL_PRODUCTS);

		    //
		    // If the system tray is present then we
		    // must be on either NT4 or Win95.
		    //
		    if ( FindWindow( "Shell_TrayWnd", NULL ) )
			{
			//
			// We can use the explorer
			//
			hInst =
			ShellExecute( hWnd, "open", "explorer.exe", g_szExecCmd, NULL, SW_SHOWNORMAL );

			if ( hInst < ( HINSTANCE )32 )
			    {
			    LaunchError( g_szCDDir );
			    }
			}
		    else
			{
			//
			// We must be on NT3.x so we need to use File Manager
			//
			WinExec( "winfile.exe", SW_SHOW );
			}

		    return( FALSE );
		    }
#endif                                  // End disabled code

		//////////////////////////////////////////////////////////////
		// Handle 'Button 5'. Enable this code if you need a fifth button:
		// case IDM_BUTTON_5:
		//      {
		//      PostMessage(hWnd, WM_CLOSE, 0, 0L);
		//      return(FALSE);
		//      }

		default:
		    break;
		}
	    }

	case MM_MCINOTIFY:
	    PostMessage(hWnd, WM_COMMAND, IDM_VIDEO_END, 0L);
	    return(FALSE);

	case WM_DESTROY:
	    {
        if( pSplash != NULL )
            {
            delete pSplash;
            pSplash = NULL;
            }


        if (g_hMainBmp != NULL)
		{                       // Delete the background bitmap
		DeleteObject((HGDIOBJ)g_hMainBmp);
		g_hMainBmp = NULL;
		}

					// Delete each button bitmap and
					// palette

	    for (int nCtr = 0; nCtr < NUM_BUTTONS; nCtr++)
		{
		for (int nCtr2 = 0; nCtr2 < NUM_BITMAPS; nCtr2++)
		    {
		    if (g_nButtons[nCtr].hBitmap[nCtr2] != NULL)
			{
			DeleteObject((HGDIOBJ)g_nButtons[nCtr].hBitmap[nCtr2]);
			g_nButtons[nCtr].hBitmap[nCtr2] = NULL;
			}

		    if (g_nButtons[nCtr].hPalette[nCtr2] != NULL)
			{
			DeleteObject((HGDIOBJ)g_nButtons[nCtr].hPalette[nCtr2]);
			g_nButtons[nCtr].hPalette[nCtr2] = NULL;
			}
		    }
		}

					// Delete the main background palette
	    if (g_hPalette != NULL)
		{
		DeleteObject((HGDIOBJ)g_hPalette);
		g_hPalette = NULL;
		}

	if( g_hFont )
	    DeleteObject( g_hFont );

	    PostQuitMessage(0);
	    break;
	    }

					// Someone has realized their palette
	case WM_PALETTECHANGED:
	    {
	    HDC         hdc;
	    HPALETTE    hOldPal;

	    if ((HWND)wParam != hWnd && g_hPalette != NULL)
		{
		hdc = GetDC(hWnd);
		hOldPal = SelectPalette(hdc, g_hPalette, FALSE);
		NRealizeThePalette(hdc);
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);

		if (hOldPal)
		    SelectPalette(hdc, hOldPal, FALSE);

		ReleaseDC(hWnd, hdc);
		}
	    break;
	    }

					// Query the changed palette to adjust
					// our colors to the new palette
	case WM_QUERYNEWPALETTE:
	    {
	    HDC         hdc;
	    HPALETTE    hOldPal;

	    if (g_hPalette != NULL)
		{
		hdc = GetDC(hWnd);

		hOldPal = SelectPalette(hdc, g_hPalette, FALSE);
		NRealizeThePalette(hdc);

		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);

		if (hOldPal)
		    SelectPalette(hdc, hOldPal, FALSE);

		ReleaseDC(hWnd, hdc);
		return(TRUE);
		}
	    return(FALSE);
	    }



	case WM_MOUSEMOVE:
	    {
	    UINT        uResult;
	    UINT        uLastState;
	    POINT       point;

	    point.x = LOWORD(lParam);
	    point.y = HIWORD(lParam);
					// Find out if the mouse is over a
					// button
	    uResult = IsMouseOnButton(hWnd, &point);

					// If the mouse is over a button, then
					// draw that button correctly and
					// restore any other button

	    if (uResult != BUTTON_ALL)
		{
					// Save the previous state of the button
		uLastState = g_nButtons[ uResult ].uState;

					// Set the new state
				g_nButtons[uResult].uState = (uCurButton == BUTTON_ALL ?
							    STATE_ACTIVE :
							    STATE_DEPRESSED);

					// If the state is changing from normal
					// to active, play the sound.
		if ( ( uLastState == STATE_NORMAL ) &&
		     ( g_nButtons[ uResult ].uState == STATE_ACTIVE ) )
		    {
		    PlaySound( "IDW_SELECTBTN", g_hInst, SND_RESOURCE | SND_ASYNC );
		    }

		for (int nCtr = 0; nCtr < NUM_BUTTONS; nCtr++)
		    {
		    if (g_nButtons[nCtr].uState != STATE_NORMAL &&
			(UINT)nCtr != uResult)
			{
			g_nButtons[nCtr].uState = STATE_NORMAL;
			DrawButtons(hWnd, (UINT)nCtr, NULL);
			}
		    }

		if (uCurButton != BUTTON_ALL && uCurButton != uResult)
		    {
		    uCurButton = BUTTON_ALL;
		    }

		DrawButtons(hWnd, uResult, NULL);
		}
	    else                        // Make sure all the buttons are
		{                       // normal
		for (int nCtr = 0; nCtr < NUM_BUTTONS; nCtr++)
		    {
		    if (g_nButtons[nCtr].uState != STATE_NORMAL)
			{
			g_nButtons[nCtr].uState = STATE_NORMAL;
			DrawButtons(hWnd, (UINT)nCtr, NULL);
			break;
			}
		    }
		uCurButton = BUTTON_ALL;
		}
	    break;
	    }


	case WM_LBUTTONDOWN:
	    {
	    UINT        uResult;
	    POINT       point;

	    point.x = LOWORD(lParam);
	    point.y = HIWORD(lParam);

					// Is the mouse over a button?

	    uResult = IsMouseOnButton(hWnd, &point);

					// If a button is being pressed,
					// draw it correctly
	    if (uResult != BUTTON_ALL)
		{
					g_nButtons[uResult].uState = STATE_DEPRESSED;
		    PlaySound( "IDW_PRESSBTN", g_hInst, SND_RESOURCE | SND_ASYNC );
					DrawButtons(hWnd, uResult, NULL);
		    uCurButton = uResult;
		}

	    break;
	    }

	case WM_LBUTTONUP:
	    {
	    UINT        uResult;
	    POINT       point;

	    point.x = LOWORD(lParam);
	    point.y = HIWORD(lParam);

					// Is the mouse over a button?
	    uResult = IsMouseOnButton(hWnd, &point);

					// If so, then launch the appropriate
					// command
	    if (uCurButton != BUTTON_ALL && !bProcessing)
		{
		if (uCurButton == uResult)
		    {
		    g_nButtons[uCurButton].uState = STATE_ACTIVE;
		    DrawButtons(hWnd, uCurButton, NULL);

		    PostMessage(hWnd, WM_COMMAND, IDM_BUTTON_1 + uResult, 0L);
		    }
		else                    // Otherwise, just make sure the
		    {                   // buttons are drawn right
		    g_nButtons[uCurButton].uState = STATE_NORMAL;
		    DrawButtons(hWnd, uCurButton, NULL);

		    if (uResult != BUTTON_ALL)
			{
							g_nButtons[uResult].uState = STATE_ACTIVE;
			    DrawButtons(hWnd, uResult, NULL);
			}
		    }
		uCurButton = BUTTON_ALL;
		}
	    bProcessing = FALSE;
	    break;
	    }


	case WM_LBUTTONDBLCLK:
	    {
	    POINT       point;

	    point.x = LOWORD(lParam);
	    point.y = HIWORD(lParam);

	    if (IsMouseOnButton(hWnd, &point) != BUTTON_ALL)
		bProcessing = TRUE;

	    return(FALSE);
	    }


	case WM_NCMOUSEMOVE:
	    {
	    PostMessage(hWnd, WM_MOUSEMOVE, 0, 0L);
	    break;
	    }

	case WM_PAINT:
	    {
	    HDC             hdc;
	    RECT            rect;
	    PAINTSTRUCT     ps;

	    BeginPaint(hWnd, &ps);
	    hdc = ps.hdc;

	    GetClientRect(hWnd, &rect);

	    NCD_DrawBitmap(hdc, &rect, g_hMainBmp, g_hPalette, TRUE);

	    DrawButtons(hWnd, BUTTON_ALL, NULL);

	    EndPaint(hWnd, &ps);
	    return(FALSE);
	    }


	default:
	    break;
	}

    return(DefWindowProc(hWnd, message, wParam, lParam));
}



///////////////////////////////////////////////////////////////////////////
//
// IsMouseOnButton - Function to determine if the current position is over
//                   one of the buttons
//
// Params:
//          HWND - handle to main window
//          LPPOINT - point structure that contains the current position
//
// Returns: UINT - Index of button mouse is over or BUTTON_ALL if none
//
///////////////////////////////////////////////////////////////////////////
// 09/02/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

UINT WINAPI IsMouseOnButton(HWND hWnd, LPPOINT lpPt)
{
    UINT            uRet = BUTTON_ALL;

    for (int nCtr = 0; nCtr < NUM_BUTTONS; nCtr++)
	{
	RECT            rect;
	UINT            uCurBut;

					// Figure out where each button
					// resides on the background
	switch(nCtr)
	    {
	    case 0:
		rect.right = BTN_BMP1_XLOC + g_nButtons[nCtr].size.cx;
		rect.left = rect.right - BUTTON1_SIZE_X;
		rect.top = BTN_BMP1_YLOC;
		rect.bottom = rect.top + BUTTON1_SIZE_Y - 1;
		uCurBut = BUTTON_INDEX_1;
		break;

	    case 1:
		rect.right = BTN_BMP2_XLOC + g_nButtons[nCtr].size.cx;
		rect.left = rect.right - BUTTON2_SIZE_X;
		rect.top = BTN_BMP2_YLOC;
		rect.bottom = rect.top + BUTTON2_SIZE_Y - 1;
		uCurBut = BUTTON_INDEX_2;
		break;

	    case 2:
		rect.right = BTN_BMP3_XLOC + g_nButtons[nCtr].size.cx;
		rect.left = rect.right - BUTTON3_SIZE_X;
		rect.top = BTN_BMP3_YLOC;
		rect.bottom = rect.top + BUTTON3_SIZE_Y - 1;
		uCurBut = BUTTON_INDEX_3;
		break;

	    case 3:
		rect.right = BTN_BMP4_XLOC + g_nButtons[nCtr].size.cx;
		rect.left = rect.right - BUTTON4_SIZE_X;
		rect.top = BTN_BMP4_YLOC;
		rect.bottom = rect.top + BUTTON4_SIZE_Y - 1;
		uCurBut = BUTTON_INDEX_4;
		break;

					// Re-enable this code if you want to
					// add a fifth button:
	    //case 4:
	    //    rect.right = BTN_BMP5_XLOC + g_nButtons[nCtr].size.cx;
	    //    rect.left = rect.right - BUTTON5_SIZE_X;
	    //    rect.top = BTN_BMP5_YLOC;
	    //    rect.bottom = rect.top + BUTTON5_SIZE_Y - 1;
	    //    uCurBut = BUTTON_INDEX_5;
	    //    break;
	    }
					// See if the current mouse position
					// is within this buttons bounding
					// rect

	if (lpPt->x >= rect.left && lpPt->x <= rect.right &&
	    lpPt->y >= rect.top && lpPt->y <= rect.bottom)
	    {
	    uRet = uCurBut;
	    break;
	    }
	}

    return(uRet);
}




///////////////////////////////////////////////////////////////////////////
//
// DrawButtons - Function to draw individual buttons or all buttons
//
// Params:
//          HWND - handle to main window
//          UINT - button to draw
//          HDC - device context to use, if NULL will get a dc to main window
//
// Returns:
//
///////////////////////////////////////////////////////////////////////////
// 09/02/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

BOOL WINAPI DrawButtons(HWND hWnd, UINT uButton, HDC hdc)
{
    int             nCtr;
    BOOL            bRet = FALSE;
    HFONT           hOrigFont;
	BOOL			bWeGotDC = FALSE;

    if (hdc == NULL)
	{
		hdc = GetDC(hWnd);
		bWeGotDC = TRUE;
	}

    for (nCtr = 0; nCtr < NUM_BUTTONS; nCtr++)
	{
	if (uButton == BUTTON_ALL || uButton == (UINT)nCtr)
	    {
	    RECT        rect;
		RECT        rectMem;
	    HBITMAP     hBitmap;
		HBITMAP     hMemBitmap;
		HBITMAP		hOldBitmap;
	    HPALETTE    hPalette;
		HDC         hdcHold;                // Compatible Memory DC to compose text on button

	    hBitmap = g_nButtons[nCtr].hBitmap[g_nButtons[nCtr].uState];
	    hPalette = g_nButtons[nCtr].hPalette[g_nButtons[nCtr].uState];

		rectMem.left = 0;
		rectMem.top = 0;
		rectMem.right = g_nButtons[nCtr].size.cx;
		rectMem.bottom = g_nButtons[nCtr].size.cy;

		hdcHold = CreateCompatibleDC( hdc );
	    hMemBitmap = CreateCompatibleBitmap(hdc, g_nButtons[nCtr].size.cx, g_nButtons[nCtr].size.cy);
		hOldBitmap = (HBITMAP) SelectObject(hdcHold, hMemBitmap);

											//Write bitmap to memory DC

		NCD_DrawBitmap(hdcHold, &rectMem, hBitmap, hPalette, TRUE);

											//Write text on button in memory Dc

		RECT rectText = {rectMem.left + BUTTON_TEXT_OFFSET_LEFT + 1,
			rectMem.top+BUTTON_TEXT_OFFSET_TOP+1, rectMem.right+1, rectMem.bottom+1};

											//If button is pressed, shift text too.

		if (g_nButtons[nCtr].uState == STATE_DEPRESSED)
		{
		     OffsetRect( &rectText, 2, 2 );
		}


	    hOrigFont = (HFONT) SelectObject(hdcHold, g_hFont);
											//Draw gray text first for shadow
		SetTextColor(hdcHold, 0xAAAAAA);
		SetBkMode(hdcHold, TRANSPARENT);

	    DrawText(hdcHold, g_nButtons[nCtr].szText, -1, &rectText, DT_LEFT | DT_VCENTER | 0);

		OffsetRect( &rectText, -2, -2 );

											//Draw black text over gray
   		SetTextColor(hdcHold, 0x000000);
	    DrawText(hdcHold, g_nButtons[nCtr].szText, -1, &rectText, DT_LEFT | DT_VCENTER | 0 );


											//Get position & size of button to be replaced
											//on screen DC
		rect.left = g_nButtons[nCtr].ptLoc.x;
	    rect.top = g_nButtons[nCtr].ptLoc.y;
	    rect.right = rect.left + g_nButtons[nCtr].size.cx;
	    rect.bottom = rect.top + g_nButtons[nCtr].size.cy;


		BitBlt(hdc,
		   rect.left,
		   rect.top,
		   rect.right - rect.left,
		   rect.bottom - rect.top,
		   hdcHold,
		   0,
		   0,
		   SRCCOPY);


		SelectObject( hdcHold, hOrigFont );
		SelectObject( hdcHold, hOldBitmap );
		DeleteObject( hMemBitmap );

		bRet = TRUE;
	    }
	}

    if (bWeGotDC)
    	ReleaseDC(hWnd, hdc);


    return(bRet);
}



///////////////////////////////////////////////////////////////////////////
//
// NCD_DrawBitmap
//
// Params:
//          HDC - Device context
//          LPRECT - Rect to draw the bitmap in
//          HBITMAP - handle to the bitmap to draw
//          HPALETTE - handle to the palette to use
//          WORD - flags, 0 to stretch bitmap to rect, !0 to put bitmap as is
//
// Returns:
//
///////////////////////////////////////////////////////////////////////////
// 09/02/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////


BOOL WINAPI NCD_DrawBitmap(HDC hDC, LPRECT lpRect, HBITMAP hBitmap,
			   HPALETTE hPalette, WORD wFlags)
{
    HDC         hDCTmp;
    BOOL        bRet = FALSE;
    BITMAP      bitmap;
    HPALETTE    hOldPal = NULL;

					// Create a compatible device context
					// and select the bitmap to display
    hDCTmp = CreateCompatibleDC(hDC);
    SelectObject(hDCTmp, hBitmap);
    SetMapMode(hDCTmp, GetMapMode(hDC));

    if (hPalette != NULL)
	{
	hOldPal = SelectPalette(hDC, hPalette, FALSE);
	NRealizeThePalette(hDC);
	SelectPalette(hDCTmp, hPalette, FALSE);
	NRealizeThePalette(hDCTmp);
	}
					// Retrieve the bitmap

    if (GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmap))
	{
	if (!wFlags)
	    {
	    StretchBlt(hDC,
		       lpRect->left,
		       lpRect->top,
		       lpRect->right - lpRect->left,
		       lpRect->bottom - lpRect->top,
		       hDCTmp,
		       0,
		       0,
		       bitmap.bmWidth,
		       bitmap.bmHeight,
		       SRCCOPY);
	    }
	else
	    {
	    BitBlt(hDC,
		   lpRect->left,
		   lpRect->top,
		   lpRect->right - lpRect->left,
		   lpRect->bottom - lpRect->top,
		   hDCTmp,
		   0,
		   0,
		   wFlags == 1 ? SRCCOPY : SRCPAINT);
	    }
	bRet = TRUE;

	if (hOldPal)
	    SelectPalette(hDC, hOldPal, FALSE);
	}

    DeleteDC(hDCTmp);                   // Free allocated resources

    return(bRet);
}


///////////////////////////////////////////////////////////////////////////
//
// LoadResourceBitmap - This function loads a bitmap as a DIB so as to
//                      retrieve the palette information.
//
// Params:
//          HINSTANCE - The program's instance handle
//          LPSTR - The resource id of the bitmap
//          HPALETTE FAR * - handle that will receive the palette handle
//
// Returns: HBITMAP - handle to the bitmap (equiv to LoadBitmap)
//
///////////////////////////////////////////////////////////////////////////
// 09/02/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

HBITMAP WINAPI LoadResourceBitmap(HINSTANCE hInstance, LPSTR lpString,
				  HPALETTE FAR *lphPalette)
{
    int                 iNumColors;
    HDC                 hdc;
    HRSRC               hRsrc;
    HGLOBAL             hGlobal;
    HBITMAP             hBitmapFinal = NULL;
    LPBITMAPINFOHEADER  lpbi;

    if (hRsrc = FindResource(hInstance, lpString, RT_BITMAP))
       {
       hGlobal = LoadResource(hInstance, hRsrc);
       lpbi = (LPBITMAPINFOHEADER)LockResource(hGlobal);

       hdc = GetDC(NULL);
       *lphPalette =  CreateDIBPalette((LPBITMAPINFO)lpbi, &iNumColors);

       if (*lphPalette)
	  {
	  SelectPalette(hdc, *lphPalette, FALSE);
	  NRealizeThePalette(hdc);
	  }

       hBitmapFinal = CreateDIBitmap(hdc,
				     (LPBITMAPINFOHEADER)lpbi,
				     (LONG)CBM_INIT,
				     (LPSTR)lpbi + lpbi->biSize + iNumColors *
				     sizeof(RGBQUAD),
				     (LPBITMAPINFO)lpbi,
				     DIB_RGB_COLORS );

       ReleaseDC(NULL, hdc);
       UnlockResource(hGlobal);
       FreeResource(hGlobal);
       }

    return (hBitmapFinal);
}


///////////////////////////////////////////////////////////////////////////
//
// CreateDIBPalette - Creates a logical palette from the bitmap loaded
//                    as a DIB file
//
// Params:
//          LPBITMAPINFO - info from the bitmap
//          LPINT - pointer to hold the number of colors
//
// Returns: HPALETtE - handle to the new palette
//
///////////////////////////////////////////////////////////////////////////
// 09/02/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

HPALETTE WINAPI CreateDIBPalette (LPBITMAPINFO lpbmi, LPINT lpiNumColors)
{
   int                  iCtr;
   HANDLE               hLogPal;
   HPALETTE             hPal = NULL;
   LPLOGPALETTE         lpPal;
   LPBITMAPINFOHEADER   lpbi;

   lpbi = (LPBITMAPINFOHEADER)lpbmi;

   if (lpbi->biBitCount <= 8)
       *lpiNumColors = (1 << lpbi->biBitCount);
   else
       *lpiNumColors = 0;  // No palette needed for 24 BPP DIB

   if (*lpiNumColors)
      {
      hLogPal = GlobalAlloc(GHND, sizeof (LOGPALETTE) +
			    sizeof (PALETTEENTRY) * (*lpiNumColors));

      lpPal = (LPLOGPALETTE)GlobalLock (hLogPal);
      lpPal->palVersion    = 0x300;
      lpPal->palNumEntries = *lpiNumColors;

      for (iCtr = 0;  iCtr < *lpiNumColors;  iCtr++)
	 {
	 lpPal->palPalEntry[iCtr].peRed   = lpbmi->bmiColors[iCtr].rgbRed;
	 lpPal->palPalEntry[iCtr].peGreen = lpbmi->bmiColors[iCtr].rgbGreen;
	 lpPal->palPalEntry[iCtr].peBlue  = lpbmi->bmiColors[iCtr].rgbBlue;
	 lpPal->palPalEntry[iCtr].peFlags = 0;
	 }

      hPal = CreatePalette (lpPal);
      GlobalUnlock(hLogPal);
      GlobalFree(hLogPal);
   }

   return hPal;
}
