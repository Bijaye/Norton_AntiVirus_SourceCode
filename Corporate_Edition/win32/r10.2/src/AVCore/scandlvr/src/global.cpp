// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/global.cpv   1.10   14 Jul 1998 16:24:12   mdunn  $
/////////////////////////////////////////////////////////////////////////////
//
// Global.cpp - contains global functions for the Scan and Deliver dll.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/global.cpv  $
//
//    Rev 1.10   14 Jul 1998 16:24:12   mdunn
// Added #include <initguid.h> to get the trialware GUIDs properly defined.
//
//    Rev 1.9   18 Jun 1998 21:27:48   SEDWARD
// Added 'DecodeString'.
//
//    Rev 1.8   11 Jun 1998 03:22:24   jtaylor
// reenabled the trialware.
//
//    Rev 1.7   11 Jun 1998 02:02:48   jtaylor
// Added support for dissabling Scan and Deliver when a trial copy of NAV expires.
//
//    Rev 1.6   08 Jun 1998 16:19:08   SEDWARD
// Added GetMainNavDir, GetClickedItemIndex and ToggleListItemCheckbox.
//
//    Rev 1.5   05 Jun 1998 22:00:34   SEDWARD
// We now derive a fully-qualified path to the "country.dat" file.
//
//    Rev 1.4   03 Jun 1998 14:22:30   jtaylor
// Removed resource.h include file.
//
//    Rev 1.3   01 Jun 1998 17:04:42   SEDWARD
// Added 'InitCountryListbox'.
//
//    Rev 1.2   26 May 1998 13:00:26   SEDWARD
// Added 'MyCharNext' and InitDbcsVars
//
//    Rev 1.1   23 Apr 1998 16:47:16   jtaylor
// added command to close the registry key.
//
//    Rev 1.0   16 Apr 1998 23:37:26   JTaylor
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "ScanDlvr.h"
#include "ScanDeliverDLL.h"
#include "xapi.h"
#include "SymSaferRegistry.h"
#include <new>
#include "vpstrutils.h"

// Included to support checking for trial install.
/*
#include "navver.h"
#ifndef INITGUID
    #define  INITGUID
    #include <initguid.h>
    #include "tryint.h"
    #undef INITGUID
#endif
*/

BOOL        g_bIsDBCS;
BYTE*       g_arLeadByteRange;
CPINFO      g_codePageInfo;
TCHAR       g_szMainNavDir[ MAX_QUARANTINE_FILENAME_LEN + 1 ] = { 0 };


////////////////////////////////////////////////////////////////////////////
// Function name    : IsTrialInstallAndExpired()
//
// Description      : This function checks to determine if this install of NAV
//                    is a trial copy and then checks to see if the trial period
//                    has expired.
//
// Return type      :  True iff and only if the install is a trial copy that
//                        has already expired.
//                     False otherwise.
//
// This core logic of this function was taken from NAVW::navw.cpp
//
////////////////////////////////////////////////////////////////////////////
// 6/11/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL IsTrialInstallAndExpired(void)
{

    // This function will now always return false.  Scan and Deliver
    // is called from applications that have the trialware code, so
    // if the application can't load, Scan and Deliver will never be loaded.
    // Because this is now a shared component in AVCore, if this functionality
    // is re-enabled, handling the product registry keys would need to be worked out.

    return (false);

/*    auto    ITrialWare  *pTrial;
    auto    BOOL        bTrialInstall = TRUE;
    auto    BOOL        bExpired = TRUE;
    auto    char        szOutBuffer[MAX_PATH];
    auto    DWORD       nBufSize = MAX_PATH;
    auto    BOOL        bRetVal = FALSE;
    auto    long        lResult;
    auto    HKEY        hKey;
    auto    DWORD       regValueType;

    // Check to see if the trialware key is in the registry.
    // open the registry key
    auto    CString         szNavInstallKey;
 */
/*
    auto    OSVERSIONINFO   stVerInfo;

    // initialize the OS info structure
    stVerInfo.dwOSVersionInfoSize = sizeof(stVerInfo);

    // determine whether we're running on Win95 or NT and fetch the location of
    // the main NAV install registry key
    GetVersionEx(&stVerInfo);
        szNavInstallKey = REGKEY_NAV_MAIN;
    szNavInstallKey = "software\\symantec\\symantec antivirus";

    if (VER_PLATFORM_WIN32_NT == stVerInfo.dwPlatformId)
        {
        szNavInstallKey = REGKEY_NAV_MAIN_NT;
        }
    else // 95
        {
        szNavInstallKey = REGKEY_NAV_MAIN;
        }
  */
  /*
      szNavInstallKey = REGKEY_NAV_MAIN;

    // Construct the rest of the path to the install key
    szNavInstallKey = szNavInstallKey + "\\" + NAV_INSTALLKEY_VER;

    lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, szNavInstallKey, 0, KEY_READ, &hKey);
    if (ERROR_SUCCESS != lResult)
        {
        bTrialInstall = FALSE;
        }
    else
        {
        lResult = SymSaferRegQueryValueEx( hKey, REGVAL_NAV_TRIAL, 0, &regValueType, (LPBYTE)szOutBuffer, &nBufSize );
        if (ERROR_SUCCESS != lResult)
            {
            bTrialInstall = FALSE;
            }

        // close the registry key - ignore return value since there is nothing we can do
        lResult = RegCloseKey( hKey );
        }

    if( FALSE == bTrialInstall )
        {
        return FALSE;
        }

    // Now we know that we are a trial install.
    // attempt to create trial object
    if ( SUCCEEDED( CoCreateInstance( CLSID_TrialWare,
                      NULL,
                      CLSCTX_INPROC_SERVER,
                      IID_ITrialWare,
                      (LPVOID *)&pTrial ) ) )
        {

        // attempt to initialize trial object
        if ( SUCCEEDED( pTrial->Initialize( TEXT( "Symantec" ),
                            NAV_PRODUCT_NAME,
                            NAV_VERSION_MAJOR,
                            NAV_VERSION_MINOR ) ) )
            {
            if ( pTrial->IsValid( HWND_DESKTOP, TRUE, NULL ) == S_OK )
                {
                bExpired = FALSE;
                }
            }

        pTrial->Release();
        }

    return ( bExpired );
*/

}

////////////////////////////////////////////////////////////////////////////
// Function name    : GetPathFromRegKey()
//
// Description      : This function reads a path in from the registry
//
// Arguments        :  HKEY   hInputKey     HKEY_LOCAL_MACHINE ...
//                     LPTSTR lpszKeyName   the name of the key
//                     LPTSTR lpszValueName The name of the value
//                     LPTSTR lpszOutBuf    a String buffer for the output
//                     DWORD  nBufSize      the sixe of the output buffer.
//
// Return type      :  True for success, false else.
//
////////////////////////////////////////////////////////////////////////////
// 4/16/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL GetPathFromRegKey(HKEY            hInputKey,
                       LPTSTR          lpszKeyName,
                       LPTSTR          lpszValueName,
                       LPTSTR          lpszOutBuf,
                       DWORD           nBufSize)
    {
    auto    BOOL        bRetVal = FALSE;
    auto    long        lResult;
    auto    HKEY        hKey;

    // Perform basic type checking
    if( ( NULL == lpszKeyName ) ||
        ( NULL == lpszValueName ) ||
        ( NULL == lpszOutBuf ) )
        {
        goto  Bail_Function;
        }

    // open the registry key
    lResult = RegOpenKeyEx( hInputKey, lpszKeyName, 0, KEY_READ, &hKey);
    if (ERROR_SUCCESS != lResult)
        {
        goto  Bail_Function;
        }

    // get the desired path value
    auto    DWORD       regValueType;
    lResult = SymSaferRegQueryValueEx( hKey, lpszValueName, 0, &regValueType, (LPBYTE)lpszOutBuf, &nBufSize );
    if ((ERROR_SUCCESS != lResult)  ||  (REG_SZ != regValueType))
        {
        goto  Bail_Function;
        }

    // close the registry key
    lResult = RegCloseKey( hKey );
    if (ERROR_SUCCESS != lResult)
        {
        goto  Bail_Function;
        }


    bRetVal = TRUE;

Bail_Function:
        return (bRetVal);
}



// ==== MyCharNext ========================================================
//
//  This function returns a pointer to the next character in the input
//  argument.  It will work for either SBCS or DBCS character sets.
//
//  NOTE: This function is copied from Nadine Kano's book "Developing
//        International Software for Windows95 and WindowsNT", Chapter 3.
//
//  Input:
//      pszStr  -- a pointer to a null-terminated character string
//
//  Output:
//      a pointer to the next character in the string
//
// ========================================================================
//  Function created: 5/98, SEDWARD
// ========================================================================

char*   MyCharNext(char*  pszStr)
{
    auto    BYTE        bRange = 0;

    // Check to see whether *pszStr is  a lead byte.  The constant 12 allows for
    // up to 6 pairs of lead-byte range values.
    while ((bRange < 12)  &&  (NULL != g_arLeadByteRange[bRange]))
        {
        if  ((*pszStr >= g_arLeadByteRange[bRange])
        &&  (*pszStr <= g_arLeadByteRange[bRange+1]))
            {
            return (pszStr + 2);    // skip two bytes
            }

        bRange += 2;    // go to the next pair of range values
        }

    return (pszStr + 1);    // skip one byte

}  // end of "MyCharNext"



// ==== InitDbcsVars ======================================================
//
//  This function initializes some global variables that are helpful when
//  working with DBCS characters.
//
//  NOTE: This function is copied from Nadine Kano's book "Developing
//        International Software for Windows95 and WindowsNT", Chapter 3.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 5/98, SEDWARD
// ========================================================================

void    InitDbcsVars(void)
{
    // initialize some variables so we know if we're running in a DBCS edition
    // of Windows
    if (GetCPInfo(CP_ACP, &g_codePageInfo))
        {
        // save a pointer to the lead byte array
        g_arLeadByteRange = g_codePageInfo.LeadByte;

        // is the max length in bytes of a character in this code page more than 1?
        g_bIsDBCS = (g_codePageInfo.MaxCharSize > 1);
        }

}  // end of "InitDbcsVars"



// ==== GetMainNavDir =====================================================
//
//  This function gets the path to the main NAV directory.
//
//  Input:
//      szDirBuf    -- a buffer to hold the NAV path
//
//  Output:
//      a value of TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 6/98, SEDWARD
// ========================================================================

BOOL    GetMainNavDir(TCHAR*  szDirBuf, DWORD nNumDirBufBytes)
{
    auto    BOOL            bResult = FALSE;
    auto    OSVERSIONINFO   stVerInfo;


    //
    // Check to see if the global NAV directory has been set.
    //
    if( g_szMainNavDir[0] != _T('\0') )
    {
        vpstrncpy( szDirBuf, g_szMainNavDir, nNumDirBufBytes );
        return TRUE;
    }

    // initialize the OS info structure
    stVerInfo.dwOSVersionInfoSize = sizeof(stVerInfo);

    // determine whether we're running on Win95 or NT and fetch the location of
    // the main NAV directory
    GetVersionEx(&stVerInfo);
    if (VER_PLATFORM_WIN32_NT == stVerInfo.dwPlatformId)
    {
        bResult = GetPathFromRegKey(HKEY_LOCAL_MACHINE, REGKEY_NAV_INSTALL_PATH
                                                        , REGVAL_NAV_INSTALL_PATHNT
                                                        , g_szMainNavDir
                                                        , MAX_QUARANTINE_FILENAME_LEN);
    }
    else // 95
    {
        bResult = GetPathFromRegKey(HKEY_LOCAL_MACHINE, REGKEY_NAV_INSTALL_PATH
                                                        , REGVAL_NAV_INSTALL_PATH95
                                                        , g_szMainNavDir
                                                        , MAX_QUARANTINE_FILENAME_LEN);
    }

    if (bResult)
	    vpstrncpy( szDirBuf, g_szMainNavDir, nNumDirBufBytes );

    return  (bResult);

}  // end of "GetMainNavDir"



// ==== InitCountryListbox ================================================
//
//  This function populates a CComboBox control with the contents of the
//  "countries" data file.
//
//  Input:
//      countryComboBox     -- a reference to a CComboBox object
//
//  Output:
//      the number of items inserted into the combo box control; if an
//      error occurs, -1 is returned
//
// ========================================================================
//  Function created: 5/98, SEDWARD
// ========================================================================

int InitCountryListbox(CComboBox&  countryComboBox)
{
    auto    CStdioFile      countryFile;
    auto    CString         szFileData;
    auto    DWORD           dwItemData = 0;
    auto    int             nIndex = 0;
    auto    TCHAR           szCountryDataFile[MAX_QUARANTINE_FILENAME_LEN] = "";


    // get the location of the main NAV directory
    if (FALSE == GetMainNavDir(szCountryDataFile, sizeof (szCountryDataFile)))
        {
        nIndex = -1;
        goto  Exit_Function;
        }

    // derive a fully-qualified path to the country data file and open it
    AppendFileName(szCountryDataFile, COUNTRY_DAT_FILENAME);
    if (FALSE == countryFile.Open(szCountryDataFile, CFile::modeRead))
        {
        nIndex = -1;
        goto  Exit_Function;
        }

    // clear out anything that may be in the combo box
    countryComboBox.ResetContent();

    // first preallocate some memory so initialization will be faster
    if (CB_ERR == countryComboBox.InitStorage(250, 75)) // num items, bytes for each item
        {
        nIndex = -1;
        goto  Exit_Function;
        }

    // loop and populate the combo box with items
    while (TRUE)
        {
        // get the country string
        if (FALSE == countryFile.ReadString(szFileData))
            {
            break;
            }
        countryComboBox.InsertString(nIndex, szFileData);

        // get the country's corresponding data
        if (FALSE == countryFile.ReadString(szFileData))
            {
            break;
            }

        // associate the data value with the current item
        dwItemData = atol(szFileData);
        countryComboBox.SetItemData(nIndex, dwItemData);

        // increment the current index
        ++nIndex;
        }


Exit_Function:

    return (nIndex);

}  // end of "InitCountryListbox"



// ===== GetClickedItemIndex ==============================================
//
//  This function
//
//
//
//
//  Input:  nothing
//  Output: a value of TRUE if an item's checkbox was hit, FALSE otherwise.
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    GetClickedItemIndex(CListCtrl&  listCtrl, int&  itemIndex)
{
    auto    BOOL        iconClicked = TRUE;
    auto    UINT        flags = 0;
    auto    POINT       pointScreen;
    auto    POINT       pointLVClient;
    auto    DWORD       dwPos;

    // get the screen coordinates
    dwPos = GetMessagePos();
    pointScreen.x = LOWORD (dwPos);
    pointScreen.y = HIWORD (dwPos);
    pointLVClient = pointScreen;

    // convert the point from screen to client coordinates,
    // relative to this control
    listCtrl.ScreenToClient(&pointLVClient);

    // update this object's member variable
    itemIndex = listCtrl.HitTest(pointLVClient, &flags);
    if (-1 != itemIndex)
        {
        // if the click did not occur on the checkbox, return FALSE
        if (0 == (flags & LVHT_ONITEMICON))
            {
            iconClicked = FALSE;
            }
        }

    return (iconClicked);

}  // end of "GetClickedItemIndex"



// ===== ToggleListItemCheckbox ===========================================
//
//  This function toggles the display of a listbox item's checkbox.
//
//  Input:
//      listCtrl    -- a reference to the host list control object
//      itemIndex   -- the list control item to toggle
//
//  Output: the new state of the checkbox (TRUE if it's checked, FALSE
//          if it's not)
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    ToggleListItemCheckbox(CListCtrl&  listCtrl, int  itemIndex)
{
    auto    BOOL            isChecked;
    auto    LV_ITEM         listItemStruct;

    memset((void*)&listItemStruct, 0, sizeof(LV_ITEM));
    listItemStruct.iItem = itemIndex;
    listItemStruct.mask = LVIF_IMAGE;

    if (0 != listCtrl.GetItem(&listItemStruct))
        {
        if (SD_CHECKED_BITMAP == listItemStruct.iImage)
            {
            listItemStruct.iImage = SD_UNCHECKED_BITMAP;
            isChecked = FALSE;
            }
        else if (SD_UNCHECKED_BITMAP == listItemStruct.iImage)
            {
            listItemStruct.iImage = SD_CHECKED_BITMAP;
            isChecked = TRUE;
            }
        }

    listCtrl.SetItem(&listItemStruct);
    return (isChecked);

}   // end of "ToggleListItemCheckbox"



// ==== DecodeString ======================================================
//
//  This function decodes the input string, using SARC_MUNGE_VALUE to
//  perform an XOR on each character.
//
//  Input:
//      szStrData   -- a NULL-terminated string
//
//  Output:
//      The number of non-NULL bytes that have been "unmunged".
//
// ========================================================================
//  Function created: 6/98, SEDWARD
// ========================================================================

DWORD   DecodeString(char*  szStrData)
{
    auto    char*       charPtr;
    auto    DWORD       dwNumCharsDecoded;

    charPtr = szStrData;
    dwNumCharsDecoded = 0;
    while (NULL != *charPtr)
        {
        *charPtr ^= SARC_MUNGE_VALUE;
        ++charPtr;
        ++dwNumCharsDecoded;
        }

    return (dwNumCharsDecoded);

}  // end of "DecodeString"



///////////////////////////////////////////////////////////////////////////////
//
// Function name: AppendFileName
//
// Description  : This routine will append the specified filename to the specified
//                path.
//
// Return type  : UINT
//
// Argument     : LPTSTR szFullPath
// Argument     : LPTSTR szFileName
//
///////////////////////////////////////////////////////////////////////////////
// 3/22/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
void AppendFileName( LPTSTR szFullPath, LPTSTR szFileName )
{
	ASSERT(AfxIsValidString(szFullPath));

    //
    // Build full path name.
    //
	CString sFullPath( szFullPath );

	//
    // Q: any real work for us to do?
    //
	if( szFileName[0] == 0 )
		{
        lstrcpy( szFullPath, sFullPath );
        return;
		}

    //
    // Remove trailing backslashes.
    //
    sFullPath.TrimRight( _T("\\") );

    //
    // Append filename.
    //
    sFullPath += _T("\\");
    sFullPath += szFileName;

    lstrcpy( szFullPath, sFullPath );
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CFileNameParse::GetFileExtension
//
// Description   :
//
// Return type   : UINT
//
// Argument      : LPCTSTR lpszPathName
// Argument      : LPTSTR lpszExt
// Argument      : UINT nMax
//
///////////////////////////////////////////////////////////////////////////////
// 2/10/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
UINT GetFileExtension( LPCTSTR lpszPathName, LPTSTR lpszExt, UINT nMax)
{
    ASSERT(AfxIsValidString(lpszPathName));

    ZeroMemory( lpszExt, nMax * sizeof( TCHAR ) );

    // always capture the complete file name including extension (if present)
    LPTSTR lpszTemp = NULL;
    for (LPCTSTR lpsz = lpszPathName; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
    {
        // remember last period
        if (*lpsz == '.' )
            lpszTemp = (LPTSTR)_tcsinc(lpsz);
    }

    if( lpszTemp )
        {
        if( lpszExt == NULL )
            return _tcslen( lpszTemp ) + 1;

        _tcsncpy( lpszExt, lpszTemp, nMax );
        }

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   WipeOutFile
//
// Description  :
//
// Return type  : BOOL
//
// Argument     : LPCTSTR lpszFileName
//
///////////////////////////////////////////////////////////////////////////////
// 3/22/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
BOOL WipeOutFile( LPCTSTR lpszFileName )
{
    BOOL bRet = TRUE;
    LPBYTE pBuffer = NULL;
    DWORD dwFileSize;
    DWORD dwBytesToWrite;

    try
        {
        //
        // Open the file for write access.
        //
        CFile file( lpszFileName, CFile::modeWrite | CFile::shareExclusive );

        //
        // Allocate Zeroed buffer for transfers.
        //
        pBuffer = new BYTE[ 0xFFFF ];
        ZeroMemory( pBuffer, 0xFFFF );

        // 
        // Save off length of file.
        // 
        dwFileSize = file.GetLength();

        // 
        // Nuke the contents of this file.
        // 
        while( dwFileSize )
            {
            dwBytesToWrite = dwFileSize > 0xFFFF ? 0xFFFF : dwFileSize;
            file.Write( pBuffer, dwBytesToWrite );
            dwFileSize -= dwBytesToWrite;
            }
        
        // 
        // Set length of file to 0 bytes.
        // 
        file.SetLength(0);

        // 
        // Finally, close and delete the file.
        // 
        file.Close();
        CFile::Remove( lpszFileName );
        }
    catch( CFileException* e )
        {
        bRet = FALSE;
        }
    catch( CMemoryException* e )
        {
        bRet = FALSE;
        }
	catch(std::bad_alloc &)
		{
			bRet = FALSE;
		}

    if( pBuffer )
        delete [] pBuffer;

    // 
    // Return results.
    // 
    return bRet;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CreateUniqueFile
//
// Description      : Creates a new file with a unique filename and the
//                    extension passed in.  The file is created in the
//                    TargetPath and the name is assigned to lpszFileName
//                    and a handle to the open file is returned.
//
// Return type      : HANDLE -- Handle of newly opened file.
//                      This value will be INVALID_HANDLE_VALUE on failure.
//
// Argument         :  LPTSTR lpszTargetPath -- Full path for the new file
//                     LPTSTR lpszExtension  -- Extension of new file 3 characters or less
//                     LPTSTR lpszFileName   -- [out] full path a and filename of the new file.
//
////////////////////////////////////////////////////////////////////////////
// 2/26/98 JTAYLOR - Function created / Header added.
// 4/19/98 JTAYLOR - Enhanced file already exists error check.
////////////////////////////////////////////////////////////////////////////
HANDLE SNDCreateUniqueFile(LPTSTR          lpszTargetPath,
                           LPTSTR          lpszExtension,
                           LPTSTR          lpszFileName)
{
    auto int            nPathLength;
    auto HANDLE         hFile = INVALID_HANDLE_VALUE;
    auto TCHAR          szGeneratedFileName[MAX_QUARANTINE_FILENAME_LEN];

    // Copy the destination path and make sure there is a slash on the end.
    nPathLength = lstrlen(lpszTargetPath);

    // seed the random number generator
    srand( (unsigned)time( NULL ) );

    while (INVALID_HANDLE_VALUE == hFile)
        {
        //Genereate a random number for the filename
        DWORD dwNumber = MAKELONG(rand(),rand());
        // Make sure that a DWORD stays defined as 32 bits.
        sssnprintf( szGeneratedFileName, sizeof(szGeneratedFileName), _T("%.8X."), dwNumber & 0xFFFFFFFF );

        // Make sure the extension is will fit in the filename
        if( _tcslen( lpszExtension ) > MAX_QUARANTINE_FILENAME_LEN - 9 )
            {
            goto Exit_Function;
            }

        // Append the extension to the filename after the '.'
        lstrcpy( &szGeneratedFileName[9], lpszExtension );

        // Copy the path to the beginning of the output buffer
        lstrcpy(lpszFileName, lpszTargetPath);

        // Append the filename to the path.
        AppendFileName( lpszFileName, szGeneratedFileName );

        // Attempt to create the new file.
        hFile = CreateFile(lpszFileName,
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
            // Find out why the file could not be created.
            DWORD dwLastError = GetLastError();

            // Loop again if the error was that the file already exists.
            // ERROR_ALREADY_EXISTS is what the API claimed is returned if the
            // file already exists, ERROR_FILE_EXISTS is what is actually
            // returned on my computer.  I have included them both, in case one
            // is pre IE 4.0.
            if( ( ERROR_ALREADY_EXISTS != dwLastError ) &&
                ( ERROR_FILE_EXISTS != dwLastError ) )
                {
                goto Exit_Function;
                }
            }
        else
            {
            return (hFile);
            }
        }

Exit_Function:
    return INVALID_HANDLE_VALUE;
}

