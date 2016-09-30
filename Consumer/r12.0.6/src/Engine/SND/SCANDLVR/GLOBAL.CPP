//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
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
#include "shlwapi.h"

BOOL        g_bIsDBCS;
BYTE*       g_arLeadByteRange;
CPINFO      g_codePageInfo;


// Reads a path in from the registry
// Arguments: HKEY   hInputKey     HKEY_LOCAL_MACHINE
//            LPTSTR lpszKeyName   the name of the key
//            LPTSTR lpszValueName The name of the value
//            LPTSTR lpszOutBuf    String buffer for the output
//            DWORD  nBufSize      the size of the output buffer.
// Return: True for success, false else.
// 4/16/98 JTAYLOR - Function created / Header added.
BOOL GetPathFromRegKey(HKEY   hInputKey,
					   LPTSTR lpszKeyName,
					   LPTSTR lpszValueName,
					   LPTSTR lpszOutBuf,
					   DWORD  nBufSize)
{
	BOOL bRetVal = FALSE;
	long lResult;
	HKEY hKey;

	// Perform basic type checking
	if((NULL == lpszKeyName) || (NULL == lpszValueName) || (NULL == lpszOutBuf))
	{
		goto  Bail_Function;
	}

	// open the registry key
	lResult = RegOpenKeyEx(hInputKey, lpszKeyName, 0, KEY_READ, &hKey);
	if(ERROR_SUCCESS != lResult)
	{
		goto  Bail_Function;
	}

	// get the desired path value
	DWORD regValueType;
	lResult = RegQueryValueEx(hKey, lpszValueName, 0, &regValueType, (LPBYTE)lpszOutBuf, &nBufSize);
	if((ERROR_SUCCESS != lResult)  ||  (REG_SZ != regValueType))
	{
		goto  Bail_Function;
	}

	// close the registry key
	lResult = RegCloseKey(hKey);
	if(ERROR_SUCCESS != lResult)
	{
		goto  Bail_Function;
	}

	bRetVal = TRUE;

Bail_Function:
	return (bRetVal);
}


// Returns a pointer to the next character in the input argument.  It will 
// work for either SBCS or DBCS character sets.
// NOTE: This function is copied from Nadine Kano's book "Developing
//   International Software for Windows95 and WindowsNT", Chapter 3.
// Input: pszStr - a pointer to a null-terminated character string
// Output: a pointer to the next character in the string
//  Function created: 5/98, SEDWARD
char* MyCharNext(char* pszStr)
{
	BYTE bRange = 0;

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


// Initializes some global variables that are helpful when working with DBCS 
// characters.
// NOTE: This function is copied from Nadine Kano's book "Developing
//   International Software for Windows95 and WindowsNT", Chapter 3.
//  Function created: 5/98, SEDWARD
void InitDbcsVars(void)
{
	// initialize some variables so we know if we're running in a DBCS edition
	// of Windows
	if(GetCPInfo(CP_ACP, &g_codePageInfo))
	{
		// save a pointer to the lead byte array
		g_arLeadByteRange = g_codePageInfo.LeadByte;

		// is the max length in bytes of a character in this code page more than 1?
		g_bIsDBCS = (g_codePageInfo.MaxCharSize > 1);
	}
}  // end of "InitDbcsVars"


// Gets the path to the main NAV directory.
// Input: szDirBuf - a buffer to hold the NAV path
// Output: TRUE if all goes well, FALSE if not
//  Function created: 6/98, SEDWARD
BOOL GetMainNavDir(TCHAR*  szDirBuf)
{
	return  GetPathFromRegKey(HKEY_LOCAL_MACHINE, REGKEY_NAV_INSTALL_PATH
		, REGVAL_NAV_INSTALL_PATH_VALUE
		, szDirBuf
		, MAX_QUARANTINE_FILENAME_LEN);
}


// Populates a CComboBox control with the contents of the "countries" data file
// Input: countryComboBox - reference to a CComboBox object
// Output: the number of items inserted into the combo box control; 
//		   if an error occurs, -1 is returned
// Function created: 5/98, SEDWARD
int InitCountryListbox(CComboBox& countryComboBox)
{
	int nIndex = 0;
	CStdioFile countryFile;
	
	// get the location of the main NAV directory
	TCHAR szCountryDataFile[MAX_QUARANTINE_FILENAME_LEN] = "";
	if(FALSE == GetMainNavDir(szCountryDataFile))
	{
		nIndex = -1;
		goto Exit_Function;
	}

	// derive a fully-qualified path to the country data file and open it
	PathAppend(szCountryDataFile, COUNTRY_DAT_FILENAME);
	if(FALSE == countryFile.Open(szCountryDataFile, CFile::modeRead))
	{
		nIndex = -1;
		goto Exit_Function;
	}

	// clear out anything that may be in the combo box
	countryComboBox.ResetContent();

	// first preallocate some memory so initialization will be faster
	if(CB_ERR == countryComboBox.InitStorage(250, 75)) // num items, bytes for each item
	{
		nIndex = -1;
		goto Exit_Function;
	}

	// loop and populate the combo box with items
	while(TRUE)
	{
		// get the country string
		CString sDataRow;
		if(FALSE == countryFile.ReadString(sDataRow))
		{
			break;
		}

		sDataRow.Trim();

		// Parse row of data
		// Row format:  Localized country name; Country code
		CString sTokens(_T(";"));
		int iCurrentPosition = 0;
		CString sLocalizedCountryName = sDataRow.Tokenize(sTokens, iCurrentPosition);
		sLocalizedCountryName.Trim();
		if(sLocalizedCountryName.IsEmpty())
		{
			CCTRACEE(_T("%s - Could not get localized country name"), __FUNCTION__);
			break;
		}

		CString sCountryCode = sDataRow.Tokenize(sTokens, iCurrentPosition);
		sCountryCode.Trim();
		if(sCountryCode.IsEmpty())
		{
			CCTRACEE(_T("%s - Could not get country code"), __FUNCTION__);
			break;
		}

		countryComboBox.InsertString(nIndex, sLocalizedCountryName);

        // associate the data value with the current item
        DWORD dwCountryCode = atol(sCountryCode);
		countryComboBox.SetItemData(nIndex, dwCountryCode);

		// increment the current index
		nIndex++;
	}

Exit_Function:
	return nIndex;
}  // end of "InitCountryListbox"


//  Output: a value of TRUE if an item's checkbox was hit, FALSE otherwise.
//  Function created: 11/96, SEDWARD
BOOL GetClickedItemIndex(CListCtrl&  listCtrl, int&  itemIndex)
{
	BOOL  iconClicked = TRUE;
	UINT  flags = 0;
	POINT pointScreen;
	POINT pointLVClient;
	DWORD dwPos;

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
	if(-1 != itemIndex)
	{
		// if the click did not occur on the checkbox, return FALSE
		if(0 == (flags & LVHT_ONITEMICON))
		{
			iconClicked = FALSE;
		}
	}

	return (iconClicked);
}  // end of "GetClickedItemIndex"


// Toggles the display of a listbox item's checkbox.
// Input: listCtrl  - a reference to the host list control object
//        itemIndex - the list control item to toggle
// Output: the new state of the checkbox (TRUE if checked, FALSE if not)
// Function created: 11/96, SEDWARD
BOOL ToggleListItemCheckbox(CListCtrl&  listCtrl, int  itemIndex)
{
	BOOL    isChecked;
	LV_ITEM listItemStruct;

	memset((void*)&listItemStruct, 0, sizeof(LV_ITEM));
	listItemStruct.iItem = itemIndex;
	listItemStruct.mask = LVIF_IMAGE;

	if(0 != listCtrl.GetItem(&listItemStruct))
	{
		if(SD_CHECKED_BITMAP == listItemStruct.iImage)
		{
			listItemStruct.iImage = SD_UNCHECKED_BITMAP;
			isChecked = FALSE;
		}
		else if(SD_UNCHECKED_BITMAP == listItemStruct.iImage)
		{
			listItemStruct.iImage = SD_CHECKED_BITMAP;
			isChecked = TRUE;
		}
	}

	listCtrl.SetItem(&listItemStruct);
	return (isChecked);
}   // end of "ToggleListItemCheckbox"


// Decodes the input string, using SARC_MUNGE_VALUE to perform an XOR on each 
// character.
// Input: szStrData - a NULL-terminated string
// Output: The number of non-NULL bytes that have been "unmunged".
//  Function created: 6/98, SEDWARD
DWORD   DecodeString(char*  szStrData)
{
	char* charPtr;
	DWORD dwNumCharsDecoded;

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


// FUNCTION TAKEN FROM QUARADD TO REMOVE DEPENDANCY ON THE LIB (NOW OWNED BY CC)

//////////////////////////////////////////////////////////////////////////////
// Creates a new file with a unique filename and the extension passed in.  
// The file is created in the TargetPath and the name is assigned to 
// lpszFileName and a handle to the open file is returned.
// Return: HANDLE - Handle of newly opened file.
//         This value will be INVALID_HANDLE_VALUE on failure.
// Arguments: LPTSTR lpszTargetPath -- Full path for the new file
//            LPTSTR lpszExtension  -- Extension of new file 3 characters or less
//            LPTSTR lpszFileName   -- [out] full path a and filename of the new file.
////////////////////////////////////////////////////////////////////////////
// 2/26/98 JTAYLOR - Function created / Header added.
// 4/19/98 JTAYLOR - Enhanced file already exists error check.
////////////////////////////////////////////////////////////////////////////
HANDLE CreateUniqueFile(LPTSTR lpszTargetPath,
						LPTSTR lpszExtension,
						LPTSTR lpszFileName)
{
	int    nPathLength;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	TCHAR  szGeneratedFileName[MAX_QUARANTINE_FILENAME_LEN];

	// Copy the destination path and make sure there is a slash on the end.
	nPathLength = STRLEN(lpszTargetPath);

	// seed the random number generator
	srand((unsigned)time(NULL));

	while (INVALID_HANDLE_VALUE == hFile)
	{
		//Genereate a random number for the filename
		DWORD dwNumber = MAKELONG(rand(),rand());
		// Make sure that a DWORD stays defined as 32 bits.
		SPRINTF(szGeneratedFileName, _T("%.8X."), dwNumber & 0xFFFFFFFF);

		// Make sure the extension is will fit in the filename
		ASSERT(_tcslen(lpszExtension) <= MAX_QUARANTINE_FILENAME_LEN - 9);
		if(_tcslen(lpszExtension) > MAX_QUARANTINE_FILENAME_LEN - 9)
		{
			goto Exit_Function;
		}

		// Append the extension to the filename after the '.'
		STRCPY(&szGeneratedFileName[9], lpszExtension);

		// Copy the path to the beginning of the output buffer
		STRCPY(lpszFileName, lpszTargetPath);

		// Append the filename to the path.
		PathAppend(lpszFileName, szGeneratedFileName);

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
		if(INVALID_HANDLE_VALUE == hFile)
		{
			// Find out why the file could not be created.
			DWORD dwLastError = GetLastError();

			// Loop again if the error was that the file already exists.
			// ERROR_ALREADY_EXISTS is what the API claimed is returned if the
			// file already exists, ERROR_FILE_EXISTS is what is actually
			// returned on my computer.  I have included them both, in case one
			// is pre IE 4.0.
			if((ERROR_ALREADY_EXISTS != dwLastError) &&
				(ERROR_FILE_EXISTS != dwLastError))
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

//////////////////////////////////////////////////////////////////////////////
// Gets the extension part of lpszFileSpec and returns the extension in the 
// buffer lpszExtension.
// An extension is now defined as the text following the last period in a file 
// spec.
// The cbExtBuffSize parameter contains the size of the buffer in number of 
// characters, not number of bytes!!!
// See Also:
//   NameReturnExtension()
// Return: NOERR - Function completed
////////////////////////////////////////////////////////////////////////
// 12/16/94 BEM Function created by copying NameReturnExtension() and
//              changing the 3s to cbExtBuffSize-1
////////////////////////////////////////////////////////////////////////
UINT ReturnExtension(LPCTSTR lpszFileSpec,
					 LPTSTR  lpszExtension,
					 UINT    cbExtBuffSize)
{
	UINT    wRet;           // Component return code
	LPCTSTR lpszFileIndex;  // Index into lpszFileSpec
	register TCHAR ch;

	ASSERT (lpszFileSpec);
	ASSERT (lpszExtension);

	// Initialize index to point to
	// '\0' in string.

	lpszFileIndex = lpszFileSpec + STRLEN(lpszFileSpec);

	// Backup to '\', ':', or '.'
	while ((lpszFileIndex = AnsiPrev (lpszFileSpec, lpszFileIndex))
		   > lpszFileSpec)
	{
		ch = *lpszFileIndex;

		if(ch == ':' || ch == '\\' || ch == '.')
			break;                      // We found character
	}

	if(*lpszFileIndex == '.')          // Was there an extension?
	{
		lpszFileIndex = AnsiNext (lpszFileIndex);

		STRNCPY(lpszExtension, lpszFileIndex, cbExtBuffSize-1);
		lpszExtension[cbExtBuffSize-1] = EOS;
		wRet = NOERR;
	}
	else
	{                                // No extension found...
		*lpszExtension = EOS;
		wRet = NOERR;
	}

	return (wRet);
} // ReturnExtension()


// FileWipe()
// Like FileDelete() but overwrites the data with zeros first.
// Returns: NOERR on success, or ERR on error.
UINT FileZeroAndDelete(LPCTSTR lpFilename, UINT uTimesToWipe)
{
	HANDLE hFile            = NULL;
	LPVOID lpvBuffer        = NULL;
	const UINT uMaxBytesToWrite = 0xFC00;
	UINT   uBytesToWrite;
	DWORD  dwFileSize;
	DWORD  dwBytesLeft;

	// Make sure we can write to the file
	if(SetFileAttributes(lpFilename, FILE_ATTRIBUTE_NORMAL) == 0)
	{
		CCTRACEW(_T("FileZeroAndDelete() - Failed to set file attibutes"));
		goto Cleanup;
	}

	// Open the file with write access
	hFile = CreateFile(lpFilename, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(INVALID_HANDLE_VALUE == hFile)
	{
		CCTRACEE(_T("FileZeroAndDelete() - Failed to create the file."));
		goto Cleanup;
	}

	// Get the file's size
	if((dwFileSize = GetFileSize(hFile, NULL)) == INVALID_FILE_SIZE)
	{
		CCTRACEE(_T("FileZeroAndDelete() - Failed to get the file size."));
		goto Cleanup;
	}

	// Allocate a data buffer
	if((lpvBuffer = malloc(uMaxBytesToWrite)) == NULL)
	{
		CCTRACEE(_T("FileZeroAndDelete() - Failed to allocate data buffer."));
		goto Cleanup;
	}

	// Set the buffer to zeros
	memset(lpvBuffer, 0, uMaxBytesToWrite);

	// Overwrite the file as many times as requested
	while (uTimesToWipe)
	{
		dwBytesLeft = dwFileSize;

		// Seek to the beginning of the file
		if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			CCTRACEE(_T("FileZeroAndDelete() - Failed to set the file pointer to the begining."));
			goto Cleanup;
		}

		// While their are bytes left to be overwritten
		while (dwBytesLeft)
		{
			// We cannot write more than the size of the data buffer at once
			uBytesToWrite = (dwBytesLeft > uMaxBytesToWrite) ? 
                            uMaxBytesToWrite :
                            (UINT) dwBytesLeft;

			DWORD uBytesWritten = 0;
			// Write over the next portion of the file
			WriteFile(hFile, lpvBuffer, uBytesToWrite, &uBytesWritten, NULL);

			dwBytesLeft -= uBytesToWrite;
		}

		uTimesToWipe--;
	}

	// Truncate the file
	if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		CCTRACEE(_T("FileZeroAndDelete() - Failed to set file pointer for truncation."));
		goto Cleanup;
	}
	if(SetEndOfFile(hFile) == 0)
	{
		CCTRACEE(_T("FileZeroAndDelete() - Failed to set the end of file."));
		goto Cleanup;
	}

	// Free the buffer and close the file
	free(lpvBuffer);
	CloseHandle(hFile);

	// Delete the file
	return (DeleteFile(lpFilename));

Cleanup:

	// Something went wrong if we're here
	if(hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	if(lpvBuffer)
		free(lpvBuffer);
	return -1;
}
