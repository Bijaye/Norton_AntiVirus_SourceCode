// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//S32RasU.H

#ifndef _S32RASU_H_
#define _S32RASU_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifdef __cplusplus
extern "C" {
#endif

   
typedef enum
{
   S32RASU_PROP_MAIN   = 0,  // Page numbering must start at 0 (zero).
   S32RASU_PROP_END
} ES32RASU_PROP_PAGES;

#define SZ_S32RASU_DLL TEXT("S32RasU.DLL")

#define S32RASU_GETPROCADDR(hInst, text) (LPFNS32RASU_ ## text )GetProcAddress(hInst, SZ_S32RASU_ ## text);
#define S32RASU_DeclarePFN(hInst, x, y) LPFNS32RASU_ ## x y = S32RASU_GETPROCADDR(hInst, x)

void _declspec(dllexport) WINAPI S32RASU_Register(void);
typedef void (WINAPI * LPFNS32RASU_REGISTER)( void );
#define SZ_S32RASU_REGISTER   TEXT("_S32RASU_Register@0")

void _declspec(dllexport) WINAPI S32RASU_Unregister(void);
typedef void (WINAPI * LPFNS32RASU_UNREGISTER)( void );
#define SZ_S32RASU_UNREGISTER   TEXT("_S32RASU_Unregister@0")

BOOL _declspec(dllexport) WINAPI S32RASU_IsConnected(void);
typedef BOOL (WINAPI * LPFNS32RASU_ISCONNECTED)( void );
#define SZ_S32RASU_ISCONNECTED   TEXT("_S32RASU_IsConnected@0")

BOOL _declspec(dllexport) WINAPI S32RASU_ConfigAutodial(HWND hWndParent);
typedef BOOL (WINAPI * LPFNS32RASU_CONFIGAUTODIAL)( HWND );
#define SZ_S32RASU_CONFIGAUTODIAL   TEXT("_S32RASU_ConfigAutodial@4")

BOOL _declspec(dllexport) WINAPI S32RASU_IsRASConfigured(void);
typedef BOOL (WINAPI * LPFNS32RASU_ISRASCONFIGURED)( void );
#define SZ_S32RASU_ISRASCONFIGURED   TEXT("_S32RASU_IsRASConfigured@0")

BOOL _declspec(dllexport) WINAPI S32RASU_IsRASConnected(void);
typedef BOOL (WINAPI * LPFNS32RASU_ISRASCONNECTED)( void );
#define SZ_S32RASU_ISRASCONNECTED   TEXT("_S32RASU_IsRASConnected@0")

BOOL _declspec(dllexport) WINAPI S32RASU_IsAutodialEnabled(void);
typedef BOOL (WINAPI * LPFNS32RASU_ISAUTODIALENABLED)( void );
#define SZ_S32RASU_ISAUTODIALENABLED   TEXT("_S32RASU_IsAutodialEnabled@0")

BOOL _declspec(dllexport) WINAPI S32RASU_IsSilentAutodialEnabled(void);
typedef BOOL (WINAPI * LPFNS32RASU_ISSILENTAUTODIALENABLED)( void );
#define SZ_S32RASU_ISSILENTAUTODIALENABLED   TEXT("_S32RASU_IsSilentAutodialEnabled@0")

BOOL _declspec(dllexport) WINAPI S32RASU_HangUp(HWND hWndParent);
typedef BOOL (WINAPI * LPFNS32RASU_HANGUP)( HWND );
#define SZ_S32RASU_HANGUP   TEXT("_S32RASU_HangUp@4")

BOOL _declspec(dllexport) WINAPI S32RASU_Connect(HWND hWndParent);
typedef BOOL (WINAPI * LPFNS32RASU_CONNECT)( HWND );
#define SZ_S32RASU_CONNECT   TEXT("_S32RASU_Connect@4")


BOOL _declspec(dllexport) WINAPI S32RASU_GetPropertySheet( ES32RASU_PROP_PAGES ePage, LPPROPSHEETPAGE ppsp );
typedef BOOL (WINAPI * LPFNS32RASU_GETPROPERTYSHEET)( ES32RASU_PROP_PAGES, LPPROPSHEETPAGE );
#define SZ_S32RASU_GETPROPERTYSHEET   TEXT("_S32RASU_GetPropertySheet@8")

BOOL _declspec(dllexport) WINAPI S32RASU_DoProperties( HWND hwndParent );
typedef BOOL (WINAPI * LPFNS32RASU_DOPROPERTIES)( HWND );
#define SZ_S32RASU_DOPROPERTIES   TEXT("_S32RASU_DoProperties@4")


BOOL _declspec(dllexport) WINAPI S32RASU_IsAOLDefault( void );
typedef BOOL (WINAPI * LPFNS32RASU_ISAOLDEFAULT)( void );
#define SZ_S32RASU_ISAOLDEFAULT   TEXT("_S32RASU_IsAOLDefault@0")


BOOL _declspec(dllexport) WINAPI S32RASU_AOL_IsConnected( void );
typedef BOOL (WINAPI * LPFNS32RASU_AOL_ISCONNECTED)( void );
#define SZ_S32RASU_AOL_ISCONNECTED   TEXT("_S32RASU_AOL_ISCONNECTED@0")


BOOL _declspec(dllexport) WINAPI S32RASU_AOL_IsConfigured( void );
typedef BOOL (WINAPI * LPFNS32RASU_AOL_ISCONFIGURED)( void );
#define SZ_S32RASU_AOL_ISCONFIGURED   TEXT("_S32RASU_AOL_ISCONFIGURED@0")

//BOOL AOL_LaunchWithURL(HINSTANCE hInstRES, LPCTSTR lpszURL);
BOOL _declspec(dllexport) WINAPI S32RASU_AOL_LaunchWithURL( HWND hWndParent, LPCTSTR lpszURL );
typedef BOOL (WINAPI * LPFNS32RASU_AOL_LAUNCHWITHURL)( HWND, LPCTSTR );
#define SZ_S32RASU_AOL_LAUNCHWITHURL   TEXT("_S32RASU_AOL_LaunchWithURL@8")


#ifdef __cplusplus
}
#endif

#endif // _S32RASU_H_