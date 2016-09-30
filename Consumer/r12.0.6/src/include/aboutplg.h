// Copyright 1999 Symantec
// -------------------------------------------------------------------------
// ABOUTPLG.H
// Contains prototype for C helper entry point to invoke the standard
// about box.
//
// NOTE:
//    You should make this function return an error code and you should pass in
//    the language and SKU/PartNumber or put them in the string table and modify
//    this routine to get them.  This is just a crude example that shows how
//    to invoke the standard about COM interface.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _PLUGINDOABT_H
#define _PLUGINDOABT_H

#define STR_PLGABOUTLIB TEXT("ABOUTPLG.DLL")
typedef BOOL (CALLBACK* EGGDLGPROC)(HWND, UINT, WPARAM, LPARAM);

/////////////////////////////////////////////////////////////////////////////
// Local Prototypes
#if defined(__cplusplus)
extern "C" {
#endif
typedef struct {
    DWORD       dwStructSize;
    DWORD       dwFlags;
    HWND        hWndOwner;
    HINSTANCE   hInst;
    HICON       hIcon;
    LPCSTR      lpszAppName;
    LPCSTR      lpszVersion;
    LPCSTR      lpszCopyright;
    LPCSTR      lpszBuildString;
    EGGDLGPROC  fpHookProc;
    LPCSTR      lpszTemplate;
    DWORD       dwUserData;
    LPCSTR      lpszSerialID;
    HBITMAP     hBitmap;
    int         nTimer;
} PLUGINABOUTSTRUCT, FAR * LPPLUGINABOUTSTRUCT;
//void WINAPI DoStdAbout(LPPLUGINABOUTSTRUCT lpAbout);

#if defined(__cplusplus)
}
#endif
#endif

