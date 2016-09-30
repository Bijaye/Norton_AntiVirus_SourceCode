/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

//#include "stdafx.h" 
#include "mmc.h"
#include "htmlhelp.h"
#include "htmlcall.h"


//char lpszPath,
//     HWND WINAPI HtmlHelpA( HWND hwndCaller, LPCSTR pszFile, UINT uCommand,DWORD dwData );
//     #define HH_DISPLAY_TOPIC        0x0000
//     #define HH_TP_HELP_CONTEXTMENU  0x0010  // text popup help, same as WinHelp HELP_CONTEXTMENU



#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


#ifdef  CALL_HTML_HELP  

BOOL CallHtmlHelpTopic( HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD dwData )
{
    HWND hHelpWin = NULL;

    hHelpWin =HtmlHelpA(
                    hwndCaller,
                    pszFile,                // (LPCTSTR)sTopic,    // "c:\\Help.chm::/Intro.htm"
                    HH_DISPLAY_TOPIC,         // 
                    NULL) ;

    return(TRUE);
}


BOOL CallHtmlHelpContext( HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD dwData )
{
    HWND hHelpWin = NULL;

    hHelpWin =HtmlHelpA(
                    hwndCaller,
                    pszFile,                // (LPCTSTR)sTopic,    // "c:\\Help.chm::/Intro.htm"
                    HH_TP_HELP_CONTEXTMENU,
                    dwData) ;

    return(TRUE);
}

#endif



#ifdef __cplusplus
}
#endif // __cplusplus

