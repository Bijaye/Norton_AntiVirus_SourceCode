// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// IntelRefCounting.cpp : Contains routines for the
// non-standard reference counting used by Intel.
//

#include "stdafx.h"
#include "savMainCA.h"
#include "cregent.h"
#include "SymSaferRegistry.h"

#define INTELSHAREDFILE_REGKEY			"SOFTWARE\\Intel\\LANDesk\\AMS2\\Installed\\Files"
#define INTELSHAREDINSTALLEDFILE_REGKEY "SOFTWARE\\Intel\\LANDesk\\AMS2\\Installed"
#define INTELSHAREDAMS2_REGKEY			"SOFTWARE\\Intel\\LANDesk\\AMS2"
#define INTEL_LANDESK_KEY				"Software\\Intel\\LANDesk"
#define SAV_PRODUCT_KEY					"SAV"

#define NUM_DLLS 5
const TCHAR *g_pszFiles[NUM_DLLS] = {_T("CBA.DLL"),_T("LOC32VC0.DLL"),_T("MSGSYS.DLL"),_T("NTS.DLL"),_T("PDS.DLL")};

DWORD	AddStrToList(LPSTR buf,LPCSTR item,DWORD delimiter=0);
DWORD	DelStrFromList(LPSTR buf,DWORD len,LPCSTR item,DWORD delimiter=0,DWORD divider=1);

// ==== UpdateIntelSharedFileCount =================================================
//
//   This exported function Updates Intels Common Tech files. (Maple libraries)
//   HKLM\Software\Intel\LANDesk\AMS2\Installed\Files\XXX
//   Input:
//      lpszFileName  -- the file name (with complete path)
//		lpszProductID -- the product ID (eg., SYMANTEC_SSCCONSOLE)
//      bInstall 	  -- TRUE - Install, FALSE - Uninstall	
//  
//	 Removed nasty delete file code from the original.
//
//   Output: a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 02/2000, RPULINT
//	Borrowed from IntelSharedFiles.cpp: 11/2003, Thomas Brock
// ========================================================================
BOOL __stdcall UpdateIntelSharedFileCount (
    LPTSTR lpszFileName,  // FileName - with complete pathname
    LPTSTR lpszProductID, // for SSC console its SYMANTEC_SSCCONSOLE
    BOOL   bInstall)      // TRUE Install, False - Uninstall
{
	TCHAR szKey[MAX_PATH];
	DWORD dwSize = MAX_PATH, dwKeySize;
	BOOL bKeyChanged = TRUE;
	HKEY hKeyIntelFiles ;
	TCHAR szClassName[] = "";
	DWORD dwDisp = 0, dwType = REG_MULTI_SZ;
	BOOL bDeleteRegistryValue = FALSE ;
	DWORD dwRet = ERROR_SUCCESS ;

	dwRet = RegCreateKeyEx ( HKEY_LOCAL_MACHINE,
							INTELSHAREDFILE_REGKEY,
							0,
							szClassName,
							REG_OPTION_NON_VOLATILE,
							KEY_READ | KEY_WRITE,0,
							&hKeyIntelFiles,
							&dwDisp);

    if (ERROR_SUCCESS != dwRet)
		return FALSE ;


	if (bInstall == TRUE )
	{
		// read the registry value if one exist - else create a new one.
		dwType = REG_MULTI_SZ ;
		dwSize = sizeof (szKey);
		memset(szKey,0,MAX_PATH);
		dwRet = SymSaferRegQueryValueEx(hKeyIntelFiles,lpszFileName,0,&dwType,(LPBYTE)szKey,&dwSize);
		
		// Update the registry only if its a first time install
		dwKeySize = AddStrToList(szKey,lpszProductID);
		if (dwKeySize != dwSize)
		{
			dwType = REG_MULTI_SZ ;
			dwSize = dwKeySize;
			RegSetValueEx(hKeyIntelFiles,lpszFileName,0,dwType,(LPBYTE)szKey,dwSize);			
		}
	}
	else
	{
		// remove product ID from the registry
		// if registry value is empty - delete the registry value and delete the file
		// 
		dwType = REG_MULTI_SZ ;
		dwSize = sizeof (szKey);
		memset(szKey,0,MAX_PATH);
		dwRet = SymSaferRegQueryValueEx(hKeyIntelFiles,lpszFileName,0,&dwType,(LPBYTE)szKey,&dwSize);
		if ( ERROR_SUCCESS != dwRet )
		{
	        RegCloseKey (hKeyIntelFiles);
			return TRUE;
		}

		dwKeySize = DelStrFromList(szKey,dwSize,lpszProductID);
		if (dwKeySize > 0)
		{
			dwType = REG_MULTI_SZ ;
			dwSize = dwKeySize ;
			RegSetValueEx(hKeyIntelFiles,lpszFileName,0,dwType,(LPBYTE)szKey,dwSize);			

		}
		else
		{
			// delete the registry value
			bDeleteRegistryValue = TRUE ;
		}
	}

	RegCloseKey (hKeyIntelFiles);

	if ( TRUE == bDeleteRegistryValue )
	{
		// Delete the registry value, if this key is emply, delete the key
		// Delete LANDesk\AMS2\Installed\Files
		// check if LANDesk is empty, if yes, delete it too

		CRegistryEntry cReg( HKEY_LOCAL_MACHINE, INTELSHAREDFILE_REGKEY  );

		cReg.SetSubkey( INTELSHAREDFILE_REGKEY );
		cReg.SetValueName (lpszFileName);
		cReg.DeleteValue( );

		cReg.SetSubkey( INTELSHAREDFILE_REGKEY );
		if ( cReg.IsKeyEmpty() )
		{
			cReg.DeleteKey( );
			
			cReg.SetSubkey( INTELSHAREDINSTALLEDFILE_REGKEY );
			if ( cReg.IsKeyEmpty() )
			{
				cReg.DeleteKey( );
			
				cReg.SetSubkey( INTELSHAREDAMS2_REGKEY );
				if ( cReg.IsKeyEmpty() )
				{
					cReg.DeleteKey( );
					cReg.SetSubkey( INTEL_LANDESK_KEY );
					if ( cReg.IsKeyEmpty() )
					{
						cReg.DeleteKey( );
					}
				}
			}
		}
	}
	return TRUE ;

} // UpdateIntelSharedFileCount

// Add string to multi-string list
DWORD AddStrToList(LPSTR buf,LPCSTR item,DWORD delimiter)
{        
	DWORD total=0;
	LPSTR tmp, q = buf;
	DWORD size;
	BOOL found=FALSE;

	while (*q) 
	{
		if (delimiter && (tmp = _tcschr(q,(char)delimiter)) && tmp != q)
			size = tmp-q+1;
		else
			size = strlen(q)+1;

		if (!_tcsnicmp(q,item,size-1))
			found = TRUE;
		total += size;
		q += size;
	}
 	if (!found) 
	{
		if (*buf && delimiter)
			*(q-1) = (char)delimiter;
		_tcscpy(q,item);
		size = strlen(q)+1;
		total += size;
		q += size;
		*q = 0;
	}
	total++;
	return total;
}

// Remove string from multi-string list
DWORD DelStrFromList(LPSTR buf,DWORD len,LPCSTR item,DWORD delimiter,DWORD divider)
{         
	DWORD total=0;
	
	LPSTR tmp, q = buf;
	DWORD size;

	while (*q) 
	{
		if (delimiter && (tmp = _tcschr(q,(char)delimiter)) && tmp != q)
			size = tmp-q+divider;
		else
			size = strlen(q)+divider;

		len -= size;
		if (!_tcsnicmp(q,item,size-divider)) 
			memmove(q,q+size,len);
		else 
		{
			total += size;
			q += size;
		}
	}
	if (*buf && *(q-1) == (char)delimiter)
		*(q-1) = '\0';

	if (total)
		total++;
	return total;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: WorkerBee
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//		BOOL - Installing or Uninstalling? (Install=TRUE)
//
// Returns:
//		ERROR_SUCCESS if successful, ERROR_INSTALL_FAILURE otherwise.
//
// Description:
//		Used by RefCountIntelFilesDec and RefCountIntelFilesDec.
//
//////////////////////////////////////////////////////////////////////////
// 11/18/03 - Thomas Brock - function created.
//////////////////////////////////////////////////////////////////////////

UINT __fastcall WorkerBee( MSIHANDLE hInstall, BOOL bInstalling )
{
	CString strFile;
	UINT retval = ERROR_SUCCESS;

	// Get the CustomActionData
	CString strDir = GetMSIProperty( hInstall, "CustomActionData" );
	if( 0 == strDir.GetLength() )
	{
		MSILogMessage( hInstall, _T("Error getting CustomActionData.") );
		retval = ERROR_INSTALL_FAILURE;
	}
	else
	{
		// Uppercase the directory...
		strDir.MakeUpper();
		AddSlash( strDir );

		for( int i = 0; i < NUM_DLLS; i++ )
		{
			// Construct value/file name...
			strFile = strDir;
			strFile += g_pszFiles[i];
			MSILogMessage( hInstall, strFile );

			if( ! UpdateIntelSharedFileCount( strFile.LockBuffer(), SAV_PRODUCT_KEY, bInstalling ) )
			{
				strFile.UnlockBuffer();
				MSILogMessage( hInstall, _T("Error updating Intel shared file count.") );
				retval = ERROR_INSTALL_FAILURE;
				break;
			}
			else
			{
				strFile.UnlockBuffer();
			}
		}
	}

	return retval;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: RefCountIntelFilesInc
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		ERROR_SUCCESS if successful, ERROR_INSTALL_FAILURE otherwise.
//
// Description:
//		Increments Intel specific reference counts.
//		RefCountIntelFilesDec is the rollback proceedure for this function.
//
//////////////////////////////////////////////////////////////////////////
// 11/18/03 - Thomas Brock - function created.
//////////////////////////////////////////////////////////////////////////

UINT _stdcall RefCountIntelFilesInc( MSIHANDLE hInstall )
{
	return WorkerBee( hInstall, TRUE );
}

//////////////////////////////////////////////////////////////////////////
//
// Function: RefCountIntelFilesDec
//
// Parameters:
//		MSIHANDLE - Handle to the current install session
//
// Returns:
//		ERROR_SUCCESS if successful, ERROR_INSTALL_FAILURE otherwise.
//
// Description:
//		Decrements Intel specific reference counts.
//		RefCountIntelFilesInc is the rollback proceedure for this function.
//
//////////////////////////////////////////////////////////////////////////
// 11/18/03 - Thomas Brock - function created.
//////////////////////////////////////////////////////////////////////////

UINT _stdcall RefCountIntelFilesDec( MSIHANDLE hInstall )
{
	return WorkerBee( hInstall, FALSE );
}
