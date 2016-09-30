//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// QuarantineDLL.cpp - Contains implementation for CQuarantineDLL
//
////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/global.cpv  $
// 
//    Rev 1.10   24 Jul 1998 12:21:08   tcashin
// Fixed a problem in GetFileSecurityDesc(). Return ok if we try get a security
// descriptor for a file on a non-NT machine (defect #118986).
// 
//    Rev 1.9   23 Jul 1998 01:47:58   jtaylor
// Updated IsFileCompressed to handle more extensions and be more expandable.
// 
//    Rev 1.8   08 Jul 1998 21:06:02   jtaylor
// Updated to Xor the contents of Quarantine files by 0xff.  No longer delete non Qfiles in Incoming directory.
// 
//    Rev 1.7   22 Jun 1998 20:49:40   jtaylor
// Added support for setting the Quarantine bloodhound level to 3.
// 
//    Rev 1.6   12 Jun 1998 20:25:46   DBuches
// Added extra error checking to TransferFile().
// 
//    Rev 1.5   11 Jun 1998 20:40:42   DBuches
// Fixed problems with getting domain name.  We now run-time link with 
// the NT LanMan routines for getting the domain name.
// 
//    Rev 1.4   11 Jun 1998 13:45:32   DBuches
// Added code to save alternate data streams in TransferFile().
// 
//    Rev 1.3   08 Jun 1998 19:47:58   SEDWARD
// Added NumFilesInQuarantine().
//
//    Rev 1.2   22 May 1998 17:31:08   DBuches
// Fixed problem with GetDomainName().  If user was not in a domain, this
// function would copy the user name to the user supplied buffer.  Now, it
// only copies when the user is actually in a domain.
//
//    Rev 1.1   18 May 1998 21:22:22   jtaylor
// Removed duplicate code, improved modularity and readability.
//
//    Rev 1.0   15 May 1998 18:47:12   jtaylor
// Initial revision.
//
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

// Taken from QuarAdd_cc.h
#include "..\..\SDKs\AutoProtect\include\apquar.h"
#include "avtypes.h"

#include "quar32.h"
#include "global.h"
#include "xapi.h"
//#include "winnetwk.h"
#include "navntutl.h"
//#include "lm.h"
#include "osinfo.h"

extern PFNCOPYALTERNATEDATASTREAMS g_pfnCopyAlternateDataStreams;
extern PFNNetWkstaGetInfo   g_pfnNetWkstaGetInfo;
extern PFNNetApiBufferFree  g_pfnNetApiBufferFree;


// ==== GetDomainName ==========================================================
// Determines the domain name of the machine
////////////////////////////////////////////////////////////////////////////
// 5/15/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL GetDomainName( LPSTR lpszDomainName, DWORD nBuffSize )
{
    BOOL bRet = FALSE;

    // For Win9x systems, we can peek at a registry key for this.
    if( !IsWinNT() )
        {
        HKEY hKey;
        LONG lResult;
        DWORD dwType = REG_SZ;

        // Clear buffer.
        ZeroMemory( lpszDomainName, nBuffSize );

        // Open MSNetwork key.
        lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                        _T("System\\CurrentControlSet\\Services\\MSNP32\\NetworkProvider"),
                        0,
                        KEY_READ,
                        &hKey );

        if( ERROR_SUCCESS == lResult )
            {
            // Read data.
            lResult = RegQueryValueEx( hKey,
                                        _T("AuthenticatingAgent"),
                                        NULL,
                                        &dwType,
                                        (LPBYTE) lpszDomainName,
                                        &nBuffSize );

            if( ERROR_SUCCESS == lResult )
                bRet = TRUE;

            // Cleanup.
            RegCloseKey(hKey);
            }

        }
    else if( g_pfnNetWkstaGetInfo && g_pfnNetApiBufferFree )
        {
        // This is ugly for NT machines.  We need to dynamically link to netapi32 in order
        // to call the function we need.  This code copied from the UNIUTIL project.
        PWKSTA_INFO_100 pWkstaInfo;
        NET_API_STATUS  naStatus;
        LPBYTE          pBuf;

        if ( lpszDomainName != NULL )
            {
            naStatus = g_pfnNetWkstaGetInfo( NULL, 100, &pBuf );
            if ( naStatus == NERR_Success )
                {
                pWkstaInfo = ( PWKSTA_INFO_100 )pBuf;
                WideCharToMultiByte( CP_ACP,
                                     0,
                                     (LPWSTR) pWkstaInfo->wki100_langroup,
                                     -1,
                                     lpszDomainName,
                                     nBuffSize,
                                     NULL,
                                     NULL );
                bRet = TRUE;
				g_pfnNetApiBufferFree( pBuf ); 
                }
            else
                {
                bRet = FALSE;
                }
            }
        }

    
    // All done.  Return results.
    return bRet;
}    
    

// ==== IsFileCompressed ==========================================================
// Determines if the file extension passed in matches one of the stored file
// extensions.  This is a case in-sensitive compare and will work for any
// extension length.
//
// Input: lpszFileExtension - The extension to check
////////////////////////////////////////////////////////////////////////////
// 5/15/98 JTAYLOR - Function created / Header added.
// 7/23/98 JTAYLOR - Function re-written to have a more dynamic list of 
//                    acceptable file extensions.
////////////////////////////////////////////////////////////////////////////
BOOL    IsFileCompressed( LPSTR lpszFileExtension )
{
    // Create a list of the accepted file extensions
    char *pCompressionStrings[] = {
        "ARC", 
        "ARJ", 
        "BZ2", 
        "CAB", 
        "GZ", 
        "LHA",
        "LZH", 
        "MIM", 
        "MME", 
        "RAR",
        "TAR", 
        "TAZ", 
        "TGZ", 
        "UUE", 
        "Z", 
        "ZIP",
        NULL };

    // Check to see if this extension is the special case of ".??_"
    if( ( _tcslen(lpszFileExtension ) == 3 ) && 
        ( _tcsncmp("_", _tcsninc(lpszFileExtension, 2), 1) == 0 ) ) 
        return TRUE;

    // Loop through the accepted file extension looking for a match.
    int index = 0;
    while( pCompressionStrings[index] )
        {
        // If the extension matches with a case-insensitive compare return TRUE
        if( 0 == stricmp( pCompressionStrings[index], lpszFileExtension ) )
            return TRUE;
        
        index++;
        }

    // No match found, return FALSE
    return FALSE;
}


// ==== IsWinNT ==========================================================
// Determines if the OS is WindowsNT
////////////////////////////////////////////////////////////////////////////
// 5/15/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL IsWinNT()
{
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &os );
    return (BOOL)( os.dwPlatformId == VER_PLATFORM_WIN32_NT );
}


// ==== GetFileSecurityDesc ==========================================================
// Gets the security descriptor for a given file
// Input: 
//	 LPSTR lpszSourceFileName - file to get security attributes from
//   LPBYTE *pDesc			  - pointer to buffer pointer.  NOTE: This
//								function allocates this buffer.  It must be
//								freed by the calling routine.
////////////////////////////////////////////////////////////////////////////
// 5/12/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
HRESULT GetFileSecurityDesc( LPSTR lpszFileName, LPBYTE *pDesc )
{
    // Do nothing if not on NT.
    if( FALSE == IsWinNT() )
        {
        return S_OK;
        }

	// Do nothing if we have a UNC path
	if( strncmp("\\\\", lpszFileName, 2) == 0 )
		{
		return S_OK;
		}
        std::vector<WCHAR> vFileName;
	ConvertLongFileToUnicodeBuffer(lpszFileName, vFileName);
	

    // Get security attributes of original file.
    SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION |
                              GROUP_SECURITY_INFORMATION |
                              DACL_SECURITY_INFORMATION;

    // This call should fail, returning the length needed for the
    // security descriptor.
    DWORD dwLengthNeeded = 0;

    if (!GetFileSecurityW( &vFileName[0],
                     si,
                     (PSECURITY_DESCRIPTOR) *pDesc,
                     0,
                     &dwLengthNeeded ))
    {
        // ERROR_NOT_SUPPORTED will be returned when asking for
        // the security descriptor from a file on a non-NT system.
        // Lie about it and tell the caller ewverything is ok.
        if (GetLastError() == ERROR_NOT_SUPPORTED)
            return S_OK;
    }

    // Allocate space needed for call.
    *pDesc = new BYTE[dwLengthNeeded];
    if( *pDesc == NULL )
        {
        return E_OUTOFMEMORY;
        }

    // This should get the actual security descriptor.
    if( FALSE == GetFileSecurityW( &vFileName[0],
                     si,
                     (PSECURITY_DESCRIPTOR) *pDesc,
                     dwLengthNeeded,
                     &dwLengthNeeded ) )
        {
        return E_FAIL;
        }

    return S_OK;
}


// ==== SetFileSecurityDesc ==========================================================
// Set file security for a filesystem object
// Input: LPSTR szFileName - File to modify
//		  LPBYTE pDesc     - descriptor to set
////////////////////////////////////////////////////////////////////////////
// 5/12/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
HRESULT SetFileSecurityDesc( LPSTR lpszFileName, LPBYTE pDesc )
{
	// Do nothing if not on NT.
	if( FALSE == IsWinNT() )
		return S_OK;

	std::vector<WCHAR> vFileName;
	ConvertLongFileToUnicodeBuffer(lpszFileName, vFileName);

	if( pDesc != NULL )
	{
		SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION |
			GROUP_SECURITY_INFORMATION |
			DACL_SECURITY_INFORMATION;

		// Ignore return result.
		SetFileSecurityW( &vFileName[0], si, pDesc );
	}

	return S_OK;
}


// ==== TransferFile ==========================================================
// Append the source file from the current location onto the current location
// of the destination file. The function assumes valid handles, and correct
// permissions.
//
// Input: HANDLE hSource - Source file
//		  HANDLE hDest   - Destination file
////////////////////////////////////////////////////////////////////////////
// 5/17/98 JTAYLOR - Function created / Header added.
// 7/08/98 JTAYLOR - Updated transfer file to XOR the data by FF
////////////////////////////////////////////////////////////////////////////
HRESULT TransferFile( LPCTSTR lpszSourceName, HANDLE hSource, LPCTSTR lpszDestName, HANDLE hDest )
{
    LPBYTE              pFileTransferBuff = NULL;
    DWORD               dwBytesWritten;
    DWORD               dwBytesRead;
    BOOL                bResult = FALSE;
    HRESULT             hr = S_OK;

    // Allocate memory for the file transfer buffer
    pFileTransferBuff = new BYTE[QUARANTINE_TRANSFER_BUFFER_SIZE];
    if( NULL == pFileTransferBuff )
        {
        return E_OUTOFMEMORY;
        }

    // Zero out the buffer
    memset(pFileTransferBuff,0,QUARANTINE_TRANSFER_BUFFER_SIZE);

    do
        {
        // Attempt a synchronous read operation.
        bResult = ReadFile(hSource, pFileTransferBuff, QUARANTINE_TRANSFER_BUFFER_SIZE, &dwBytesRead, NULL);

        if( FALSE == bResult )
            {
            hr = E_FILE_READ_FAILED;
            break;
            }

        if( dwBytesRead > 0)
            {
            bResult = WriteFile(hDest, pFileTransferBuff, dwBytesRead, &dwBytesWritten, NULL);
            if( FALSE == bResult )
                {
                hr = E_FILE_WRITE_FAILED;
                break;
                }

            SYM_ASSERT(dwBytesRead == dwBytesWritten);
            }
        } while(dwBytesRead != 0);

    // Free the buffer
    if( NULL != pFileTransferBuff )
        {
        delete []pFileTransferBuff;
        pFileTransferBuff = NULL;
        }

    // Copy over any alternate data streams if running on NT.
    if( hr == S_OK && g_pfnCopyAlternateDataStreams )
        {
        hr = g_pfnCopyAlternateDataStreams( hSource, lpszSourceName, lpszDestName ) ?
                            S_OK : E_FILE_WRITE_FAILED;
        }

    return hr;
}


// ==== NumFilesInQuarantine =================================================
// Returns the number of files (ALL files) in the Quarantine directory.
//
// Output: An integer containing the number of Quarantine files,
//         or -1 if an error occurs.
////////////////////////////////////////////////////////////////////////////
// 6/4/98 SEDWARD - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
int NumFilesInQuarantine(void)
{
    auto    BOOL            bContinue = TRUE;
    auto    BOOL            bResult = TRUE;
    auto    int             numFiles = 0;
    auto    HANDLE          hFinder = NULL;
	auto    TCHAR           szQuarantinePath[MAX_QUARANTINE_FILENAME_LEN] = {0};
	auto    TCHAR           szQuarantinePathFilespec[MAX_QUARANTINE_FILENAME_LEN] = {0};
    auto    WIN32_FIND_DATA winFindData;

    // read the Quarantine path from the registry
    bResult = GetQuarantinePath(szQuarantinePath, MAX_QUARANTINE_FILENAME_LEN
                                                            , TARGET_QUARANTINE_PATH);
    if (FALSE == bResult)
        {
        numFiles = -1;
        goto  Exit_Function;
        }

    // append the "\*.*" to the path
    _tcscpy(szQuarantinePathFilespec, szQuarantinePath);
    NameAppendFile(szQuarantinePathFilespec, "*.*");

    // try to get a handle to a file (any file!)
    hFinder = FindFirstFile(szQuarantinePathFilespec, &winFindData);
    if (INVALID_HANDLE_VALUE==hFinder)
        {
        numFiles = -1;
        goto  Exit_Function;
        }

    // loop and count files
    while (TRUE == bContinue)
        {
        // if the file is not a directory...
        if (!(FILE_ATTRIBUTE_DIRECTORY & winFindData.dwFileAttributes))
            {
            // ... increment the counter
            ++numFiles;
            }

        // stop the loop the first time FindNextFile fails
        bContinue = FindNextFile(hFinder, &winFindData);
        }

    // close the handle
    FindClose(hFinder);


Exit_Function:

    return  numFiles;
}  // end of "NumFilesInQuarantine"


void ConvertLongFileToUnicodeBuffer( const char * szFileName, std::vector<WCHAR> &vUnicodeBuffer )
{
	// Compute length of buffer and reserve space for it
	int iBufferLength = strlen( szFileName ) + 10;
	vUnicodeBuffer.reserve( iBufferLength );

	// Construct extra long filename string.
	wcscpy( &vUnicodeBuffer[0], L"\\\\?\\" );
	if(AreFileApisANSI() == TRUE)
		MultiByteToWideChar( CP_ACP, 0, szFileName, -1, &vUnicodeBuffer[4], iBufferLength - 4 );
	else
		MultiByteToWideChar( CP_OEMCP, 0, szFileName, -1, &vUnicodeBuffer[4], iBufferLength - 4 );

}

BOOL SafeDeleteFile(LPTSTR szFileName)
{
	// Truncate the file and delete it.
	HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL,
		TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		COSInfo osi;
		if(osi.IsWinNT())
		{
			// Try again with unicode version for super long paths
            std::vector<WCHAR> vwFileName;
            ConvertLongFileToUnicodeBuffer(szFileName, vwFileName);

			hFile = CreateFileW(&vwFileName[0], GENERIC_WRITE, 0, NULL,
			TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

            if (hFile == INVALID_HANDLE_VALUE)
            {
                CCTRACEE(_T("SafeDeleteFile() - Failed to open handle to %s for the safe delete"), szFileName);
		        return FALSE;
            }

            // Close the handle to the file
            CloseHandle(hFile);

            // Delete the file using the unicode version
            return (DeleteFileW(&vwFileName[0]));
		}
        else
        {
            CCTRACEE(_T("SafeDeleteFile() - Failed to open handle to %s for the safe delete"), szFileName);
		    return FALSE;
        }
	}
    else
    {
        // Close the handle
        CloseHandle(hFile);

        // Delete the file
        return(DeleteFile(szFileName));
    }
}


// GPOLK - Jan 23., 2001
// This solves the problem of Norton Protected Recycle Bin backing up our
// deleted or restored files.
//
// Returns : TRUE - File was found and truncated OK.
//           FALSE - File not found or couldn't be truncated (no rights?)
BOOL TruncateFile(LPCTSTR lpcszFilePath)
{
    BOOL bReturn = FALSE;

	if((NULL == lpcszFilePath) || (0 == _tcslen(lpcszFilePath)))
		return bReturn;

    // Remove read-only file attribute
    DWORD dwFileAttr = GetFileAttributes(lpcszFilePath);
    if (INVALID_FILE_ATTRIBUTES != dwFileAttr)
    {
        if((dwFileAttr & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY)
            dwFileAttr -= FILE_ATTRIBUTE_READONLY;

        SetFileAttributes(lpcszFilePath, dwFileAttr);

        // Truncate the file to 0 bytes so the NProtect doesn't
        // put it in the protected Recycle Bin
        HANDLE hFile = 0;
        hFile = CreateFile(lpcszFilePath,       // pointer to name of the file
                            GENERIC_WRITE,      // access (read-write) mode
                            0,                  // share mode
                            NULL,               // pointer to security attributes
                            TRUNCATE_EXISTING,  // how to create
                            NULL,               // file attributes
                            NULL);

        if (hFile)
        {
            // Looks good
            bReturn = TRUE;
            CloseHandle (hFile);
			DeleteFile(lpcszFilePath);
        }
    }

    return bReturn;
} // END TruncateFile()


// Added for NAV 2005.5/r11.5
// File extension will be truncated to 3 characters, if it is greater
// Empty string will be returned if no extension
LPTSTR GetFileExtension(LPTSTR szSourceFileName, LPTSTR szExtension, bool bThreeCharacterExtension)
{
	_tcscpy(szExtension, _T(""));

	LPTSTR pszExt = _tcsrchr(szSourceFileName, _T('.'));
	if ((pszExt != NULL) && (pszExt+1 != NULL))
	{
		if(bThreeCharacterExtension)
			_tcsncpy(szExtension, pszExt+1, 3); // 3 character extension max, default
		else
			_tcscpy(szExtension, pszExt+1);
	}

	return szExtension;
}
