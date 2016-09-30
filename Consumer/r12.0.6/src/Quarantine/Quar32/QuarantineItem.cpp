//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// QuarantineItem.cpp: implementation of the CQuarantineItem class.
//

#include <windows.h>
#include <winbase.h>
#include "stdafx.h"

#include "resource.h"

// Taken from QuarAdd_cc.h
#include "..\..\SDKs\AutoProtect\include\apquar.h"
#include "avtypes.h"

#define INITIIDS
#include "Quar32.h"
#include "QuarantineItem.h"

#include "xapi.h"
#include "global.h"
#include "qsfields.h"
//#include <stdexcept>
#include "QuarantineDLL.h"
#include "SimpleScanner.h"
#include "CommonUIInterface.h"

#include "ccSymMemoryStreamImpl.h"
#include "ccSymFileStreamImpl.h"
#include "ccSymStringImpl.h"
#include "ccSymStringConvert.h"
#include "ccCoInitialize.h"
#include "ccSettingsInterface.h"

#include "SnapshotServerLoader.h"
#include "SystemSnapshot.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuarantineItem::CQuarantineItem()
{
	m_hQuarItem = NULL;
	m_pQScanner = NULL;
}

CQuarantineItem::~CQuarantineItem()
{
	// Release NavScan
	if(NULL != m_pQScanner)
	{
		delete m_pQScanner;
		m_pQScanner = NULL;
	}

	if(m_pScanProps != NULL)
	{
		m_pScanProps.Release();
		m_pScanProps = NULL;
	}

	if(m_pScanSink != NULL)
	{
		m_pScanSink.Release();
		m_pScanSink = NULL;
	}

	if(m_pScanner!= NULL)
	{
		m_pScanner.Release();
		m_pScanner = NULL;
	}

	if(m_pEraser != NULL)
	{
		m_pEraser.Release();
		m_pEraser = NULL;
	}

	// Release the handle to the QSPAK file
	if(m_hQuarItem)
		QsPakReleaseItem(m_hQuarItem);
} // END CQuarantineItem::~CQuarantineItem()


////////////////////////////////////////////////////////////////////////////////
// IQuarantineItem implementation (Syminterface functions)


// ==== SaveItem ==========================================================
// Writes current contents of the file info structure to the Quarantine file.
//  Output: -- S_OK if all goes well
// ========================================================================
//  Function created: 3/98, SEDWARD
// 4/28/98 - JTAYLOR Improved tracking and deleting on the pBuff variable.
// ========================================================================
STDMETHODIMP CQuarantineItem::SaveItem()
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(QSPAKSTATUS_OK != QsPakSaveItem(m_hQuarItem))
		return E_FAIL;

	return S_OK;
} // END CQuarantineItem::SaveItem()


// ==== DeleteItem ========================================================
// Deletes a Quarantine Item file (uses this object's current filename).
//  Output: -- S_OK if all goes well, E_UNEXPECTED if not
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================
STDMETHODIMP CQuarantineItem::DeleteItem()
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    BOOL	bResult = TRUE;
    char    szCharBuf[MAX_QUARANTINE_FILENAME_LEN];
    HRESULT retVal = S_OK;

    bResult = GetQuarantinePath(szCharBuf, MAX_QUARANTINE_FILENAME_LEN, TARGET_QUARANTINE_PATH);
    if(FALSE == bResult)
	{
		CCTRACEE(_T("%s - Error Unable to get Quarantine path"), __FUNCTION__);
		retVal = E_UNABLE_TO_GET_QUARANTINE_PATH;
		goto  Exit_Function;
	}

	if(!DeleteDataFolder())
	{
		CCTRACEE(_T("%s - !DeleteDataFolder()"), __FUNCTION__);
		retVal = E_FAIL;
	}

    if(!TruncateFile(m_szQuarantineItemPath))
	{
		CCTRACEE(_T("%s - !TruncateFile(%s)"), __FUNCTION__, m_szQuarantineItemPath);
		retVal = E_FAIL;
	}

Exit_Function:
    return retVal;

}  // END CQuarantineItem::DeleteItem


// Added for NAV 2005.5/r11.5
bool CQuarantineItem::DeleteDataFolder()
{
	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return false;
	}

	// Remove data subfolder for this item and its contents
	TCHAR szDataSubfolderName[MAX_QUARANTINE_FILENAME_LEN] = {0};
	DWORD dwBuffSize = MAX_QUARANTINE_FILENAME_LEN;
	QSPAKSTATUS qsResult = QsPakQueryItemValue(m_hQuarItem, QSERVER_ITEM_INFO_DATA_SUBFOLDER, QSPAK_TYPE_STRING, szDataSubfolderName, &dwBuffSize);
	if(QSPAKSTATUS_NO_SUCH_FIELD == qsResult)  // Can do additional checking, such as of remediation count
	{
		CCTRACEI(_T("%s - Field does not exist: QSERVER_ITEM_INFO_DATA_SUBFOLDER"), __FUNCTION__);
		return true;
	}
	else if(QSPAKSTATUS_OK != qsResult)
	{
		CCTRACEE(_T("%s - Could not get QSERVER_ITEM_INFO_DATA_SUBFOLDER"), __FUNCTION__);
		return false;
	}

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	bool bFinishedSearching = false;

	TCHAR szDataFiles[MAX_QUARANTINE_FILENAME_LEN] = {0};
	_tcscpy(szDataFiles, szDataSubfolderName);
	NameAppendFile(szDataFiles, _T("*.*"));

	hFind = FindFirstFile(szDataFiles, &FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)
	{
		CCTRACEI(_T("%s - Couldn't find a file, GetLastError()=%d"), __FUNCTION__, GetLastError());
		bFinishedSearching = true;
	}

	while(!bFinishedSearching)
	{
		TCHAR szDataFile[MAX_QUARANTINE_FILENAME_LEN] = {0};
		_tcscpy(szDataFile, szDataSubfolderName);
		NameAppendFile(szDataFile, FindFileData.cFileName);

		DWORD dwAttributes = GetFileAttributes(szDataFile);
		if((INVALID_FILE_ATTRIBUTES != dwAttributes) 
			&& !((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY))
		{
			if(!(::DeleteFile(szDataFile)))
				CCTRACEE(_T("%s - Failed DeleteFile(%s)"), __FUNCTION__, szDataFile);
		}

		if(!FindNextFile(hFind, &FindFileData))
		{
			DWORD dwLastError = GetLastError();
			if(ERROR_NO_MORE_FILES == dwLastError)
			{ 
				bFinishedSearching = true;
			} 
			else 
			{ 
				CCTRACEE(_T("%s - Unknown error finding next file, GetLastError()=%d"), __FUNCTION__, dwLastError);
				bFinishedSearching = true;
			} 
		}
	} 

	if((INVALID_HANDLE_VALUE != hFind) && !FindClose(hFind))
		CCTRACEE(_T("%s - Couldn't close search handle, GetLastError()=%d"), __FUNCTION__, GetLastError());

	DWORD dwAttributes = 0;
	dwAttributes = GetFileAttributes(szDataSubfolderName);
	if((INVALID_FILE_ATTRIBUTES != dwAttributes) 
		&& ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY))
	{ 
		if(!(::RemoveDirectory(szDataSubfolderName)))
		{
			CCTRACEE(_T("%s - Failed RemoveDirectory(%s)"), __FUNCTION__, szDataSubfolderName);
			return false;
		}
	}

	return true;
} // END CQuarantineItem::DeleteDataFolder()


// ==== RepairAndRestoreMainFileToTemp =======================================
// Scan a file to determine if it has a virus.  Update and save the virus
// information structure.  Then attempt to repair the file.
// Input:  
//   szDestPath - [out] Character buffer for folder + filename of the repaired file
//   dwPathSize - [out] The size of the buffer
// ========================================================================
// 3/11/98 Function created: JTAYLOR
// ========================================================================
STDMETHODIMP CQuarantineItem::RepairAndRestoreMainFileToTemp(char* szDestPath, DWORD dwPathSize)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	return ScanRepairMainFile(szDestPath, dwPathSize, TRUE);
}  // END CQuarantineItem::RepairAndRestoreMainFileToTemp


// ==== ScanMainFile ====================================================
// Scan a file to determine if it has a virus.  Update and save the virus
// information structure.
// ========================================================================
// 3/11/98 Function created: JTAYLOR
// ========================================================================
STDMETHODIMP CQuarantineItem::ScanMainFile()
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	return ScanRepairMainFile(NULL, 0, FALSE);
}  // END "CQuarantineItem::ScanMainFile"


// ==== RestoreItem ==========================================================
// Restores a Quarantine item to a specified location.  The caller MUST supply
// a destination path, and can optionally supply a filename (if one is not
// specified, the original is used).  Once the item has been successfully 
// restored, it is removed from the Quarantine area.
//
// Input:
//  lpszDestPath     - destination path for the file. REQUIRED.
//  lpszDestFilename - name of the destination file; if the caller
//                     does not specify this, the original filename
//                     is used
//  bOverwrite       - TRUE to overwrite the file if it exists, FALSE
//                     to leave it alone
//  Output:
//      S_OK if all goes well
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================
/*STDMETHODIMP CQuarantineItem::RestoreItem(LPSTR lpszDestPath
                                        , LPSTR lpszDestFilename
                                        , BOOL  bOverwrite)

{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    char szFileNameBuf[MAX_QUARANTINE_FILENAME_LEN];
    HRESULT retVal = S_OK;
    UINT nResult = 0;
	char szBuff[MAX_QUARANTINE_FILENAME_LEN];
	DWORD dwBuffSize = MAX_QUARANTINE_FILENAME_LEN;

    // make sure the caller has specified a valid path
    if((NULL == lpszDestPath)  ||  (NULL == *lpszDestPath))
    {
        retVal = E_INVALIDARG;
        goto  Exit_Function;
    }

    // get the filename - if no filename specified, use the original name
    if((NULL == lpszDestFilename)  ||  (NULL == *lpszDestFilename))
    {
		if(QSPAKSTATUS_OK != QsPakQueryItemValue(m_hQuarItem, QSERVER_ITEM_INFO_FILENAME_ANSI, QSPAK_TYPE_STRING, szBuff, &dwBuffSize))
		{
			retVal = E_FAIL;
			goto Exit_Function;
		}

		nResult = NameReturnFile(szBuff, szFileNameBuf);
        if(NOERR != nResult)
        {
            retVal = E_FILE_FILENAME_MANIPULATION_FAILED;
            goto  Exit_Function;
        }
    }
    else
    {
        // use the caller-supplied filename
        STRCPY(szFileNameBuf, lpszDestFilename);
    }

    // unpackage the file, remove the Quarantine item and return
    retVal = UnpackageMainFile(lpszDestPath, szFileNameBuf, NULL, 0, bOverwrite
                                                                    , FALSE);
    if(SUCCEEDED(retVal))
    {
		retVal = RestoreAllRemediations();
		if(SUCCEEDED(retVal))
	        retVal = DeleteItem();
    }

Exit_Function:

    return retVal;

}*/ // END "CQuarantineItem::RestoreItem()"


// ==== Initialize ========================================================
// Opens the target Quarantine file and reads the file header.  If successful,
// and the file is indeed a Quarantine file, the "m_fileHeader" member is
// initialized and the file is closed.
// Input:
//   lpszFileName - Complete path+file specification to the target file in 
//					Quarantine
// Output:
//   S_OK if all goes well
// ========================================================================
//  Function created: 2/98, SEDWARD
//  06/22/98 - JTAYLOR - Changed current filename member to be the name
//                       of the file that was initialized.
// ========================================================================
STDMETHODIMP CQuarantineItem::Initialize(LPSTR szQuarantineItemPath)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	QSPAKSTATUS status;

    // If we already have a handle to a QSPAK file open, close it
	if(m_hQuarItem)
		QsPakReleaseItem(m_hQuarItem);

	// return if we failed to read the file
	status = QsPakOpenItem(szQuarantineItemPath, &m_hQuarItem);
	if(status != QSPAKSTATUS_OK)
		return E_INVALID_QUARANTINE_FILE;

	// save off the Current Filename
	_tcscpy(m_szQuarantineItemPath, szQuarantineItemPath);

	return S_OK;
}  // end of "CQuarantineItem::Initialize"


// ==== UnpackageMainFile ===============================================
// Unpackage a Quarantine file to the specified location.
// The caller MUST provide a destination directory. 
// The caller can optionally provide a target filename, and a flag specifying
// whether to overwrite the file if it exists.
//
// Input:
//  lpszDestinationFolder  : folder where file will be unpackaged
//  lpszDestinationFileName: name to use for unpackaged file (can be NULL)
//  lpszUnpackagedFileName : pointer to buffer for unpackaged filename (only 
//							 used if no filename is passed in)
//  dwBufSize              : size of "lpszUnpackagedFileName" buffer
//  bOverwrite             : TRUE if we should overwrite an existing file, 
//							 FALSE if not
//  bMakeTemporaryFile     : have CreateFile() create the unpackaged file
//							 with the temporary attribute
// Output:
//   S_OK if all goes well
// ========================================================================
//  Function created: 3/98, SEDWARD
// 4/25/98 - JTAYLOR - Added support for restoring the files original date
//                     time values.
// 5/20/98 JTAYLOR - Fixed Quarantine extension lenght to MAX_QUARANTINE_EXTENSION_LENGTH
// 7/01/98 JTAYLOR - Changed the file time functions to not fail the process
// ========================================================================
STDMETHODIMP CQuarantineItem::UnpackageMainFile(LPSTR lpszDestinationFolder // [in]
                                            , LPSTR lpszDestinationFileName // [in]
                                            , LPSTR lpszUnpackagedFileName  // [out]
                                            , DWORD dwBufSize               // [in]
                                            , BOOL bOverwrite               // [in]
                                            , BOOL bMakeTemporaryFile)      // [in]
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	LPBYTE			lpSecurityDesc = NULL;
	TCHAR			szDestFilename[MAX_QUARANTINE_FILENAME_LEN];
	FILETIME        ftCreation;
    FILETIME        ftAccess;
    FILETIME        ftLastWrite;
    FILETIME        ftZero;
	SYSTEMTIME		stTemp;
	QSPAKDATE		stQSDate;
	DWORD			dwQSDateSize = sizeof(stQSDate);
	HANDLE          destFileHandle = INVALID_HANDLE_VALUE;
    LPTSTR          pszBuffer = NULL;

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}
	
    // if the caller did not supply a valid destination directory, return
    if((NULL == lpszDestinationFolder)  ||  (NULL == *lpszDestinationFolder))
    {
		CCTRACEE(_T("%s - Invalid destination directory"), __FUNCTION__);
		return E_INVALIDARG;
    }
	
	try
	{
		// Save off security attributes.
		if(FAILED(GetFileSecurityDesc(m_szQuarantineItemPath, &lpSecurityDesc)))
			throw std::runtime_error("Failed to get File Security attributes");

		// Create the destination filename

		// if the input filename is NULL, we assume the caller doesn't care and we
		// create a file with a unique filename (however, we must keep the current
		// file extension)
		if((NULL == lpszDestinationFileName)  ||  (NULL == *lpszDestinationFileName))
		{
			TCHAR szExtension[MAX_QUARANTINE_EXTENSION_LENGTH] = {0};
			HANDLE hNewFileHandle = INVALID_HANDLE_VALUE;
			pszBuffer = new TCHAR[MAX_QUARANTINE_EXTENSION_LENGTH];
			if(NULL == pszBuffer)
				throw std::runtime_error("Could not allocate a buffer to unpack the item");
			DWORD dwBuffSize = MAX_QUARANTINE_EXTENSION_LENGTH;

			// Get the original filename
			QSPAKSTATUS qsPakStatus = QsPakQueryItemValue(m_hQuarItem, QSERVER_ITEM_INFO_FILENAME_ANSI, QSPAK_TYPE_STRING, pszBuffer, &dwBuffSize);
			if(QSPAKSTATUS_OK != qsPakStatus)
			{
				// Make sure the buffer was big enough
				if(QSPAKSTATUS_BUFFER_TOO_SMALL == qsPakStatus)
				{
					delete [] pszBuffer;
					pszBuffer = NULL;
					dwBuffSize++;
					pszBuffer = new TCHAR[dwBuffSize];
					if(NULL == pszBuffer)
						throw std::runtime_error("Could not allocate a buffer to unpack the item");
					qsPakStatus = QsPakQueryItemValue(m_hQuarItem, QSERVER_ITEM_INFO_FILENAME_ANSI, QSPAK_TYPE_STRING, pszBuffer, &dwBuffSize);
					if(QSPAKSTATUS_OK != qsPakStatus)
					{
						if(NULL != pszBuffer)
						{
							delete [] pszBuffer;
							pszBuffer = NULL;
						}
						throw std::runtime_error("Could not read Original Filename");
					}
				}
			}

			// Get the file extension for the file name to extract to
			GetFileExtension(pszBuffer, szExtension);

			if(NULL != pszBuffer)
			{
				delete [] pszBuffer;
				pszBuffer = NULL;
			}

			// Get a unique filename for the new quarantine file
			hNewFileHandle = CreateUniqueFile(lpszDestinationFolder, szExtension, szDestFilename);
			if(hNewFileHandle == INVALID_HANDLE_VALUE)
				throw std::runtime_error("Unable to create unique file name");

			CloseHandle(hNewFileHandle);
			DeleteFile(szDestFilename);
		}
		else    // the caller supplied a target filename, so use it
		{
			_tcsncpy(szDestFilename, lpszDestinationFolder, MAX_QUARANTINE_FILENAME_LEN);
			bool bAddBackSlash = false;

			// Make sure there is a backslash in between the folder and filename
			TCHAR chLastChar = szDestFilename[_tcslen(szDestFilename)-1];
			if(chLastChar != _TCHAR('\\'))
			{
				if(lpszDestinationFileName && *lpszDestinationFileName)
				{
					// Now check if the file name already has a lead backslash
					if(*lpszDestinationFileName != _TCHAR('\\'))
					{
						bAddBackSlash = true;
					}
				}
				else
				{
					bAddBackSlash = true;
				}
			}

			if(bAddBackSlash)
			{
				// Need to add a backslash to the end of the destination folder name
				_tcsncat(szDestFilename, _T("\\"), MAX_QUARANTINE_FILENAME_LEN-_tcslen(szDestFilename));
			}

			// Add the file name to the path
			_tcsncat(szDestFilename, lpszDestinationFileName, MAX_QUARANTINE_FILENAME_LEN - _tcslen(szDestFilename));

			if(bOverwrite)
			{
				// Try to truncate the file if it exists. If we don't Norton Protected Recycle Bin
				// will make a backup of the original.
				TruncateFile (szDestFilename);
			}
		}

		// save the unique filename
		if(NULL != lpszUnpackagedFileName)
		{
			STRNCPY(lpszUnpackagedFileName, szDestFilename, dwBufSize);
		}

		// Now actually Unpack the item
		if(QSPAKSTATUS_OK != QsPakUnpackageMainFile(m_hQuarItem, szDestFilename))
			throw std::runtime_error("Unpackaging file Failed");

		// set "temporary" file attribute
		if(bMakeTemporaryFile)
			SetFileAttributes(szDestFilename, FILE_ATTRIBUTE_TEMPORARY);

		// Set proper security attributes.
		SetFileSecurityDesc(szDestFilename, lpSecurityDesc);

		// Create a zero file time
		ftZero.dwLowDateTime = 0;
		ftZero.dwHighDateTime = 0;
        
        FILETIME ftCurrentTime;
        GetSystemTimeAsFileTime(&ftCurrentTime);

		// Write the file date/times 
		if(QsPakQueryItemValue(m_hQuarItem, QSERVER_ITEM_INFO_FILE_CREATED_TIME, QSPAK_TYPE_DATE, &stQSDate, &dwQSDateSize) == QSPAKSTATUS_OK)
        {
            QSDateToSysTime(stQSDate, &stTemp);
            SystemTimeToFileTime(&stTemp,  &ftCreation);
        }
        else
        {
            memcpy(&ftCreation, &ftCurrentTime, sizeof(FILETIME));
        }

		dwQSDateSize = sizeof(stQSDate);
		if(QsPakQueryItemValue(m_hQuarItem, QSERVER_ITEM_INFO_FILE_MODIFIED_TIME, QSPAK_TYPE_DATE, &stQSDate, &dwQSDateSize) == QSPAKSTATUS_OK)
        {
            QSDateToSysTime(stQSDate, &stTemp);
            SystemTimeToFileTime(&stTemp,  &ftLastWrite);
        }
        else
        {
            memcpy(&ftLastWrite, &ftCurrentTime, sizeof(FILETIME));
        }

		dwQSDateSize = sizeof(stQSDate);
		if(QsPakQueryItemValue(m_hQuarItem, QSERVER_ITEM_INFO_FILE_ACCESSED_TIME, QSPAK_TYPE_DATE, &stQSDate, &dwQSDateSize) == QSPAKSTATUS_OK)
        {
            QSDateToSysTime(stQSDate, &stTemp);
            SystemTimeToFileTime(&stTemp, &ftAccess);
        }
        else
        {
            memcpy(&ftAccess, &ftCurrentTime, sizeof(FILETIME));
        }

		// Open a handle to the file so we can set the file times
		destFileHandle = CreateFile(szDestFilename             // filename
			, GENERIC_WRITE         // access
			, 0                     // don't share
			, NULL                  // don't allow handle inheritance
			, OPEN_EXISTING        // creation mode
			, FILE_ATTRIBUTE_NORMAL  // attributes and flags
			, NULL);               // no template handle
		if(destFileHandle == INVALID_HANDLE_VALUE)
			throw std::runtime_error("Could not open extracted file to update times");

		// Set the file time.  If any of the times is 0, then do not set that value.
		if(!SetFileTime(destFileHandle,
			(0 != CompareFileTime(&ftCreation, &ftZero))  ? &ftCreation : NULL,
			(0 != CompareFileTime(&ftAccess, &ftZero))    ? &ftAccess : NULL,
			(0 != CompareFileTime(&ftLastWrite, &ftZero)) ? &ftLastWrite : NULL))
			throw std::runtime_error("Failed to set the File Time");

		// Free security descriptor.
		if(NULL != lpSecurityDesc)
		{
			delete [] lpSecurityDesc;
			lpSecurityDesc = NULL;
		}

		CloseHandle(destFileHandle);
	}
	catch(exception& Ex)
	{
		CCTRACEE(Ex.what());
		
		// Cleanup
		// Free security descriptor.
		if(NULL != lpSecurityDesc)
		{
			delete [] lpSecurityDesc;
			lpSecurityDesc = NULL;
		}

		if(destFileHandle != INVALID_HANDLE_VALUE)
			CloseHandle(destFileHandle);

        if(NULL != pszBuffer)
        {
            delete [] pszBuffer;
            pszBuffer = NULL;
        }

		return E_FAIL;
	}

	return S_OK;
} // END CQuarantineItem::UnpackageMainFile()


////////////////////////////////////////////////////////////////////////////////
// CQuarantineItem access functions

STDMETHODIMP CQuarantineItem::GetUniqueID(UUID&  uuid)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	TCHAR*	lpszBuffer;
	DWORD	dwBufferLen = MAX_PATH;
	WCHAR*	wcharStr= NULL;	

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}
	
	// Allocate a buffer for the data
	lpszBuffer = new TCHAR[dwBufferLen];
	if(!lpszBuffer)
		return E_FAIL;

	wcharStr = new WCHAR[dwBufferLen];
	if(!wcharStr)
	{
		delete[] lpszBuffer;
		return E_FAIL;
	}

	// Query for the UUID
	if(QSPAKSTATUS_OK != QsPakQueryItemValue(m_hQuarItem, QSERVER_ITEM_INFO_UUID, QSPAK_TYPE_STRING, lpszBuffer , &dwBufferLen))
	{
		delete[] lpszBuffer;
		delete[] wcharStr;
		return E_FAIL;
	}

	// Convert it to wchar
	if(!mbstowcs(wcharStr, lpszBuffer, dwBufferLen))
	{
		delete[] lpszBuffer;
		delete[] wcharStr;
		return E_FAIL;
	}
	
	//Now convert to a GUID
	if(S_OK != IIDFromString(wcharStr, &uuid))
	{
		delete[] lpszBuffer;
		delete[] wcharStr;
		return E_FAIL;
	}

	delete[] lpszBuffer;
	delete[] wcharStr;
    return S_OK;
} // END CQuarantineItem::GetUniqueID()


STDMETHODIMP CQuarantineItem::SetUniqueID(UUID  newID)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	WCHAR	wcharStr[MAX_QUARANTINE_FILENAME_LEN];	
	TCHAR	szLocalBuffer[MAX_QUARANTINE_FILENAME_LEN];

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}
	
	StringFromGUID2(newID, wcharStr, MAX_QUARANTINE_FILENAME_LEN);
	
	wcstombs(szLocalBuffer, wcharStr, MAX_QUARANTINE_FILENAME_LEN);

	if(QSPAKSTATUS_OK != QsPakSetItemValue(m_hQuarItem, QSERVER_ITEM_INFO_UUID, QSPAK_TYPE_STRING, szLocalBuffer, _tcslen(szLocalBuffer)))  
		return E_FAIL;

    return S_OK;
} // END CQuarantineItem::SetUniqueID()


STDMETHODIMP CQuarantineItem::GetFileStatus(DWORD* dwFileStatus)
{
	return GetDWORD(QSERVER_ITEM_INFO_STATUS, dwFileStatus);
}


STDMETHODIMP CQuarantineItem::SetFileStatus(DWORD newFileStatus)
{
	return SetDWORD(QSERVER_ITEM_INFO_STATUS, newFileStatus);
}


STDMETHODIMP CQuarantineItem::GetFileType(DWORD* fileType)
{
	return GetDWORD(QSERVER_ITEM_INFO_SAMPLE_DWTYPE, fileType);
}


STDMETHODIMP CQuarantineItem::SetFileType(DWORD newFileType)
{
	return SetDWORD(QSERVER_ITEM_INFO_SAMPLE_DWTYPE, newFileType);
}


STDMETHODIMP CQuarantineItem::GetDateQuarantined(SYSTEMTIME* dateQuarantined)
{
	return GetDate(QSERVER_ITEM_INFO_QUARANTINE_QDATE, dateQuarantined);
}


STDMETHODIMP CQuarantineItem::SetDateQuarantined(SYSTEMTIME* newQuarantineDate)
{
	return SetDate(QSERVER_ITEM_INFO_QUARANTINE_QDATE, newQuarantineDate);
}


STDMETHODIMP CQuarantineItem::GetOriginalFileDates(SYSTEMTIME* pstOriginalFileDateCreated,
                                                   SYSTEMTIME* pstOriginalFileDateAccessed,
                                                   SYSTEMTIME* pstOriginalFileDateWritten)
{
	HRESULT hResult = GetDate(QSERVER_ITEM_INFO_FILE_CREATED_TIME, pstOriginalFileDateCreated);
	if(FAILED(hResult))
		return hResult;

	hResult = GetDate(QSERVER_ITEM_INFO_FILE_ACCESSED_TIME, pstOriginalFileDateAccessed);
	if(FAILED(hResult))
		return hResult;

	return GetDate(QSERVER_ITEM_INFO_FILE_MODIFIED_TIME, pstOriginalFileDateWritten);
} // END CQuarantineItem::GetOriginalFileDates()


STDMETHODIMP CQuarantineItem::SetOriginalFileDates(SYSTEMTIME* pstOriginalFileDateCreated,
                                                   SYSTEMTIME* pstOriginalFileDateAccessed,
                                                   SYSTEMTIME* pstOriginalFileDateWritten)
{
	HRESULT hResult = SetDate(QSERVER_ITEM_INFO_FILE_CREATED_TIME, pstOriginalFileDateCreated);
	if(FAILED(hResult))
		return hResult;

	hResult = SetDate(QSERVER_ITEM_INFO_FILE_ACCESSED_TIME, pstOriginalFileDateAccessed);
	if(FAILED(hResult))
		return hResult;

	return SetDate(QSERVER_ITEM_INFO_FILE_MODIFIED_TIME, pstOriginalFileDateWritten);
} // END CQuarantineItem::SetOriginalFileDates()


STDMETHODIMP CQuarantineItem::GetDateOfLastScan(SYSTEMTIME* dateLastScanned)
{
	return GetDate(QSERVER_ITEM_INFO_SCAN_QDATE, dateLastScanned);
}


STDMETHODIMP CQuarantineItem::SetDateOfLastScan(SYSTEMTIME* newDateLastScan)
{
	return SetDate(QSERVER_ITEM_INFO_SCAN_QDATE, newDateLastScan);
}


STDMETHODIMP CQuarantineItem::GetDateOfLastScanDefs(SYSTEMTIME* dateOfLastScannedDefs)
{
	return GetDate(QSERVER_ITEM_INFO_SCAN_DEFS_QDATE, dateOfLastScannedDefs);
}


STDMETHODIMP CQuarantineItem::SetDateOfLastScanDefs(SYSTEMTIME*  newDateLastScanDefs)
{
	return SetDate(QSERVER_ITEM_INFO_SCAN_DEFS_QDATE, newDateLastScanDefs);
}


STDMETHODIMP CQuarantineItem::GetDateSubmittedToSARC(SYSTEMTIME* dateSubmittedToSARC)
{
	HRESULT hResult = GetDate(QSERVER_ITEM_INFO_SUBMIT_QDATE, dateSubmittedToSARC);
	if(E_QSPAKERROR_NO_SUCH_FIELD == hResult)
	{
		// If the value is not found, zero out the date and return
		ZeroMemory(dateSubmittedToSARC, sizeof(SYSTEMTIME));
		return S_OK;
	}
	else if(FAILED(hResult))
	{
		// Zero out the date and return
		ZeroMemory(dateSubmittedToSARC, sizeof(SYSTEMTIME));
	}

	return hResult;
} // END CQuarantineItem::GetDateSubmittedToSARC()


STDMETHODIMP CQuarantineItem::SetDateSubmittedToSARC(SYSTEMTIME* newDateSubmittedToSARC)
{
	return SetDate(QSERVER_ITEM_INFO_SUBMIT_QDATE, newDateSubmittedToSARC);
}


STDMETHODIMP CQuarantineItem::GetOriginalFilesize(DWORD* originalFileSize)
{
	return GetDWORD(QSERVER_ITEM_INFO_FILESIZE, originalFileSize);
}

STDMETHODIMP CQuarantineItem::SetOriginalFilesize(DWORD newOriginalFileSize)
{
	return SetDWORD(QSERVER_ITEM_INFO_FILESIZE, newOriginalFileSize);
}


HRESULT CQuarantineItem::GetItemPath(char* szDestBuf, DWORD* bufSize)
{
	return GetString(QSERVER_ITEM_INFO_QUAR_PATH, szDestBuf, bufSize);
}

HRESULT CQuarantineItem::SetItemPath(char* szNewPath)
{
	return SetString(QSERVER_ITEM_INFO_QUAR_PATH, szNewPath, strlen(szNewPath));
}


HRESULT CQuarantineItem::GetItemFilename(char* szDestBuf, DWORD* bufSize)
{
	return GetString(QSERVER_ITEM_INFO_QUAR_FILENAME, szDestBuf, bufSize);
}

HRESULT CQuarantineItem::SetItemFilename(char* szNewCurrentFilename)
{
	return SetString(QSERVER_ITEM_INFO_QUAR_FILENAME, szNewCurrentFilename, strlen(szNewCurrentFilename));
}


STDMETHODIMP CQuarantineItem::GetOriginalAnsiFilename(char* szDestBuf, DWORD* dwBufferSize)
{
	return GetString(QSERVER_ITEM_INFO_FILENAME_ANSI, szDestBuf, dwBufferSize);
}


STDMETHODIMP CQuarantineItem::SetOriginalAnsiFilename(char* szNewOriginalAnsiFilename)
{
	return SetString(QSERVER_ITEM_INFO_FILENAME_ANSI, szNewOriginalAnsiFilename, strlen(szNewOriginalAnsiFilename));
}


STDMETHODIMP CQuarantineItem::GetOriginalOwnerName(char*  szDestBuf, DWORD* dwBufferSize)
{
	return GetString(QSERVER_ITEM_INFO_USERNAME, szDestBuf, dwBufferSize);
}


STDMETHODIMP CQuarantineItem::SetOriginalOwnerName(char* szNewOwnerName)
{
	return SetString(QSERVER_ITEM_INFO_USERNAME, szNewOwnerName, strlen(szNewOwnerName));
}


STDMETHODIMP CQuarantineItem::GetOriginalMachineDomain(char* szDestBuf, DWORD* dwBufferSize)
{
	return GetString(QSERVER_ITEM_INFO_DOMAINNAME, szDestBuf, dwBufferSize);
}


STDMETHODIMP CQuarantineItem::SetOriginalMachineDomain(char* szNewMachineDomain)
{
	return SetString(QSERVER_ITEM_INFO_DOMAINNAME, szNewMachineDomain, strlen(szNewMachineDomain));
}


STDMETHODIMP CQuarantineItem::GetOriginalMachineName(char* szDestBuf, DWORD* dwBufferSize)
{
	return GetString(QSERVER_ITEM_INFO_MACHINENAME, szDestBuf, dwBufferSize);
}


STDMETHODIMP CQuarantineItem::SetOriginalMachineName(char* szNewMachineName)
{
	return SetString(QSERVER_ITEM_INFO_MACHINENAME, szNewMachineName, strlen(szNewMachineName));
}


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetVirusName(char* szVirusName, DWORD* dwBufferSize)
{
	return GetString(QSERVER_ITEM_INFO_VIRUSNAME, szVirusName, dwBufferSize);
}


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::SetVirusName(const char* szVirusName)
{
	DWORD dwBufferSize = _tcslen(szVirusName);
	TCHAR* szVirNameCopy = new TCHAR[dwBufferSize];
	_tcsncpy(szVirNameCopy, szVirusName, dwBufferSize);

	HRESULT hResult = SetString(QSERVER_ITEM_INFO_VIRUSNAME, szVirNameCopy, dwBufferSize);
	delete[] szVirNameCopy;
	
	return hResult;
} // END CQuarantineItem::SetVirusName()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetVirusID(DWORD* dwVirusID)
{
	return GetDWORD(QSERVER_ITEM_INFO_VIRUSID, dwVirusID);
}


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::SetVirusID(DWORD dwVirusID)
{
	return SetDWORD(QSERVER_ITEM_INFO_VIRUSID, dwVirusID);
}


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::ClearVirusName()
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	HRESULT hResult = QsPakDeleteItemField(m_hQuarItem, QSERVER_ITEM_INFO_VIRUSNAME);
	if(QSPAKSTATUS_OK == hResult)
		return S_OK;
	else 
		return E_FAIL;
} // END CQuarantineItem::ClearVirusName()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::ClearVirusID()
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	HRESULT hResult = QsPakDeleteItemField(m_hQuarItem, QSERVER_ITEM_INFO_VIRUSID);
	if(QSPAKSTATUS_OK == hResult)
		return S_OK;
	else 
		return E_FAIL;
} // END CQuarantineItem::ClearVirusID()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetItemSize(__int64* nItemSize)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	*nItemSize = 0;

	// Get size of QuarantineItem file - NOT the main file size.
	TCHAR szQuarItemPath[MAX_QUARANTINE_FILENAME_LEN] = {0};
	DWORD dwItemPathSize = MAX_QUARANTINE_FILENAME_LEN;
	HRESULT hResult = GetItemPath(szQuarItemPath, &dwItemPathSize);
	if(FAILED(hResult))
		return hResult;

	WIN32_FILE_ATTRIBUTE_DATA fdItemFileData;
	DWORD dwItemAttributes = GetFileAttributesEx(szQuarItemPath, GetFileExInfoStandard, &fdItemFileData);
	if((INVALID_FILE_ATTRIBUTES != dwItemAttributes) 
		&& !((dwItemAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY))
	{
		*nItemSize = (fdItemFileData.nFileSizeHigh * ((__int64)MAXDWORD + 1)) + fdItemFileData.nFileSizeLow;
	}

	// Get data subfolder for this item
	TCHAR szDataSubfolderName[MAX_QUARANTINE_FILENAME_LEN] = {0};
	DWORD dwBuffSize = MAX_QUARANTINE_FILENAME_LEN;
	QSPAKSTATUS qsResult = QsPakQueryItemValue(m_hQuarItem, QSERVER_ITEM_INFO_DATA_SUBFOLDER, QSPAK_TYPE_STRING, szDataSubfolderName, &dwBuffSize);
	if(QSPAKSTATUS_NO_SUCH_FIELD == qsResult)
	{
		CCTRACEI(_T("%s - Field does not exist: QSERVER_ITEM_INFO_DATA_SUBFOLDER"), __FUNCTION__);
		return S_OK;
	}
	else if(QSPAKSTATUS_OK != qsResult)
	{
		CCTRACEE(_T("%s - Could not get QSERVER_ITEM_INFO_DATA_SUBFOLDER"), __FUNCTION__);
		return E_FAIL;
	}

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	bool bFinishedSearching = false;

	TCHAR szDataFiles[MAX_QUARANTINE_FILENAME_LEN] = {0};
	_tcscpy(szDataFiles, szDataSubfolderName);
	NameAppendFile(szDataFiles, _T("*.*"));

	hFind = FindFirstFile(szDataFiles, &FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)
	{
		CCTRACEI(_T("%s - Couldn't find a file, GetLastError()=%d"), __FUNCTION__, GetLastError());
		bFinishedSearching = true;
	}

	while(!bFinishedSearching)
	{
		TCHAR szDataFile[MAX_QUARANTINE_FILENAME_LEN] = {0};
		_tcscpy(szDataFile, szDataSubfolderName);
		NameAppendFile(szDataFile, FindFileData.cFileName);

		WIN32_FILE_ATTRIBUTE_DATA fdFileData;
		DWORD dwAttributes = GetFileAttributesEx(szDataFile, GetFileExInfoStandard, &fdFileData);
		if((INVALID_FILE_ATTRIBUTES != dwAttributes) 
			&& !((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY))
		{
			*nItemSize += (fdFileData.nFileSizeHigh * ((__int64)MAXDWORD + 1)) + fdFileData.nFileSizeLow;
		}

		if(!FindNextFile(hFind, &FindFileData))
		{
			DWORD dwLastError = GetLastError();
			if(ERROR_NO_MORE_FILES == dwLastError)
			{ 
				bFinishedSearching = true;
				hResult = S_OK;
			} 
			else 
			{ 
				CCTRACEE(_T("%s - Unknown error finding next file, GetLastError()=%d"), __FUNCTION__, dwLastError);
				bFinishedSearching = true;
				hResult = E_FAIL;
			} 
		}
	} 

	if((INVALID_HANDLE_VALUE != hFind) && !FindClose(hFind))
		CCTRACEE(_T("%s - Couldn't close search handle, GetLastError()=%d"), __FUNCTION__, GetLastError());


	return hResult;
} // END CQuarantineItem::GetItemSize()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetAnomalyID(char* szValue, DWORD* dwBufferSize)
{
	return GetString(QSERVER_ITEM_INFO_ANOMALY_ID, szValue, dwBufferSize);
}

// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::SetAnomalyID(const char* szValue)
{
	DWORD dwBufferSize = _tcslen(szValue);
	TCHAR* szValueCopy = new TCHAR[dwBufferSize];
	_tcsncpy(szValueCopy, szValue, dwBufferSize);

	HRESULT hResult = SetString(QSERVER_ITEM_INFO_ANOMALY_ID, szValueCopy, dwBufferSize);
	delete[] szValueCopy;

	return hResult;
} // END CQuarantineItem::SetAnomalyID()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetAnomalyName(char* szValue, DWORD* dwBufferSize)
{
	return GetString(QSERVER_ITEM_INFO_ANOMALY_NAME, szValue, dwBufferSize);
}

// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::SetAnomalyName(const char* szValue)
{
	DWORD dwBufferSize = _tcslen(szValue);
	TCHAR* szValueCopy = new TCHAR[dwBufferSize];
	_tcsncpy(szValueCopy, szValue, dwBufferSize);

	HRESULT hResult = SetString(QSERVER_ITEM_INFO_ANOMALY_NAME, szValueCopy, dwBufferSize);
	delete[] szValueCopy;

	return hResult;
} // END CQuarantineItem::SetAnomalyName()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetAnomalyCategories(char* szValue, DWORD* dwBufferSize)
{
	return GetString(QSERVER_ITEM_INFO_ANOMALY_CATEGORIES, szValue, dwBufferSize);
}


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::SetAnomalyCategories(cc::IIndexValueCollection* pCategories)
{
	CString sAnomalyCategories;

	size_t nCatCount = pCategories->GetCount();
    for(size_t nCur = 0; nCur < nCatCount; nCur++)
    {
        DWORD dwCategory = 0;
        if(pCategories->GetValue(nCur, dwCategory))
        {
			sAnomalyCategories.Format(_T("%s %d"), sAnomalyCategories, dwCategory);
        }
    }

	sAnomalyCategories.Trim();

	return SetString(QSERVER_ITEM_INFO_ANOMALY_CATEGORIES, sAnomalyCategories.GetBuffer(), sAnomalyCategories.GetLength());
} // END CQuarantineItem::SetAnomalyCategories


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetAnomalyDamageFlag(DWORD dwFlag, DWORD* dwValue)
{
	char szQueryName[MAX_PATH] = {0};
	sprintf(szQueryName, QSERVER_ITEM_INFO_ANOMALY_DAMAGE_FLAG, dwFlag);
	return GetDWORD(szQueryName, dwValue);
}


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::SetAnomalyDamageFlag(DWORD dwFlag, DWORD dwValue)
{
	char szQueryName[MAX_PATH] = {0};
	sprintf(szQueryName, QSERVER_ITEM_INFO_ANOMALY_DAMAGE_FLAG, dwFlag);
	return SetDWORD(szQueryName, dwValue);
}

// Added for NAV 2006
STDMETHODIMP CQuarantineItem::GetAnomalyDependencyFlag(DWORD* dwHasDependencies)
{
	return GetDWORD(QSERVER_ITEM_INFO_ANOMALY_DEPENDENCY_FLAG, dwHasDependencies);
}

// Added for NAV 2006
STDMETHODIMP CQuarantineItem::SetAnomalyDependencyFlag(DWORD dwHasDependencies)
{
	return SetDWORD(QSERVER_ITEM_INFO_ANOMALY_DEPENDENCY_FLAG, dwHasDependencies);
}


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetRemediationDescription(DWORD dwRemediationIndex, char* szRemediationDescription, DWORD* dwBufferSize)
{
	char szQueryName[MAX_PATH] = {0};
	sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_DESCRIPTION, dwRemediationIndex);

	return GetString(szQueryName, szRemediationDescription, dwBufferSize);
}


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetRemediationOriginalFilename(DWORD dwRemediationIndex, char* szValue, DWORD* dwBufferSize)
{
	char szQueryName[MAX_PATH] = {0};
	sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_ORIGINAL_FILENAME, dwRemediationIndex);

	return GetString(szQueryName, szValue, dwBufferSize);
}


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetRemediationActionSucceeded(DWORD dwRemediationIndex, bool* bSucceeded)
{
	char szQueryName[MAX_PATH] = {0};
	sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_ACTION_SUCCEEDED, dwRemediationIndex);

	DWORD dwValue;
	HRESULT hResult = GetDWORD(szQueryName, &dwValue);
	if(FAILED(hResult))
	{
		CCTRACEE(_T("%s - FAILED(GetDWORD(%s)), hResult=0x%08X"), __FUNCTION__, szQueryName, hResult);
		*bSucceeded = false;
		return hResult;
	}

	*bSucceeded = (1 == dwValue);
	return hResult;
}


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetRemediationUndoAvailable(DWORD dwRemediationIndex, bool* bUndoAvailable)
{
	*bUndoAvailable = false;

	TCHAR szDataSubfolderName[MAX_QUARANTINE_FILENAME_LEN] = {0};
	DWORD dwBuffSize = MAX_QUARANTINE_FILENAME_LEN;
	HRESULT hResult = GetString(QSERVER_ITEM_INFO_DATA_SUBFOLDER, szDataSubfolderName, &dwBuffSize);
	if(FAILED(hResult))
		return hResult;

	char szQueryName[MAX_PATH] = {0};
	TCHAR szUndoFile[MAX_PATH * 2];
	DWORD dwSize = MAX_PATH * 2;
	sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_UNDO_FILE, dwRemediationIndex);

	hResult = GetString(szQueryName, szUndoFile, &dwSize);
	if(FAILED(hResult))
		return hResult;

	TCHAR szUndoPath[MAX_PATH * 2];
	_tcscpy(szUndoPath, szDataSubfolderName);
	NameAppendFile(szUndoPath, szUndoFile);

	CCTRACEI(_T("%s - Undo filename %d = %s"), __FUNCTION__, dwRemediationIndex, szUndoPath);

	*bUndoAvailable = ::PathFileExists(szUndoPath) ? true : false;
	return S_OK;
}


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetRemediationActionType(DWORD dwRemediationIndex, DWORD* dwValue)
{
	char szQueryName[MAX_PATH] = {0};
	sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_ACTION_TYPE, dwRemediationIndex);

	return GetDWORD(szQueryName, dwValue);
}


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetRemediationOperationType(DWORD dwRemediationIndex, DWORD* dwValue)
{
	char szQueryName[MAX_PATH] = {0};
	sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_OPERATION_TYPE, dwRemediationIndex);

	return GetDWORD(szQueryName, dwValue);
}


// ==== OriginalFileExists ================================================
// Tests if the original file exists at the original location.
//
// Input:  bResult - a pointer to a BOOL
//
// Output: TRUE if the file with the original path+filename exists,
//			FALSE if it does not
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================
STDMETHODIMP CQuarantineItem::OriginalFileExists(BOOL*  bResult)
{
    HRESULT retVal = S_OK;
	char szBuffer[MAX_QUARANTINE_FILENAME_LEN] = {0};
	DWORD dwBuffSize = MAX_QUARANTINE_FILENAME_LEN;

	retVal = GetString(QSERVER_ITEM_INFO_FILENAME_ANSI, szBuffer, &dwBuffSize);
	if(FAILED(retVal))
		*bResult = FALSE;
	else
		*bResult = ::PathFileExists(szBuffer);

	return retVal;
}  // end of "CQuarantineItem::OriginalFileExists"


// ==== ScanRepairMainFile ====================================================
// Scan the main file to determine if it has a virus.  Update and save the
// virus information structure.  Then attempt to repair the file.
//
// Input:
//   szDestPath - [out] Character buffer for folder + filename of the repaired
//				  file, if repairing.
//   dwPathSize - [out] Size of the buffer
//   bRepair    - TRUE if we want to try to repair the virus and keep the
//				  repaired file.
//				  FALSE if we just want to scan the file and report how it went
// Output: HRESULT
//   Multiple success codes
//     S_OK						 A virus was found
//     S_NO_VIRUS_FOUND			 No virus found
//     E_AVAPI_VS_CANT_REPAIR	 Non-deletable, repairable virus found
//     S_UNDELETABLE_VIRUS_FOUND Non-deletable, non-repairable virus found
// ========================================================================
// 3/11/98 Function created: JTAYLOR
// 5/21/98 JTAYLOR -- Added support for undeletable virus found return code.
// 6/05/98 JTAYLOR -- Changed the time for last scan to GMT.
// 6/22/98 JTAYLOR -- If no virus detected, then clear the virus info.
// ========================================================================
HRESULT CQuarantineItem::ScanRepairMainFile(char* szDestPath,
											DWORD dwPathSize,
											BOOL bRepair)
{
	// If we do not have a Scanner Class create one and initialize it
	if(!m_pQScanner)
	{
		m_pQScanner = new CSymStaticRefCount<CQScanner>;
		if(m_pQScanner == NULL)
			return E_FAIL;

		if(FAILED(m_pQScanner->Initialize(QUARANTINE_APP_ID)))
		{
			delete m_pQScanner;
			m_pQScanner = NULL;
			return E_FAIL;
		}
	}

    // Unpackage the main file to the temporary directory
	char szFolder[MAX_QUARANTINE_FILENAME_LEN] = "";
    BOOL bSuccess = GetQuarantinePath(szFolder, MAX_QUARANTINE_FILENAME_LEN, TARGET_QUARANTINE_TEMP_PATH);
    if(!bSuccess || (szFolder[0] == '\0'))
        return E_UNABLE_TO_GET_QUARANTINE_PATH;

	if(!CreateFolderIfNotExist(szFolder))
		return E_FAIL;

	char szGeneratedFilename[MAX_QUARANTINE_FILENAME_LEN] = "";
	HRESULT hr = UnpackageMainFile(szFolder, szGeneratedFilename, szGeneratedFilename, MAX_QUARANTINE_FILENAME_LEN, TRUE, TRUE);
    if(FAILED(hr))
        return hr;

	// Save the current scan time and definitions data
    SYSTEMTIME tTemp;
    HRESULT hrGetDefsDate = m_pQScanner->GetCurrentDefinitionsDate(&tTemp);

	CScanPtr<IScanInfection> pIVirus;
    SCANSTATUS result = m_pQScanner->Scan(szGeneratedFilename, &pIVirus);

    // If the Scan returned and error, then return its error
	HRESULT  hRetVal = E_UNEXPECTED;
    if(result != SCAN_OK)
    {
        hRetVal = result;
        goto Exit_Function;
    }

    // If the file was successfully scanned, updated the date/times
    if(result == SCAN_OK)
    {
        if(SUCCEEDED(hrGetDefsDate))
        {
            hr = SetDateOfLastScanDefs(&tTemp);
            if(FAILED(hr))
            {
                hRetVal = hr;
                goto Exit_Function;
            }
        }

        GetSystemTime(&tTemp);
        hr = SetDateOfLastScan(&tTemp);
        if(FAILED(hr))
        {
            hRetVal = hr;
            goto Exit_Function;
        }
    }

	BOOL bFoundVirus = FALSE;
	BOOL bCanDeleteItem = TRUE;
	DWORD dwFileType = 0;

    if(result == SCAN_OK && pIVirus != NULL) // Virus was found
    {
        BOOL bRepairSuccessful = FALSE;
        bFoundVirus = TRUE;

        // Update the virus information
		// Get the Virus Info object
		CScanPtr<IScanFileInfection> pFileInfection;
		if(SYM_SUCCEEDED(pIVirus->QueryInterface(IID_ScanFileInfection, (void**)&pFileInfection)))
			bCanDeleteItem = (pFileInfection->CanDelete() == true);
		else
			bCanDeleteItem = TRUE;
		
		SetVirusName(pIVirus->GetVirusName());
		SetVirusID(pIVirus->GetVirusID());

        // Add the category information to the item
        int nCatCount = pIVirus->GetCategoryCount();
        TCHAR szCategories[MAX_PATH] = {0};
        const unsigned long* pCategories = pIVirus->GetCategories();
        for(int nCatIndex=0; nCatIndex<nCatCount; nCatIndex++, pCategories++)
        {
            if(NULL != pCategories)
            {
                TCHAR szCurCat[10] = {0};
                _stprintf(szCurCat, _T("%u "), *pCategories);
                _tcsncat(szCategories, szCurCat, MAX_PATH);
            }
            else
			{
                break;
			}
        }
        SetString(QSERVER_ITEM_INFO_CATEGORIES, szCategories, MAX_PATH);

        // Find out if the file is supposed to be repairable.
        hr = GetFileType(&dwFileType);
        if(FAILED(hr))
        {
            hRetVal = hr;
            goto Exit_Function;
        }

        result = (m_pQScanner->RepairInfection(pIVirus));
        if(result != SCAN_OK)
        {
            // If the repair fails, and the file is supposed to be repairable
            // Mark it unrepairable.
            if(dwFileType & QFILE_TYPE_REPAIRABLE)
            {
                dwFileType = dwFileType - QFILE_TYPE_REPAIRABLE;

                // Since we have already encountered an error, do not report other errors
                // Update the type and save the new header.
                SetFileType(dwFileType);
            }

            SaveItem();
            hRetVal = E_AVAPI_VS_CANT_REPAIR;
            goto Exit_Function;
        }

        bRepairSuccessful = TRUE;

        // Add the repairable bit
        dwFileType = dwFileType | QFILE_TYPE_REPAIRABLE;

        hr = SetFileType(dwFileType);
        if(FAILED(hr))
        {
            hRetVal = hr;
            goto Exit_Function;
        }

        hr = SaveItem();
        if(FAILED(hr))
        {
            hRetVal = hr;
            goto Exit_Function;
        }

        if(bRepair)
        {
            // Copy the repaired filename
            STRNCPY(szDestPath, szGeneratedFilename, dwPathSize);

			DWORD dwFileStatus = 0;
			hr = GetFileStatus(&dwFileStatus);
			if(FAILED(hr))
			{
				dwFileStatus = QFILE_STATUS_BACKUP_FILE;
			}
			else
			{
				dwFileStatus &= ~QFILE_STATUS_QUARANTINED;
				dwFileStatus |= QFILE_STATUS_BACKUP_FILE;
			}
			hr = SetFileStatus(dwFileStatus);
            if(FAILED(hr))
            {
                hRetVal = hr;
                goto Exit_Function;
            }
        }
        else
        {
            // If the caller did not want a repaired copy, then delete the
            // repaired file
			TruncateFile(szGeneratedFilename);	
        }
    }
    else // Virus was NOT found
    {
        // We cannot detect a virus, Use a wipe on it.
        BOOL bDeleteSuccessfull = TruncateFile(szGeneratedFilename);
        if(FALSE == bDeleteSuccessfull)
        {
            hRetVal = E_FILE_DELETE_FAILED;
            goto Exit_Function;
        }

        // Clear out virus fields for the file since we cannot detect a virus
        ClearVirusName();
		ClearVirusID();
    }

    // Update the header.
    hr = SaveItem();
    if(FAILED(hr))
    {
        hRetVal = hr;
        goto Exit_Function;
    }

    // If there was a virus found then return S_OK
    // If there was no virus found return S_NO_VIRUS_FOUND
    HRESULT hRet = S_NO_VIRUS_FOUND;
    if(!bCanDeleteItem)
        hRet = S_UNDELETABLE_VIRUS_FOUND;
    else if(bFoundVirus)
        hRet = S_OK;

    return hRet;

Exit_Function:

    // There was an error, wipe the virus infected file.
	TruncateFile(szGeneratedFilename);
    
	// If the error is that we cannot repair the item, and we cannot
    // delete the item, then return that we cannot delete the item.   
	if((E_AVAPI_VS_CANT_REPAIR == hRetVal) && !bCanDeleteItem)
        return S_UNDELETABLE_VIRUS_FOUND;

    return hRetVal;
} // END CQuarantineItem::ScanRepairMainFile()


STDMETHODIMP CQuarantineItem::ScanFileRemediation(DWORD dwRemediationIndex)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	return RestoreFileRemediationToTempAndScanRepair(dwRemediationIndex, false, NULL, NULL, NULL, NULL);
}  // END CQuarantineItem::ScanFileRemediation


STDMETHODIMP CQuarantineItem::RepairAndRestoreFileRemediationToTemp(DWORD dwRemediationIndex,
										/*[out]*/ LPSTR szDestinationFolder,
									 /*[in,out]*/ DWORD* dwDestFolderSize,
										/*[out]*/ LPSTR szDestinationFilename,
									 /*[in,out]*/ DWORD* dwDestFilenameSize)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if((NULL == szDestinationFolder) || (NULL == dwDestFolderSize) 
		|| (NULL == szDestinationFilename) || (NULL == dwDestFilenameSize))
    {
		CCTRACEE(_T("%s - NULL output pointer(s)"), __FUNCTION__);
		return E_INVALIDARG;
    }

	return RestoreFileRemediationToTempAndScanRepair(dwRemediationIndex, 
													true, 
													szDestinationFolder, 
													dwDestFolderSize, 
													szDestinationFilename, 
													dwDestFilenameSize);
}  // END CQuarantineItem::RepairAndRestoreFileRemediationToTemp


// ==== RestoreFileRemediationToTempAndScanRepair ============================
// Input:
//  bKeepCleanFile - TRUE if we want to try to repair the virus and keep the
//					 repaired file.
//					 FALSE if we just want to scan the file and report how it went
//  Output: Multiple success codes
//     S_OK						 A virus was found
//     S_NO_VIRUS_FOUND			 No virus found
//     E_AVAPI_VS_CANT_REPAIR	 Non-deletable, repairable virus found
//     S_UNDELETABLE_VIRUS_FOUND Non-deletable, non-repairable virus found
HRESULT CQuarantineItem::RestoreFileRemediationToTempAndScanRepair(DWORD dwRemediationIndex, 
										/*[in]*/ bool bKeepCleanFile,
									   /*[out]*/ LPSTR szDestinationFolder,
									/*[in,out]*/ DWORD* dwDestFolderSize,
									   /*[out]*/ LPSTR szDestinationFilename,
									/*[in,out]*/ DWORD* dwDestFilenameSize)
{
	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	// If we do not have a Scanner Class create one and initialize it
	if(!m_pQScanner)
	{
		m_pQScanner = new CSymStaticRefCount<CQScanner>;
		if(m_pQScanner == NULL)
			return E_FAIL;

		if(FAILED(m_pQScanner->Initialize(QUARANTINE_APP_ID)))
		{
			delete m_pQScanner;
			m_pQScanner = NULL;
			return E_FAIL;
		}
	}

	TCHAR szScanFolder[MAX_QUARANTINE_FILENAME_LEN] = {0};
	TCHAR szScanFilename[MAX_QUARANTINE_FILENAME_LEN] = {0};
	DWORD dwScanFolderSize = MAX_QUARANTINE_FILENAME_LEN;
	DWORD dwScanFilenameSize = MAX_QUARANTINE_FILENAME_LEN;
	ccEraser::eResult eEraserResult;
	HRESULT hResult = RestoreFileRemediationToTemp(dwRemediationIndex, 
													szScanFolder, 
													&dwScanFolderSize, 
													szScanFilename, 
													&dwScanFilenameSize,
													&eEraserResult);
	if(FAILED(hResult))
	{
		CCTRACEE(_T("%s FAILED(RestoreFileRemediationToTemp()), hResult=0x%08X, eEraserResult=%d"), __FUNCTION__, hResult, eEraserResult);
		return hResult;
	}

	// Copy file/folder info to output params
	if((NULL != szDestinationFolder) && (NULL != dwDestFolderSize))
	{
		if(*dwDestFolderSize < dwScanFolderSize)
	{
		CCTRACEE(_T("%s Output buffer(s) too small"), __FUNCTION__);
		return E_BUFFER_TOO_SMALL;
	}

		_tcsncpy(szDestinationFolder, szScanFolder, *dwDestFolderSize);
		*dwDestFolderSize = dwScanFolderSize;
	}
	if((NULL != szDestinationFilename) && (NULL != dwDestFilenameSize))
	{
		if(*dwDestFilenameSize < dwScanFilenameSize)
		{
			CCTRACEE(_T("%s Output buffer(s) too small"), __FUNCTION__);
			return E_BUFFER_TOO_SMALL;
		}

		_tcsncpy(szDestinationFilename, szScanFilename, *dwDestFilenameSize);
		*dwDestFilenameSize = dwScanFilenameSize;
	}

	TCHAR szScanFilePath[MAX_QUARANTINE_FILENAME_LEN] = {0};
	_tcsncpy(szScanFilePath, szScanFolder, MAX_QUARANTINE_FILENAME_LEN);
	NameAppendFile(szScanFilePath, szScanFilename);
	
	CScanPtr<IScanInfection> pIVirus;
    SCANSTATUS ScanResult = m_pQScanner->Scan(szScanFilePath, &pIVirus);

    // If the Scan returned an error, return that error
	HRESULT hRetVal = E_UNEXPECTED;
    if(ScanResult != SCAN_OK)
    {
        hRetVal = ScanResult;
        goto Exit_Function;
    }

	bool bFoundVirus = false;
	bool bCanDeleteFile = true;

    if((ScanResult == SCAN_OK) && (pIVirus != NULL)) // Virus was found
    {
        bool bRepairSuccessful = false;
        bFoundVirus = true;

		// Get the Virus Info object
		CScanPtr<IScanFileInfection> pFileInfection;
		if(SYM_SUCCEEDED(pIVirus->QueryInterface(IID_ScanFileInfection, (void**)&pFileInfection)))
			bCanDeleteFile = pFileInfection->CanDelete();
		else
			bCanDeleteFile = true;
		
        ScanResult = m_pQScanner->RepairInfection(pIVirus);
        if(ScanResult != SCAN_OK)
        {
            hRetVal = E_AVAPI_VS_CANT_REPAIR;
            goto Exit_Function;
        }

        bRepairSuccessful = true;

        if(!bKeepCleanFile)
			TruncateFile(szScanFilePath); // Caller doesn't want a repaired/non-infected copy so delete file
    }
    else if(!bKeepCleanFile)  // Virus was NOT found, see if need to toss file
	{
		if(!TruncateFile(szScanFilePath))
		{
			hRetVal = E_FILE_DELETE_FAILED;
			goto Exit_Function;
		}
	}

    // If there was a virus found and successfully repaired, return S_OK
    // If there was no virus found return S_NO_VIRUS_FOUND
    if(!bCanDeleteFile)
        return S_UNDELETABLE_VIRUS_FOUND;
    else if(bFoundVirus)
        return S_OK;

    return S_NO_VIRUS_FOUND;

Exit_Function:

    // There was an error, wipe the virus infected file.
	TruncateFile(szScanFilePath);
    
	// If we can't repair the file AND can't delete the file, 
	// return that we can't delete the file.
	if((E_AVAPI_VS_CANT_REPAIR == hRetVal) && !bCanDeleteFile)
        return S_UNDELETABLE_VIRUS_FOUND;

    return hRetVal;
} // END CQuarantineItem::RestoreFileRemediationToTempAndScanRepair()


// ==== IsQuarantineFile ==================================================
//  Tests if the current object ("this") is a valid Quarantine file.
//
//  Output: TRUE if "this" is a valid Quarantine file, FALSE if it is not
// ========================================================================
//  Function created: 2/98, SEDWARD
// ========================================================================
BOOL CQuarantineItem::IsQuarantineFile()
{
    if(QSPAKSTATUS_OK == QsPakIsQserverFile(m_szQuarantineItemPath))
		return TRUE;

    return FALSE;
}  // end of "CQuarantineItem::IsQuarantineFile"


// ==== IsInitialized =====================================================
// Tests if the current object ("this") has been properly initialized.
// Currently we just make sure we have a handle
//
//  Output: TRUE if "this" has been properly initialized, FALSE if it has not
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================
BOOL    CQuarantineItem::IsInitialized()
{
    if(m_hQuarItem)
		return TRUE;
	else
		return FALSE;
}  // end of "CQuarantineItem::IsInitialized"


// ==== SysTimeToQSDate ==================================================
// Helper Function to Convert Date/Time Formats from SYSTEMTIME structure to
// QSPAKDATE structure
//
// Output: Translated QSPAKDATE Structure
////////////////////////////////////////////////////////////////////////////
QSPAKDATE CQuarantineItem::SysTimeToQSDate(SYSTEMTIME stSysTime)
{
	QSPAKDATE stQSDate;
	stQSDate.byMonth = (BYTE)stSysTime.wMonth;
	stQSDate.byDay = (BYTE)stSysTime.wDay;
	stQSDate.wYear = stSysTime.wYear;
	stQSDate.byHour = (BYTE)stSysTime.wHour;
	stQSDate.byMinute = (BYTE)stSysTime.wMinute;
	stQSDate.bySecond = (BYTE)stSysTime.wSecond;
	return stQSDate;
} // END CQuarantineItem::SysTimeToQSDate()


// ==== QSDateToSysTime ==================================================
// Helper Function to Convert Date/Time Formats from QSPAKDATE structure to
// SYSTEMTIME structure
//
// Output: Translated QSPAKDATE Structure
////////////////////////////////////////////////////////////////////////////
void CQuarantineItem::QSDateToSysTime(QSPAKDATE stQSDate, SYSTEMTIME* stSysTime)
{
	stSysTime->wMonth = (WORD)stQSDate.byMonth;
	stSysTime->wDay = (WORD)stQSDate.byDay;
	stSysTime->wYear = stQSDate.wYear;
	stSysTime->wHour = (WORD)stQSDate.byHour;	
	stSysTime->wMinute = (WORD)stQSDate.byMinute;
	stSysTime->wSecond = (WORD)stQSDate.bySecond;
	stSysTime->wMilliseconds = 0;
} // END CQuarantineItem::QSDateToSysTime()


HRESULT CQuarantineItem::GetString(char* szProperty, char* szValue, DWORD* pulBuffSize)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	QSPAKSTATUS qsResult = QsPakQueryItemValue(m_hQuarItem, szProperty, QSPAK_TYPE_STRING, szValue, pulBuffSize);
	if(QSPAKSTATUS_BUFFER_TOO_SMALL == qsResult)
	{
		return E_BUFFER_TOO_SMALL;
	}
	else if(QSPAKSTATUS_NO_SUCH_FIELD == qsResult)
	{
		return E_QSPAKERROR_NO_SUCH_FIELD;
	}
	else if(QSPAKSTATUS_OK != qsResult)
	{
		CCTRACEE(_T("%s - QsPakQueryItemValue(%s, QSPAK_TYPE_STRING) != QSPAKSTATUS_OK, qsResult=%d"), __FUNCTION__, szProperty, qsResult);
		return E_FAIL;
	}

	return S_OK;
} // END CQuarantineItem::GetString()


HRESULT CQuarantineItem::SetString(char* szProperty, char* szValue, DWORD dwBuffSize)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	if(NULL == szValue)
	{
		CCTRACEE(_T("%s - NULL input argument"), __FUNCTION__);
		return E_INVALIDARG;
	}

	QSPAKSTATUS qsResult = QsPakSetItemValue(m_hQuarItem, szProperty, QSPAK_TYPE_STRING, szValue, dwBuffSize);
	if(QSPAKSTATUS_OK != qsResult)
	{
		CCTRACEE(_T("%s - QsPakSetItemValue(%s, QSPAK_TYPE_STRING, %s) != QSPAKSTATUS_OK, qsResult=%d"), __FUNCTION__, szProperty, szValue, qsResult);
		return E_FAIL;
	}

	return S_OK;
} // END CQuarantineItem::SetString()


HRESULT CQuarantineItem::GetBinary(char* szProperty, BYTE* pBuff, DWORD* pulBuffSize)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

    if(!pBuff)
	{
		CCTRACEE(_T("%s - NULL output buffer"), __FUNCTION__);
        return E_INVALIDARG;
	}

	QSPAKSTATUS qsResult = QsPakQueryItemValue(m_hQuarItem, szProperty, QSPAK_TYPE_BINARY, pBuff, pulBuffSize);
	if(QSPAKSTATUS_NO_SUCH_FIELD == qsResult)
	{
		return E_QSPAKERROR_NO_SUCH_FIELD;
	}
	else if(QSPAKSTATUS_OK != qsResult)
	{
		CCTRACEE(_T("%s - QsPakQueryItemValue(%s, QSPAK_TYPE_BINARY) != QSPAKSTATUS_OK, qsResult=%d"), __FUNCTION__, szProperty, qsResult);
		return E_FAIL;
	}

	return S_OK;
} // END CQuarantineItem::GetBinary()


HRESULT CQuarantineItem::SetBinary(char* szProperty, BYTE* pBuff, DWORD dwBuffSize)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

    if(!pBuff)
	{
		CCTRACEE(_T("%s - NULL input"), __FUNCTION__);
        return E_INVALIDARG;
	}

	QSPAKSTATUS qsResult = QsPakSetItemValue(m_hQuarItem, szProperty, QSPAK_TYPE_BINARY, pBuff, dwBuffSize);
	if(QSPAKSTATUS_OK != qsResult)
	{
		CCTRACEE(_T("%s - QsPakSetItemValue(%s, QSPAK_TYPE_BINARY) != QSPAKSTATUS_OK, qsResult=%d"), __FUNCTION__, szProperty, qsResult);
		return E_FAIL;
	}

	return S_OK;
	
} // END CQuarantineItem::SetBinary()


HRESULT CQuarantineItem::GetDWORD(char* szProperty, DWORD* dwValue)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	if(NULL == dwValue)
	{
		CCTRACEE(_T("%s - NULL output argument"), __FUNCTION__);
		return E_INVALIDARG;
	}

	DWORD dwSize = sizeof(DWORD);
	QSPAKSTATUS qsResult = QsPakQueryItemValue(m_hQuarItem, szProperty, QSPAK_TYPE_DWORD, dwValue, &dwSize);
	if(QSPAKSTATUS_NO_SUCH_FIELD == qsResult)
	{
		return E_QSPAKERROR_NO_SUCH_FIELD;
	}
	else if(QSPAKSTATUS_OK != qsResult)
	{
		CCTRACEE(_T("%s - QsPakQueryItemValue(%s, QSPAK_TYPE_DWORD) != QSPAKSTATUS_OK, qsResult=%d"), __FUNCTION__, szProperty, qsResult);
		return E_FAIL;
	}

	return S_OK;
} // END CQuarantineItem::GetDWORD()


HRESULT CQuarantineItem::SetDWORD(char* szProperty, DWORD dwValue) 
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	QSPAKSTATUS qsResult = QsPakSetItemValue(m_hQuarItem, szProperty, QSPAK_TYPE_DWORD, &dwValue, sizeof(DWORD));
	if(QSPAKSTATUS_OK != qsResult)
	{
		CCTRACEE(_T("%s - QsPakSetItemValue(%s, QSPAK_TYPE_DWORD, %d) != QSPAKSTATUS_OK, qsResult=%d"), __FUNCTION__, szProperty, dwValue, qsResult);
		return E_FAIL;
	}

	return S_OK;
} // END CQuarantineItem::SetDWORD()


HRESULT CQuarantineItem::GetDate(char* szProperty, SYSTEMTIME* stTime)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	if(NULL == stTime)
	{
		CCTRACEE(_T("%s - NULL output argument"), __FUNCTION__);
		return E_INVALIDARG;
	}

	DWORD dwQSDateSize = sizeof(QSPAKDATE);
	QSPAKDATE date;

	QSPAKSTATUS qsResult = QsPakQueryItemValue(m_hQuarItem, szProperty, QSPAK_TYPE_DATE, &date, &dwQSDateSize);
	if(QSPAKSTATUS_NO_SUCH_FIELD == qsResult)
	{
		return E_QSPAKERROR_NO_SUCH_FIELD;
	}
	else if(QSPAKSTATUS_OK != qsResult)
	{
		CCTRACEE(_T("%s - QsPakQueryItemValue(%s, QSPAK_TYPE_DATE) != QSPAKSTATUS_OK, qsResult=%d"), __FUNCTION__, szProperty, qsResult);
		return E_FAIL;
	}

	QSDateToSysTime(date, stTime);
	return S_OK;
} // END CQuarantineItem::GetDate()


HRESULT CQuarantineItem::SetDate(char* szProperty, SYSTEMTIME* stTime)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	if(NULL == stTime)
	{
		CCTRACEE(_T("%s - NULL input argument"), __FUNCTION__);
		return E_INVALIDARG;
	}

	QSPAKDATE date = SysTimeToQSDate(*stTime);

	QSPAKSTATUS qsResult = QsPakSetItemValue(m_hQuarItem, szProperty, QSPAK_TYPE_DATE, &date, sizeof(QSPAKDATE));
	if(QSPAKSTATUS_OK != qsResult)
	{
		CCTRACEE(_T("%s - QsPakSetItemValue(%s, QSPAK_TYPE_DATE) != QSPAKSTATUS_OK, qsResult=%d"), __FUNCTION__, szProperty, qsResult);
		return E_FAIL;
	}
	return S_OK;
} // END CQuarantineItem::SetDate()


HRESULT CQuarantineItem::RestoreCCGserSideEffects(char* szRestoredFileName)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

    if((NULL == szRestoredFileName)  ||  (NULL == *szRestoredFileName))
    {
        CCTRACEE(_T("%s - No restored file name specified."), __FUNCTION__);
        return E_INVALIDARG;
    }

    // Find out if there are side effect items to restore
    DWORD dwCount = 0;
    DWORD dwSize = sizeof(DWORD);
    if(QSPAKSTATUS_OK == QsPakQueryItemValue(m_hQuarItem, QSERVER_ITEM_SE_COUNT, QSPAK_TYPE_DWORD, &dwCount, &dwSize) && dwCount > 0)
    {
        // Get each side effect to restore
        char szQueryName[MAX_PATH] = {0};
        char* pszValue = new char[MAX_PATH*2];

        // Buffer used for side effect item data
        BYTE* pBuff = NULL;

        if(!pszValue)
        {
            CCTRACEE(_T("%s - Failed to allocate memory"), __FUNCTION__);
            return E_OUTOFMEMORY;
        }
        dwSize = MAX_PATH*2;
        DWORD dwCurBufSize = dwSize;

        for(DWORD curNum=0; curNum<dwCount; curNum++)
        {
            // Get the type for the current item to retrieve
            sprintf(szQueryName, QSERVER_ITEM_SE_TYPE, curNum);

            DWORD dwType = 0;
            dwSize = sizeof(DWORD);
            if(QSPAKSTATUS_OK == QsPakQueryItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_DWORD, &dwType, &dwSize))
            {
                // Now get the first data item for this side effect
                sprintf(szQueryName, QSERVER_ITEM_SE_DATA, curNum, 0);
                dwSize = dwCurBufSize;
				QSPAKSTATUS qsResult = QsPakQueryItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_STRING, pszValue, &dwSize);
				if(QSPAKSTATUS_OK == qsResult)
				{
                    // Startup folder ?
                    if(SE_TYPE_STARTUP == dwType)
                    {
                        // Don't have to do anything for startup folder items
                        CCTRACEI(_T("%s - Nothing to do for startup folder side effect restore. Side effect data = %s. Actual restored file = %s."), __FUNCTION__, pszValue, szRestoredFileName);
                        continue;
                    }
                    // Registry key ?
                    else if(SE_TYPE_REGKEY == dwType)
                    {
                        // Get the root key
                        HKEY hRootKey = NULL;
                        std::string strFullKey = pszValue;
                        std::string strTemp = strFullKey.substr(0,strFullKey.find_first_of('\\'));;
                        if(strTemp.compare("HKEY_LOCAL_MACHINE") == 0)
                            hRootKey = HKEY_LOCAL_MACHINE;
                        else if(strTemp.compare("HKEY_CURRENT_USER") == 0)
                            hRootKey = HKEY_CURRENT_USER;
                        else if(strTemp.compare("HKEY_CLASSES_ROOT") == 0)
                            hRootKey = HKEY_CLASSES_ROOT;
                        else if(strTemp.compare("HKEY_USERS") == 0)
                            hRootKey = HKEY_USERS;
                        else if(strTemp.compare("HKEY_CURRENT_CONFIG") == 0)
                            hRootKey = HKEY_CURRENT_CONFIG;
                        else
                        {
                            CCTRACEE(_T("%s - Failed to get root key for reg key side effect %s"), __FUNCTION__, strFullKey.c_str());
                            continue;
                        }

                        // Get the subkey
                        strTemp = strFullKey.substr(strFullKey.find_first_of('\\')+1);
                        strTemp.erase(strTemp.find_last_of('\\'));

                        // Open the registry key
                        CRegKey regKey;
                        if(ERROR_SUCCESS != regKey.Open(hRootKey, strTemp.c_str()))
                        {
                            CCTRACEW(_T("%s - Failed to open the registry key %s. Subkey text = %s. Will attempt to create the key"), __FUNCTION__, strFullKey.c_str(), strTemp.c_str());
                            if(ERROR_SUCCESS != regKey.Create(hRootKey, strTemp.c_str()))
                            {
                                CCTRACEE(_T("%s - Failed to create the registry key %s."), __FUNCTION__, strTemp.c_str());
                                continue;
                            }
                            CCTRACEI(_T("%s - Successfully created the registry key %s."), __FUNCTION__, strTemp.c_str());
                        }

                        // Get the value name for this key
                        strTemp = strFullKey.substr(strFullKey.find_last_of('\\')+1);

                        // Check if this value already exists
                        dwSize = dwCurBufSize;
                        if(ERROR_SUCCESS == regKey.QueryValue(strTemp.c_str(), &dwType, pszValue, &dwSize))
                        {
                            CCTRACEW(_T("%s - Not restoring side effect %s, because the key already exists with value %s."), __FUNCTION__, strFullKey.c_str(), pszValue);
                            continue;
                        }

                        // Get the type of reg key
                        sprintf(szQueryName, QSERVER_ITEM_SE_DATA, curNum, 1);
                        DWORD dwRegType = 0;
                        dwSize = sizeof(DWORD);
                        if(QSPAKSTATUS_OK != QsPakQueryItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_DWORD, &dwRegType, &dwSize))
                        {
                            CCTRACEE(_T("%s - Failed to get the registry key type for key %s. Skipping this side effect restore."), __FUNCTION__, strFullKey.c_str());
                            continue;
                        }

                        CCTRACEI(_T("%s - Reg key side effect %s is of data type %d"), __FUNCTION__, strFullKey.c_str(), dwRegType);

                        // Is this the string value that needs to have the restored file name inserted?
                        bool bUseRestoreFileName = false;
                        sprintf(szQueryName, QSERVER_ITEM_SE_DATA, curNum, 2);
                        dwType = 0;
                        dwSize = sizeof(DWORD);
                        if(QSPAKSTATUS_OK == QsPakQueryItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_DWORD, &dwType, &dwSize)
                            && dwType == 1)
                        {
                            CCTRACEI(_T("%s - This is the key that requires the side effect file name. key = %s"), __FUNCTION__, strFullKey.c_str());
                            bUseRestoreFileName = true;
                        }

                        // Get the data to store in this key
                        sprintf(szQueryName, QSERVER_ITEM_SE_DATA, curNum, 3);
                        pBuff = NULL;
                        dwSize = 0;
                        
                        QSPAKSTATUS qStat = QsPakQueryItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_BINARY, pBuff, &dwSize);
                        // First query for the data buffer size necessary
                        if(QSPAKSTATUS_BUFFER_TOO_SMALL == qStat)
                        {
                            // This item exists so allocate a buffer to hold it plus an extra spot for a
                            // terminating NULL
                            pBuff = new BYTE[dwSize+1];
                            if(pBuff == NULL)
                            {
                                CCTRACEE(_T("%s - Unable to allocate a buffer of size %d to get the reg key data for key %s. Skipping this item."), __FUNCTION__, dwSize, strFullKey.c_str());
                                continue;
                            }

                            // Clear out the buffer
                            ZeroMemory(pBuff, dwSize+1);

                            // Now get the data
                            qStat = QsPakQueryItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_BINARY, pBuff, &dwSize);
                            if(QSPAKSTATUS_OK != qStat)
                            {
                                CCTRACEE(_T("%s - Failed to get the data for reg key side effect %s. Skipping this item. QSPAKSTATUS error = %d."), __FUNCTION__, strFullKey.c_str(), qStat);
                                if(pBuff)
                                {
                                    delete [] pBuff;
                                    pBuff = NULL;
                                }
                                continue;
                            }

                            if(dwRegType == REG_SZ || dwRegType == REG_EXPAND_SZ || dwRegType == REG_MULTI_SZ)
                                CCTRACEI(_T("%s - Successfully retrieved string-type data to restore. Key = %s. Data = %s"), __FUNCTION__, strFullKey.c_str(), (LPTSTR)pBuff);
                            else if(dwRegType == REG_DWORD || dwRegType == REG_QWORD)
                                CCTRACEI(_T("%s - Successfully retrieved DWORD/QWORD data to restore. Key = %s. Data = %d"), __FUNCTION__, strFullKey.c_str(),(DWORD)*pBuff);
                            else
                                CCTRACEI(_T("%s - Successfully retrieved BINARY data to restore. Key = %s. Data in string form = %s"), __FUNCTION__, strFullKey.c_str(), (LPTSTR)pBuff);
                        }
                        else if(QSPAKSTATUS_NO_SUCH_FIELD == qStat && bUseRestoreFileName)
                        {
                            CCTRACEI(_T("%s - Restoring reg key side effect with just the side effect file name as the data: %s. Key = %s"), __FUNCTION__, szRestoredFileName, strFullKey.c_str());
                        }
                        else
                        {
                            CCTRACEE(_T("%s - Failed to get the data for reg side effect %s. This was not the side effect file name key so restore will be aborted. QSPAKSTATUS error = %d."), __FUNCTION__, strFullKey.c_str(), qStat);
                            continue;
                        }

                        // If this is the item that needs the side effect file use special handling for it
                        if(bUseRestoreFileName)
                        {
                            std::string strDataToStore("\"");       // open quote
                            strDataToStore += szRestoredFileName;   // file name
                            strDataToStore += "\"";                // close quote

                            // If there is data to append to the file name add it first
                            if(pBuff)
                            {
                                strDataToStore += (LPTSTR)pBuff;
                                delete [] pBuff;
                                pBuff = NULL;
                            }

                            // Set the value
                            if(ERROR_SUCCESS != regKey.SetStringValue(strTemp.c_str(), strDataToStore.c_str()))
                            {
                                CCTRACEE(_T("%s - Failed to set the value for registry key %s. Value = %s. Data = %s"), __FUNCTION__, strFullKey.c_str(), strTemp.c_str(), strDataToStore.c_str());
                                continue;
                            }
                            CCTRACEI(_T("%s - Successfully restored side effect registry key: %s, with side effect file data: %s"), __FUNCTION__, strFullKey.c_str(), strDataToStore.c_str());
                        }
                        // Just drop in what we have
                        else if(pBuff)
                        {
                            if(ERROR_SUCCESS != regKey.SetValue(strTemp.c_str(), dwRegType, pBuff, dwSize))
                                CCTRACEE(_T("%s - Failed to set the value for registry key %s. Value = %s. Value type: %d"), __FUNCTION__, strFullKey.c_str(), dwRegType);
                            else
                                CCTRACEI(_T("%s - Successfully restored side effect registry key: %s. Value type: %d"), __FUNCTION__, strFullKey.c_str(), dwRegType);

                            // Free the buffer
                            delete [] pBuff;
                            pBuff = NULL;
                        }

                        // Done
                        continue;
                    }
                    // INI File ?
                    else if(SE_TYPE_INI == dwType)
                    {
                        // pszValue is currently Data0 which is the INI file name
                        std::string strFile = pszValue;

                        // Get the Section (Data1)
                        sprintf(szQueryName, QSERVER_ITEM_SE_DATA, curNum, 1);
                        dwSize = dwCurBufSize;
                        if(QSPAKSTATUS_OK != QsPakQueryItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_STRING, pszValue, &dwSize))
                        {
                            CCTRACEE(_T("%s - Failed to get the INI file section."), __FUNCTION__);
                            continue;
                        }

                        std::string strSection = pszValue;

                        // Get the Value (Data2)
                        sprintf(szQueryName, QSERVER_ITEM_SE_DATA, curNum, 2);
                        dwSize = dwCurBufSize;
                        if(QSPAKSTATUS_OK != QsPakQueryItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_STRING, pszValue, &dwSize))
                        {
                            CCTRACEE(_T("%s - Failed to get the INI value."), __FUNCTION__);
                            continue;
                        }

                        // First see if this already exists
                        szQueryName[0] = NULL;
                        if(0 != GetPrivateProfileString(strSection.c_str(), pszValue, _T(""), szQueryName, MAX_PATH, strFile.c_str()) && _tcslen(szQueryName))
                        {
                            CCTRACEW(_T("%s - There is already an ini file entry for file file %s, section %s with value %s. Not writing out data %s to this ini file."), __FUNCTION__, strFile.c_str(), strSection.c_str(), szQueryName, szRestoredFileName);
                            continue;
                        }

                        if(!WritePrivateProfileString(strSection.c_str(), pszValue, szRestoredFileName, strFile.c_str()))
                        {
                            CCTRACEE(_T("%s - Failed to write out the ini file side effect to file %s, section %s, value %s"), __FUNCTION__, strFile.c_str(), strSection.c_str(), pszValue);
                            continue;
                        }

                        CCTRACEI(_T("%s - Successfully restored side effect ini file %s, section %s, value %s"), __FUNCTION__, strFile.c_str(), strSection.c_str(), pszValue);
                        continue;
                    }
                    // Batch File ?
                    else if(SE_TYPE_BATCH == dwType)
                    {
                        // pszValue is the batch file we need to write to

                        // Open batch file for read/write.
	                    HANDLE hBatchFile = CreateFile(pszValue, GENERIC_READ | GENERIC_WRITE,
		                    FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	                    if(hBatchFile == INVALID_HANDLE_VALUE)
	                    {
		                    CCTRACEE(_T("%s - Unable to open %s."), __FUNCTION__, pszValue);
		                    continue;
	                    }

                        // Seek to the last byte of the batch file
                        if(INVALID_SET_FILE_POINTER == SetFilePointer(hBatchFile, -1, NULL, FILE_END))
                        {
                            CloseHandle(hBatchFile);
                            CCTRACEE(_T("%s - Unable to seek to the end of the batch file %s."), pszValue);
		                    continue;
                        }

                        // Read in the last byte
                        char chLastChar = '\0';
                        DWORD dwNumBytes = 0;
                        if(ReadFile(hBatchFile, (LPVOID) &chLastChar, 1, &dwNumBytes, NULL) == FALSE)
		                {
                            CloseHandle(hBatchFile);
			                CCTRACEE(_T("%s - Unable to read the last character from batch file %s."), __FUNCTION__, pszValue);
                        }

                        // Add a newline if needed
                        char szNewLine[3] = "\r\n";
                        if(chLastChar != '\n')
                        {
                            if(WriteFile(hBatchFile,
							            (LPVOID) szNewLine,
							            2,
							            &dwNumBytes,
							            NULL) == FALSE)
				            {
                                CCTRACEE(_T("%s - Unable to write the newline to %s."), __FUNCTION__, pszValue);
                            }
                        }

                        // Write out the side effect data
                        if(WriteFile(hBatchFile,
							        (LPVOID) szRestoredFileName,
							        (DWORD) strlen(szRestoredFileName),
							        &dwNumBytes,
							        NULL) == FALSE)
				        {
                            CCTRACEE(_T("%s - Unable to write the data to batch file %s."), __FUNCTION__,pszValue);
                        }
                        else
                        {
                            // append a newline
                            WriteFile(hBatchFile, (LPVOID) szNewLine, 2, &dwNumBytes, NULL);
                            CCTRACEI(_T("%s - Successfully restored data %s to batch file %s."), __FUNCTION__, szRestoredFileName, pszValue);
                        }

                        CloseHandle(hBatchFile);
                        continue;
                    }
                    // Who knows ?
                    else
                    {
                        CCTRACEE(_T("%s - Unknown side effect type: %d"), __FUNCTION__, dwType);
                        continue;
                    }
                }
                // Bigger buffer necessary?
                else if((QSPAKSTATUS_BUFFER_TOO_SMALL == qsResult) && pszValue && dwCurBufSize < dwSize)
                {
                    CCTRACEI(_T("%s - Need to allocate a bigger buffer. New size: %d"), __FUNCTION__, dwSize);
                    delete [] pszValue;
                    pszValue = new char [dwSize];
                    if(pszValue)
                    {
                        // Set the new buffer size
                        dwCurBufSize = dwSize;

                        // Re-try this query
                        --curNum;
                    }
                }
            }
            // Failed to get the side effect type
            else
            {
                CCTRACEE(_T("%s - Failed to query the side effect type for side effect %d"), __FUNCTION__, curNum);
            }
        }

        if(pszValue)
            delete [] pszValue;
    }

    return S_OK;
} // END CQuarantineItem::RestoreCCGserSideEffects()


// Added for NAV 2005.5/r11.5
// Just the filename, not the path
bool CQuarantineItem::GenerateRemeditionDataName(LPTSTR szFilename, bool bRemediationAction, DWORD dwCount)
{
	if(NULL == szFilename)
		return false;

	_tcscpy(szFilename, _T(""));

	if(dwCount >= 0)
	{
		if(bRemediationAction)
			sprintf(szFilename, REMEDIATION_ACTION_FILE_NAME_FORMAT, dwCount);
		else
			sprintf(szFilename, REMEDIATION_UNDO_FILE_NAME_FORMAT, dwCount);
	}

	if((dwCount < 0) || (_tcslen(szFilename) <= 0))
		return false;

	return true;
} // END CQuarantineItem::GenerateRemeditionDataName()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetRemediationCount(DWORD* dwCount)
{
	HRESULT hResult = GetDWORD(QSERVER_ITEM_REMEDIATION_COUNT, dwCount);
	if(E_QSPAKERROR_NO_SUCH_FIELD == hResult)
	{
		CCTRACEI(_T("%s - Field QSERVER_ITEM_REMEDIATION_COUNT does not exist, just return 0"), __FUNCTION__);
		*dwCount = 0;
		hResult = S_OK;
	}

	return hResult;
} // END CQuarantineItem::GetRemediationCount()


// Added for NAV 2005.5/r11.5
HRESULT CQuarantineItem::SetRemediationCount(DWORD dwCount)
{
	return SetDWORD(QSERVER_ITEM_REMEDIATION_COUNT, dwCount);
}


// Added for NAV 2005.5/r11.5
HRESULT CQuarantineItem::IncrementRemediationCount(DWORD* dwRemCount)
{
	DWORD dwCount = 0;
	HRESULT hResult = GetRemediationCount(&dwCount);
	if(SUCCEEDED(hResult) || (E_QSPAKERROR_NO_SUCH_FIELD == hResult))
	{
		dwCount++;
		hResult = SetRemediationCount(dwCount);
	}
	else
	{
		dwCount = -1;
	}

	if(NULL != dwRemCount)
		*dwRemCount = dwCount;

	return hResult;
} // END CQuarantineItem::IncrementRemediationCount()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetRemediationFailureCount(DWORD* dwCount)
{
	HRESULT hResult = GetDWORD(QSERVER_ITEM_REMEDIATION_FAILURE_COUNT, dwCount);
	if(E_QSPAKERROR_NO_SUCH_FIELD == hResult)
	{
		CCTRACEI(_T("%s - Field QSERVER_ITEM_REMEDIATION_FAILURE_COUNT does not exist, just return 0"));
		*dwCount = 0;
		hResult = S_OK;
	}

	return hResult;
} // END CQuarantineItem::GetRemediationFailureCount()


// Added for NAV 2005.5/r11.5
HRESULT CQuarantineItem::SetRemediationFailureCount(DWORD dwCount)
{
	return SetDWORD(QSERVER_ITEM_REMEDIATION_FAILURE_COUNT, dwCount);
}


// Added for NAV 2005.5/r11.5
HRESULT CQuarantineItem::IncrementRemediationFailureCount(DWORD* dwFailCount)
{
	DWORD dwCount = 0;
	HRESULT hResult = GetRemediationFailureCount(&dwCount);
	if(SUCCEEDED(hResult) || (E_QSPAKERROR_NO_SUCH_FIELD == hResult))
	{
		dwCount++;
		hResult = SetRemediationFailureCount(dwCount);
	}
	else
	{
		dwCount = -1;
	}

	if(NULL != dwFailCount)
		*dwFailCount = dwCount;

	return hResult;
} // END CQuarantineItem::IncrementRemediationFailureCount()


// Added for NAV 2005.5/r11.5
bool CQuarantineItem::IsSuccessfulRemediation(ccEraser::IRemediationAction* pRemediation)
{
	bool bRet = false;
	cc::IKeyValueCollectionPtr pProps;

	if(!ccEraser::Succeeded(pRemediation->GetProperties(pProps.m_p)))
	{
		CCTRACEE(_T("%s - Can't get properties"), __FUNCTION__);
		return bRet;
	}

	DWORD dwState = ccEraser::IRemediationAction::NotRemediated;
	if(!pProps->GetValue(ccEraser::IRemediationAction::State, dwState))
	{
		CCTRACEE(_T("%s - Error, failed to get State"), __FUNCTION__);
		return bRet;
	}
	
	if(ccEraser::IRemediationAction::Remediated == dwState)
	{
		bRet = true;
	}

	return bRet;
} // END CQuarantineItem::IsSuccessfulRemediation()


// Added for NAV 2005.5/r11.5
ccEraser::eResult CQuarantineItem::GetRemediationActionType(ccEraser::IRemediationAction* pRemediation, ccEraser::eObjectType& objectType)
{
	objectType = static_cast<ccEraser::eObjectType>(-1);

	ccEraser::eResult eEraserRes = pRemediation->GetType(objectType);
	if(!ccEraser::Succeeded(eEraserRes))
		CCTRACEE(_T("%s Can't get eObjectType, eRes=%d"), __FUNCTION__, eEraserRes);
	else
		CCTRACEI(_T("%s - objectType=%d, eRes=%d"), __FUNCTION__, (DWORD)objectType, eEraserRes);

	return eEraserRes;
} // END CQuarantineItem::GetRemediationActionType()


// Added for NAV 2005.5/r11.5
DWORD CQuarantineItem::GetRemediationOperationType(ccEraser::IRemediationAction* pRemediation)
{
	cc::IKeyValueCollectionPtr pProps;
	if(!ccEraser::Succeeded(pRemediation->GetProperties(pProps.m_p)))
	{
		CCTRACEE(_T("%s - Can't get properties"), __FUNCTION__);
		return -1;
	}

	DWORD dwOperation_Type = -1;
	if(!pProps->GetValue(ccEraser::IRemediationAction::Operation, dwOperation_Type))
	{
		CCTRACEE(_T("%s - Error, failed to get Operation_Type"), __FUNCTION__);
		return -1;
	}

	return dwOperation_Type;
} // END CQuarantineItem::GetRemediationOperationType()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::CreateDataSubfolderIfNotExist(/*[out]*/ LPTSTR szDataFolderName, /*[out]*/ DWORD* dwBuffSize)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	// Does subfolder field exist yet
	DWORD dwSaveSize = *dwBuffSize;
	HRESULT hResult = GetString(QSERVER_ITEM_INFO_DATA_SUBFOLDER, szDataFolderName, dwBuffSize);
	if(FAILED(hResult))
	{
		// Generate data subfolder name
		TCHAR szQuarFolder[MAX_QUARANTINE_FILENAME_LEN] = {0};
		BOOL bSuccess = GetQuarantinePath(szQuarFolder, MAX_QUARANTINE_FILENAME_LEN, TARGET_QUARANTINE_PATH);
		if(!bSuccess || (szQuarFolder[0] == '\0'))
			return E_UNABLE_TO_GET_QUARANTINE_PATH;
		
		TCHAR szGUID[MAX_QUARANTINE_FILENAME_LEN] = {0};
		DWORD dwBufferSize = MAX_QUARANTINE_FILENAME_LEN;
		HRESULT hResult = GetString(QSERVER_ITEM_INFO_UUID, szGUID, &dwBufferSize);
		if(FAILED(hResult))
			return hResult;
		
		TCHAR szDataSubfolderTemp[MAX_QUARANTINE_FILENAME_LEN] = {0};
		_tcscpy(szDataSubfolderTemp, szQuarFolder);
		NameAppendFile(szDataSubfolderTemp, szGUID);

		if(dwSaveSize <= _tcslen(szDataSubfolderTemp))
			return E_FAIL;

		_tcscpy(szDataFolderName, szDataSubfolderTemp);

		hResult = SetString(QSERVER_ITEM_INFO_DATA_SUBFOLDER, szDataFolderName, strlen(szDataFolderName));
		if(FAILED(hResult))
		{
			CCTRACEE(_T("%s - Unable to save folder name, hResult=0x%08X"), __FUNCTION__, hResult);
			return hResult;
		}
	}

	return (CreateFolderIfNotExist(szDataFolderName) ? S_OK : E_FAIL);

} // END CreateDataSubfolderIfNotExist


// Added for NAV 2005.5/r11.5
bool CQuarantineItem::CreateFolderIfNotExist(LPTSTR szFolderName)
{
	// Does subfolder exist yet
	DWORD dwAttributes = ::GetFileAttributes(szFolderName);
	if((INVALID_FILE_ATTRIBUTES != dwAttributes) 
		&& ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY))
	{ 
		CCTRACEI(_T("%s - Folder(%s) already exists"), __FUNCTION__, szFolderName);
	} 
	else
	{
		if(::CreateDirectory(szFolderName, NULL))
		{
			CCTRACEI(_T("%s - CreateDirectory(%s) succeeded"), __FUNCTION__, szFolderName);
		}
		else
		{ 
			CCTRACEE(_T("%s - CreateDirectory(%s) failed"), __FUNCTION__, szFolderName);
			return false;
		} 
	}

	return true;
} // END CQuarantineItem::CreateFolderIfNotExist()


// Added for NAV 2005.5/r11.5
// Keep in synch with CEZRemediation::GetDisplayString()
HRESULT CQuarantineItem::SaveRemediationDescription(DWORD dwRemediationIndex, ccEraser::IRemediationAction* pRemediation, ccEraser::eObjectType eType)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	cc::IKeyValueCollectionPtr pProps;
	if(ccEraser::Failed(pRemediation->GetProperties(pProps)))
	{
		CCTRACEE(_T("%s - Failed(pRemediationAction->GetProperties())"), __FUNCTION__);
		return E_FAIL;
	}
		
	cc::IStringPtr pDesc;

	switch(eType)
	{
	case ccEraser::InfectionRemediationActionType:
		//if(ccEraser::InfectionRemediationActionType == eType)
		{
			// Scan manager puts the description for infection remediations in the
			// remediation action user data
			ISymBasePtr pSymBase;
			if(!pProps->GetValue(ccEraser::IRemediationAction::UserData, pSymBase))
			{
				CCTRACEE(_T("%s - Error getting the remediation user data for scan infection item"), __FUNCTION__);
				return E_FAIL;
			}

			cc::IKeyValueCollectionQIPtr pUserData = pSymBase;
			if(pUserData.m_p == NULL)
			{
				CCTRACEE(_T("%s - Error QI'ing for the remediation UserData values for scan infection item"), __FUNCTION__);
				return E_FAIL;
			}

			if(!pUserData->GetValue(RemediationDescription, reinterpret_cast<ISymBase*&>(pDesc)))
			{
				CCTRACEE(_T("%s - Error getting the remediation description for scan infection item"), __FUNCTION__);
				return E_FAIL;
			}
			break;
		}
	case ccEraser::FileRemediationActionType:
	case ccEraser::DirectoryRemediationActionType:
	case ccEraser::ProcessRemediationActionType:
	case ccEraser::BatchRemediationActionType:
	case ccEraser::INIRemediationActionType:
	case ccEraser::COMRemediationActionType:
		{
			if(!pProps->GetValue(ccEraser::IRemediationAction::Path, reinterpret_cast<ISymBase*&>(pDesc)))
			{
				CCTRACEE(_T("%s - !pProps->GetValue(ccEraserIRemediationAction::Path...)"), __FUNCTION__);
				return E_FAIL;
			}
			break;
		}
	case ccEraser::ServiceRemediationActionType:
		{
			if(!pProps->GetValue(ccEraser::IRemediationAction::ServiceName, reinterpret_cast<ISymBase*&>(pDesc)))
			{
				CCTRACEE(_T("%s - !pProps->GetValue(ccEraserIRemediationAction::ServiceName...)"), __FUNCTION__);
				return E_FAIL;
			}
			break;
		}
	case ccEraser::RegistryRemediationActionType:
		{
			if(!pProps->GetValue(ccEraser::IRemediationAction::KeyName, reinterpret_cast<ISymBase*&>(pDesc)))
			{
				CCTRACEE(_T("%s - !pProps->GetValue(ccEraserIRemediationAction::KeyName...)"), __FUNCTION__);
				return E_FAIL;
			}
			break;
		}
	case ccEraser::HostsRemediationActionType:
		{
			ATL::CAtlString strTemp;

			// Get the hosts Domain Name entry
			if(!pProps->GetValue(ccEraser::IRemediationAction::DomainName, reinterpret_cast<ISymBase*&>(pDesc)) || (pDesc == NULL))
			{
				CCTRACEW(_T("%s - Error getting the Hosts remediation action domain name."), __FUNCTION__);
			}
			else
			{
				strTemp = pDesc->GetStringA();
				pDesc.Release();
			}

			// Get the hosts IP address
			if(!pProps->GetValue(ccEraser::IRemediationAction::IPAddress, reinterpret_cast<ISymBase*&>(pDesc)) || (pDesc == NULL))
				CCTRACEW(_T("%s - Error getting the Hosts remediation action IP address."), __FUNCTION__);

			if(strTemp.IsEmpty() && (pDesc == NULL))
			{
				CCTRACEE(_T("%s - Error getting any Hosts remediation action display data."), __FUNCTION__);
				return E_FAIL;
			}

			// Concat the Domain name and IP address if we have both
			if(!strTemp.IsEmpty())
			{
				if(pDesc != NULL)
				{
					// Append the IP Address to the host name
					strTemp += " - ";
					strTemp += pDesc->GetStringA();
					pDesc.Release();
				}

				pDesc = ccSym::CStringImpl::CreateStringImpl(strTemp);
			}
			break;
		}
	case ccEraser::LSPRemediationActionType:
		{
			if(!pProps->GetValue(ccEraser::IRemediationAction::ProviderID, reinterpret_cast<ISymBase*&>(pDesc)) || (pDesc == NULL))
			{
				CCTRACEW(_T("%s - Failed to get the LSP remediation action provider ID. Attempting to use the path property."), __FUNCTION__);

				if(!pProps->GetValue(ccEraser::IRemediationAction::Path, reinterpret_cast<ISymBase*&>(pDesc)))
				{
					CCTRACEE(_T("%s - !pProps->GetValue(ccEraserIRemediationAction::Path...)"), __FUNCTION__);
					return E_FAIL;
				}
			}
			break;
		}
	}

	if(!pDesc || (pDesc->GetLength() <= 0))
	{
		CCTRACEE(_T("%s - No Remediation description generated"), __FUNCTION__);
		return S_FALSE;
	}

    CString strRemediationDescription = pDesc->GetStringA();

	char szQueryName[MAX_PATH] = {0};
	sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_DESCRIPTION, dwRemediationIndex);
	QSPAKSTATUS qsResult = QsPakSetItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_STRING, strRemediationDescription.GetBuffer(pDesc->GetLength()+1), pDesc->GetLength());
	strRemediationDescription.ReleaseBuffer();

	if(QSPAKSTATUS_OK != qsResult)
	{
		CCTRACEE(_T("%s - QsPakSetItemValue(%s)"), __FUNCTION__, szQueryName);
		return E_FAIL;
	}

	if(FAILED(SaveItem()))
	{
		CCTRACEE(_T("%s - SaveItem()"), __FUNCTION__);
		return E_FAIL;
	}

	return S_OK;
} // END CQuarantineItem::SaveRemediationDescription()


// Added for NAV 2005.5/r11.5
HRESULT CQuarantineItem::SaveRemediationOriginalFilename(DWORD dwRemediationIndex, ccEraser::IRemediationAction* pRemediation, ccEraser::eObjectType eType)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	cc::IKeyValueCollectionPtr pProps;
	if(ccEraser::Failed(pRemediation->GetProperties(pProps)))
	{
		CCTRACEE(_T("%s - Failed(pRemediationAction->GetProperties())"), __FUNCTION__);
		return E_FAIL;
	}

	if((ccEraser::FileRemediationActionType != eType) && (ccEraser::InfectionRemediationActionType != eType))
	{
		CCTRACEI(_T("%s - Not a file type remediation"), __FUNCTION__);
		return S_FALSE;
	}

	ISymBasePtr pSymBase;
	if(ccEraser::InfectionRemediationActionType == eType)
	{
		IScanInfectionPtr pScanInfection;
		if(!pProps->GetValue(ccEraser::IRemediationAction::ScanInfection, reinterpret_cast<ISymBase*&>(pScanInfection)))
		{
			CCTRACEE(_T("%s - Failed to get the scan infection object"), __FUNCTION__);
			return E_FAIL;
		}

		IScanFileInfectionQIPtr pFileInfection = pScanInfection;
		if(pFileInfection == NULL)
		{
			CCTRACEI(_T("%s - Not a scan file infection object, Failed to QI"), __FUNCTION__);
			return S_FALSE;
		}
	}
	
	cc::IStringPtr pFilePath;
	if(!pProps->GetValue(ccEraser::IRemediationAction::Path, reinterpret_cast<ISymBase*&>(pFilePath)))
	{
		CCTRACEE(_T("%s - !pProps->GetValue(ccEraserIRemediationAction::Path...)"), __FUNCTION__);
		return E_FAIL;
	}

	TCHAR szFilePath[MAX_PATH * 2] = {0};
	_tcscpy(szFilePath, ccSym::CStringConvert::GetStringT(pFilePath));

	TCHAR szQueryName[MAX_PATH] = {0};
	sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_ORIGINAL_FILENAME, dwRemediationIndex);
	QSPAKSTATUS qsResult = QsPakSetItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_STRING, szFilePath, _tcslen(szFilePath));
	if(QSPAKSTATUS_OK != qsResult)
	{
		CCTRACEE(_T("%s - QsPakSetItemValue(%s)"), __FUNCTION__, szQueryName);
		return E_FAIL;
	}

	return S_OK;
} // END QuarantineItem::SaveRemediationOriginalFilename()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::AddRemediationData(ccEraser::IRemediationAction* pRemediation, cc::IStream* pStream)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	DWORD dwRemediationIndex = 0;
	HRESULT hResult = IncrementRemediationCount(&dwRemediationIndex);
	if(FAILED(hResult))
		return hResult;

	TCHAR szDataSubfolderName[MAX_QUARANTINE_FILENAME_LEN] = {0};
	DWORD dwBuffSize = MAX_QUARANTINE_FILENAME_LEN;
	CreateDataSubfolderIfNotExist(szDataSubfolderName, &dwBuffSize);
	
	TCHAR szRemediationActionFilename[MAX_QUARANTINE_FILENAME_LEN] = {0};
	if(!GenerateRemeditionDataName(szRemediationActionFilename, true, dwRemediationIndex))
	{
		CCTRACEE(_T("%s - !GenerateRemeditionDataName(szRemediationName)"), __FUNCTION__);
		return E_FAIL;
	}

	TCHAR szRemediationActionPath[MAX_QUARANTINE_FILENAME_LEN] = {0};
	_tcscpy(szRemediationActionPath, szDataSubfolderName);
	NameAppendFile(szRemediationActionPath, szRemediationActionFilename);

	// Write filename to quar item in here
	QSPAKSTATUS qsResult = QSPAKSTATUS_OK;
	if(SUCCEEDED(SaveRemediationToFile(pRemediation, szRemediationActionPath)))
	{
		char szQueryName[MAX_PATH] = {0};
		sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_ACTION_FILE, dwRemediationIndex);

		qsResult = QsPakSetItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_STRING, szRemediationActionFilename, _tcslen(szRemediationActionFilename));
		if(QSPAKSTATUS_OK != qsResult)
		{
			CCTRACEE(_T("%s - QsPakSetItemValue(%s)"), __FUNCTION__, szQueryName);
			return E_FAIL;
		}
	}
	
	DWORD dwState = 0;
	if(IsSuccessfulRemediation(pRemediation) && (pStream != NULL))
	{
		dwState = 1;

		TCHAR szRemediationUndoFilename[MAX_QUARANTINE_FILENAME_LEN] = {0};
		if(!GenerateRemeditionDataName(szRemediationUndoFilename, false, dwRemediationIndex))
		{
			CCTRACEE(_T("%s - !GenerateRemeditionDataName(szUndoName)"), __FUNCTION__);
			return E_FAIL;
		}

		TCHAR szRemediationUndoPath[MAX_QUARANTINE_FILENAME_LEN] = {0};
		_tcscpy(szRemediationUndoPath, szDataSubfolderName);
		NameAppendFile(szRemediationUndoPath, szRemediationUndoFilename);

		qsResult = QSPAKSTATUS_OK;
		if(SUCCEEDED(SaveUndoToFile(pStream, szRemediationUndoPath)))
		{
			char szQueryName[MAX_PATH] = {0};
			sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_UNDO_FILE, dwRemediationIndex);

			qsResult = QsPakSetItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_STRING, szRemediationUndoFilename, _tcslen(szRemediationUndoFilename));
			if(QSPAKSTATUS_OK != qsResult)
			{
				CCTRACEE(_T("%s - QsPakSetItemValue(%s)"), __FUNCTION__, szQueryName);
				return E_FAIL;
			}
		}
	}
	else
	{
		IncrementRemediationFailureCount();
	}

	char szQueryName[MAX_PATH] = {0};
	sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_ACTION_SUCCEEDED, dwRemediationIndex);
	qsResult = QsPakSetItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_DWORD, &dwState, sizeof(DWORD));
	if(QSPAKSTATUS_OK != qsResult)
	{
		CCTRACEE(_T("%s - QsPakSetItemValue(%s)"), __FUNCTION__, szQueryName);
		return E_FAIL;
	}

	ccEraser::eObjectType eType = static_cast<ccEraser::eObjectType>(-1);
	if(ccEraser::Failed(GetRemediationActionType(pRemediation, eType)))
	{
		CCTRACEE(_T("%s - Failed GetRemediationActionType()"), __FUNCTION__);
			return E_FAIL;
		}

	DWORD dwActionType = (DWORD)eType;
	sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_ACTION_TYPE, dwRemediationIndex);
	qsResult = QsPakSetItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_DWORD, &dwActionType, sizeof(DWORD));
	if(QSPAKSTATUS_OK != qsResult)
	{
		CCTRACEE(_T("%s - QsPakSetItemValue(%s)"), __FUNCTION__, szQueryName);
		return E_FAIL;
	}

	SaveRemediationDescription(dwRemediationIndex, pRemediation, eType);
	
	hResult = SaveRemediationOriginalFilename(dwRemediationIndex, pRemediation, eType);
	if(FAILED(hResult))
	{
		CCTRACEE(_T("%s - FAILED(SaveRemediationOriginalFilename(%d)), 0x%08X"), __FUNCTION__, dwRemediationIndex, hResult);
		return hResult;
	}

	DWORD dwOperation_Type = GetRemediationOperationType(pRemediation);
	if(-1 != dwOperation_Type)
	{
		sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_OPERATION_TYPE, dwRemediationIndex);
		qsResult = QsPakSetItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_DWORD, &dwOperation_Type, sizeof(DWORD));
		if(QSPAKSTATUS_OK != qsResult)
		{
			CCTRACEE(_T("%s - QsPakSetItemValue(%s)"), __FUNCTION__, szQueryName);
			return E_FAIL;
		}
	}
		
	if(FAILED(SaveItem()))
	{
		CCTRACEE(_T("%s - SaveItem()"), __FUNCTION__);
		return E_FAIL;
	}

	return S_OK;
} // END CQuarantineItem::AddRemediationData()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::RestoreAllRemediations()
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	// Find out if there are remediations to restore
	DWORD dwCount = 0;
	HRESULT hResult = GetRemediationCount(&dwCount);
	CCTRACEI(_T("%s - GetRemediationCount()=%d"), __FUNCTION__, dwCount);

	if(dwCount <= 0)
	{
		return S_FALSE;
	}

	ccEraser::eResult eUndoResult = ccEraser::Fail;
	for(DWORD curNum=dwCount; (curNum > 0); curNum--)
	{
		hResult = RestoreRemediation(curNum, &eUndoResult);
		if(FAILED(hResult))
		{
			CCTRACEE(_T("%s - FAILED(RestoreRemediation(%d)), 0x%08X, eResult=%d"), __FUNCTION__, curNum, hResult, eUndoResult);
			// Try to restore the rest
		}
	}

	return S_OK;
} // END CQuarantineItem::RestoreAllRemediations()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::RestoreRemediation(DWORD dwRemediationIndex, ccEraser::eResult* pUndoResult)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	DWORD dwCount = 0;
	HRESULT hResult = GetRemediationCount(&dwCount);
	CCTRACEI(_T("%s - GetRemediationCount()=%d, dwRemediationIndex=%d"), __FUNCTION__, dwCount, dwRemediationIndex);
	if(dwCount < dwRemediationIndex)
	{
		return E_FAIL;
	}

	// Check if Remediation initially succeeded, if not, shouldn't be restored
	bool bSucceeded = false;
	hResult = GetRemediationActionSucceeded(dwRemediationIndex, &bSucceeded);
	if(FAILED(hResult) || !bSucceeded)
	{
		CCTRACEE(_T("%s - Not a successful RemediationAction"), __FUNCTION__);
		return E_UNAVAILABLE_REMEDIATION_UNDO;
	}

	TCHAR szDataSubfolderName[MAX_QUARANTINE_FILENAME_LEN] = {0};
	DWORD dwBuffSize = MAX_QUARANTINE_FILENAME_LEN;
	QSPAKSTATUS qsResult = QsPakQueryItemValue(m_hQuarItem, QSERVER_ITEM_INFO_DATA_SUBFOLDER, QSPAK_TYPE_STRING, szDataSubfolderName, &dwBuffSize);
	if(QSPAKSTATUS_OK != qsResult)
	{
		CCTRACEE(_T("%s - Could not get QSERVER_ITEM_INFO_DATA_SUBFOLDER"), __FUNCTION__);
		return E_FAIL;
	}

	char szQueryName[MAX_PATH] = {0};
	TCHAR szRemediationFile[MAX_PATH * 2];
	TCHAR szUndoFile[MAX_PATH * 2];
	DWORD dwSize = MAX_PATH * 2;

	sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_ACTION_FILE, dwRemediationIndex);
	if(QSPAKSTATUS_OK != QsPakQueryItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_STRING, szRemediationFile, &dwSize))
	{
		CCTRACEE(_T("%s - Could not get %s"), __FUNCTION__, szQueryName);
		return E_FAIL;
	}

	TCHAR szRemediationPath[MAX_PATH * 2];
	_tcscpy(szRemediationPath, szDataSubfolderName);
	NameAppendFile(szRemediationPath, szRemediationFile);

	CCTRACEI(_T("%s - Remediation filename %d = %s"), __FUNCTION__, dwRemediationIndex, szRemediationPath);

	sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_UNDO_FILE, dwRemediationIndex);
	dwSize = MAX_PATH * 2;
	if(QSPAKSTATUS_OK != QsPakQueryItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_STRING, szUndoFile, &dwSize))
	{
		CCTRACEE(_T("%s - Could not get %s"), __FUNCTION__, szQueryName);
		return E_UNAVAILABLE_REMEDIATION_UNDO;
	}
	TCHAR szUndoPath[MAX_PATH * 2];
	_tcscpy(szUndoPath, szDataSubfolderName);
	NameAppendFile(szUndoPath, szUndoFile);

	CCTRACEI(_T("%s - Undo filename %d = %s"), __FUNCTION__, dwRemediationIndex, szUndoPath);

	hResult = UndoRemediation(szRemediationPath, szUndoPath, pUndoResult);
	if(FAILED(hResult))
	{
		if(NULL == pUndoResult)
			CCTRACEE(_T("%s - FAILED(UndoRemediation(%s, %s)), hResult=0x%08X"), __FUNCTION__, szRemediationPath, szUndoPath, hResult);
		else
			CCTRACEE(_T("%s - FAILED(UndoRemediation(%s, %s)), hResult=0x%08X, eResult=%d"), __FUNCTION__, szRemediationPath, szUndoPath, hResult, *pUndoResult);
		return E_FAIL;
	}

	return S_OK;
} // END CQuarantineItem::RestoreRemediation()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::RestoreFileRemediationToLocation(DWORD dwRemediationIndex, LPTSTR szAlternateDirectory, LPTSTR szAlternateFilename, ccEraser::eResult* pUndoResult)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	DWORD dwCount = 0;
	HRESULT hResult = GetRemediationCount(&dwCount);
	CCTRACEI(_T("%s - GetRemediationCount()=%d, dwRemediationIndex=%d"), __FUNCTION__, dwCount, dwRemediationIndex);
	if(dwCount < dwRemediationIndex)
	{
		return E_FAIL;
	}

	// Check if Remediation initially succeeded, if not, shouldn't be restored
	bool bSucceeded = false;
	hResult = GetRemediationActionSucceeded(dwRemediationIndex, &bSucceeded);
	if(FAILED(hResult) || !bSucceeded)
	{
		CCTRACEE(_T("%s - Not a successful RemediationAction"), __FUNCTION__);
		return E_UNAVAILABLE_REMEDIATION_UNDO;
	}

	// Generate RemediationAction and Undo file paths
	TCHAR szDataSubfolderName[MAX_QUARANTINE_FILENAME_LEN] = {0};
	DWORD dwBuffSize = MAX_QUARANTINE_FILENAME_LEN;
	hResult = GetString(QSERVER_ITEM_INFO_DATA_SUBFOLDER, szDataSubfolderName, &dwBuffSize);
	if(FAILED(hResult))
		return E_FAIL;

	char szQueryName[MAX_PATH] = {0};
	sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_ACTION_FILE, dwRemediationIndex);

	// Remediation file
	TCHAR szRemediationFile[MAX_PATH * 2];
	dwBuffSize = MAX_PATH * 2;
	hResult = GetString(szQueryName, szRemediationFile, &dwBuffSize);
	if(FAILED(hResult))
		return E_FAIL;

	TCHAR szRemediationPath[MAX_PATH * 2];
	_tcscpy(szRemediationPath, szDataSubfolderName);
	NameAppendFile(szRemediationPath, szRemediationFile);
	CCTRACEI(_T("%s - Remediation filename %d = %s"), __FUNCTION__, dwRemediationIndex, szRemediationPath);

	// Undo file
	sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_UNDO_FILE, dwRemediationIndex);

	TCHAR szUndoFile[MAX_PATH * 2];
	dwBuffSize = MAX_PATH * 2;
	hResult = GetString(szQueryName, szUndoFile, &dwBuffSize);
	if(FAILED(hResult))
		return E_UNAVAILABLE_REMEDIATION_UNDO;

	TCHAR szUndoPath[MAX_PATH * 2];
	_tcscpy(szUndoPath, szDataSubfolderName);
	NameAppendFile(szUndoPath, szUndoFile);
	CCTRACEI(_T("%s - Undo filename %d = %s"), __FUNCTION__, dwRemediationIndex, szUndoPath);

	hResult = RestoreFileRemediationToLocationFromDataFiles(szRemediationPath, szUndoPath, szAlternateDirectory, szAlternateFilename, pUndoResult);
	if(FAILED(hResult))
	{
		if(NULL == pUndoResult)
			CCTRACEE(_T("%s - FAILED(UndoRemediationFromDataFiles(%s, %s)), hResult=0x%08X"), __FUNCTION__, szRemediationPath, szUndoPath, hResult);
		else
			CCTRACEE(_T("%s - FAILED(UndoRemediationFromDataFiles(%s, %s)), hResult=0x%08X, eResult=%d"), __FUNCTION__, szRemediationPath, szUndoPath, hResult, *pUndoResult);
		return E_FAIL;
	}

	return S_OK;
} // END CQuarantineItem::RestoreFileRemediationToLocation()


STDMETHODIMP CQuarantineItem::RestoreFileRemediationToTemp(DWORD dwRemediationIndex, 
												/*[out]*/ LPSTR szDestinationFolder,
											 /*[in,out]*/ DWORD* dwDestFolderSize,
												/*[out]*/ LPSTR szDestinationFilename,
											 /*[in,out]*/ DWORD* dwDestFilenameSize,
												/*[out]*/ ccEraser::eResult* pEraserResult)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}
	
    if((NULL == szDestinationFolder) || (NULL == dwDestFolderSize) 
		|| (NULL == szDestinationFilename) || (NULL == dwDestFilenameSize))
    {
		CCTRACEE(_T("%s - NULL output pointer(s)"), __FUNCTION__);
		return E_INVALIDARG;
    }

	try
	{
		TCHAR szTempFolder[MAX_QUARANTINE_FILENAME_LEN] = {0};
		BOOL bGetQuarTempPath = GetQuarantinePath(szTempFolder, MAX_QUARANTINE_FILENAME_LEN, TARGET_QUARANTINE_TEMP_PATH);
		if(!bGetQuarTempPath || (_tcslen(szTempFolder) <= 0))
		{
			CCTRACEE(_T("%s - Failed to get the temp quarantine path"), __FUNCTION__);
			return E_UNABLE_TO_GET_QUARANTINE_PATH;
		}

		if(!CreateFolderIfNotExist(szTempFolder) || (NULL == szTempFolder))
		{
			CCTRACEE(_T("%s - Failed to get the qurantine path"), __FUNCTION__);
			return E_QUARANTINE_DIRECTORY_INVALID;
		}

		if(*dwDestFolderSize < (_tcslen(szTempFolder) + 1))
		{
			CCTRACEE(_T("%s - szDestinationFolder buffer too small"), __FUNCTION__);
			*dwDestFolderSize = _tcslen(szTempFolder) + 1;
			return E_BUFFER_TOO_SMALL;
		}

		_tcsncpy(szDestinationFolder, szTempFolder, *dwDestFolderSize);
		*dwDestFolderSize = _tcslen(szTempFolder) + 1;

		TCHAR szRemediationFile[MAX_PATH * 2];
		DWORD dwSize = MAX_PATH * 2;
		TCHAR szQueryName[MAX_PATH] = {0};
		sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_ACTION_FILE, dwRemediationIndex);
		if(QSPAKSTATUS_OK != QsPakQueryItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_STRING, szRemediationFile, &dwSize))
		{
			CCTRACEE(_T("%s - Could not get %s"), __FUNCTION__, szQueryName);
			return E_FAIL;
		}

		TCHAR szOriginalFilename[MAX_PATH * 2] = {0};
		DWORD dwFilenameSize = MAX_PATH * 2;
		TCHAR szExtension[MAX_PATH] = {0};
		sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_ORIGINAL_FILENAME, dwRemediationIndex);
		QSPAKSTATUS qsResult = QsPakQueryItemValue(m_hQuarItem, szQueryName, QSPAK_TYPE_STRING, szOriginalFilename, &dwFilenameSize);
		if(QSPAKSTATUS_OK != qsResult)
		{
			CCTRACEE(_T("%s - QsPakQueryItemValue(%s)"), __FUNCTION__, szQueryName);
			return E_FAIL;
		}

		TCHAR szTempFilename[MAX_QUARANTINE_FILENAME_LEN] = {0};
		TCHAR szTempFilePath[MAX_QUARANTINE_FILENAME_LEN] = {0};
		HANDLE hNewFileHandle = INVALID_HANDLE_VALUE;
		hNewFileHandle = CreateUniqueFile(szDestinationFolder, GetFileExtension(szOriginalFilename, szExtension, false), szTempFilePath, szTempFilename);
		if((hNewFileHandle == INVALID_HANDLE_VALUE) || (NULL == szTempFilePath) || (NULL == szTempFilename))
		{
			CCTRACEE(_T("%s - Unable to create unique file name"), __FUNCTION__);
			return E_FAIL;
		}

		CloseHandle(hNewFileHandle);
		DeleteFile(szTempFilePath);

		if(*dwDestFilenameSize < (_tcslen(szTempFilename) + 1))
		{
			CCTRACEE(_T("%s - szTempFilename buffer too small"), __FUNCTION__);
			*dwDestFilenameSize = _tcslen(szTempFilename) + 1;
			return E_BUFFER_TOO_SMALL;
		}

		_tcsncpy(szDestinationFilename, szTempFilename, *dwDestFilenameSize);
		*dwDestFilenameSize = _tcslen(szTempFilename) + 1;

		HRESULT hResult = RestoreFileRemediationToLocation(dwRemediationIndex, szDestinationFolder, szDestinationFilename, pEraserResult);
		if(FAILED(hResult))
		{
			if(NULL == pEraserResult)
				CCTRACEE(_T("%s - FAILED(RestoreFileRemediationToLocation()), hResult=0x%08X"), __FUNCTION__, hResult);
			else
				CCTRACEE(_T("%s - FAILED(RestoreFileRemediationToLocation()), hResult=0x%08X, eResult=%d"), __FUNCTION__, hResult, *pEraserResult);
			return hResult;
		}
	}
	catch(exception& Ex)
	{
		CCTRACEE(Ex.what());
		return E_FAIL;
	}

	return S_OK;
} // END CQuarantineItem::RestoreFileRemediationToTemp()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::IsGenericAnomaly(bool* bGeneric)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	// Is Anomaly generic?
	TCHAR szAnomalyCategories[MAX_PATH] = {0};
	DWORD dwSize = MAX_PATH;
	*bGeneric = false;
	HRESULT hResult = GetAnomalyCategories(szAnomalyCategories, &dwSize);
	if(SUCCEEDED(hResult))
	{
		CAtlString strAnomalyCategories(szAnomalyCategories);
		CAtlString strTokenized;
		TCHAR* szTokens = _T(" ,");
		int iCurPos= 0;

		strAnomalyCategories.Trim();
		strTokenized = strAnomalyCategories.Tokenize(szTokens, iCurPos);
		while(strTokenized != "")
		{
			if(atoi(strTokenized) == ccEraser::IAnomaly::GenericLoadPoint)
			{
				*bGeneric = true;
				return S_OK;
			}

			strTokenized = strAnomalyCategories.Tokenize(szTokens, iCurPos);
		}
	}

	return S_OK;
} // END CQuarantineItem::IsGenericAnomaly()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::RestoreGenericRemediations()
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	bool bGeneric = false;
	HRESULT hr = IsGenericAnomaly(&bGeneric);
	if(SUCCEEDED(hr) && bGeneric)
	{
		hr = RestoreAllRemediations();
	}

	return hr;
} // END CQuarantineItem::RestoreGenericRemediations()


// Added for NAV 2005.5/r11.5
HRESULT CQuarantineItem::SaveRemediationToFile(ccEraser::IRemediationAction* pRemediation, LPTSTR szFileName)
{
	if((NULL == szFileName) || (0 == _tcslen(szFileName)))
    {
		CCTRACEE(_T("%s - (NULL == szFileName) || (0 == _tcslen(szFileName))"), __FUNCTION__);
        return E_INVALIDARG;
    }

	if(pRemediation == NULL)
    {
		CCTRACEE(_T("%s - Remediation pointer is NULL"), __FUNCTION__);
        return E_INVALIDARG;
    }

	ccSym::CFileStreamImplPtr pFileStream;
	pFileStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
	if(pFileStream == NULL)
	{
		CCTRACEE(_T("%s - NULL value, pFileStream = ccSym::CFileStreamImpl::CreateFileStreamImpl()"), __FUNCTION__);
		return E_FAIL;
	}

	if(FALSE == pFileStream->GetFile().Open(szFileName, 
											GENERIC_READ | GENERIC_WRITE,
											0, 
											NULL, 
											CREATE_ALWAYS, 
											0,
											NULL))
	{
		CCTRACEE(_T("%s - pFileStream->GetFile().Open() failed"), __FUNCTION__);
		return E_FAIL;
	}

	cc::IStreamPtr pStream(pFileStream);
	if(pStream == NULL)
	{
		CCTRACEE(_T("%s - pStream(pFileStream) == NULL"), __FUNCTION__);
		return E_FAIL;
	}

	// Get ISerialize interface
	cc::ISerializeQIPtr pSerialize(pRemediation);
	ATLASSERT(pSerialize != NULL);
	if(pSerialize == NULL)
	{
		CCTRACEE(_T("%s - pSerialize(pProps) == NULL"), __FUNCTION__);
		return E_FAIL;
	}

	if(pSerialize->Save(pStream) == false)
	{
		//setStatusText(_T("pSerialize->Save(pStream) == false"), true);
		return E_FAIL;
	}

	pFileStream->GetFile().Close();
	
	//szMsg.Format(_T("IRemediationAction was saved %s"), szFileName);

	return S_OK;
} // END CQuarantineItem::SaveRemediationToFile()


// Added for NAV 2005.5/r11.5
HRESULT CQuarantineItem::SaveUndoToFile(cc::IStream* pStream, LPTSTR szFileName)
{
	if((szFileName == NULL) || (0 == _tcslen(szFileName)))
	{
		CCTRACEE(_T("%s - Invalid filename passed in."), __FUNCTION__);
		return E_INVALIDARG;
	}

	if(pStream == NULL)
	{
		CCTRACEE(_T("%s - NULL cc::IStream* passed in."), __FUNCTION__);
		return E_INVALIDARG;
	}

	if(!pStream->SetPosition(0))
	{
		CCTRACEE(_T("%s !pStream->SetPosition(0)"), __FUNCTION__);
		return E_FAIL;
	}

    try
    {
		ccLib::CFile UndoFile;
		if(FALSE == UndoFile.Open(szFileName, 
			GENERIC_WRITE,
			0, 
			NULL, 
			CREATE_ALWAYS, 
			0,
			NULL))
		{
			CCTRACEE(_T("%s - pFileStream->GetFile().Open() failed"), __FUNCTION__);
			return E_FAIL;
		}

		ULONGLONG nStreamSize = 0;
		if(!pStream->GetSize(nStreamSize) || (nStreamSize <= 0))
		{
			CCTRACEE(_T("%s - !pStream->GetSize(nStreamSize) || (nStreamSize <= 0)"), __FUNCTION__);
			return E_FAIL;
		}

		ccLib::CMemory Buffer;
		if(Buffer.NewAlloc((SIZE_T)nStreamSize) == NULL)
		{
			CCTRACEE(_T("%s - Buffer.NewAlloc(nStreamSize) == NULL"), __FUNCTION__);
			return E_FAIL;
		}

		pStream->Read((LPVOID)Buffer, (SIZE_T)nStreamSize);

		if(!UndoFile.Write((LPCVOID)Buffer, (DWORD)nStreamSize))
		{
			CCTRACEE(_T("%s - !pFileStream->Write()"), __FUNCTION__);
			return E_FAIL;
		}

		UndoFile.Close();
    }
    catch(ccLib::CArchive::CArchiveException&)
    {
        CCTRACEE(_T("%s - Couldn't read stream data"), __FUNCTION__);
        return E_FAIL;
    }

    return S_OK;
} // END CQuarantineItem::SaveUndoToFile()


// Added for NAV 2005.5/r11.5
HRESULT CQuarantineItem::UndoRemediation(LPTSTR szRemdiationFilename, LPTSTR szUndoFilename, ccEraser::eResult* pUndoResult)
{
	ccEraser::eResult EraserResult = ccEraser::Fail;

	if((NULL == szRemdiationFilename) || (NULL == szUndoFilename) || (0 == _tcslen(szRemdiationFilename)) || (0 == _tcslen(szUndoFilename)))
	{
		CCTRACEE(_T("%s - Remediation and/or undo file name NOT passed in."), __FUNCTION__);
		return E_INVALIDARG;
	}

	HRESULT hResult = E_FAIL;

	try
	{
		ccLib::CCoInitialize CoInit;
		hResult = CoInit.Initialize(ccLib::CCoInitialize::eSTAModel);
		if(FAILED(hResult))
		{
			CCTRACEE(_T("%s - FAILED(CoInit.Initialize()), 0x%08X"), __FUNCTION__, hResult);
			return hResult;
		}
		
		hResult = InitializeEraser(&EraserResult);
		if(NULL != pUndoResult)
			*pUndoResult = EraserResult;
		if(FAILED(hResult))
		{
			CCTRACEE(_T("%s - FAILED(InitializeEraser()), hResult=0x%08X, eResult=%d"), __FUNCTION__, hResult, EraserResult);
			return hResult;
		}

		ccEraser::IRemediationActionPtr pRemediation;
		hResult = LoadRemediationFromFile(szRemdiationFilename, pRemediation);
		if(FAILED(hResult))
		{
			CCTRACEE(_T("%s - FAILED(LoadRemediationFromFile()), 0x%08X"), __FUNCTION__, hResult);
			return hResult;
		}

		cc::IStreamPtr pUndoStream;
		hResult = LoadUndoFromFile(szUndoFilename, pUndoStream);
		if(FAILED(hResult))
		{
			CCTRACEE(_T("%s - FAILED(LoadUndoFromFile()), 0x%08X"), __FUNCTION__, hResult);
			return hResult;
		}

		EraserResult = pRemediation->Undo(pUndoStream);
		if(NULL != pUndoResult)
			*pUndoResult = EraserResult;
		if(ccEraser::Failed(EraserResult))
		{
			//eResult2String(eRes, sz);
			CCTRACEE(_T("%s - Failed(pRemediation->Undo(pStream)), EraserResult == %d"), __FUNCTION__, EraserResult);
			return E_FAIL;
		}

		return S_OK;
	}
	catch(exception& Ex)
	{
		CCTRACEE(Ex.what());
		hResult = E_FAIL;
	}

	return hResult;
} // END CQuarantineItem::UndoRemediation()


// Added for NAV 2005.5/r11.5
HRESULT CQuarantineItem::RestoreFileRemediationToLocationFromDataFiles(LPTSTR szRemdiationFilename, LPTSTR szUndoFilename, LPTSTR szAlternateDirectory, LPTSTR szAlternateFilename, ccEraser::eResult* pUndoResult)
{
	ccEraser::eResult EraserResult = ccEraser::Fail;

	if((NULL == szRemdiationFilename) || (NULL == szUndoFilename) || (0 == _tcslen(szRemdiationFilename)) || (0 == _tcslen(szUndoFilename)))
	{
		CCTRACEE(_T("%s - Remediation and/or undo file name NOT passed in."), __FUNCTION__);
		return E_INVALIDARG;
	}

	if((NULL == szAlternateDirectory) || (_tcslen(szAlternateDirectory) <= 0))
	{
		CCTRACEE(_T("%s - szAlternateDirectory NOT passed in."), __FUNCTION__);
		return E_INVALIDARG;
	}

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	HRESULT hResult = E_FAIL;

	try
	{
		ccLib::CCoInitialize CoInit;
		hResult = CoInit.Initialize(ccLib::CCoInitialize::eSTAModel);
		if(FAILED(hResult))
		{
			CCTRACEE(_T("%s - FAILED(CoInit.Initialize()), 0x%08X"), __FUNCTION__, hResult);
			return hResult;
		}

		hResult = InitializeEraser(&EraserResult);
		if(NULL != pUndoResult)
			*pUndoResult = EraserResult;
		if(FAILED(hResult))
		{
			CCTRACEE(_T("%s - FAILED(InitializeEraser()), hResult=0x%08X, eResult=%d"), __FUNCTION__, hResult, EraserResult);
			return hResult;
		}

		ccEraser::IRemediationActionPtr pRemediation;
		hResult = LoadRemediationFromFile(szRemdiationFilename, pRemediation);
		if(FAILED(hResult))
		{
			CCTRACEE(_T("%s - FAILED(LoadRemediationFromFile()), 0x%08X"), __FUNCTION__, hResult);
			return hResult;
		}

		ccEraser::eObjectType eType;
		EraserResult = pRemediation->GetType(eType);
		if(NULL != pUndoResult)
			*pUndoResult = EraserResult;
		if(ccEraser::Succeeded(EraserResult) && (eType == ccEraser::FileRemediationActionType || eType == ccEraser::InfectionRemediationActionType))
		{
			cc::IStreamPtr pUndoStream;
			hResult = LoadUndoFromFile(szUndoFilename, pUndoStream);
			if(FAILED(hResult))
			{
				CCTRACEE(_T("%s - FAILED(LoadUndoFromFile()), 0x%08X"), __FUNCTION__, hResult);
				return hResult;
			}

			cc::IStringPtr pDirStr;
			pDirStr.Attach(ccSym::CStringImpl::CreateStringImpl(szAlternateDirectory));

			if((NULL == szAlternateFilename) || (_tcslen(szAlternateFilename) <= 0)) // Save with original filename
			{
				EraserResult = pRemediation->SaveUndoInformation(pDirStr, pUndoStream);
				if(NULL != pUndoResult)
					*pUndoResult = EraserResult;
				if(Failed(EraserResult))
				{
					CCTRACEE(_T("%s - Failed(pRemediation->SaveUndoInformation()), EraserResult=%d"), __FUNCTION__, EraserResult);
					return E_FAIL;
				}
				else
				{
					cc::IStringPtr pDescriptionStr;
					ccEraser::eResult eResDes = pRemediation->GetDescription(pDescriptionStr);
					if(Failed(eResDes))
						CCTRACEI(_T("%s pRemediation->SaveUndoInformation(), EraserResult=%d"), __FUNCTION__, EraserResult);
					else
						CCTRACEI(_T("%s pRemediation->SaveUndoInformation(%s ...), EraserResult=%d"), __FUNCTION__, ccSym::CStringConvert::GetStringT(pDescriptionStr), EraserResult);
				}
			}
			else if((NULL != szAlternateFilename) && (_tcslen(szAlternateFilename) > 0)) // Save with passed in filename
			{
				cc::IStringPtr pFileStr;
				pFileStr.Attach(ccSym::CStringImpl::CreateStringImpl(szAlternateFilename));

				EraserResult = pRemediation->SaveUndoInformation(pDirStr, pFileStr, pUndoStream);
				if(NULL != pUndoResult)
					*pUndoResult = EraserResult;
				if(Failed(EraserResult))
				{
					CCTRACEE(_T("%s Failed(pRemediation->SaveUndoInformation()), EraserResult=%d"), __FUNCTION__, EraserResult);
					return E_FAIL;
				}
				else
				{
					cc::IStringPtr pDescriptionStr;
					ccEraser::eResult eResDes = pRemediation->GetDescription(pDescriptionStr);
					if(Failed(eResDes))
						CCTRACEI(_T("%s pRemediation->SaveUndoInformation(), EraserResult=%d"), __FUNCTION__, EraserResult);
					else
						CCTRACEI(_T("%s pRemediation->SaveUndoInformation(%s ...), EraserResult=%d"), __FUNCTION__, ccSym::CStringConvert::GetStringT(pDescriptionStr), EraserResult);
				}
			}
		}

		return S_OK;
	}
	catch(exception& Ex)
	{
		CCTRACEE(Ex.what());
		hResult = E_FAIL;
	}

	return hResult;
} // END CQuarantineItem::RestoreFileRemediationToLocationFromDataFiles()


// Added for NAV 2005.5/r11.5
HRESULT CQuarantineItem::LoadRemediationFromFile(LPTSTR szRemdiationFilename, ccEraser::IRemediationAction*& pRemediation)
{
	HRESULT hResult = E_FAIL;

	try
	{
		ccSym::CFileStreamImplPtr pActionStream;
		pActionStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
		if(pActionStream == NULL)
		{
			CCTRACEE(_T("%s - (pActionStream == NULL)"), __FUNCTION__);
			return E_FAIL;
		}

		BOOL bActionFileOpen = pActionStream->GetFile().Open(szRemdiationFilename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(!bActionFileOpen)
		{
			CCTRACEE(_T("%s - !pActionStream->GetFile().Open()"), __FUNCTION__);
			return E_FAIL;
		}

		ccEraser::eObjectType type = static_cast<ccEraser::eObjectType>(-1);
		ISymBasePtr pBase;
		ccEraser::eResult eRes = m_pEraser->CreateObjectFromStream(pActionStream, &type, pBase);
		if(ccEraser::Failed(eRes))
		{
			CCTRACEE(_T("%s - Failed(m_pEraser->CreateObjectFromStream(pActionStream,...)), eRes == %d"), __FUNCTION__, eRes);
			pActionStream->GetFile().Close();
			return E_FAIL;
		}

		ccEraser::IRemediationActionQIPtr pRemediationAction = pBase;
		if(pRemediationAction == NULL)
		{
			CCTRACEE(_T("%s - pRemediationAction == NULL)"), __FUNCTION__);
			pActionStream->GetFile().Close();
			return E_FAIL;
		}

		pRemediation = pRemediationAction.Detach();
		if(pRemediation == NULL)
		{
			CCTRACEE(_T("%s - pRemediationAction == NULL)"), __FUNCTION__);
			pActionStream->GetFile().Close();
			return E_FAIL;
		}

		pActionStream->GetFile().Close();

		hResult = S_OK;
	}
	catch (_com_error err)
	{
		hResult = err.Error();
		CCTRACEE(_T("%s - COM error, 0x%x"), __FUNCTION__, hResult);
	}
	catch(exception& Ex)
	{
		CCTRACEE(Ex.what());
		hResult = E_FAIL;
	}

	return hResult;
} // END CQuarantineItem::LoadRemediationFromFile()


// Added for NAV 2005.5/r11.5
HRESULT CQuarantineItem::LoadUndoFromFile(LPTSTR szUndoFilename, cc::IStream*& pOutStream)
{
	HRESULT hResult = E_FAIL;

	try
	{
		// Create a file stream object
		ccSym::CFileStreamImplPtr pUndoStream;
		pUndoStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
		if(pUndoStream == NULL)
		{
			CCTRACEE(_T("%s - (pUndoStream == NULL)"), __FUNCTION__);
			return E_FAIL;
		}

		// Open the reference data file
		BOOL bUndoFileOpen = pUndoStream->GetFile().Open(szUndoFilename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(!bUndoFileOpen)
		{
			CCTRACEE(_T("%s - pFileStream->GetFile().Open(%s,...) failed"), __FUNCTION__, szUndoFilename);
			return E_FAIL;
		}

		bool bReturn = pUndoStream->SetPosition(0);

		ULONGLONG nStreamSize = 0;
		if(!pUndoStream->GetSize(nStreamSize) || (nStreamSize <= 0))
		{
			CCTRACEE(_T("%s - !pUndoStream->GetSize(nStreamSize) || (%d <= 0)"), __FUNCTION__, nStreamSize);
			return E_FAIL;
		}

		ccLib::CMemory Buffer;
		if(Buffer.NewAlloc((SIZE_T)nStreamSize) == NULL)
		{
			CCTRACEE(_T("%s - Buffer.NewAlloc(%d) == NULL"), __FUNCTION__, nStreamSize);
			return E_FAIL;
		}

		pUndoStream->Read((LPVOID)Buffer, (SIZE_T)nStreamSize);

		ccSym::CMemoryStreamImplPtr pMemStream = new ccSym::CMemoryStreamImpl(Buffer);
		if(pMemStream == NULL)
		{
			CCTRACEE(_T("%s - Failed to create memory stream impl for remediation data."), __FUNCTION__);
			return E_FAIL;
		}

		ULONGLONG nOutStreamSize = 0;
		if(!pMemStream->GetSize(nOutStreamSize) || (nOutStreamSize <= 0) || (nOutStreamSize != nStreamSize))
		{
			CCTRACEE(_T("%s - !pOutStream->GetSize(nStreamSize) || (%d <= 0) || (nOutStreamSize != nStreamSize)"), __FUNCTION__, nStreamSize);
			return E_FAIL;
		}

		pOutStream = pMemStream.Detach();
		if(pOutStream == NULL)
		{
			CCTRACEE(_T("%s pOutStream == NULL)"), __FUNCTION__);
			pUndoStream->GetFile().Close();
			return E_FAIL;
		}

		pUndoStream->GetFile().Close();

		hResult = S_OK;
	}
	catch (_com_error err)
	{
		hResult = err.Error();
		CCTRACEE (_T("%s - COM error, 0x%x"), __FUNCTION__, hResult);
	}
	catch(exception& Ex)
	{
		CCTRACEE(Ex.what());
		hResult = E_FAIL;
	}

	return hResult;
} // END CQuarantineItem::LoadUndoFromFile()


// Added for NAV 2005.5/r11.5
HRESULT CQuarantineItem::InitializeEraser(ccEraser::eResult* pEraserResult)
{
	HRESULT hResult = E_FAIL;
	ccEraser::eResult eResult = ccEraser::Fail;

	try
	{
		if(m_pEraser != NULL)
		{
			CCTRACEI(_T("%s - m_pEraser already initialized"), __FUNCTION__);
			return S_FALSE;
		}

		if(!m_pQScanner)
		{
			m_pQScanner = new CSymStaticRefCount<CQScanner>;
			if(m_pQScanner == NULL)
			{
				CCTRACEE(_T("%s - Failed to create new CSymStaticRefCount<CQScanner>"), __FUNCTION__);
				return E_FAIL;
			}

			hResult = m_pQScanner->Initialize(QUARANTINE_APP_ID);
			if(FAILED(hResult))
			{
				CCTRACEE(_T("%s - FAILED(m_pQScanner->Initialize(QUARANTINE_APP_ID)), hResult=0x%08X"), __FUNCTION__, hResult);
		
				delete m_pQScanner;
				m_pQScanner = NULL;

				return hResult;
			}
		}

		TCHAR szEraserPath[MAX_PATH] = {0};
		hResult = m_pQScanner->GetCurrentDefinitionsDirectory(szEraserPath);
		if(FAILED(hResult))
		{
			CCTRACEE(_T("%s - FAILED(m_pQScanner->GetCurrentDefinitionsDirectory()), hResult=0x%08X"), __FUNCTION__, hResult);
			return hResult;
		}

		NameAppendFile(szEraserPath, cc::sz_ccEraser_dll);

		if(SYM_FAILED(m_ccEraserLoader.Initialize(szEraserPath)))
		{
			CCTRACEE(_T("%s - SYM_FAILED(m_ccEraserLoader.Initialize(%s))"), __FUNCTION__, szEraserPath);
			return E_FAIL;
		}

		CCTRACEI(_T("%s - m_ccEraserLoader.Initialize(%s)"), __FUNCTION__, szEraserPath);

		// load the eraser object
		if(SYM_FAILED(m_ccEraserLoader.CreateObject(ccEraser::IID_Eraser, ccEraser::IID_Eraser, reinterpret_cast<void**>(&m_pEraser))))
		{
			CCTRACEE(_T("%s - SYM_FAILED(EraserLoader.CreateObject())"), __FUNCTION__);
			return E_FAIL;
		}

		m_SimpleScan.GetScanner(m_pScanner);
		m_SimpleScan.GetScanSink(m_pScanSink);
		m_SimpleScan.GetScanProperties(m_pScanProps);

		eResult = m_pEraser->Initialize(m_pScanner, m_pScanSink, m_pScanProps);
		if(NULL != pEraserResult)
			*pEraserResult = eResult;
		if(ccEraser::Failed(eResult))
		{
			CCTRACEE(_T("%s - Failed(m_pEraser->Initialize()), eResult = %d"), __FUNCTION__, eResult);
			return E_FAIL;
		}

		hResult = S_OK;
	}
	catch(_com_error err)
	{
		hResult = err.Error();
		CCTRACEE(_T("%s() - COM error, 0x%x"), __FUNCTION__, hResult);
	}
	catch(exception& Ex)
	{
		CCTRACEE(Ex.what());
		hResult = E_FAIL;
	}

	return hResult;
} // END CQuarantineItem::InitializeEraser()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetRemediationData(LPSTR szDataDestinationFolder,
												 DWORD dwRemediationIndex,
									   /*[out]*/ LPSTR szRemediationActionName,
									   /*[in, out]*/ DWORD* dwBufferSizeAction,	 
									   /*[out]*/ LPSTR szRemediationUndoName,
									   /*[in, out]*/ DWORD* dwBufferSizeUndo)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	// Load path of data files for this item
	TCHAR szDataSubfolderName[MAX_PATH * 2] = {0};
	DWORD dwBuffSize = MAX_PATH * 2;
	QSPAKSTATUS qsResult = QsPakQueryItemValue(m_hQuarItem, QSERVER_ITEM_INFO_DATA_SUBFOLDER, QSPAK_TYPE_STRING, szDataSubfolderName, &dwBuffSize);
	if((QSPAKSTATUS_OK != qsResult) || (_tcslen(szDataSubfolderName) <= 0))
		return E_FAIL;

	TCHAR szRemediationQuery[MAX_PATH] = {0};
	TCHAR szUndoQuery[MAX_PATH] = {0};
	sprintf(szRemediationQuery, QSERVER_ITEM_REMEDIATION_ACTION_FILE, dwRemediationIndex);
	sprintf(szUndoQuery, QSERVER_ITEM_REMEDIATION_UNDO_FILE, dwRemediationIndex);
	
	qsResult = QsPakQueryItemValue(m_hQuarItem, szRemediationQuery, QSPAK_TYPE_STRING, szRemediationActionName, dwBufferSizeAction);
	if((QSPAKSTATUS_OK != qsResult) || (_tcslen(szRemediationActionName) <= 0))
		return E_FAIL;

	bool bUndoAvailable = true;
	qsResult = QsPakQueryItemValue(m_hQuarItem, szUndoQuery, QSPAK_TYPE_STRING, szRemediationUndoName, dwBufferSizeUndo);
	if((QSPAKSTATUS_OK != qsResult) || (_tcslen(szRemediationUndoName) <= 0))
	{
		bUndoAvailable = false;
		_tcscpy(szRemediationUndoName, _T(""));
		*dwBufferSizeUndo = 0;
	}

	TCHAR szRemediationActionSource[MAX_PATH * 2] = {0};
	_tcscpy(szRemediationActionSource, szDataSubfolderName);
	NameAppendFile(szRemediationActionSource, szRemediationActionName);

	TCHAR szRemediationActionDest[MAX_PATH * 2] = {0};
	_tcscpy(szRemediationActionDest, szDataDestinationFolder);
	NameAppendFile(szRemediationActionDest, szRemediationActionName);

	BOOL bCopied = CopyFile(szRemediationActionSource, szRemediationActionDest, FALSE);
	if(!bCopied)
	{
		DWORD dwError = GetLastError();
		CCTRACEE(_T("%s - Failed CopyFile(%s, %s), GetLastError()=%d"), __FUNCTION__, szRemediationActionSource, szRemediationActionDest, dwError);
	}

	if(bUndoAvailable)
	{
		TCHAR szRemediationUndoSource[MAX_PATH * 2] = {0};
		_tcscpy(szRemediationUndoSource, szDataSubfolderName);
		NameAppendFile(szRemediationUndoSource, szRemediationUndoName);

		TCHAR szRemediationUndoDest[MAX_PATH * 2] = {0};
		_tcscpy(szRemediationUndoDest, szDataDestinationFolder);
		NameAppendFile(szRemediationUndoDest, szRemediationUndoName);

		BOOL bUndoCopied = CopyFile(szRemediationUndoSource, szRemediationUndoDest, FALSE);
		if(!bUndoCopied)
		{
			DWORD dwError = GetLastError();
			CCTRACEE(_T("%s - Failed CopyFile(%s, %s), GetLastError()=%d"), __FUNCTION__, szRemediationUndoSource, szRemediationUndoDest, dwError);
		}
		bCopied = bCopied && bUndoCopied;
	}

	return (bCopied ? S_OK : E_FAIL);
} // END CQuarantineItem::GetRemediationData()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::TakeSystemSnapshot()
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	// Load path of data files for this item
	TCHAR szDataSubfolderName[MAX_PATH * 2] = {0};
	DWORD dwBuffSize = MAX_PATH * 2;
    HRESULT hResult = CreateDataSubfolderIfNotExist(szDataSubfolderName, &dwBuffSize);

	TCHAR szSnapShotPath[MAX_PATH * 2] = {0};
	_tcscpy(szSnapShotPath, szDataSubfolderName);
	NameAppendFile(szSnapShotPath, SYSTEM_SNAPSHOT_FILE_NAME_FORMAT);
	

	// Pass snapshot taking function the path: szSnapShotPath
	CSystemSnapshot Snapper;
    hResult = Snapper.TakeSystemSnapshot(szSnapShotPath);
	if(FAILED(hResult))
	{
		CCTRACEE(_T("%s - Failed to take snapshot, hResult=0x%08X"), __FUNCTION__, hResult);
		return E_FAIL;
	}

	TCHAR szSnapShotName[MAX_PATH * 2] = {0};
	_tcscpy(szSnapShotName, SYSTEM_SNAPSHOT_FILE_NAME_FORMAT);

	// store file name returned from snapshot function
	QSPAKSTATUS qsResult = QsPakSetItemValue(m_hQuarItem, QSERVER_ITEM_SYSTEM_SNAPSHOT_FILE, QSPAK_TYPE_STRING, szSnapShotName, strlen(szSnapShotName));
	if(QSPAKSTATUS_OK != qsResult)
	{
		CCTRACEE(_T("%s - Failed QsPakSetItemValue(QSERVER_ITEM_SYSTEM_SNAPSHOT_FILE), qsResult=%d"), __FUNCTION__, qsResult);
		return E_FAIL;
	}

	SaveItem();

	return S_OK;
} // END TakeSystemSnapshot


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::CopySnapshotToFolder(LPSTR szDestinationFolder, 
											 /*[out]*/ LPSTR szSnapShotName, 
											 /*[in, out]*/ DWORD *dwBufferSize)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	// Load path of data files for this item
	TCHAR szDataSubfolderName[MAX_PATH * 2] = {0};
	DWORD dwSubFolderBuffSize = MAX_PATH * 2;
	QSPAKSTATUS qsResult = QsPakQueryItemValue(m_hQuarItem, QSERVER_ITEM_INFO_DATA_SUBFOLDER, QSPAK_TYPE_STRING, szDataSubfolderName, &dwSubFolderBuffSize);
	if((QSPAKSTATUS_OK != qsResult) || (_tcslen(szDataSubfolderName) <= 0))
		return E_FAIL;

	qsResult = QsPakQueryItemValue(m_hQuarItem, QSERVER_ITEM_SYSTEM_SNAPSHOT_FILE, QSPAK_TYPE_STRING, szSnapShotName, dwBufferSize);
	if(QSPAKSTATUS_BUFFER_TOO_SMALL == qsResult)
		return E_BUFFER_TOO_SMALL;
	if((QSPAKSTATUS_OK != qsResult) || (_tcslen(szSnapShotName) <= 0))
		return E_FAIL;

	TCHAR szSnapShotSource[MAX_PATH * 2] = {0};
	_tcscpy(szSnapShotSource, szDataSubfolderName);
	NameAppendFile(szSnapShotSource, szSnapShotName);

	TCHAR szSnapShotDest[MAX_PATH * 2] = {0};
	_tcscpy(szSnapShotDest, szDestinationFolder);
	NameAppendFile(szSnapShotDest, szSnapShotName);

	BOOL bCopied = CopyFile(szSnapShotSource, szSnapShotDest, FALSE);

	return (bCopied ? S_OK : E_FAIL);
} // END CopySnapshotToFolder


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::InitialDummyFile(bool* pbDummy)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	if(NULL == pbDummy)
	{
		CCTRACEE(_T("%s - NULL output parameter"), __FUNCTION__);
		return E_INVALIDARG;
	}

	*pbDummy = false;

	DWORD dwDummyFile = 0;
	DWORD dwBufSize = sizeof(DWORD);

	// Find out how big the buffer needs to be

	QSPAKSTATUS qsResult = QsPakQueryItemValue(m_hQuarItem, QSERVER_ITEM_USING_DUMMY_FILE, QSPAK_TYPE_DWORD, &dwDummyFile, &dwBufSize);
	if( QSPAKSTATUS_OK == qsResult && dwDummyFile == 1 )
	{
		*pbDummy = true;
	}

	return S_OK;
} // END CQuarantineItem::InitialDummyFile()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineItem::GetOriginalScanDefsDate(SYSTEMTIME* dateOfOriginalScannedDefs)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!IsInitialized())
	{
		CCTRACEE(_T("%s - !IsInitialized()"), __FUNCTION__);
		return E_UNINITIALIZED_ITEM;
	}

	QSPAKDATE stQSDate;
	DWORD dwQSDateSize = sizeof(stQSDate);
	
	QSPAKSTATUS qsResult = QsPakQueryItemValue(m_hQuarItem, QSERVER_ITEM_INFO_ORIGINAL_SCAN_DEFS_QDATE, QSPAK_TYPE_DATE, &stQSDate, &dwQSDateSize);
	if(QSPAKSTATUS_OK != qsResult)
		return E_FAIL;

	QSDateToSysTime(stQSDate, dateOfOriginalScannedDefs);
    return S_OK;
} // END CQuarantineItem::GetOriginalScanDefsDate()
