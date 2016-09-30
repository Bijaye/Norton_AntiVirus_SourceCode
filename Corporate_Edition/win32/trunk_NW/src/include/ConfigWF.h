// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef __SYM_CONFIGUE_WF__
#define __SYM_CONFIGUE_WF__
//
//	WIN32/64 only
//
#if defined(WIN64) || defined(WIN32)


#include "IWinFW.h"
#ifdef __cplusplus
extern "C"
{
#endif
//
//	szReg_WinFW_Setting is a binary type value under SCS root reg
//	Currently it represents yes or not
//	in the future it can represent the whole object of windows 
//	firewall settings
//
static const TCHAR szReg_WinFW_Setting[] = _T("WinFWSetting");
static const TCHAR  szReg_WinFW_Config[] =	_T("WinFwConfig");
static const TCHAR szReg_WinFW_WinVer[] =	_T("WinVer");
static const TCHAR szReg_WinFW_RTVScanVM[] =_T("RTVScanVM");
static const TCHAR szReg_WinFW_RTVScanVL[] =_T("RTVScanVL");

static const TCHAR szReg_WinFW_NSCTopVM	[] =_T("NSCTopVM");
static const TCHAR szReg_WinFW_NSCTopVL	[] =_T("NSCTopVL");
static const TCHAR szReg_WinFW_PDSVM[] =	_T("PDSVM");
static const TCHAR szReg_WinFW_PDSVL[] =	_T("PDSVL");

typedef UINT (WINAPI *_GetSystemWow64Directory)(LPTSTR,UINT );

typedef HRESULT (* _ConfigWinFw4Apps)	(	IN LPCTSTR szWinFwDll, IN SYMAPP_LIST& apps);
typedef HRESULT (* _UnConfigWinFw4Apps) (	IN LPCTSTR szWinFwDll, IN SYMAPP_LIST& apps);


///////////////////////////////////////////////////////////////////////
//	Two help functions to configure/unconfigure Windows Firewall 
//	for SCS
///////////////////////////////////////////////////////////////////////s
HRESULT ConfigWF4SCS();
HRESULT UnConfigWF4SCS();

#ifdef __cplusplus
}
#endif



#endif //!#ifdef WIN32

#endif //!__SYM_CONFIGUE_WF__
