// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------

#include "oleimage.h"

#define Wow6432Node                     "SOFTWARE\\Wow6432Node"

struct BUTTONINFO;
struct SYMMENUINFO;

// master object - this is the public interface
class CButtonMenu
{
public:
    // ctor
    CButtonMenu();

    // dtor
    virtual ~CButtonMenu();

    // interface calls
    int GetButtonCount()
    {
        return m_nButtonCount;
    };

    // current button
    static const int NO_BUTTON; // = -1;
    int GetCurrentButton()
    {
        return m_nCurrentButton;
    }
    void SetCurrentButton(int nNewValue)
    {
        m_nCurrentButton = nNewValue;
    }

    COleImage *GetBackgroundBitmap();
    void DrawButton (HDC hdc, int nButton);
    void InvalidateButton(HWND hwnd, int nButton);

    // gets button # from x, y position
    int GetButtonFromPosition (int X, int Y);

    // gets button # from cmd ID
    int GetButtonFromID (UINT nButtonID);

    // perform action associated with button
    void ProcessButtonCommand(HWND hWnd, int nButton);
    void SwitchMenu(HWND hwnd, LPCTSTR cszName);
    void SetMenu(LPCTSTR cszName);
    HACCEL GetCurrentAccelerator();

    static UINT GetAppNameID();

    // Point cursor to the first button
    void SetDefaultButton(HWND hWnd, int nButton);

    // is mouse down
    bool m_bMouseDown; 

private:
    // ptr to button info array
    BUTTONINFO *m_buttoninfo;
    SYMMENUINFO *m_menuinfo;

    // size of array
    int m_nButtonCount; 
    int m_nMenuCount;

    // current button (can = NO_BUTTON)
    int m_nCurrentButton;
    // current menu
    int m_nCurrentMenu;

    // font for button text
    HFONT m_hFont;
    HFONT m_hDescFont;

    COleImage * GetButtonDisabledBitmap();
    COleImage * GetButtonNormalBitmap();
    COleImage * GetButtonDownBitmap();
    COleImage * GetButtonHighlightBitmap();

    // Load menu specific resources
    BOOL InitializeMenuInfo();

    // this is used to disable buttons (ie MSDE)
    bool IsButtonEnabled(int nButton);
	bool IsMachine64bit();

    // fns for drawing a button
    BOOL DrawButtonText(HDC hDC, int nButton, COLORREF Color, BOOL bShift);

};

// launches a button cmd
DWORD Execute(HWND hwnd, LPCTSTR strCmd, LPCTSTR strParam, int nErrorCode, bool bWait = false);
// Launches the specified MSI with logging enabled in the TEMP directory to the specified filename
// logFilenameAndArgs is the node name of the file in temp directory and any other desired arguments listed after that
// Needed because InstallShield's SETUP.EXE doesn't seem to want to pass %TEMP% through to MSIEXEC
DWORD ExecuteMSI( HWND windowHandle, LPCTSTR installShieldEXEpath, LPCTSTR logFilenameAndArgs, int errorCodeResourceID, bool wait = false );

// 
bool AlwaysEnabledButton(int nButton);
bool AlwaysDisabledButton(int nButton);