// Copyright 1996 Symantec Corporation
//****************************************************************************
//
// $Header:   S:/CDSTART/VCS/cdsplsh.cpv   1.1   04 Aug 1997 21:01:58   GWESTER  $
//
//****************************************************************************
// $Log:   S:/CDSTART/VCS/cdsplsh.cpv  $
// 
//    Rev 1.1   04 Aug 1997 21:01:58   GWESTER
// Fixed bug in which video came up uncentered.
// 
//    Rev 1.0   18 Apr 1997 13:45:06   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:08:00   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:45:54   TSmith
// Initial revision.
//****************************************************************************

#include "platform.h"
#include <windowsx.h>
#include <mmsystem.h>
#include <digitalv.h>
#include "cdstart.h"

static const DWORD FULL_SCREEN_VIDEO = 0x00000200;
static const LPCTSTR MCIAVI_KEYNAME = "Software\\Microsoft\\Multimedia\\Video For Windows\\MCIAVI";

///////////////////////////////////////////////////////////////////////////
//
// NCDS_CreateVideoWnd - Creates the child window that will hold the video
//                       presentation.
//
// Params:
//          HWND - handle of the windows that will be the parent
//          HINSTANCE - the instance handle to be associated with the window
//
// Returns: HWND of new windows if successful
//
///////////////////////////////////////////////////////////////////////////
// 09/19/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

HWND WINAPI NCDS_CreateVideoWnd(HWND hWnd, HINSTANCE hInst)
{
    HWND                hNewWnd = NULL;
    WNDCLASS            wc;
    BOOL                bRegistered;

                                        // Determine if our video window class
                                        // has already been registered.
    bRegistered = GetClassInfo( hInst, "Nort_CDStartVideoClass", &wc );
    
    if( bRegistered == FALSE )
        {
        wc.style = 0;
        wc.lpfnWndProc = VideoWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInst;
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = "Nort_CDStartVideoClass";
        bRegistered = RegisterClass( &wc );
        }

    if (bRegistered)
        {
        POINT           pt;

        pt.x = ((GetSystemMetrics(SM_CXSCREEN) - VIDEO_WIDTH ) / 2);
        pt.y = ((GetSystemMetrics(SM_CYSCREEN) - VIDEO_HEIGHT) / 2);

        hNewWnd = CreateWindowEx(WS_EX_TOPMOST, "Nort_CDStartVideoClass",
                                 "Norton Video",
                                 WS_POPUP,
                                 pt.x, pt.y, VIDEO_WIDTH, VIDEO_HEIGHT,
                                 hWnd, NULL, hInst, NULL);
        }

    return(hNewWnd);
}


///////////////////////////////////////////////////////////////////////////
//
// VideoWndProc - Handles messages for the Video window
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
// 09/19/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

LRESULT SYM_EXPORT WINAPI VideoWndProc(HWND hWnd, UINT uMessage,
                                       WPARAM wParam, LPARAM lParam)
{
    return(DefWindowProc(hWnd, uMessage, wParam, lParam));
}


///////////////////////////////////////////////////////////////////////////
// CVideo object Construction & Initialization
///////////////////////////////////////////////////////////////////////////

CVideo::CVideo()
{
	m_wDeviceID = 0;
    m_fResetToFullScreen = FALSE;
}

///////////////////////////////////////////////////////////////////////////
//
// CVideo::Init - Initializes the AVI device.  Should be called once prior
//                to playing video(s).
//
// Params:
//          NONE -
//
// Returns: TRUE if the device was opened
//
///////////////////////////////////////////////////////////////////////////
// 09/19/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

BOOL CVideo::Init(void)
{
                                        // Query Registry to ensure video
                                        // played in a window, not full-screen.
    HKEY MCIAVI = { 0 };

    if( RegOpenKeyEx( HKEY_CURRENT_USER, MCIAVI_KEYNAME, 0, KEY_EXECUTE | KEY_READ, &MCIAVI ) == ERROR_SUCCESS )
        {
        DWORD Type = REG_DWORD;
        DWORD Buff = 0;
        DWORD Size = sizeof( DWORD );

        if( RegQueryValueEx( MCIAVI, "DefaultOptions", NULL, &Type, (LPBYTE)&Buff, &Size ) == ERROR_SUCCESS )
            {
                                        // Use member variable so when we call
                                        // term we can reset if necessary.
                                        
            m_fResetToFullScreen = FULL_SCREEN_VIDEO & Buff;

                                        // Reset the full screen bit.
            if( m_fResetToFullScreen )
                {
                                        // This may be a bit vector (don't know
                                        // for sure), so reset just the bit.
                Buff = ~FULL_SCREEN_VIDEO & Buff;
                RegSetValueEx( MCIAVI, "DefaultOptions", NULL, Type, (CONST BYTE *)&Buff, Size );
                }
            }

        RegCloseKey( MCIAVI );
        }


	MCI_DGV_OPEN_PARMS	mciOpen = { 0 };
	mciOpen.dwCallback = NULL;
	mciOpen.wDeviceID = 0;
	mciOpen.lpstrDeviceType = "avivideo";
	mciOpen.lpstrElementName = NULL;
	mciOpen.lpstrAlias = NULL;
	mciOpen.dwStyle = 0;
	mciOpen.hWndParent = NULL;

                                        // Open the MCI digital video device
   return(mciSendCommand(0, MCI_OPEN, (DWORD)(MCI_OPEN_TYPE),
                         (DWORD)(LPMCI_DGV_OPEN_PARMS)&mciOpen) == 0);
}


///////////////////////////////////////////////////////////////////////////
//
// CVideo::Term - Closes the AVI device.  Should be called after playing
//                video(s).
//
// Params:
//          NONE -
//
// Returns: Nothing
//
///////////////////////////////////////////////////////////////////////////
// 09/19/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////


void CVideo::Term(void)
{
	WORD               wID;
	MCI_GENERIC_PARMS  mciClose;

                                        // Get the device ID for the opened
                                        // device type and then close the
                                        // device type.
	wID = mciGetDeviceID("avivideo");

	mciSendCommand(wID, MCI_CLOSE, 0L,
		           (DWORD)(LPMCI_GENERIC_PARMS)&mciClose);

                                        // If necessary, reset the MCI Registry
                                        // value indicating videos should be
                                        // played full-screen.
    if( !m_fResetToFullScreen )
        return;

    HKEY MCIAVI = { 0 };

    if( RegOpenKeyEx( HKEY_CURRENT_USER, MCIAVI_KEYNAME, 0, KEY_READ, &MCIAVI ) == ERROR_SUCCESS )
        {
        DWORD Type = REG_DWORD;
        DWORD Buff = 0;
        DWORD Size = sizeof( DWORD );

        if( RegQueryValueEx( MCIAVI, "DefaultOptions", NULL, &Type, (LPBYTE)&Buff, &Size ) == ERROR_SUCCESS )
            {

                                        // Set the full screen bit.
                                        // This may be a bit vector (don't know
                                        // for sure), so reset just the bit.
            Buff = FULL_SCREEN_VIDEO | Buff;
            RegSetValueEx( MCIAVI, "DefaultOptions", NULL, Type, (CONST BYTE *)&Buff, Size );
            }

        RegCloseKey( MCIAVI );
        }

}


///////////////////////////////////////////////////////////////////////////
// Opening and Closing
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
// CVideo::Open - Opens an AVI file into a window.  Only one AVI file may
//                be open for this CVideo object.  Call Close when done.
//                Video is displayed paused on the first frame.
//
// Params:
//          LPSTR - The AVI file to open
//          HWND - Handle to the playback window
//
// Returns: Nothing
//
///////////////////////////////////////////////////////////////////////////
// 09/19/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

BOOL CVideo::Open(LPSTR lpFile, HWND hPlayback)
{
    BOOL                    bRet = TRUE;
	DWORD	                dwRet;
	MCI_DGV_OPEN_PARMS		mciOpen = { 0 };

	mciOpen.dwCallback = NULL;
	mciOpen.wDeviceID = 0;
	mciOpen.lpstrDeviceType = NULL;
	mciOpen.lpstrElementName = lpFile;
	mciOpen.lpstrAlias = NULL;
	mciOpen.dwStyle = WS_CHILD;
	mciOpen.hWndParent = hPlayback;

	                                    // Open the AVI file

	dwRet = mciSendCommand(0, MCI_OPEN,
		                   (DWORD)(MCI_OPEN_ELEMENT | MCI_DGV_OPEN_PARENT |
		                           MCI_DGV_OPEN_WS),
		                   (DWORD)(LPMCI_DGV_OPEN_PARMS)&mciOpen);
	if (dwRet == 0)
	    {
		MCI_DGV_WINDOW_PARMS	mciWindow = { 0 };
		                                // Save the device ID for playing
		                                // & closing
		m_wDeviceID = mciOpen.wDeviceID;

		                                // Show the playback window

		mciWindow.dwCallback = NULL;
		mciWindow.hWnd = NULL;
		mciWindow.nCmdShow = SW_SHOW;
		mciWindow.lpstrText = (LPSTR)NULL;

		dwRet = mciSendCommand(m_wDeviceID, MCI_WINDOW,
			                   MCI_DGV_WINDOW_STATE,
			                   (DWORD)(LPMCI_DGV_WINDOW_PARMS)&mciWindow);

        bRet = TRUE;
	    }

    return(bRet);
}


///////////////////////////////////////////////////////////////////////////
//
// CVideo::Close - Closes a previously opened AVI file
//
// Params:
//          NONE
//
// Returns: Nothing
//
///////////////////////////////////////////////////////////////////////////
// 09/19/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

void CVideo::Close(void)
{
	MCI_GENERIC_PARMS   mciGeneric;

	mciSendCommand(m_wDeviceID, MCI_CLOSE, 0L,
		           (DWORD)(LPMCI_GENERIC_PARMS)&mciGeneric);
}


///////////////////////////////////////////////////////////////////////////
//
// CVideo::GetLength - Gets the length of a previously opened AVI file
//
// Params:
//          NONE
//
// Returns: DWORD - Total length of the avi file
//
///////////////////////////////////////////////////////////////////////////
// 09/19/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

DWORD CVideo::GetLength(void)
{
	MCI_STATUS_PARMS        mciParms;

	memset((LPVOID)&mciParms, 0, sizeof(MCI_STATUS_PARMS));

	mciParms.dwItem = MCI_STATUS_LENGTH;

	mciSendCommand(m_wDeviceID, MCI_STATUS, MCI_STATUS_ITEM,
		           (DWORD)(LPMCI_STATUS_PARMS)&mciParms);

    return(mciParms.dwReturn);
}



///////////////////////////////////////////////////////////////////////////
//
// CVideo::GetCurrentPosition - Returns the current location of an AVI file
//
// Params:
//          NONE
//
// Returns: DWORD current position of the avi file
//
///////////////////////////////////////////////////////////////////////////
// 09/19/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

DWORD CVideo::GetCurrentPosition(void)
{
	MCI_STATUS_PARMS        mciParms;

	memset((LPVOID)&mciParms, 0, sizeof(MCI_STATUS_PARMS));

	mciParms.dwItem = MCI_STATUS_POSITION;

	mciSendCommand(m_wDeviceID, MCI_STATUS, MCI_STATUS_ITEM,
		           (DWORD)(LPMCI_STATUS_PARMS)&mciParms);

    return(mciParms.dwReturn);
}

///////////////////////////////////////////////////////////////////////////
// Playing
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
// CVideo::Play - Plays a previously opened AVI file.
//
// Params:
//          HWND - Parent window that will be notified when the file is
//                 finished
//
// Returns: TRUE if successful
//
///////////////////////////////////////////////////////////////////////////
// 09/19/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

BOOL CVideo::Play(HWND hParent)
{
	DWORD	        dwFlags;
	DWORD	        dwRet;

	                                    // Position the video at the start

	MCI_SEEK_PARMS	    mciSeek;

	mciSeek.dwCallback = 0;
	mciSeek.dwTo = 0;
	dwFlags = MCI_SEEK_TO_START;

	dwRet = mciSendCommand(m_wDeviceID, MCI_SEEK, dwFlags,
		                   (DWORD)(LPMCI_SEEK_PARMS)&mciSeek);

	                                    // Play the video

	MCI_DGV_PLAY_PARMS	mciPlay;

	mciPlay.dwCallback = MAKELONG(hParent,0);
	mciPlay.dwFrom = mciPlay.dwTo = 0;
	dwFlags = MCI_NOTIFY;
	dwRet = mciSendCommand(m_wDeviceID, MCI_PLAY, dwFlags,
		                   (DWORD)(LPMCI_DGV_PLAY_PARMS)&mciPlay);

	return(dwRet == 0);
}


///////////////////////////////////////////////////////////////////////////
// Handling palettes
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
// CVideo::RealizeVideoPaletee - Realizes the AVI palette into the window.
//                               if bBackground is TRUE, the palette is
//                               realized as a background palette.
//
// Params:
//          BOOL - Flag to indicate whether the palette should be realized
//                 as a background or foreground palette.
//
// Returns: MCI error code, or 0 if okay
//
///////////////////////////////////////////////////////////////////////////
// 09/19/95 - BruceH, Function created
///////////////////////////////////////////////////////////////////////////

DWORD CVideo::RealizeVideoPalette(BOOL bBackground)
{
	DWORD				dwFlags;
	MCI_GENERIC_PARMS	mciGen;

	if (bBackground)
		dwFlags = MCI_DGV_REALIZE_BKGD;
	else
		dwFlags = MCI_DGV_REALIZE_NORM;

	return(mciSendCommand(m_wDeviceID, MCI_REALIZE, dwFlags,
		                  (DWORD)(LPMCI_GENERIC_PARMS)&mciGen));
}
