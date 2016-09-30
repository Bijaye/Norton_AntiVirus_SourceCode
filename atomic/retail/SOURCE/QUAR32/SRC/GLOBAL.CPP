////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/global.cpv   1.10   24 Jul 1998 12:21:08   tcashin  $
////////////////////////////////////////////////////////////////////////////
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
#include "quaradd.h"
#include "global.h"
#include "quaradd.h"
#include "xapi.h"
#include "winnetwk.h"
#include "navntutl.h"
#include "lm.h"

extern PFNCOPYALTERNATEDATASTREAMS g_pfnCopyAlternateDataStreams;
extern PFNNetWkstaGetInfo   g_pfnNetWkstaGetInfo;


////////////////////////////////////////////////////////////////////////////
// Function name    : GetDomainName
//
// Description      : Determines the domain name of the machine
//
// Return type      : BOOL
//
////////////////////////////////////////////////////////////////////////////
// 5/15/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL    GetDomainName( LPSTR lpszDomainName, DWORD nBuffSize )
{
    BOOL bRet = FALSE;

    //
    // For Win9x systems, we can peek at a registry key for this.
    //
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
    else if( g_pfnNetWkstaGetInfo )
        {
        //
        // This is ugly for NT machines.  We need to dynamically link to netapi32 in order
        // to call the function we need.  This code copied from the UNIUTIL project.
        //
        PWKSTA_INFO_100 pWkstaInfo;
        NET_API_STATUS  naStatus;
        BYTE            Buffer[ 1024 ];
        LPBYTE          pBuf = &Buffer[ 0 ];
        LPBYTE*         ppBuf = &pBuf;

        if ( lpszDomainName != NULL )
            {
            naStatus = g_pfnNetWkstaGetInfo( NULL, 100, ppBuf );

            if ( naStatus == 0 )
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
    

////////////////////////////////////////////////////////////////////////////
// Function name    : IsFileCompressed
//
// Description      : Determines if the file extension passed in matches
//                    one of the stored file extensions.  This is a case in-sensitive
//                    compare and will work for any extension length.
//
// Arguments        : lpszFileExtension - This is the extension to check
// Return type      : BOOL
//
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
    if( ( strlen( lpszFileExtension ) == 3 ) && 
        ( '_' == lpszFileExtension[2] ) )
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


////////////////////////////////////////////////////////////////////////////
// Function name    : IsWinNT
//
// Description      : Determines if the OS is WindowsNT
//
// Return type      : BOOL
//
////////////////////////////////////////////////////////////////////////////
// 5/15/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL IsWinNT( void )
{
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &os );
    return (BOOL)( os.dwPlatformId == VER_PLATFORM_WIN32_NT );
}

////////////////////////////////////////////////////////////////////////////
// Function name    : GetFileSecurityDesc
//
// Description	    : Gets the security descriptor for a given file
//
// Return type		: HRESULT
//
// Argument         : LPSTR lpszSourceFileName - file to get security attributes from
// Argument         : LPBYTE *pDesc - pointer to buffer pointer.  Note that this
//                    function allocates this buffer.  It must be freed by the
//                    calling routine.
//
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

    // Get security attributes of original file.
    SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION |
                              GROUP_SECURITY_INFORMATION |
                              DACL_SECURITY_INFORMATION;

    // This call should fail, returning the length needed for the
    // security descriptor.
    DWORD dwLengthNeeded = 0;

    if (!GetFileSecurity( lpszFileName,
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
    if( FALSE == GetFileSecurity( lpszFileName,
                     si,
                     (PSECURITY_DESCRIPTOR) *pDesc,
                     dwLengthNeeded,
                     &dwLengthNeeded ) )
        {
        return E_FAIL;
        }

    return S_OK;
}




////////////////////////////////////////////////////////////////////////////
// Function name    : SetFileSecurityDesc
//
// Description	    : Set file security for a filesystem object
//
// Return type		: HRESULT
//
// Argument         : LPSTR szFileName - File to modify
// Argument         : LPBYTE pDesc     - descriptor to set
//
////////////////////////////////////////////////////////////////////////////
// 5/12/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
HRESULT SetFileSecurityDesc( LPSTR lpszFileName, LPBYTE pDesc )
{
    // Do nothing if not on NT.
    if( FALSE == IsWinNT() )
        {
        return S_OK;
        }

    if( pDesc != NULL )
        {
        SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION |
                                  GROUP_SECURITY_INFORMATION |
                                  DACL_SECURITY_INFORMATION;

        // Ignore return result.
        SetFileSecurity( lpszFileName,
                         si,
                         pDesc );
        }

    return S_OK;
}


////////////////////////////////////////////////////////////////////////////
// Function name    : TransferFile
//
// Description      : Append the source file from the current location on,
//                    to the current location of the destination file.
//                    The function assumes valid handles, and correct permissions.
//
// Return type		: HRESULT
//
// Argument         : HANDLE hSource - Source file
// Argument         : HANDLE hDest - Destination file
//
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
            // XOR the bytes that are in memory
            for(unsigned int i = 0; i< dwBytesRead; i++ )
                {
                pFileTransferBuff[i] ^= 0xFF;
                }

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

    //
    // Copy over any alternate data streams if running on NT.
    //
    if( hr == S_OK && g_pfnCopyAlternateDataStreams )
        {
        hr = g_pfnCopyAlternateDataStreams( hSource, lpszSourceName, lpszDestName ) ?
                            S_OK : E_FILE_WRITE_FAILED;
        }

    return hr;
}



////////////////////////////////////////////////////////////////////////////
// Function name    : NumFilesInQuarantine
//
// Description      : This function returns the number of files (ALL files)
//                    in the Quarantine directory.
//
// Return type      : An integer containing the number of Quarantine files,
//                    or -1 if an error occurs.
//
////////////////////////////////////////////////////////////////////////////
// 6/4/98 SEDWARD - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
int NumFilesInQuarantine(void)
{
    auto    BOOL            bContinue = TRUE;
    auto    BOOL            bResult = TRUE;
    auto    int             numFiles = 0;
    auto    HANDLE          hFinder = NULL;
    auto    TCHAR           szQuarantinePath[MAX_QUARANTINE_FILENAME_LEN];
    auto    TCHAR           szQuarantinePathFilespec[MAX_QUARANTINE_FILENAME_LEN];
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

    return  (numFiles);

}  // end of "NumFilesInQuarantine"



////////////////////////////////////////////////////////////////////////////
// Function name    : QuarantineGetConfigInfo
//
// Description      : This function is used to set the Bloodhound level in
//                    Quarantine.  It is currently always set to the highest
//                    level available: 3.  This is the setting used for all
//                    scans in Quarantine.
//
////////////////////////////////////////////////////////////////////////////
// 6/22/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
VOID WINAPI QuarantineGetConfigInfo
(
    LPTSTR lpszID,
    LPVOID lpvResult,
    UINT   uMaxResultLen,
    LPBOOL lpbImplemented,
    LPVOID lpvCookie
)
{
#if defined(SYM_DOSX) || defined (SYM_WIN32)

    char szLevel[2] = "3";

    *lpbImplemented = TRUE;

    strncpy((LPSTR)lpvResult, szLevel, uMaxResultLen);

#else

    *lpbImplemented = FALSE;

#endif
}

