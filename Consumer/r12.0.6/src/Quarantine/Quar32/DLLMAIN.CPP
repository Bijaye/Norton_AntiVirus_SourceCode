//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// Dllmain.cpp - contains exported functions for ScanObj project
//
#include "stdafx.h"

#include "Quar32.h"
#include "iquaran.h"            // Interfaces

#include "global.h"
#include "navntutl.h"

// Taken from QuarAdd_cc.h
#include "..\..\SDKs\AutoProtect\include\apquar.h"
#include "avtypes.h"

#include <time.h>
#include "xapi.h"

/////////////////////////////////////////////////////////////////////////////
// Global Data

HINSTANCE g_hInstance = NULL;   // Global instance handle to this module.


                                // Runtime links to NT helper DLLs.
HINSTANCE g_hInstNAVNTUTL = NULL;
HINSTANCE g_hInstNetApi32 = NULL;
PFNCOPYALTERNATEDATASTREAMS g_pfnCopyAlternateDataStreams = NULL;
PFNNetWkstaGetInfo   g_pfnNetWkstaGetInfo = NULL;
PFNNetApiBufferFree  g_pfnNetApiBufferFree = NULL;

/////////////////////////////////////////////////////////////////////////////
// Local constants

const int iGUIDSize = 64;

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if(DLL_PROCESS_ATTACH == dwReason)
    {
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls(hInstance);
        
        g_hInstance = hInstance;
    }

	return TRUE;    // ok
}


// ==== CreateUniqueFile =====================================================
// Creates a new file with a unique filename and the extension passed in.  The
// file is created in the TargetPath and the name is assigned to lpszFileName
// and a handle to the open file is returned.
//
// Input:  
//	 LPTSTR lpszTargetPath - Full path for the new file
//   LPTSTR lpszExtension  - Extension of new file 3 characters or less
//   LPTSTR lpszFilePath   - [out] full path a and filename of the new file.
//   LPTSTR lpszFilename   - [out] filename of the new file (new) Can be NULL.
// Output: HANDLE - Handle of newly opened file.
//                  This value will be INVALID_HANDLE_VALUE on failure.
////////////////////////////////////////////////////////////////////////////
// 2/26/98 JTAYLOR - Function created / Header added.
// 4/19/98 JTAYLOR - Enhanced file already exists error check.
////////////////////////////////////////////////////////////////////////////
HANDLE CreateUniqueFile(LPTSTR lpszTargetPath,
                        LPTSTR lpszExtension,
                        LPTSTR lpszFilePath,
						LPTSTR lpszFilename)
{
	// If no extension was passed in don't use one
	bool bUseExtension = true;
	if(lpszExtension == NULL || _tcslen(lpszExtension) == 0)
		bUseExtension = false;

	// Copy the destination path and make sure there is a slash on the end.

	// seed the random number generator
	srand((unsigned)time(NULL));

	auto HANDLE hFile = INVALID_HANDLE_VALUE;
	auto TCHAR szGeneratedFileName[MAX_QUARANTINE_FILENAME_LEN];
	while (INVALID_HANDLE_VALUE == hFile)
	{
		//Generate a random number for the filename
		DWORD dwNumber = MAKELONG(rand(),rand());
		
		if(bUseExtension)
		{
			// Make sure that a DWORD stays defined as 32 bits.
			SPRINTF( szGeneratedFileName, _T("%.8X."), dwNumber & 0xFFFFFFFF );

			// Append the extension to the filename after the '.'
			// only allow 3 character extensions
			_tcsncat(szGeneratedFileName, lpszExtension, 3);
		}
		else
		{
			// Make sure that a DWORD stays defined as 32 bits.
			SPRINTF( szGeneratedFileName, _T("%.8X"), dwNumber & 0xFFFFFFFF );
		}
		
		// Copy the path to the beginning of the output buffer
		_tcsncpy(lpszFilePath, lpszTargetPath, MAX_QUARANTINE_FILENAME_LEN);

		if(NULL != lpszFilename)
			_tcscpy(lpszFilename, szGeneratedFileName);

		// Append the filename to the path.
		NameAppendFile(lpszFilePath, szGeneratedFileName);

		// Attempt to create the new file.
		hFile = CreateFile(lpszFilePath,
			GENERIC_WRITE | GENERIC_READ,
			0,
			NULL,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		// If we were unable to create the file, loop only if the file could not
		// be created due to a filename collision.
		if( INVALID_HANDLE_VALUE == hFile )
		{
			// Loop again if the error was 'file already exists'.
			// ERROR_ALREADY_EXISTS is what the API claimed is returned if the
			// file already exists, ERROR_FILE_EXISTS is what is actually
			// returned on my computer.  I have included them both, in case one
			// is pre IE 4.0.
			DWORD dwLastError = GetLastError();
			if( ( ERROR_ALREADY_EXISTS != dwLastError ) &&
				( ERROR_FILE_EXISTS != dwLastError ) )
			{
				// Try creating the file without an extension in case the
				// Extension has illegal characters like \ * etc
				if(bUseExtension)
				{
					bUseExtension = false;
					continue;
				}

				return INVALID_HANDLE_VALUE;
			}
		}
		else
		{
			return hFile;
		}
	}

	return INVALID_HANDLE_VALUE;
}


// ==== GetQuarantinePath ====================================================
// Retrieves the Quarantine paths from the registry.  In VXD mode, the only 
// path that is retrievable is the Incoming path.
//
// Input: 
//	 LPTSTR lpszOutBuf   - Buffer in which the path will be stored. [OUT]
//   DWORD  nBufSize     - Size of lpszOutBuf in bytes.
//   int nTargetPathInfo - Number of the path to retrieve. 
//							Argument is ignored in VXD/NTK
// Output: BOOL  TRUE  for success
//               FALSE for failure
////////////////////////////////////////////////////////////////////////////
// 3/09/98 SEDWARD - Function created / Header added.
// 4/26/98 JTAYLOR - Header updated.
////////////////////////////////////////////////////////////////////////////
BOOL GetQuarantinePath(LPTSTR lpszOutBuf, DWORD nBufSize, int nTargetPathInfo)
{
	auto BOOL   bRetVal = TRUE;
	auto HKEY   hKey = NULL;
	auto long   lResult;
	auto LPTSTR lpszValueName = NULL;

	// open the Quarantine path registry key
	lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, REGKEY_QUARANTINE_PATH, 0, KEY_READ, &hKey);
	if (ERROR_SUCCESS != lResult)
	{
		bRetVal = FALSE;
		goto  Bail_Function;
	}

	// get the appropriate Quarantine path
	switch (nTargetPathInfo)
	{
	case   TARGET_QUARANTINE_PATH:
		lpszValueName = REGVAL_QUARANTINE_PATH_STR;
		break;

	case   TARGET_QUARANTINE_INCOMING_PATH:
		lpszValueName = REGVAL_QUARANTINE_INCOMING_PATH_STR;
		break;

	case   TARGET_QUARANTINE_TEMP_PATH:
		lpszValueName = REGVAL_QUARANTINE_TEMP_PATH_STR;
		break;

	default:
		lpszValueName = NULL;
		break;
	}

	if (NULL == lpszValueName)
	{
		bRetVal = FALSE;
		goto  Bail_Function;
	}

	// get the desired path value
	auto DWORD regValueType;
	lResult = RegQueryValueEx( hKey, lpszValueName, 0, &regValueType, (LPBYTE)lpszOutBuf, &nBufSize );
	if ((ERROR_SUCCESS != lResult)  ||  (REG_SZ != regValueType))
	{
		bRetVal = FALSE;
	}

Bail_Function:
	if (hKey)
	{
		RegCloseKey( hKey );
	}

	return bRetVal;
}
