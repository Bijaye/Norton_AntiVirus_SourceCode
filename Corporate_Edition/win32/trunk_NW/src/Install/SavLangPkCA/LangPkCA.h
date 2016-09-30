// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// LANGPKCA.h : main header file for the LANGPKCA DLL
//

#if !defined(AFX_LANGPKCA_H__1F7FF97E_52CC_4CFC_AD9B_38473CB52AA2__INCLUDED_)
#define AFX_LANGPKCA_H__1F7FF97E_52CC_4CFC_AD9B_38473CB52AA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <setupapi.h>
#include <string>
#include <map>

using namespace std;


typedef WINSETUPAPI BOOL (WINAPI *SETUPITERATECABPROC)(PCSTR CabinetFile, DWORD Reserved, PSP_FILE_CALLBACK_A MsgHandler, PVOID Context);
#define SETUPITERATECABNAME		"SetupIterateCabinetA"
#define LOG_PREFIX "SAVINST: "

struct CabArgs
{
	string sourceFilename;
	string targetFilename;
	bool extractedFile;

	CabArgs() : sourceFilename(""), targetFilename(""), extractedFile(false) {};
};

CString GetMSIProperty(MSIHANDLE hInstall, CString strProperty);
UINT MSILogMessage(MSIHANDLE, CString&);
UINT MSILogMessage(MSIHANDLE, TCHAR []);

static UINT WINAPI CabHandler(PVOID context, UINT notificationCode, UINT param1, UINT param2);
BOOL CabDecompressFile(string sourceFilename, string szLocation);
BOOL RemoveAllLanguagePackFiles(string sSearchPattern); //** to be exported **
string CreateTempResDir(string sInstallLocation);
BOOL GetLanguagePacks(string sSearchPattern, string sLocation);
BOOL DeleteDirectory(const TCHAR* sPath, MSIHANDLE);
HRESULT CabInitialize(MSIHANDLE hInstall);

UINT __stdcall InstallLanguagePacks( MSIHANDLE );
UINT __stdcall RemoveLanguagePacks( MSIHANDLE );




#endif // !defined(AFX_LANGPKCA_H__1F7FF97E_52CC_4CFC_AD9B_38473CB52AA2__INCLUDED_)
