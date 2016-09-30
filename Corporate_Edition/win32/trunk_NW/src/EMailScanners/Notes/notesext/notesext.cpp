// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#include "notesext.h"
#include "install.h"

// notesext.c Utility functions for the storage extension.

// Global data

HINSTANCE	g_hResource = NULL;
HANDLE		g_hShutdown = NULL;
HANDLE      g_hUserAccessToken  = NULL;

/*===========================================================================*/
// Misc functions
/*===========================================================================*/

/*---------------------------------------------------------------------------*/

void GetExtension(LPTSTR szFileName, LPTSTR szExt)
{
	TCHAR	szTemp[_MAX_EXT];

	_tsplitpath(szFileName, NULL, NULL, NULL, szTemp);		
	_tcsupr(szTemp);
	
	// don't copy the '.'
	_tcscpy(szExt, _tcsinc(szTemp));
}

/*---------------------------------------------------------------------------*/

//	GetFakeSID formats a static "fake" SID for identification purposes.
//	the SID is used as an indentifier for an SNODE.

void GetFakeSID(mSID * SID)
{
	// note that SID_SIZE is defined as 64, and that a SID is
	// defined as an array of DWORD [SID_SIZE], so the actual
	// number of TCHARs in the array is a multiple of SID_SIZE;
	// 4*SID_SIZE for chars, and 2*SID_SIZE for wchars.
	DWORD maxLen = SID_SIZE * (sizeof(DWORD) / sizeof(TCHAR));
	DWORD userLen;
	mSID  mySID = {0};
	TCHAR szSep[] = _T("\x01");
		
	mySID.stuff[0] = 0xFFFFFFFF;
	maxLen -= sizeof(DWORD) / sizeof(TCHAR);

	userLen = maxLen;
	GetUserName((LPTSTR)&mySID.stuff[1], &userLen);
	_tcscat((LPTSTR)&mySID.stuff[1], szSep);
	maxLen -= _tcslen((LPTSTR)&mySID.stuff[1]);
	GetComputerName((LPTSTR)&mySID.stuff[1] + _tcslen((LPTSTR)&mySID.stuff[1]), &maxLen);
	
	*SID = mySID;
}

/*===========================================================================*/
// Install functions
/*===========================================================================*/

/*---------------------------------------------------------------------------*/

//	NSE_Install performs the primary installation necessary for the 
//	notes storage extension to function.
//
//	Parameters:
//		flags	A DWORD indicating whether this is an install or uninstall.
//
//	Returns:
//		ERROR_SUCCESS for success

DWORD NSE_Install(DWORD install)
{
	return ERROR_SUCCESS;
}

/*--- end of source ---*/
