////////////////////////////////////////////////////////////////////////////
// NQFonts.cpp : header file
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
////////////////////////////////////////////////////////////////////////////
// $Header:   S:/UNIUTIL/VCS/nqfonts.cpv   1.0   18 Apr 1997 14:09:50   DSACKIN  $
//
// Description:
//
//  This module DBCS-enables the controls within a dialog.  It basically
//  does the same thing as "ngfont.cpp" in the SYMGUI project, but without
//  depending on quake (thanks Stahl!).
//
////////////////////////////////////////////////////////////////////////////
//
// $Log:   S:/UNIUTIL/VCS/nqfonts.cpv  $
// 
//    Rev 1.0   18 Apr 1997 14:09:50   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:19:32   RFULLER
// Initial revision
// 
//    Rev 1.0   03 Feb 1997 17:03:14   ksackin
// Initial revision.
//
//    Rev 1.0   21 Jan 1997 18:39:54   SEDWARD
// Initial revision.
//
//
////////////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include <windows.h>

#define SYM_EXPORT _declspec(dllexport)

#include "dbcs.h"
#include "UniUtil.h"

#include "codepage.h"
#define PT_TO_PIXELS(hdc, pt)	\
    MulDiv(-(pt), GetDeviceCaps(hdc, LOGPIXELSY), 72)


typedef struct {
    TCHAR       szClassName[128];
    DWORD       dwStyle;
    HFONT       hFont;
} NGFONTCNTRL, FAR * LPNGFONTCNTRL;


BOOL WINAPI NGSetControlFontCallBack (
   HWND             hWndControl,        // [in]    Control Window
   LPNGFONTCNTRL    lpnnufont           // [in]    Font control structure
);






//------------------------------------------------------------------------
// NQ_SetFontsInDialog
//
// This routine is used to set the controls in a dialog to have the
// Norton "look".  This has the following definition:
//
// - All standard push buttons are BOLD default font
// - All group box titles are also BOLD default font
// - All other controls are NORMAL default font
//
// NOTE: Once this function completes, then the caller should set
// any other controls to their special font values.
//
// Returns: nothing
//------------------------------------------------------------------------
VOID WINAPI NQ_SetFontsInDialog(
                                    HWND    hDlg,        // [in]     Dialog window
                                    DWORD   dwReserved   // [in]     Reserved (user zero)
                                )
{
    dwReserved = dwReserved;
    auto HFONT  hFont;

    OSVERSIONINFO osVersionInfo;
    ZeroMemory((LPVOID)&osVersionInfo,sizeof(osVersionInfo));

    osVersionInfo.dwOSVersionInfoSize = sizeof(osVersionInfo);

    if( !GetVersionEx(&osVersionInfo) )
        {
        return;
        }

    if(osVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS /* 95 */)
        {
        if ( !_DBCSInstalled() )
            {
            return;
            }
        }

    // Get the "normal" default font
    // and set all controls in the
    // dialog to use it...
    NQ_FontGet (hDlg, &hFont, FW_NORMAL, 0);
    NQ_SetControlFont (hDlg, hFont, NULL, 0 );

#if 0
    // Now, set all the buttons to
    // have a bold font
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_31)
    {
        NQ_FontGet (hDlg, &hFont, FW_BOLD, 0);
        NQ_SetControlFont (hDlg, hFont, szButton, 0);

        // Set fonts for checkboxes
        // and radio buttons to normal again
        NQ_FontGet (hDlg, &hFont, FW_NORMAL, 0 );
        NQ_SetControlFont (hDlg, hFont, szButton,  BS_CHECKBOX );
        NQ_SetControlFont (hDlg, hFont, szButton,  BS_AUTOCHECKBOX );
        NQ_SetControlFont (hDlg, hFont, szButton,  BS_RADIOBUTTON );
        NQ_SetControlFont (hDlg, hFont, szButton,  BS_AUTORADIOBUTTON );
    }
#endif

}  // end of "NQ_SetFontsInDialog"




//------------------------------------------------------------------------
// NQ_FontGet
//
// This routine was "lifted" from the NDW Viper source code (nwin, nwinutil.c)
//
// This routine handles the creation and reuse of bold and normal default
// size fonts.  The default fonts are automatically deleted at shutdown
// time, but custom fonts are the responsiblity of the caller.
//
// Returns: 0x0000 for success or error code (0xffff).
//------------------------------------------------------------------------
WORD  WINAPI NQ_FontGet(
                            HWND  hWnd,             // Window to get DC from...
                            HFONT FAR *lphFont,     // If NULL, then free cached fonts
                            int nWeight,            // FW_NORMAL, FW_BOLD, or custom wt
                            int nHeight )           // If zero, then use default height
{
    static HFONT        hBold   = NULL;
    static HFONT        hNormal = NULL;
    auto   HFONT FAR    *lphTemp = NULL;
    auto   LOGFONT      lf;
    auto   HDC          hdc;
    auto   DWORD        dwVersion;



    if (!lphFont)                       // Free the fonts
        {
#if 0
        if (GetSystemMetrics(SM_DEBUG))
            {
            auto    TCHAR   szStr[MAX_PATH];
            if (hBold)
                {
                wsprintf(szStr, NGLoadStr(NQ_FONT_DONE), Q_VER, hBold);
                OutputDebugString(szStr);
                }
            if (hNormal)
                {
                wsprintf(szStr, NGLoadStr(NQ_FONT_DONE), Q_VER, hNormal);
                OutputDebugString(szStr);
                }
            }
#endif
        if (hBold)
            {
            DeleteObject(hBold);
            }
        if (hNormal)
            {
            DeleteObject(hNormal);
            }

        hBold = hNormal = NULL;
        return (0x0000);
        }

    /*
    **  Set lphTemp to the appropriate handle to be used later.
    */
    if (!nHeight)
        {
        nHeight = _DBCSInstalled() ? NQ_GetDBCSFontSize() : 8;
        switch (nWeight)
            {
            case FW_NORMAL:
            case FW_DONTCARE:
                lphTemp = &hNormal;
                break;

            case FW_BOLD:
                lphTemp = &hBold;
                break;

            default:
                break;
            }
        }

    /*
    **  Return the handle if the font has already been created.
    */
    if (lphTemp  &&  *lphTemp)
        {
        *lphFont = *lphTemp;
        return (0);
        }

    hdc = GetDC(hWnd);
    ZeroMemory((LPVOID)&lf,sizeof(lf));

    // DBCS needs non-bold, since unreadable.
    if ((FW_BOLD == nWeight)  &&  (_DBCSInstalled()))
        {
        nWeight = FW_NORMAL;
        }

    lf.lfWeight         = nWeight;
    lf.lfCharSet        = _DBCSInstalled() ? NQ_GetDBCSCharSet() : ANSI_CHARSET;
    lf.lfOutPrecision   = OUT_STRING_PRECIS;
    lf.lfClipPrecision  = CLIP_STROKE_PRECIS;
    lf.lfQuality        = DRAFT_QUALITY;

    // Force to exactly eight point
    lf.lfHeight         = PT_TO_PIXELS(hdc, nHeight);
    lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;

    dwVersion = GetVersion();
    lstrcpy (lf.lfFaceName, _DBCSInstalled() ? NQ_GetDBCSFaceName() : _T("MS Sans Serif"));
    ReleaseDC(hWnd, hdc);

    /*
    **  6/9/92: Return ERR if the font cannot be created.
    */
    if (NULL == (*lphFont = CreateFontIndirect(&lf)))
        {
        return(0xFFFF);
        }

        /*
        **  Save the handle for future use if it is one of the types that
        **  are always available.
    */
    if (lphTemp)
        {
        *lphTemp = *lphFont;
        }

    return( 0x0000 );

}  // end of "NQ_FontGet"




//------------------------------------------------------------------------
// NQ_SetControlFont
//
// This routine is called to set the dialog box control's fonts to be
// the font specified.  The class name of the controls to set is
// can be specified to limit the scope or nothing to set all child
// controls.
//
// Thanks Torsten for the idea...
//
// NOTE:    DWORD   dwStyle parameter will only compare the exact value
//          of the LOWORD to the window's LOWORD class style.
//
// Returns: nothing
//
// DOUGN 8/27/92
//------------------------------------------------------------------------
VOID  WINAPI NQ_SetControlFont(
                                    HWND    hWnd,       // [in] Dialog Box Window
                                    HFONT   hFont,      // [in] Font to use
                                    LPTSTR   lpszClass, // [in] Control window class or ""
                                    DWORD   dwStyle     // [in] Optional style bit to match
                                )
{
    auto    NGFONTCNTRL     NGfont;

    // Initialize the structure to pass
    NGfont.szClassName[0] = NULL;

    if (lpszClass)
        {
        lstrcpy (NGfont.szClassName,lpszClass);
        CharUpper(NGfont.szClassName);
        }

    NGfont.hFont   = hFont;
    NGfont.dwStyle = dwStyle;

    // Enumerate all the child controls of this dialog box.
    EnumChildWindows(hWnd, (WNDENUMPROC) NGSetControlFontCallBack
                                            , (DWORD)(LPNGFONTCNTRL)&NGfont);

}  // end of "NQ_SetControlFont"





//***************************************************************************
// NQ_GetDBCSFontSize()
//
// Description:
// Function that checks the codepage, and returns the hardcoded DBCS
// font size (from DBCS.H). For Win95, but should work on NT. Should only be
// called if _DBCSInstalled() already returned TRUE.
//
// Parameters: None
//
// Return Value: int The hardcoded size of the DBCS font (from DBCS.H)

//***************************************************************************
// 10/09/1996 GWESTER Function Created.
//***************************************************************************
int WINAPI NQ_GetDBCSFontSize(VOID)
{
    switch ( GetACP() )
        {
        case CODEPAGE_CHINESE_TRADITIONAL:
        case CODEPAGE_CHINESE_SIMPLIFIED:
            return DBCS_FONT_SIZE_CHINESE;

        case CODEPAGE_KOREAN_WANSUNG:
        case CODEPAGE_KOREAN_JOHAB:
            return DBCS_FONT_SIZE_KOREAN;

        default:                    // Assume CODEPAGE_JAPAN
            return DBCS_FONT_SIZE_JAPANESE;
        }

}  // end of "NQ_GetDBCSFontSize"





//***************************************************************************
// NQ_GetDBCSCharSet()
//
// Description:
// Function that checks the codepage, and returns the proper character set
// identifier (from the SDK). For Win95, but should work on NT. Should only be
// called if _DBCSInstalled() already returned TRUE.
//
// Parameters: None
//
// Return Value: BYTE The hardcoded character set of the DBCS font (from SDK)
//
//***************************************************************************
// 10/09/1996 GWESTER Function Created.
//***************************************************************************
BYTE WINAPI NQ_GetDBCSCharSet ( VOID )
{
    switch ( GetACP() )
        {
        case CODEPAGE_CHINESE_TRADITIONAL:
        case CODEPAGE_CHINESE_SIMPLIFIED:
            return CHINESEBIG5_CHARSET;

        case CODEPAGE_KOREAN_WANSUNG:
        case CODEPAGE_KOREAN_JOHAB:
            return HANGEUL_CHARSET;

        default:                    // Assume CODEPAGE_JAPAN
            return SHIFTJIS_CHARSET;
        }

}  // end of "NQ_GetDBCSCharSet"




static BOOL gbDBCSFirstTime = FALSE;
static BOOL gbDBCSInstalled = FALSE;

//***************************************************************************
// _DBCSInstalled
//
// Description:
// Parameters:
// Return Value:
//
//***************************************************************************
// 10/09/1996 GWESTER Function Created.
//***************************************************************************
BOOL WINAPI _DBCSInstalled(VOID)
{
    if(gbDBCSFirstTime)
        {
        return gbDBCSInstalled;
        }

    gbDBCSFirstTime = TRUE;
    for (int i = 0; i <= 255; i++)
        {
        if (IsDBCSLeadByte((BYTE) i))
            {
            gbDBCSInstalled = TRUE;
            break;
            }
        }
    return gbDBCSInstalled;

}  // end of "_DBCSInstalled"




//***************************************************************************
// NQ_GetDBCSFaceName()
//
// Description:
// Function that checks the codepage, and returns the hardcoded DBCS
// font face name (from DBCS.H). The variable width, not fixed width, font is
// returned. For Win95, but should work on NT. Should only be called if
// DBCSInstalled() already returned TRUE.
//
// Parameters: None
//
// Return Value: LPCHAR The hardcoded name of the DBCS font (from DBCS.H)
//
//***************************************************************************
// 10/09/1996 GWESTER Function Created.
//***************************************************************************
LPTSTR WINAPI NQ_GetDBCSFaceName(VOID)
{
    switch ( GetACP() )
        {
        case CODEPAGE_CHINESE_TRADITIONAL:
        case CODEPAGE_CHINESE_SIMPLIFIED:
            return _T(DBCS_FONT_CHINESE);

        case CODEPAGE_KOREAN_WANSUNG:
        case CODEPAGE_KOREAN_JOHAB:
            return _T(DBCS_FONT_KOREAN);

        default:                    // Assume CODEPAGE_JAPAN
            return _T(DBCS_FONT_JAPANESE);
        }


}  // end of "NQ_GetDBCSFaceName"




//------------------------------------------------------------------------
// NGSetControlFontCallBack
//
// This is the call back for the set control font routine enumeration.
//
// Returns: TRUE to continue enumeration
//------------------------------------------------------------------------
BOOL  WINAPI NGSetControlFontCallBack(
                HWND            hWndControl,    // [in]    Control Window
                LPNGFONTCNTRL   lpNGfont        // [in]    Font control structure
)
{
    auto TCHAR   szClassName[128];
    auto DWORD   dwStyle = 0;



    szClassName[0] = NULL;
                                        // Get the class name of the window
                                        // and it's Style
    GetClassName (hWndControl,szClassName,128);
    CharUpper (szClassName);

    dwStyle = GetWindowLong (hWndControl, GWL_STYLE);


                                        // If there is a class name and
                                        // this window doesn't match, then
                                        // skip...
    if  ((lpNGfont->szClassName[0])
    &&  (lstrcmp (lpNGfont->szClassName, szClassName)))
        {
        return TRUE;                      // (continue enumeration)
        }

                                        // If there is a flag bit to match
                                        // in the style, then match it
    if  ((lpNGfont->dwStyle)
    &&  (LOWORD(lpNGfont->dwStyle) != LOWORD(dwStyle)))
        {
        return TRUE;
        }

                                        // Set the font for this control...
    SendMessage(hWndControl, WM_SETFONT, (WPARAM)lpNGfont->hFont, 0L);

                                        // Continue enumeration... (true)
    return TRUE;

}  // end of "NGSetControlFontCallBack"

