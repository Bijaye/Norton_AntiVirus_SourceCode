// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------

#include "stdafx.h"

#include "Resource.h"

#include "CDStart.h"
#include "ButtonMenu.h"
#include "SymSaferStrings.h"

// value for when no button is current
const int CButtonMenu::NO_BUTTON = -1;

//-------------------------------------------------------------------------
//
// BUTTONINFO
//
// This structure defines the position, text, and action of the single menu 
// button. It assumes that the button is in a fixed location and size.
//

typedef DWORD (* FN_BUTTON_ACTION)(HWND hwnd);
typedef bool (* FN_BUTTON_ENABLE)(int nButton);

struct BUTTONINFO 
{
    // The button position & size.
    int X;
    int Y;
    int Width;
    int Height;
    // The resource id of the main text of the button
    UINT nButtonTextID; 
    // The resource id of descriptive smaller lower text
    UINT nButtonDescriptionTextID;  
    // The button id, this must be UNIQUE in the menu.
    // This can be used to map keyboard accelerators.
    UINT nButtonID;   
    // The kind of action taken when this button is select.
    enum
    {
        eSUBMENU,
        eEXECUTE,                       // userParam is an LPCTSTR for command line arguments
        eEXECUTE_MSI,                   // userParam is the log filename + other MSI args
        eEXECUTE_MSI_BY_OS,             // As above, but executes a different MSI for Win32/Win64.  userParam is a *ExecuteMsiByOSArgs
        eEXECUTE_CUSTOM_FUNCTION,       // userParam is an FN_CUSTOM_FUNCTION
        eEXIT,
        eFILLER
    } eActionType; 
    // User parameter for the button type.
    void* userParam;
    // The strButtonCommand parameter: 
    //   eSUBMENU: the name of the menu to go to.
    //   eEXECUTE: the name of the process to launch.
    //   eEXIT:    unused
    //   eEXECUTE_MSI_BY_OS:  unused (see ExecuteMsiByOSArgs instead)
    LPCTSTR strButtonCommand;
    FN_BUTTON_ENABLE fnButtonEnable;
    // The resource id of the text to display if the command fails.
    UINT nErrorTextID;
};

// Arguments for eAtionType::eEXECUTE_MSI_BY_OS
struct ExecuteMsiByOSArgs
{
    LPCTSTR win32ButtonCommand;
    LPCTSTR win32Argument;
    
    LPCTSTR win64ButtonCommand;
    LPCTSTR win64Argument;
    
    ExecuteMsiByOSArgs( LPCTSTR newWin32ButtonCommand, LPCTSTR newWin32Argument, LPCTSTR newWin64ButtonCommand, LPCTSTR newWin64Argument );
};

//-------------------------------------------------------------------------
//
// SYMMENUINFO
//
// This structure is made up of two parts: a static part, and a run-time 
// part.
//
// The static portion contains descriptive details of the menu, include the
// IDs of the various menu resources.
//
// The run-time portion is used to track the loaded resources.
//

struct SYMMENUINFO 
{
    // *** Menu description ***
    // The name of the Menu, used to activate a menu.
    LPCTSTR m_cszName;
    // The array of BUTTONINFO structres defining the menu.
    BUTTONINFO * m_buttoninfo;
    // The number of items in the m_buttoninfo array.
    int m_nButtons;
    // The resource id of the menu background.
    UINT m_nBackDropID;
    // The resource id of the menu accelerator table.
    UINT m_nAcceleratorID;
    // The resource id of the normal (up) button graphic.
    UINT m_nButtonNormalID;
    // The resource id of the pressed (down) button graphic.
    UINT m_nButtonDownID;
    // The resource id of the current (highlighted) button graphic.
    UINT m_nButtonHighlightID;
    // The resource id of the disabled button graphic.
    UINT m_nButtonDisabledID;

    // *** Run-time storage ***
    HACCEL m_hAccel;
    COleImage m_BackgroundBmp;
    COleImage m_ButtonNormalBmp;
    COleImage m_ButtonDownBmp;
    COleImage m_ButtonHighlightBmp;
    COleImage m_ButtonDisabledBmp;
};

#define array_count(x) (sizeof(x) / sizeof(x[0]))

// These include files define the menus for the products.
#if defined(CDSTART_FOR_SCS)
    #include "ScsMenus.h"
#elif defined(CDSTART_FOR_SAV)
    #include "SavMenus.h"
#endif
    
UINT CButtonMenu::GetAppNameID()
{
    return APP_NAME_ID;
}

// future - allow different background bmp for each submenu
COleImage * CButtonMenu::GetBackgroundBitmap()
{
    return &m_menuinfo[m_nCurrentMenu].m_BackgroundBmp;
}

COleImage * CButtonMenu::GetButtonNormalBitmap()
{
    return &m_menuinfo[m_nCurrentMenu].m_ButtonNormalBmp;
}

COleImage * CButtonMenu::GetButtonDownBitmap()
{
    return &m_menuinfo[m_nCurrentMenu].m_ButtonDownBmp;
}

COleImage * CButtonMenu::GetButtonHighlightBitmap()
{
    return &m_menuinfo[m_nCurrentMenu].m_ButtonHighlightBmp;
}

COleImage * CButtonMenu::GetButtonDisabledBitmap()
{
    return &m_menuinfo[m_nCurrentMenu].m_ButtonDisabledBmp;
}

HACCEL CButtonMenu::GetCurrentAccelerator()
{ 
    return m_menuinfo[m_nCurrentMenu].m_hAccel; 
}



BOOL CButtonMenu::InitializeMenuInfo()
{
    BOOL bSuccess = TRUE;
    // Init Menus
    for (int i=0; i<m_nMenuCount; i++)
    {
        m_menuinfo[i].m_hAccel = LoadAccelerators( g_hInstance, MAKEINTRESOURCE(m_menuinfo[i].m_nAcceleratorID));
        if (m_menuinfo[i].m_BackgroundBmp.Load( m_menuinfo[i].m_nBackDropID ))
            bSuccess = FALSE;
        if (m_menuinfo[i].m_ButtonNormalBmp.Load( m_menuinfo[i].m_nButtonNormalID ) == FALSE)
            bSuccess = FALSE;
        if (m_menuinfo[i].m_ButtonDownBmp.Load( m_menuinfo[i].m_nButtonDownID ) == FALSE)
            bSuccess = FALSE;
        if (m_menuinfo[i].m_ButtonHighlightBmp.Load( m_menuinfo[i].m_nButtonHighlightID ) == FALSE)
            bSuccess = FALSE;
        if (m_menuinfo[i].m_ButtonDisabledBmp.Load( m_menuinfo[i].m_nButtonDisabledID ) == FALSE)
            bSuccess = FALSE;
    }

    return bSuccess;
}

// CButtonMenu Implemenation
CButtonMenu::CButtonMenu(void) : 
    m_bMouseDown(false),
    m_buttoninfo(NULL),
    m_nButtonCount(0),
    m_nCurrentButton(0),
    m_hFont(NULL),
    m_hDescFont(NULL),
    m_nCurrentMenu(0)
{
    // Create the font
    TCHAR szFont[MAX_PATH];
    LoadString(g_hInstance, IDS_FONT_SIZE, szFont, MAX_PATH);
    int nFontSize = atoi(szFont);
    LoadString(g_hInstance, IDS_FONT, szFont, MAX_PATH);

    // use a fixed DPI value here to prevent scaling,
    //  since the buttons keep the same size
    m_hFont = CreateFont(-MulDiv(nFontSize, 96, 72), 0, 0, 0, FW_NORMAL, FALSE,
        FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH, szFont);

    // Create the description font
    LoadString(g_hInstance, IDS_DESC_FONT_SIZE, szFont, MAX_PATH);
    nFontSize = atoi(szFont);
    LoadString(g_hInstance, IDS_DESC_FONT, szFont, MAX_PATH);

    // use a fixed DPI value here to prevent scaling,
    //  since the buttons keep the same size
    m_hDescFont = CreateFont(-MulDiv(nFontSize, 96, 72), 0, 0, 0, FW_NORMAL, FALSE,
        FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH, szFont);

    // set menu to default/root menu
    m_menuinfo = miMenus;
    m_nMenuCount = array_count(miMenus);

    InitializeMenuInfo();
    SetMenu(MENU_MAIN);
}

CButtonMenu::~CButtonMenu(void)
{
    // Delete the font object
    DeleteObject(m_hFont);
    DeleteObject(m_hDescFont);
}

void CButtonMenu::SetMenu(LPCTSTR cszName)
{
    for (int i=0; i < m_nMenuCount; i++)
    {
        if (strcmp(m_menuinfo[i].m_cszName,cszName)==0)
        {
            m_buttoninfo = m_menuinfo[i].m_buttoninfo;
            m_nButtonCount = m_menuinfo[i].m_nButtons;

            // Highlight the first button
            m_nCurrentButton = 0;

            m_nCurrentMenu = i;
        }
    }
}

void CButtonMenu::SwitchMenu(HWND hwnd, LPCTSTR cszName)
{
    SetMenu(cszName);

    SetDefaultButton(hwnd, 0);

    InvalidateRect(hwnd, NULL, false);
}

bool CButtonMenu::IsButtonEnabled(int nButton)
{
    if (m_buttoninfo[nButton].fnButtonEnable)
    {
        return m_buttoninfo[nButton].fnButtonEnable(nButton);
    }

    if (m_buttoninfo[nButton].eActionType == BUTTONINFO::eFILLER)
    {
        return false;
    }

    return true;
}


int CButtonMenu::GetButtonFromPosition (int X, int Y)
{
    for (int I=0; I<m_nButtonCount; I++)
    {
        if (X >= m_buttoninfo[I].X && (X <= m_buttoninfo[I].X + m_buttoninfo[I].Width) &&
            Y >= m_buttoninfo[I].Y && (Y <= m_buttoninfo[I].Y + m_buttoninfo[I].Height))
        {
            if (IsButtonEnabled(I))
            {
                return I;
            }
            else
            {
                // disabled buttons don't react
                return NO_BUTTON;
            }
        }
    }
    return NO_BUTTON;
};

// gets button # from cmd ID
int CButtonMenu::GetButtonFromID (UINT nButtonID)
{
    for (int I=0; I<m_nButtonCount; I++)
    {
        if (m_buttoninfo[I].nButtonID == nButtonID)
        {
            return I;
        }
    }
    return NO_BUTTON;
}


void CButtonMenu::InvalidateButton(HWND hwnd, int nButton)
{
    RECT Rect;
    Rect.left = m_buttoninfo[nButton].X;
    Rect.top = m_buttoninfo[nButton].Y;
    Rect.right = m_buttoninfo[nButton].X + m_buttoninfo[nButton].Width;
    Rect.bottom = m_buttoninfo[nButton].Y + m_buttoninfo[nButton].Height;

    InvalidateRect(hwnd, &Rect, false);
}

void CButtonMenu::DrawButton (HDC hdc, int nButton)
{
    // if disabled, no other state matters
    if (!IsButtonEnabled(nButton))
    {
        GetButtonDisabledBitmap()->Draw(hdc, m_buttoninfo[nButton].X, m_buttoninfo[nButton].Y);
        DrawButtonText(hdc, nButton, DISABLED_COLOR, FALSE);
        return;
    }

    if (m_nCurrentButton == nButton)
    {
        if (m_bMouseDown)
        {
            GetButtonDownBitmap()->Draw(hdc, m_buttoninfo[nButton].X,
                m_buttoninfo[nButton].Y);
            DrawButtonText(hdc, nButton, MOUSEDOWN_COLOR, TRUE);
        }
        else // mouse over hilite only
        {
            GetButtonHighlightBitmap()->Draw(hdc, m_buttoninfo[nButton].X, m_buttoninfo[nButton].Y);
            DrawButtonText(hdc, nButton, MOUSEOVER_COLOR,FALSE);
        }
    }
    else
    {
        GetButtonNormalBitmap()->Draw(hdc, m_buttoninfo[nButton].X, m_buttoninfo[nButton].Y);
        DrawButtonText(hdc, nButton, NORMAL_COLOR, FALSE);
    }
}

BOOL CButtonMenu::DrawButtonText(HDC hDC, int nButton, COLORREF Color, BOOL bShift)
{
    if (nButton >= m_nButtonCount || nButton == NO_BUTTON)
    {
        return FALSE;
    }

    // Set the text attributes
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, Color | 0x02000000);

    // Select the font
    HFONT hOldFont = (HFONT) SelectObject(hDC, m_hFont);

    // Load the text string from the string table
    TCHAR szButtonText[256];
    LoadString(g_hInstance, m_buttoninfo[nButton].nButtonTextID, szButtonText, sizeof(szButtonText)/sizeof(TCHAR));
    TCHAR szButtonDescText[256];
    LoadString(g_hInstance, m_buttoninfo[nButton].nButtonDescriptionTextID, szButtonDescText, sizeof(szButtonText)/sizeof(TCHAR));

	// We only want to leave space if there is actually a description to display.
    bool bHasDesc = true;
    if (szButtonDescText[0]==0 || _tcscmp(szButtonDescText,_T(" "))==0 )
    {
        bHasDesc = false;
    }


    // Calculate the button text rectangle
    RECT Rect;
    Rect.left = m_buttoninfo[nButton].X + BUTTON_TEXT_H_OFFSET;
    Rect.top = m_buttoninfo[nButton].Y + BUTTON_TEXT_V_OFFSET;
    Rect.right = m_buttoninfo[nButton].X + m_buttoninfo[nButton].Width;
    Rect.bottom = m_buttoninfo[nButton].Y + m_buttoninfo[nButton].Height;

    const int iDescVertOffset = BUTTON_TEXT_V_DESC_OFFSET;
    const int iDescHorzOffset = BUTTON_TEXT_H_DESC_OFFSET;

    // Save room for the description
    if (bHasDesc)
    {
        Rect.bottom -= iDescVertOffset;
    }

    // Do we want to shift the text over by a pixel?
    if (bShift == TRUE)
    {
        Rect.top++;
        Rect.left++;
    }

    // Draw the text!
    DrawText(hDC, szButtonText, -1, &Rect, DT_SINGLELINE | DT_VCENTER);

    if (bHasDesc)
    {
        // Move the description down and to the right.
        Rect.left += iDescHorzOffset;
        Rect.top += iDescVertOffset;
        Rect.bottom += iDescVertOffset;

        SelectObject(hDC, m_hDescFont);
        DrawText(hDC, szButtonDescText, -1, &Rect, DT_SINGLELINE | DT_VCENTER);
    }

    // Select the old font again
    SelectObject(hDC, hOldFont);

    return TRUE;
}

BOOL GetAppDirectory( LPTSTR szDirectory, DWORD nSize)
{
    if( GetModuleFileName( NULL, szDirectory, nSize ) == FALSE )
    {
        szDirectory[0] = _T('\0');
        return FALSE;
    }

    LPTSTR pTrail = NULL;
    for( LPTSTR p = szDirectory; p && *p; p = CharNext(p) )
    {
        if( *p == _T('\\') )
        {
            pTrail = p;
        }
    }
    
    if( pTrail )
    {
        *pTrail = _T('\0');
    }

    return( pTrail ? TRUE : FALSE );
}


DWORD Execute(HWND hwnd, LPCTSTR strCmd, LPCTSTR strParam, int nErrorCode, bool bWait)
{
    DWORD dwReturn = ERROR_SUCCESS;
    TCHAR szFilePath[MAX_PATH+1] = _T("");
    TCHAR szDirectory[MAX_PATH+1] = _T("");
    GetAppDirectory( szDirectory, MAX_PATH+1);

    if (_tcslen(szDirectory) + _tcslen(strCmd) <= MAX_PATH)
    {
        _tcscat(szFilePath, szDirectory);
        _tcscat(szFilePath, strCmd);
    }
    else
    {
        DisplayError(hwnd,IDS_ERROR_BUFFER_SIZE);
        return ERROR_BUFFER_OVERFLOW;
    }
    
    SHELLEXECUTEINFO Info = {0};
	Info.cbSize = sizeof(Info);
	Info.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
	Info.hwnd = hwnd;
    Info.lpVerb = _T("open");
	Info.lpFile = szFilePath;
	Info.lpParameters = strParam;
	Info.nShow = SW_SHOWNORMAL;

    if (ShellExecuteEx(&Info))
    {
        if (bWait)
        {
            ::ShowWindow(hwnd,SW_HIDE);
            while(MsgWaitForMultipleObjects (1, &Info.hProcess,
				FALSE, 1000, QS_ALLINPUT) != WAIT_OBJECT_0)
		    {
			    MSG msg = {0};
			    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			    {
				    TranslateMessage(&msg);
				    DispatchMessage(&msg);
			    };
		    }

            // process has ended - get return value
            ::GetExitCodeProcess(Info.hProcess, &dwReturn);
            ::ShowWindow(hwnd,SW_SHOW);
        }

        ::CloseHandle(Info.hProcess);
    }
    else
    {
        dwReturn = ::GetLastError();
        DisplayError(hwnd, nErrorCode);
    }

    return dwReturn;
}

void CButtonMenu::ProcessButtonCommand(HWND hwnd, int nButton)
{
    int nSavVersion = 0;
    // Play our button down sound
    PlaySound(MAKEINTRESOURCE(IDR_WAVE_BTNDOWN), g_hInstance,
        SND_RESOURCE | SND_SYNC);

    if( hCursorWait )
    {
        SetCursor( hCursorWait );
    }

    if (nButton != NO_BUTTON)
    {
        if (m_buttoninfo[nButton].eActionType == BUTTONINFO::eSUBMENU)
        {
            SwitchMenu(hwnd, m_buttoninfo[nButton].strButtonCommand);
        }
        else if (m_buttoninfo[nButton].eActionType == BUTTONINFO::eEXECUTE)
        {
            Execute(hwnd, m_buttoninfo[nButton].strButtonCommand, reinterpret_cast<LPCTSTR>(m_buttoninfo[nButton].userParam), m_buttoninfo[nButton].nErrorTextID);
        }
        else if (m_buttoninfo[nButton].eActionType == BUTTONINFO::eEXECUTE_MSI)
        {
            ExecuteMSI(hwnd, m_buttoninfo[nButton].strButtonCommand, reinterpret_cast<LPCTSTR>(m_buttoninfo[nButton].userParam), m_buttoninfo[nButton].nErrorTextID);
        }        
        else if (m_buttoninfo[nButton].eActionType == BUTTONINFO::eEXECUTE_MSI_BY_OS)
		{
			ExecuteMsiByOSArgs*		arguments	= reinterpret_cast<ExecuteMsiByOSArgs*>(m_buttoninfo[nButton].userParam);
            bool					bIs64Bit	= false;

            // Determine target operating system
            bIs64Bit = IsMachine64bit();

            if (!bIs64Bit)
				ExecuteMSI(hwnd, arguments->win32ButtonCommand, arguments->win32Argument, m_buttoninfo[nButton].nErrorTextID);
            else
				ExecuteMSI(hwnd, arguments->win64ButtonCommand, arguments->win64Argument, m_buttoninfo[nButton].nErrorTextID);
		}
        else if (m_buttoninfo[nButton].eActionType == BUTTONINFO::eEXECUTE_CUSTOM_FUNCTION)
        {
            FN_BUTTON_ACTION customButtonAction = NULL;
            customButtonAction = reinterpret_cast<FN_BUTTON_ACTION>(m_buttoninfo[nButton].userParam);
            customButtonAction(hwnd);
        }
        else
        {
            // not EXECUTE - must be EXIT
            _ASSERT (m_buttoninfo[nButton].eActionType == BUTTONINFO::eEXIT);
            PostMessage( hwnd, WM_CLOSE, 0, 0 );
        }
    }

    if( hCursorHand )
    {
        SetCursor( hCursorHand );
    }
} 

//////////////////////////////////////////////////////////////
//  IsMachine64bit
//  Checks for the existence of the WOW64 key which would
//  indicate if this is being run on a 64bit machine.
//
//  7/17/2006 - DKOWALYSHYN
//////////////////////////////////////////////////////////////
bool CButtonMenu::IsMachine64bit()
{
    HKEY hRemoteHKLM = NULL;
    LONG lRCRerr=0;
    bool bIS64OS = false;

    lRCRerr = RegConnectRegistry( NULL, HKEY_LOCAL_MACHINE, &hRemoteHKLM );
    if( lRCRerr == ERROR_SUCCESS )
    {
        HKEY hNTCurrentVersion = NULL;
        // Check if the client is 32 or 64 bit if the Wow6432Node key exists
        if(ERROR_SUCCESS == RegOpenKeyEx(hRemoteHKLM, Wow6432Node, 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hNTCurrentVersion) )
        {
            bIS64OS = TRUE;
        }
        else
        {
            bIS64OS = FALSE;
        }
        RegCloseKey(hNTCurrentVersion);
        RegCloseKey(hRemoteHKLM);
    }
    return bIS64OS;
}

void CButtonMenu::SetDefaultButton(HWND hwnd, int nButton)
{
    POINT point;
    point.x = m_buttoninfo[nButton].X + (m_buttoninfo[nButton].Width/2);
    point.y = m_buttoninfo[nButton].Y + (m_buttoninfo[nButton].Height/2);
    ClientToScreen(hwnd, &point);
    SetCursorPos(point.x, point.y);
}

ExecuteMsiByOSArgs::ExecuteMsiByOSArgs( LPCTSTR newWin32ButtonCommand, LPCTSTR newWin32Argument, LPCTSTR newWin64ButtonCommand, LPCTSTR newWin64Argument ) : win32ButtonCommand(newWin32ButtonCommand), win32Argument(newWin32Argument), win64ButtonCommand(newWin64ButtonCommand), win64Argument(newWin64Argument)
{
    // No code needed    
}

// 
bool AlwaysEnabledButton(int /*nButton*/)
{
    // return enabled state
    return true;
}

bool AlwaysDisabledButton(int /*nButton*/)
{
    // return enabled state
    return false;
}

DWORD ExecuteMSI( HWND windowHandle, LPCTSTR installShieldEXEpath, LPCTSTR logFilenameAndArgs, int errorCodeResourceID, bool wait )
// Launches the specified MSI with logging enabled in the TEMP directory to the specified filename
// logFilenameAndArgs is the node name of the file in temp directory and any other desired arguments listed after that
// Needed because InstallShield's SETUP.EXE doesn't seem to want to pass %TEMP% through to MSIEXEC
{
    TCHAR installShieldArguments[3*MAX_PATH+1]          = {_T("")};
    TCHAR tempPath[MAX_PATH]                            = {_T("")};

    GetTempPath(sizeof(tempPath)/sizeof(tempPath[0]), tempPath);
    sssnprintf(installShieldArguments, sizeof(installShieldArguments), "/V\"/l*v %s\\%s\"", tempPath, logFilenameAndArgs);
    return Execute(windowHandle, installShieldEXEpath, installShieldArguments, errorCodeResourceID, wait);
}