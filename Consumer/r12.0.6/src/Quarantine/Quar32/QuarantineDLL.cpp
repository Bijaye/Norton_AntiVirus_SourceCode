//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// QuarantineDLL.cpp - Contains implementation for CQuarantineDLL
//

#include "stdafx.h"
#include "Quar32.h"
#include "QuarantineDLL.h"
#include "EnumQuarantineItems.h"
#include "QuarantineItem.h"
#include "Quaropts.h"

// Taken from QuarAdd_cc.h
#include "..\..\SDKs\AutoProtect\include\apquar.h"
#include "avtypes.h"

#include "xapi.h"
//#include "navcb.h"
#include "global.h"
#include <stdexcept>
#include "OSInfo.h"
//#include "NAVInfo.h"
#include "md5.h"
#include "ccSymCommonClientInfo.h"

#include "navntutl.h"

using namespace std;

//QSPak Includes
#include "qspak.h"
#include "qsfields.h"

#include "SystemSnapshot.h"


// Runtime links to NT helper DLLs.
extern HINSTANCE g_hInstNAVNTUTL;
extern HINSTANCE g_hInstNetApi32;
extern PFNCOPYALTERNATEDATASTREAMS g_pfnCopyAlternateDataStreams;
extern PFNNetWkstaGetInfo   g_pfnNetWkstaGetInfo;
extern PFNNetApiBufferFree  g_pfnNetApiBufferFree;


CQuarantineDLL::CQuarantineDLL()
{
    m_bInitialized = false;
    //m_hAPDll = NULL;
    //m_bDissabledAP = FALSE;
    m_pQScanner = NULL;
	m_bSnapshot = false;
}


// 2/23/98 DBUCHES - Function created / Header added.
// 3/24/98 JTAYLOR - Code added to dissable AP for this process
// 3/30/98 JTAYLOR - Moved Dissable AP code
// 4/22/98 JTAYLOR - Only enable AP if we were initialized (i.e. AP dissabled)
CQuarantineDLL::~CQuarantineDLL()
{
	if(m_bSnapshot)
	{
		// Delete snapshot
		TCHAR szSnapshotPath[MAX_PATH * 2] = {0};
		DWORD dwBuffSize = MAX_PATH * 2;
		BOOL bSuccess = GetQuarantinePath(szSnapshotPath, dwBuffSize, TARGET_QUARANTINE_TEMP_PATH);
		if(bSuccess && (_tcslen(szSnapshotPath) > 0) && ::PathFileExists(szSnapshotPath))
		{
			NameAppendFile(szSnapshotPath, SYSTEM_SNAPSHOT_FILE_NAME_FORMAT);
			if(::PathFileExists(szSnapshotPath))
				::DeleteFile(szSnapshotPath);
		}
	}

    // If AP was dissabled Enable AP for this process.
    if(m_bInitialized)
    {
        if (!m_APWrapper.EnableAPForThisThread())
            CCTRACEE(_T("%s - failed to unregister AP"), __FUNCTION__);
    }

	if( NULL != g_hInstNAVNTUTL )
	{
		FreeLibrary( g_hInstNAVNTUTL );
		g_hInstNAVNTUTL = NULL;
	}

	if( NULL != g_hInstNetApi32 )
	{
		FreeLibrary( g_hInstNetApi32 );
		g_hInstNetApi32 = NULL;
	}

    // Release the quarantine scanner
	if(NULL != m_pQScanner)
	{
		delete m_pQScanner;
		m_pQScanner = NULL;
	}
}


// ==== GetVersion ==========================================================
// Description      : Returns the version number of this object
// Return type      : UINT
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(UINT) CQuarantineDLL::GetVersion()
{
    return 1;
}


// ==== Initialize ==========================================================
// Description      : Initialize the Quarantine Class
// Return type      : HRESULT S_OK on success.
////////////////////////////////////////////////////////////////////////////
// 3/13/98 JTAYLOR - Function created / Header added.
// 3/24/98 JTAYLOR - Code added to dissable AP for this process
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::Initialize()
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(m_bInitialized)
		return ERROR_ALREADY_INITIALIZED;

	BOOL bSuccess = GetQuarantinePath( m_szQuarantineFolder, MAX_PATH, TARGET_QUARANTINE_PATH );
	if( FALSE == bSuccess || m_szQuarantineFolder == "")
		return E_UNABLE_TO_GET_QUARANTINE_PATH;

	// Check to make sure that the Quarantine Path is valid.
	DWORD dwTempAttributes = 0;
	dwTempAttributes = GetFileAttributes(m_szQuarantineFolder);

	if((INVALID_FILE_ATTRIBUTES == dwTempAttributes) 
		|| !((FILE_ATTRIBUTE_DIRECTORY & dwTempAttributes) == FILE_ATTRIBUTE_DIRECTORY))
	{
		return E_QUARANTINE_DIRECTORY_INVALID;
	}

	// If running on WinNT machine, load our NT helper dll.
	if( IsWinNT() && g_hInstNAVNTUTL == NULL )
	{
		// Load our helper libraries.
		g_hInstNAVNTUTL = LoadLibrary( _T("NAVNTUTL.DLL") );
		g_hInstNetApi32 = LoadLibrary( _T("NetApi32.DLL") );                

		// Get our function pointers
		g_pfnCopyAlternateDataStreams = (PFNCOPYALTERNATEDATASTREAMS)
			GetProcAddress( g_hInstNAVNTUTL,  
			_T( "CopyAlternateDataStreams" ));

		g_pfnNetWkstaGetInfo = (PFNNetWkstaGetInfo) GetProcAddress( g_hInstNetApi32,
			_T( "NetWkstaGetInfo") );

		g_pfnNetApiBufferFree = (PFNNetApiBufferFree) GetProcAddress( g_hInstNetApi32,
			_T( "NetApiBufferFree") );
	}

	// Dissable AP for this process.
	if (!m_APWrapper.DisableAPForThisThread())
    {
        CCTRACEE ("CQuarantineDLL::Initialize() - Failed to unprotect process, quarantine would generate AP alerts");
        return E_UNABLE_TO_DISSABLE_AP;
    }

	// If we do not have a QScanner create it now
	if(!m_pQScanner)
		m_pQScanner = new CSymStaticRefCount<CQScanner>;

	// Load the quarantine scanner
	if( m_pQScanner != NULL )
	{
		if( FAILED(m_pQScanner->Initialize(QUARANTINE_APP_ID)) )
		{
			// If this fails it's not the end of the world yet, because we'll fall back to the qurantine item
			// class for scanning the item
			CCTRACEE(_T("CQuarantineDLL::Initialize() - Failed to initialize the quarantine scanner"));
			delete m_pQScanner;
			m_pQScanner = NULL;
		}
		else
		{
			// Flag for later that
			CCTRACEI(_T("CQuarantineDLL::Initialize() - Loaded the quarantine scanner"));
		}
	}
	else
	{
		CCTRACEE(_T("CQuarantineDLL::Initialize() - Failed to create the quarantine scanner"));
	}

	m_bInitialized = true;

	return S_OK;
}

// CDAVIS: This was added so the PreInstall scanner can pass in its own Scanner pointer
// when quarantine is being used from the CD, it won't be able to load ccScan.
// the scanner that PreInstall scan passes in is already Initialized.
STDMETHODIMP CQuarantineDLL::SetScanner(IScanner* pScanner)
{
    // this should be called before initialize!
    if(m_bInitialized)
		return ERROR_ALREADY_INITIALIZED;
    
    // make sure our pScanner is ok
    if( !pScanner )
        return ERROR_INVALID_PARAMETER;
    
    if( !m_pQScanner )
        m_pQScanner = new CSymStaticRefCount<CQScanner>;

    return m_pQScanner->InitializeWithExternalScanner(pScanner);
}


STDMETHODIMP CQuarantineDLL::GetVirusInformation( unsigned long ulVirusID, IScanVirusInfo** ppInfo )
{
    if(!m_pQScanner)
    {
        CCTRACEE(_T("CQuarantineDLL::GetVirusInformation() - Do not have a valid scanner object"));
        return E_FAIL;
    }

    if( FAILED(m_pQScanner->GetVirusInformation(ulVirusID, ppInfo)) )
    {
        CCTRACEE(_T("CQuarantineDLL::GetVirusInformation() - Failed to get the virus information"));
        return E_FAIL;
    }

    return S_OK;
}


// ==== GetQuarantineFolder ==========================================================
// Description      : Gets the quarantine folder
// Return type      : HRESULT S_OK on success.
// Argument         : LPSTR lpszFolder - buffer to recive folder name
//					  UINT uBufferSize - size of buffer
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::GetQuarantineFolder( LPSTR lpszFolder,
    UINT uBufferSize )
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    if(!m_bInitialized)
        return E_UNINITIALIZED;

    if( NULL == lstrcpyn( lpszFolder, m_szQuarantineFolder, uBufferSize ))
        return E_COPY_FAILED;

    return S_OK;
}


// ==== CreateNewQuarantineItemFromFile ======================================
// Function name: CQuarantineDLL::AddFileToQuarantine --> CreateNewQuarantineItemFromFile()
// Description  : Addes a file to the quarantine system.
// Return type  : HRESULT - S_OK on success, E_FAIL on error.
// Argument     : LPSTR lpszFileName -- Path and filename of the file to Quarantine
//						This should be a long ANSI filename.
//                DWORD dwFileType -- The type of file (compressed...)
//                DWORD dwItemStatus -- The status of the file (Quarantine, backup...)
//                IQuarantineItem **pNewItem  -- This is a buffer for returning a
//						Interface to the new QuarantineItem.  This is used for changing
//						the file information (for example, SetStatus).
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
// 2/26/98 JTAYLOR - Added rough implementation.
// 3/11/98 JTAYLOR - Filled out implementation added scanning for viruses.
// 3/11/98 JTAYLOR - Added file type and file status arguements.
// 3/30/98 JTAYLOR - Added error codes and handling.
// 4/18/98 JTAYLOR - Added support for returning and IQuarantineItem pointer.
// 4/20/98 JTAYLOR - Added a file length check to make sure the whole file was Quarantined
//                   Added a check to make sure the buffer was allocated correctly and free it.
// 4/24/98 JTAYLOR - Added check to see if the file was compressed.
// 4/26/98 JTAYLOR - Added support for saving the original files date/times
// 5/08/98 JTAYLOR - Made the file transfer buffer not be on the stack.
// 5/20/98 JTAYLOR - Fixed 2 default handle values to be INVALID_HANDLE_VALUE
// 5/28/98 JTAYLOR - Set the *pNewItem to NULL to make sure it is NULL on failure.
// 6/03/98 JTAYLOR - Converted the Time/Date added to Quarantine back to GMT.
// 7/01/98 JTAYLOR - Changed the file time functions to not fail the process
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::CreateNewQuarantineItemFromFile(LPSTR lpszSourceFileName, DWORD dwFileType, DWORD dwItemStatus, IQuarantineItem **pNewItem)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    if(!m_bInitialized)
        return E_UNINITIALIZED;

    // Clear out the argument to make sure it is NULL on failure
    if( NULL != pNewItem )
        *pNewItem = NULL;

	HQSERVERITEM hQuarItem = NULL;
	TCHAR* lpszLocalBuffer = NULL;
	WCHAR* wcharStr= NULL;
	LPBYTE lpSecurityDesc = NULL;
	TCHAR szQuarFileName[MAX_QUARANTINE_FILENAME_LEN] = {0};

	try
	{
        TCHAR szQuarFileExtension[MAX_QUARANTINE_FILENAME_LEN] = {0};
		HANDLE hNewQuarFileHandle = INVALID_HANDLE_VALUE;
        hNewQuarFileHandle = CreateUniqueFile(m_szQuarantineFolder, GetFileExtension(lpszSourceFileName, szQuarFileExtension), szQuarFileName);
		
        CCTRACEI(_T("CQuarantineDLL::CreateNewQuarantineItemFromFile() - Unique quarantine filename: %s. Original File name: %s."), szQuarFileName, lpszSourceFileName);
		
		// Make sure we have a unique filename for the new quarantine file
		if(hNewQuarFileHandle == INVALID_HANDLE_VALUE)
			throw runtime_error("Unable to get unique filename");

		CloseHandle(hNewQuarFileHandle);
		DeleteFile(szQuarFileName);

		// Create a new Quarantine File
		QSPAKSTATUS qsResult;
		qsResult = QsPakCreateItemFromFile(&hQuarItem, lpszSourceFileName, szQuarFileName); 
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to create a new quarantine file");
		
		// Store the original filename
		// If the name is longer than MAX_QUARANTINE_FILENAME_LEN, truncate it
		DWORD dwNameLen = _tcslen(lpszSourceFileName);

		// If the current File APIS are in OEM mode, convert the file name to ANSI for display
		vector<char> vAnsiFileName;
		vAnsiFileName.reserve(dwNameLen);
		
		if (AreFileApisANSI() == TRUE)
			_tcsncpy(&vAnsiFileName[0], lpszSourceFileName, dwNameLen);
		else
			OemToCharBuff(lpszSourceFileName, &vAnsiFileName[0],  dwNameLen);

		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_FILENAME_ANSI, QSPAK_TYPE_STRING, &vAnsiFileName[0], dwNameLen);
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to save file name");

        dwNameLen = _tcslen(szQuarFileName);
        vAnsiFileName.clear();
        vAnsiFileName.reserve(dwNameLen+1);
        if (AreFileApisANSI() == TRUE)
            _tcsncpy(&vAnsiFileName[0], szQuarFileName, dwNameLen+1);
        else
            OemToCharBuff(szQuarFileName, &vAnsiFileName[0],  dwNameLen+1);

		// Store the Quarantine Item path/filename
        CCTRACEI(_T("CQuarantineDLL::CreateNewQuarantineItemFromFile() - saving the quarantine item path name as %s"), &vAnsiFileName[0]);
		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_QUAR_PATH, QSPAK_TYPE_STRING, &vAnsiFileName[0], sizeof(dwNameLen));
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to save item path");

        TCHAR szTempBuff[MAX_QUARANTINE_FILENAME_LEN];
		if( NOERR != NameReturnFile(&vAnsiFileName[0],szTempBuff))
			throw runtime_error("File manipulation Failed");

		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_QUAR_FILENAME, QSPAK_TYPE_STRING, szTempBuff, sizeof(szTempBuff));
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to save item file name");

		// Get File attributes
		WIN32_FILE_ATTRIBUTE_DATA fdFileData;
		if(!GetFileAttributesEx(lpszSourceFileName,GetFileExInfoStandard, &fdFileData))
		{
			COSInfo osi;
			if(osi.IsWinNT())
			{
				vector<WCHAR> vSourceFileName;
				ConvertLongFileToUnicodeBuffer(lpszSourceFileName, vSourceFileName);
				if(!GetFileAttributesExW(&vSourceFileName[0],GetFileExInfoStandard, &fdFileData))
					throw runtime_error("Unable to get file attributes on file to be Quarantined");
			}
			else
			{
				throw runtime_error("Unable to get file attributes on file to be Quarantined");
			}
		}
		
		// Save off original file size.
		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_FILESIZE, QSPAK_TYPE_DWORD, &fdFileData.nFileSizeLow, sizeof(DWORD)); 
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to Save file size");		
		
		// Save off the file type
		if(QSPAKSTATUS_OK != QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_SAMPLE_DWTYPE, QSPAK_TYPE_DWORD, &dwFileType, sizeof(dwFileType)))
			throw runtime_error("Unable to Save file type");

		   // copy the file status
		if(QSPAKSTATUS_OK != QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_STATUS, QSPAK_TYPE_DWORD, &dwItemStatus, sizeof(dwItemStatus)))
			throw runtime_error("Unable to Save file status");

		// Save off the current time
		SYSTEMTIME stSysTime;
		QSPAKDATE stQSDate;
		GetSystemTime(&stSysTime);
		stQSDate = SysTimeToQSDate(stSysTime);

		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_QUARANTINE_QDATE, QSPAK_TYPE_DATE, &stQSDate, sizeof(stQSDate));  
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to save current time");

		if(FAILED(SetOriginalScanDefsDateToCurrent(hQuarItem)))
			throw runtime_error("Unable to save original def date");

        // Get and Set the md5 hash for the file
        HANDLE hOrigFile = CreateFile(lpszSourceFileName,
                                      GENERIC_READ,
                                      FILE_SHARE_READ,
                                      NULL,
                                      OPEN_EXISTING,
                                      FILE_ATTRIBUTE_NORMAL,
                                      NULL);
        if( INVALID_HANDLE_VALUE == hOrigFile  )
        {
            // Now try for SLFN's if on NT
            COSInfo osi;
			if(osi.IsWinNT())
			{
				vector<WCHAR> vSourceFileName;
				ConvertLongFileToUnicodeBuffer(lpszSourceFileName, vSourceFileName);
				hOrigFile = CreateFileW(&vSourceFileName[0],
                                       GENERIC_READ,
                                       FILE_SHARE_READ,
                                       NULL,
                                       OPEN_EXISTING,
                                       FILE_ATTRIBUTE_NORMAL,
                                       NULL);
			}
        }

        // Set the hash if we have a valid file handle
        if( INVALID_HANDLE_VALUE != hOrigFile )
        {
            CMD5 md5;
            char szMd5[33] = {0};
            if( md5.SetHash(hOrigFile) )
            {
                // Write the hash to the sring in hex
                _stprintf(szMd5, _T("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"), 
                                                                            md5.pbyResultHash[0],
                                                                            md5.pbyResultHash[1],
                                                                            md5.pbyResultHash[2],
                                                                            md5.pbyResultHash[3],
                                                                            md5.pbyResultHash[4],
                                                                            md5.pbyResultHash[5],
                                                                            md5.pbyResultHash[6],
                                                                            md5.pbyResultHash[7],
                                                                            md5.pbyResultHash[8],
                                                                            md5.pbyResultHash[9],
                                                                            md5.pbyResultHash[10],
                                                                            md5.pbyResultHash[11],
                                                                            md5.pbyResultHash[12],
                                                                            md5.pbyResultHash[13],
                                                                            md5.pbyResultHash[14],
                                                                            md5.pbyResultHash[15]);

                qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_MD5, QSPAK_TYPE_STRING, szMd5, sizeof(szMd5));  
		        if( qsResult != QSPAKSTATUS_OK )
			        throw runtime_error("Unable to save md5 hash");
        

                CloseHandle(hOrigFile);
                hOrigFile = NULL;
            }
        }

		// Save the original file time information.
		// Copy the original date/times of the source files.
		FileTimeToSystemTime(&fdFileData.ftCreationTime, &stSysTime);
		stQSDate = SysTimeToQSDate(stSysTime);
		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_FILE_CREATED_TIME, QSPAK_TYPE_DATE, &stQSDate, sizeof(stQSDate));  
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to Save file creation time");

		FileTimeToSystemTime(&fdFileData.ftLastAccessTime, &stSysTime);
		stQSDate = SysTimeToQSDate(stSysTime);
		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_FILE_ACCESSED_TIME, QSPAK_TYPE_DATE, &stQSDate, sizeof(stQSDate));  
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to Save last file accessed time");
		
		FileTimeToSystemTime(&fdFileData.ftLastWriteTime, &stSysTime);
		stQSDate = SysTimeToQSDate(stSysTime);
		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_FILE_MODIFIED_TIME, QSPAK_TYPE_DATE, &stQSDate, sizeof(stQSDate));  
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to Save last file write time");


		// Allocate a buffer
		DWORD dwLocalBufferSize = MAX_QUARANTINE_FILENAME_LEN;
		lpszLocalBuffer = new TCHAR[dwLocalBufferSize];
		if(lpszLocalBuffer == NULL)
			throw runtime_error("Memory Allocation Failure");

		// Save off the user name
		GetUserName( lpszLocalBuffer, &dwLocalBufferSize ); // dwLocalBufferSize includes the terminating null character
		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_USERNAME, QSPAK_TYPE_STRING, lpszLocalBuffer, (dwLocalBufferSize - 1));  
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to save user name");

		// Save off the computer name
		dwLocalBufferSize = MAX_QUARANTINE_FILENAME_LEN; // reset the buffer size 
		GetComputerName( lpszLocalBuffer, &dwLocalBufferSize );  // dwLocalBufferSize does NOT include the terminating null character
		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_MACHINENAME, QSPAK_TYPE_STRING, lpszLocalBuffer, dwLocalBufferSize);  
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to save computer name");
		
		// get domain name here
		dwLocalBufferSize = MAX_QUARANTINE_FILENAME_LEN; // reset the buffer size 
		GetDomainName( lpszLocalBuffer, dwLocalBufferSize );
		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_DOMAINNAME, QSPAK_TYPE_STRING, lpszLocalBuffer, dwLocalBufferSize);
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to save domain name");

		// Fill in the GUID field of the header.
		GUID idGuid;
		dwLocalBufferSize = MAX_QUARANTINE_FILENAME_LEN;
		if( FAILED( CoCreateGuid( &idGuid ) ) )
			throw runtime_error("Unable to create GUID for file");

		// Allocate a buffer
		wcharStr = new WCHAR[MAX_QUARANTINE_FILENAME_LEN ];
		if(wcharStr == NULL)
			throw runtime_error("Unable to allocate Buffer");

		StringFromGUID2(idGuid, wcharStr, MAX_QUARANTINE_FILENAME_LEN );
		
		dwLocalBufferSize = wcstombs(lpszLocalBuffer, wcharStr, dwLocalBufferSize); // return value does NOT include terminating null
		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_UUID, QSPAK_TYPE_STRING, lpszLocalBuffer, dwLocalBufferSize);
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to save GUID");

		// Save subfolder name for later use in saving remediation data files
		TCHAR szDataSubfolderName[MAX_QUARANTINE_FILENAME_LEN] = {0};
		_tcscpy(szDataSubfolderName, m_szQuarantineFolder);
		NameAppendFile(szDataSubfolderName, lpszLocalBuffer);
        qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_DATA_SUBFOLDER, QSPAK_TYPE_STRING, szDataSubfolderName, strlen(szDataSubfolderName));
		if(qsResult != QSPAKSTATUS_OK)
			throw runtime_error("Unable to save subfolder name");
        
		// Make sure we preserve security on NT systems.
		if( FAILED( GetFileSecurityDesc( lpszSourceFileName, &lpSecurityDesc ) ) )
		{
			// Cleanup
            CCTRACEE("CQuarantineDLL::CreateNewQuarantineItemFromFile() - Failed to set the security descriptor.");
			QsPakReleaseItem(hQuarItem);
			return E_FAIL;
		}
		SetFileSecurityDesc( szQuarFileName, lpSecurityDesc );

		//Save out the Changes
		QsPakSaveItem(hQuarItem);

		//Cleanup
		QsPakReleaseItem(hQuarItem);

		// Free buffers
		if(NULL != lpszLocalBuffer)
		{
			delete[] lpszLocalBuffer;
			lpszLocalBuffer = NULL;
		}

		if(NULL != wcharStr)
		{
			delete[] wcharStr;
			wcharStr = NULL;
		}

		// Free security descriptor.
		if( NULL != lpSecurityDesc )
		{
			delete [] lpSecurityDesc;
			lpSecurityDesc = NULL;
		}
	}
	catch(exception& Ex)
	{
		CCTRACEE(_T("CQuarantineDLL::CreateNewQuarantineItemFromFile() - Caught exception %s"), Ex.what());
		
		// Cleanup
		if( hQuarItem )
			QsPakReleaseItem(hQuarItem);

		// Free buffers
		if(NULL != lpszLocalBuffer)
		{
			delete[] lpszLocalBuffer;
			lpszLocalBuffer = NULL;
		}

		if(NULL != wcharStr)
		{
			delete[] wcharStr;
			wcharStr = NULL;
		}

		// Free security descriptor.
		if( NULL != lpSecurityDesc )
		{
			delete [] lpSecurityDesc;
			lpSecurityDesc = NULL;
		}
		
		return E_FAIL;    
	}

	// Scan the new Quarantine Item to update its virus information
    try
    {
	    return ScanNewQuarantineMainFile(szQuarFileName, pNewItem);
    }
    catch(exception& Ex)
    {
        CCTRACEE(_T("CQuarantineDLL::CreateNewQuarantineItemFromFile() - Caught exception %s, attempting to scan new quarantine item."), Ex.what());
        return E_FAIL;
    }
    catch(...)
    {
        CCTRACEE(_T("CQuarantineDLL::CreateNewQuarantineItemFromFile() - Caught unknown exception attempting to scan new quarantine item."));
        return E_FAIL;
    }
} // END CQuarantineDLL::CreateNewQuarantineItemFromFile()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineDLL::CreateNewQuarantineItemWithoutFile(DWORD dwItemStatus, IQuarantineItem **pNewItem)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    if(!m_bInitialized)
        return E_UNINITIALIZED;

    // Clear out the argument to make sure it is NULL on failure
    if( NULL != pNewItem )
        *pNewItem = NULL;

	HQSERVERITEM hQuarItem = NULL;
	TCHAR* lpszLocalBuffer = NULL;
	WCHAR* wcharStr= NULL;
	TCHAR szQuarFileName[MAX_QUARANTINE_FILENAME_LEN] = {0};

	try
	{
		TCHAR szDummyFilePath[MAX_PATH * 2] = {0};
		GenerateUniqueDummyFileInQuarantineFolder(szDummyFilePath);

		HANDLE hNewQuarFileHandle = INVALID_HANDLE_VALUE;
        hNewQuarFileHandle = CreateUniqueFile(m_szQuarantineFolder, _T(""), szQuarFileName);

        CCTRACEI(_T("CQuarantineDLL::CreateNewQuarantineItemWithoutFile() - Unique quarantine filename: %s. Generated Dummy File name: %s."), szQuarFileName, szDummyFilePath);
		
		// Make sure we have a unique filename for the new quarantine file
		if(hNewQuarFileHandle == INVALID_HANDLE_VALUE)
			throw runtime_error("Unable to get unique filename");

		CloseHandle(hNewQuarFileHandle);
		DeleteFile(szQuarFileName);

		// Create a new Quarantine File
		QSPAKSTATUS qsResult;
		qsResult = QsPakCreateItemFromFile(&hQuarItem, szDummyFilePath, szQuarFileName);
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to create a new quarantine file");

		DeleteFile(szDummyFilePath);
		
		// Store a dummy filename
		DWORD dwNameLen = _tcslen(QUARANTINE_DUMMYFILE_NAME);
		TCHAR* szDummyName = new TCHAR[dwNameLen + 1];
		ZeroMemory(szDummyName, (dwNameLen + 1));
		if(szDummyName == NULL)
			throw runtime_error("Memory Allocation Failure");

		_tcscpy(szDummyName, QUARANTINE_DUMMYFILE_NAME);
		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_FILENAME_ANSI, QSPAK_TYPE_STRING, szDummyName, dwNameLen);
		delete[] szDummyName;
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to save file name");

		// Store the Quarantine Item Filename
		TCHAR szTempBuff[MAX_QUARANTINE_FILENAME_LEN];
		if( NOERR != NameReturnFile(szQuarFileName,szTempBuff))
			throw runtime_error("File manipulation Failed");

		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_QUAR_FILENAME, QSPAK_TYPE_STRING, szTempBuff, _tcslen(szTempBuff));
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to save file name");

		// Save off original file size.
		DWORD dwFileSize = 0;
		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_FILESIZE, QSPAK_TYPE_DWORD, &dwFileSize , sizeof(DWORD)); 
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to Save file size");		
		
		DWORD dwTemp = 1;
		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_USING_DUMMY_FILE, QSPAK_TYPE_DWORD, &dwTemp , sizeof(DWORD)); 
		if( qsResult != QSPAKSTATUS_OK )
			CCTRACEE("Unable to set dummy file flag");		

		// Save off the file type
		DWORD dwFileType = QFILE_TYPE_DUMMY;
		if(QSPAKSTATUS_OK != QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_SAMPLE_DWTYPE, QSPAK_TYPE_DWORD, &dwFileType, sizeof(dwFileType)))
			throw runtime_error("Unable to Save file type");

		// copy the file status
		if(QSPAKSTATUS_OK != QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_STATUS, QSPAK_TYPE_DWORD, &dwItemStatus, sizeof(dwItemStatus)))
			throw runtime_error("Unable to Save file status");

		// Save off the current time
		SYSTEMTIME stSysTime;
		QSPAKDATE stQSDate;
		GetSystemTime(&stSysTime);
		stQSDate = SysTimeToQSDate(stSysTime);

		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_QUARANTINE_QDATE, QSPAK_TYPE_DATE, &stQSDate, sizeof(stQSDate));  
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to save current time");

		if(FAILED(SetOriginalScanDefsDateToCurrent(hQuarItem)))
			throw runtime_error("Unable to save original def date");
        
        // Save the current time as the file time information.
        qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_FILE_CREATED_TIME, QSPAK_TYPE_DATE, &stQSDate, sizeof(stQSDate));  
        if( qsResult != QSPAKSTATUS_OK )
            throw runtime_error("Unable to Save file creation time");

        qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_FILE_ACCESSED_TIME, QSPAK_TYPE_DATE, &stQSDate, sizeof(stQSDate));  
        if( qsResult != QSPAKSTATUS_OK )
            throw runtime_error("Unable to Save last file accessed time");

        qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_FILE_MODIFIED_TIME, QSPAK_TYPE_DATE, &stQSDate, sizeof(stQSDate));  
        if( qsResult != QSPAKSTATUS_OK )
            throw runtime_error("Unable to Save last file write time");


		// Don't bother setting MD5 hash information

		// Don't bother saving the original file time information.

		// Allocate a buffer
		DWORD dwLocalBufferSize = MAX_QUARANTINE_FILENAME_LEN;
		lpszLocalBuffer = new TCHAR[dwLocalBufferSize];
		if(lpszLocalBuffer == NULL)
			throw runtime_error("Memory Allocation Failure");

		// Save off the user name
		GetUserName( lpszLocalBuffer, &dwLocalBufferSize ); // dwLocalBufferSize  includes the terminating null character
		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_USERNAME, QSPAK_TYPE_STRING, lpszLocalBuffer, (dwLocalBufferSize - 1));
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to save user name");

		// Save off the computer name
		dwLocalBufferSize = MAX_QUARANTINE_FILENAME_LEN;
		dwLocalBufferSize = MAX_QUARANTINE_FILENAME_LEN; // reset the buffer size 
		GetComputerName( lpszLocalBuffer, &dwLocalBufferSize ); // dwLocalBufferSize does NOT include the terminating null character
		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_MACHINENAME, QSPAK_TYPE_STRING, lpszLocalBuffer, dwLocalBufferSize);  
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to save computer name");
		
		// get domain name here
		dwLocalBufferSize = MAX_QUARANTINE_FILENAME_LEN;
		dwLocalBufferSize = MAX_QUARANTINE_FILENAME_LEN; // reset the buffer size 
		GetDomainName( lpszLocalBuffer, dwLocalBufferSize );
		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_DOMAINNAME, QSPAK_TYPE_STRING, lpszLocalBuffer, _tcslen(lpszLocalBuffer));  
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to save domain name");

		// Fill in the GUID field of the header.
		GUID idGuid;
		dwLocalBufferSize = MAX_QUARANTINE_FILENAME_LEN;
		if( FAILED( CoCreateGuid( &idGuid ) ) )
			throw runtime_error("Unable to create GUID for file");

		// Allocate a buffer
		wcharStr = new WCHAR[MAX_QUARANTINE_FILENAME_LEN ];
		if(wcharStr == NULL)
			throw runtime_error("Unable to allocate Buffer");

		StringFromGUID2(idGuid, wcharStr, MAX_QUARANTINE_FILENAME_LEN );
		
		dwLocalBufferSize = wcstombs(lpszLocalBuffer, wcharStr, dwLocalBufferSize); // return value does NOT include terminating null
		qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_UUID, QSPAK_TYPE_STRING, lpszLocalBuffer, dwLocalBufferSize);
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to save GUID");

		// Save subfolder path for later use in saving remediation data files
		// Using GUID as subfolder name.
		TCHAR szDataSubfolderName[MAX_QUARANTINE_FILENAME_LEN] = {0};
		_tcscpy(szDataSubfolderName, m_szQuarantineFolder);
		NameAppendFile(szDataSubfolderName, lpszLocalBuffer);
        qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_DATA_SUBFOLDER, QSPAK_TYPE_STRING, szDataSubfolderName, strlen(szDataSubfolderName));
		if( qsResult != QSPAKSTATUS_OK )
			throw runtime_error("Unable to save subfolder name");
        
		// Not trying to preserve security on NT systems, since there was no original file

		//Save out the Changes
		QsPakSaveItem(hQuarItem);

		//Cleanup
		QsPakReleaseItem(hQuarItem);

		// Free buffers
		if(NULL != lpszLocalBuffer)
		{
			delete[] lpszLocalBuffer;
			lpszLocalBuffer = NULL;
		}

		if(NULL != wcharStr)
		{
			delete[] wcharStr;
			wcharStr = NULL;
		}

		if(NULL != pNewItem) // Check to see if the person calling the function wanted an IQuarantineItem back
		{
			HRESULT hReturn = E_FAIL;
			IQuarantineItem* pQuarItem;
			pQuarItem = new CQuarantineItem;
			if(NULL == pQuarItem)
			{
				CCTRACEE(_T("CQuarantineDLL::CreateNewQuarantineItemWithoutFile() - Failed to create quarantine item."));
				return E_OUTOFMEMORY;
			}

			pQuarItem->AddRef();

			hReturn = pQuarItem->Initialize(szQuarFileName);
			if(FAILED(hReturn))
			{
				CCTRACEE(_T("CQuarantineDLL::CreateNewQuarantineItemWithoutFile() - Failed to initialize the quarantine item"));
				pQuarItem->Release();
				pQuarItem = NULL;
				return hReturn;
			}

			hReturn = pQuarItem->SaveItem();
			if(FAILED(hReturn))
			{
				CCTRACEE(_T("CQuarantineDLL::CreateNewQuarantineItemWithoutFile() - Failed to save the item"));
				pQuarItem->Release();
				return hReturn;
			}
			
			*pNewItem = pQuarItem;
		}
	}
	catch(exception& Ex)
	{
		CCTRACEE(_T("CQuarantineDLL::CreateNewQuarantineItemWithoutFile() - Caught exception %s"), Ex.what());

		// Cleanup
		if( hQuarItem )
			QsPakReleaseItem(hQuarItem);

		// Free buffers
		if(NULL != lpszLocalBuffer)
		{
			delete[] lpszLocalBuffer;
			lpszLocalBuffer = NULL;
		}

		if(NULL != wcharStr)
		{
			delete[] wcharStr;
			wcharStr = NULL;
		}

		return E_FAIL;    
	}

	return S_OK;

} // END CQuarantineDLL::CreateNewQuarantineItemWithoutFile()


// Added for NAV 2005.5/r11.5
HRESULT CQuarantineDLL::GenerateUniqueDummyFileInQuarantineFolder(LPTSTR szDummyFilePath)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	// seed the random number generator
	srand((unsigned)time(NULL));

	HANDLE hFile = INVALID_HANDLE_VALUE;
	TCHAR szGeneratedFileName[MAX_QUARANTINE_FILENAME_LEN];
	while(INVALID_HANDLE_VALUE == hFile)
	{
		//Generate a random number for the filename
		DWORD dwNumber = MAKELONG(rand(),rand());
		// Make sure that a DWORD stays defined as 32 bits.
		SPRINTF( szGeneratedFileName, _T("%.8X"), dwNumber & 0xFFFFFFFF );

		// Copy the path to the beginning of the output buffer
		_tcsncpy(szDummyFilePath, m_szQuarantineFolder, MAX_QUARANTINE_FILENAME_LEN);

		// Append the filename to the path.
		NameAppendFile(szDummyFilePath, szGeneratedFileName);

		// Attempt to create the new file.
		hFile = CreateFile(szDummyFilePath,
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
			// Loop again if the error was 'file already exists'.
			// ERROR_ALREADY_EXISTS is what the API claimed is returned if the
			// file already exists, ERROR_FILE_EXISTS is what is actually
			// returned on my computer.  I have included them both, in case one
			// is pre IE 4.0.
			DWORD dwLastError = GetLastError();
			if((ERROR_ALREADY_EXISTS != dwLastError) &&
				(ERROR_FILE_EXISTS != dwLastError))
			{
				//return INVALID_HANDLE_VALUE;
				return E_FAIL;
			}
		}
		else
		{
			CloseHandle(hFile);
			return S_OK;
		}
	}

	return E_FAIL;
}


// ==== SysTimeToQSDate ==============================================
// Helper Function to Convert Date/Time Formats from a SYSTEMTIME structure to
// a QSPAKDATE structure               
// Returns: A Translated QSPAKDATE Structure
QSPAKDATE CQuarantineDLL::SysTimeToQSDate(SYSTEMTIME stSysTime)
{
	QSPAKDATE stQSDate;
	stQSDate.byMonth = (BYTE)stSysTime.wMonth;
	stQSDate.byDay = (BYTE)stSysTime.wDay;
	stQSDate.wYear = stSysTime.wYear;
	stQSDate.byHour = (BYTE)stSysTime.wHour;
	stQSDate.byMinute = (BYTE)stSysTime.wMinute;
	stQSDate.bySecond = (BYTE)stSysTime.wSecond;
	return stQSDate;
}


// ==== QSDateToSysTime ==============================================
// Helper Function to Convert Date/Time Formats from a QSPAKDATE structure to
// a SYSTEMTIME structure
// Returns: A Translated QSPAKDATE Structure
void CQuarantineDLL::QSDateToSysTime(QSPAKDATE stQSDate, SYSTEMTIME* stSysTime)
{
	stSysTime->wMonth = (WORD)stQSDate.byMonth;
	stSysTime->wDay = (WORD)stQSDate.byDay;
	stSysTime->wYear = stQSDate.wYear;
	stSysTime->wHour = (WORD)stQSDate.byHour;
	stSysTime->wMinute = (WORD)stQSDate.byMinute;
	stSysTime->wSecond = (WORD)stQSDate.bySecond;
	stSysTime->wMilliseconds = 0;
}


// ==== ProcessAllIncomingFiles ==============================================
// Iterates through the Quarantine Incoming directory and calls
// ProcessIncomingFile on each file in it.  After it is done with that, it
// calls ForwardAllFiles
// Output: HRESULT - S_OK on success
//                   E_UNINITIALIZED if the IQuarantineDLL is not initialized
//                   E_FAIL if there is any problem processing files.
////////////////////////////////////////////////////////////////////////////
// 4/26/98 JTAYLOR - Function created / Header added.
// 4/27/98 JTAYLOR - Updated to acutally support file searching.
// 5/26/98 JTAYLOR - Changed process to not stop on first error.
// 5/28/98 JTAYLOR - Added a call to ForwardAllFiles.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::ProcessAllIncomingFiles()
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    if(!m_bInitialized)
        return E_UNINITIALIZED;

    WIN32_FIND_DATA     wfd;
    BOOL    bContinue = TRUE;
    BOOL    bResult   = TRUE;
    TCHAR   szCurrentFilename[MAX_QUARANTINE_FILENAME_LEN];
    TCHAR   szQuarantineIncomingPath[MAX_QUARANTINE_FILENAME_LEN];
    TCHAR   szQuarantineIncomingPathFilespec[MAX_QUARANTINE_FILENAME_LEN];

    // Read the incoming path from the registry.
    bResult = GetQuarantinePath( szQuarantineIncomingPath,
                                 MAX_QUARANTINE_FILENAME_LEN,
                                 TARGET_QUARANTINE_INCOMING_PATH );
    if( FALSE == bResult )
        {
        return E_FAIL;
        }

    // Append the \*.* to the path
    _tcscpy( szQuarantineIncomingPathFilespec, szQuarantineIncomingPath );
    NameAppendFile(szQuarantineIncomingPathFilespec, "*.*" );

    HANDLE  hFinder = FindFirstFile(szQuarantineIncomingPathFilespec,&wfd);
    // Add all of the test files to Quarantine
    if(INVALID_HANDLE_VALUE==hFinder)
        {
        return E_FAIL;
        }

    // Loop through each of the files.
    while(bContinue)
        {
        // If the file is not a directory then process it.
        if(!(FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes))
            {
            strcpy(szCurrentFilename, szQuarantineIncomingPath);
            NameAppendFile( szCurrentFilename, wfd.cFileName );

            HRESULT retVal = ProcessIncomingFile(szCurrentFilename);
            if( FAILED( retVal ) )
                {
                bResult = FALSE;
                }
            }

        // Stop the loop the first time FindNextFile fails.
        bContinue = FindNextFile(hFinder,&wfd);
        }

    FindClose(hFinder);

    if( FALSE == bResult )
        return E_FAIL;

    return S_OK;
}


// ==== ProcessIncomingFile ==================================================
// Processes a file from the incoming directory and adds it to the Quarantine
// directory.  This also upgrades the header used to insert the file to the
// full Quarantine header.  This function deletes the source file.
// Input: LPSTR lpszFileName - Path and filename of the file to process
//                             This should be a long ANSI filename.
// Output: HRESULT - S_OK on success
////////////////////////////////////////////////////////////////////////////
// 4/25/98 JTAYLOR - Function created / Header added.
// 4/26/98 JTAYLOR - Added support for saving the original files date/times
// 4/26/98 JTAYLOR - Added check to make sure that invalid files are skipped.
// 5/08/98 JTAYLOR - Made the file transfer buffer not be on the stack.
// 5/20/98 JTAYLOR - Fixed the delete, and added delete for the destination
//                      file when the processing fails.
// 7/01/98 JTAYLOR - Changed the file time functions to not fail the process
// 7/01/98 JTAYLOR - Do not delete the source file if it does not have a
//                      valid Quarantine signature.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::ProcessIncomingFile( LPSTR lpszSourceFileName )
{	
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    if(!m_bInitialized)
        return E_UNINITIALIZED;

    char                szDestFileName[MAX_QUARANTINE_FILENAME_LEN] = "";
    HANDLE              hSource = INVALID_HANDLE_VALUE;
	HANDLE				hTempFile = INVALID_HANDLE_VALUE;
	char                szTempFileName[MAX_QUARANTINE_FILENAME_LEN] = "";
    PQFILE_AP_INFO_STRUCT  pstQuarantineSource = NULL;
    QFILE_AP_HEADER_STRUCT stQuarantineHeader = {0};
	DWORD               dwBytesRead = 0;
	DWORD               dwBytesWritten = 0;
    HRESULT             retVal = S_OK;
    HRESULT             hrTemp = S_OK;
    BOOL                bResult = FALSE;
    BOOL                bInvalidFile = FALSE;
	HQSERVERITEM		hQuarItem = NULL;
    TCHAR				szQuarFileName[MAX_QUARANTINE_FILENAME_LEN] = {0};
	HANDLE				hNewQuarFileHandle = NULL;
	TCHAR*				lpszLocalBuffer = NULL;
	DWORD				dwLocalBufferSize = MAX_QUARANTINE_FILENAME_LEN;
	WCHAR*				wcharStr = NULL;	
    GUID				idGuid = {0};
    TCHAR				szTempPath[MAX_PATH] = {0};
    QSPAKDATE           qsDate = {0};
    CMD5                md5;
	
    // Open the source file for reading.
    hSource = CreateFile( lpszSourceFileName,
              GENERIC_READ,
              0,
              NULL,
              OPEN_EXISTING,
              FILE_FLAG_SEQUENTIAL_SCAN,
              NULL );

    if( hSource == INVALID_HANDLE_VALUE )
    {
        retVal = E_FILE_CREATE_FAILED;
        goto  Exit_Function;
    }

    // Read in the header of the AP header structure.
    bResult = ReadFile(hSource, &stQuarantineHeader, sizeof(QFILE_AP_HEADER_STRUCT), &dwBytesRead, NULL);
    if( FALSE == bResult )
    {
        retVal = E_FILE_READ_FAILED;
        goto  Exit_Function;
    }

    // If the header signature is wrong, then return S_OK.
    if( QUARANTINE_SIGNATURE != stQuarantineHeader.dwSignature )
    {
        bInvalidFile = TRUE;
        retVal = S_OK;
        goto Exit_Function;
    }

    // Reset to the begining of the file
    if( 0xFFFFFFFF == SetFilePointer(hSource, 0, NULL, FILE_BEGIN ) )
    {
        retVal = E_FILE_READ_FAILED;
        goto  Exit_Function;
    }

    // Allocate appropriate memory for the quarantine structure
    pstQuarantineSource = (PQFILE_AP_INFO_STRUCT)new BYTE[stQuarantineHeader.dwHeaderByteSize];
    if( NULL == pstQuarantineSource )
    {
        retVal = E_FILE_READ_FAILED;
        goto  Exit_Function;
    }

    // Read in the entire quarantine header information
    bResult = ReadFile(hSource, pstQuarantineSource, stQuarantineHeader.dwHeaderByteSize, &dwBytesRead, NULL);
    if( FALSE == bResult )
    {
        retVal = E_FILE_READ_FAILED;
        goto  Exit_Function;
    }

	// Now Copy the actual file from the AP file to a temp file
	GetTempPath(MAX_PATH, szTempPath);
	GetTempFileName(szTempPath, "tmp", 0, szTempFileName);

    // Create a file to copy into
    hTempFile = CreateFile( szTempFileName,
              GENERIC_READ | GENERIC_WRITE,
              0,
              NULL,
              CREATE_ALWAYS,
              FILE_ATTRIBUTE_TEMPORARY,
              NULL );

    // Seek the hSource to the end of the header.
    if( 0xFFFFFFFF == SetFilePointer(hSource, stQuarantineHeader.dwHeaderByteSize, NULL, FILE_BEGIN ) )
    {
        retVal = E_FILE_READ_FAILED;
        goto  Exit_Function;
    }

    // Transfer the orignal file into the new Quarantine file.
    retVal = TransferFile( lpszSourceFileName, hSource, szTempFileName, hTempFile );
    if( FAILED( retVal ) )
    {
        goto  Exit_Function;
    }

    // Make sure the the entire file was copied.  
    dwBytesRead = pstQuarantineSource->dwOriginalFileSize;
    dwBytesWritten = GetFileSize( hTempFile, NULL );
    if( dwBytesRead != dwBytesWritten )
    {
        retVal = E_FILE_WRITE_FAILED;
        goto  Exit_Function;
    }	

    // Close the handle to our temp file
    CloseHandle(hTempFile);
    hTempFile = INVALID_HANDLE_VALUE;

    // Re-open the file with read access to get the md5 hash
    hTempFile = CreateFile(szTempFileName,
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);

    // Get the md5 hash of the original infected item
    char szMd5[33] = {0};
    if( hTempFile != INVALID_HANDLE_VALUE )
    {
        if( md5.SetHash(hTempFile) )
        {
            // Write the hash to the sring in hex
            _stprintf(szMd5, _T("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"), 
                                                                            md5.pbyResultHash[0],
                                                                            md5.pbyResultHash[1],
                                                                            md5.pbyResultHash[2],
                                                                            md5.pbyResultHash[3],
                                                                            md5.pbyResultHash[4],
                                                                            md5.pbyResultHash[5],
                                                                            md5.pbyResultHash[6],
                                                                            md5.pbyResultHash[7],
                                                                            md5.pbyResultHash[8],
                                                                            md5.pbyResultHash[9],
                                                                            md5.pbyResultHash[10],
                                                                            md5.pbyResultHash[11],
                                                                            md5.pbyResultHash[12],
                                                                            md5.pbyResultHash[13],
                                                                            md5.pbyResultHash[14],
                                                                            md5.pbyResultHash[15]);
            CCTRACEI(_T("CQuarantineDLL::ProcessIncomingFile() - Hash value = %s"), szMd5);
        }
        else
            CCTRACEE(_T("CQuarantineDLL::ProcessIncomingFile() - Failed to set the hash value"));

        // Close the handle to our temp file
        CloseHandle(hTempFile);
        hTempFile = INVALID_HANDLE_VALUE;
    }
    else
        CCTRACEE(_T("CQuarantineDLL::ProcessIncomingFile() - Failed to open the temp file to get the hash value"));

	// First get a unique filename for the new quarantine file
	TCHAR szQuarFileExtension[MAX_QUARANTINE_FILENAME_LEN] = {0};
	hNewQuarFileHandle = CreateUniqueFile(m_szQuarantineFolder, GetFileExtension(lpszSourceFileName, szQuarFileExtension), szQuarFileName);

    CCTRACEI(_T("CQuarantineDLL::ProcessIncomingFile() - Unique quarantine filename: %s. Original File name: %s."), szQuarFileName, lpszSourceFileName);

	CloseHandle(hNewQuarFileHandle);
	DeleteFile(szQuarFileName);

	// Create a new Quarantine File
	hrTemp = QsPakCreateItemFromFile(&hQuarItem, szTempFileName, szQuarFileName);    
	if( hrTemp != QSPAKSTATUS_OK )
    {
		retVal = E_FAIL;
		goto Exit_Function;
	}

    // Store the md5 hash
    if( 0 != _tcslen(szMd5) )
    {
        hrTemp = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_MD5, QSPAK_TYPE_STRING, szMd5, sizeof(szMd5));  
	    if( hrTemp != QSPAKSTATUS_OK )
        {
		    retVal = E_FAIL;
		    goto Exit_Function;
        }
    }

	// Store the Quarantine Filename
	TCHAR szTempBuff[MAX_QUARANTINE_FILENAME_LEN];
	if( NOERR != NameReturnFile(szQuarFileName, szTempBuff))
    {
		retVal = E_FAIL;
		goto Exit_Function;
	}

	hrTemp = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_QUAR_FILENAME, QSPAK_TYPE_STRING, szTempBuff, _tcslen(szTempBuff));  
	if( hrTemp != QSPAKSTATUS_OK )
    {
		retVal = E_FAIL;
		goto Exit_Function;
	}

    // copy the file status
	if(QSPAKSTATUS_OK != QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_STATUS,QSPAK_TYPE_DWORD, 
                                           &pstQuarantineSource->dwFileStatus, 
										   sizeof(pstQuarantineSource->dwFileStatus)))
    {
		retVal = E_FAIL;
		goto Exit_Function;
	}

	// copy the file type
	if(QSPAKSTATUS_OK != QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_SAMPLE_DWTYPE, QSPAK_TYPE_DWORD, 
										   &pstQuarantineSource->dwFileType, 
										   sizeof(pstQuarantineSource->dwFileType)))
    {
		retVal = E_FAIL;
		goto Exit_Function;
	}

	// Allocate a buffer
	lpszLocalBuffer = new TCHAR[dwLocalBufferSize];
	if(lpszLocalBuffer == NULL)
    {
		retVal = E_FAIL;
		goto Exit_Function;
	}	
	
	// Save off the user name
	GetUserName(lpszLocalBuffer, &dwLocalBufferSize); // dwLocalBufferSize includes the terminating null character
	hrTemp = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_USERNAME, QSPAK_TYPE_STRING, lpszLocalBuffer, dwLocalBufferSize - 1);
	if( hrTemp != QSPAKSTATUS_OK )
    {
		retVal = E_FAIL;
		goto Exit_Function;
	}

	// Save off the computer name
	dwLocalBufferSize = MAX_QUARANTINE_FILENAME_LEN;
	GetComputerName( lpszLocalBuffer, &dwLocalBufferSize );   // dwLocalBufferSize does NOT include the terminating null character
	hrTemp = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_MACHINENAME, QSPAK_TYPE_STRING, lpszLocalBuffer, dwLocalBufferSize);  
	if( hrTemp != QSPAKSTATUS_OK )
    {
		retVal = E_FAIL;
		goto Exit_Function;
	}	
	
	// get domain name here
	dwLocalBufferSize = MAX_QUARANTINE_FILENAME_LEN;
	GetDomainName( lpszLocalBuffer, dwLocalBufferSize );
	hrTemp = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_DOMAINNAME, QSPAK_TYPE_STRING, lpszLocalBuffer, _tcslen(lpszLocalBuffer));  
	if( hrTemp != QSPAKSTATUS_OK )
	{
		retVal = E_FAIL;
		goto Exit_Function;
	}

	// Fill in the GUID field of the header.
	dwLocalBufferSize = MAX_QUARANTINE_FILENAME_LEN;
	if( FAILED( CoCreateGuid( &idGuid ) ) )
	{
		retVal = E_FAIL;
		goto Exit_Function;
	}

	// Allocate a buffer
	wcharStr = new WCHAR[MAX_QUARANTINE_FILENAME_LEN ];
	if(wcharStr == NULL)
    {
		retVal = E_FAIL;
		goto Exit_Function;
	}

	StringFromGUID2(idGuid, wcharStr, MAX_QUARANTINE_FILENAME_LEN );	
	dwLocalBufferSize = wcstombs(lpszLocalBuffer, wcharStr, dwLocalBufferSize); // return value does NOT include terminating null
	hrTemp = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_UUID, QSPAK_TYPE_STRING, lpszLocalBuffer, dwLocalBufferSize);  
	if( hrTemp != QSPAKSTATUS_OK )
    {
		retVal = E_FAIL;
		goto Exit_Function;
	}

    // Copy the original date/times for the file
	qsDate = SysTimeToQSDate(pstQuarantineSource->stDateQuarantined);
	if(QSPAKSTATUS_OK != QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_QUARANTINE_QDATE, QSPAK_TYPE_DATE, &qsDate, sizeof(qsDate)) )
    {
		retVal = E_FAIL;
		goto Exit_Function;
	}

	if(FAILED(SetOriginalScanDefsDateToCurrent(hQuarItem)))
	{
		CCTRACEE(_T("Unable to save original def date"));
		retVal = E_FAIL;
		goto Exit_Function;
	}

    // If we are on Windows 95, then translate the time that AP provided to a GMT time.
    if( !IsWinNT() )
    {
        // Get the information on the current timezone
        TIME_ZONE_INFORMATION tzInfo;
		BOOL bDaylightSavings = FALSE;
        DWORD wTZInfoRet = GetTimeZoneInformation( &tzInfo );
		if (wTZInfoRet == TIME_ZONE_ID_DAYLIGHT)
		{
			bDaylightSavings = TRUE;
		}

        // Get the date the file was quarantined as a FILETIME
        FILETIME tDateQuarantined;
		QSPAKDATE qsDateTemp;
		SYSTEMTIME stSysTimeTemp;
		DWORD dwQsDateSize = sizeof(qsDateTemp);
		if(QSPAKSTATUS_OK != QsPakQueryItemValue(hQuarItem, QSERVER_ITEM_INFO_QUARANTINE_QDATE, QSPAK_TYPE_DATE, &qsDateTemp, &dwQsDateSize))
		{
			retVal = E_FAIL;
			goto Exit_Function;
		}
		QSDateToSysTime(qsDateTemp, &stSysTimeTemp);

        SystemTimeToFileTime( &stSysTimeTemp, &tDateQuarantined );

        // Convert the filetime to a LONGLONG
        LONGLONG nFileTime;
        nFileTime = tDateQuarantined.dwHighDateTime;
        nFileTime <<= 32;
        nFileTime += tDateQuarantined.dwLowDateTime;

        // Adjust for timezone and daylight savings time
        LONGLONG nFileBias;
        // (Timezone difference in minutes + Daylight savings time in minutes ) * seconds in minute * 100 nanosecond intervals in a minute
        nFileBias = (LONGLONG)tzInfo.Bias * (LONGLONG)60 * (LONGLONG)10000000;
		if (bDaylightSavings == TRUE)
		{
			nFileBias += (LONGLONG)tzInfo.DaylightBias * (LONGLONG)60 * (LONGLONG)10000000;
		}
        nFileTime += nFileBias;

        // Convert back.
        unsigned int nFileTimeLow;
        unsigned int nFileTimeHigh;

        nFileTimeLow = (int)nFileTime & 0xFFFFFFFF;
        nFileTime >>= 32;
        nFileTimeHigh = (int)nFileTime & 0xFFFFFFFF;

        tDateQuarantined.dwLowDateTime = nFileTimeLow;
        tDateQuarantined.dwHighDateTime = nFileTimeHigh;

        // save the UTC time into the header structure.
		FileTimeToSystemTime(&tDateQuarantined, &stSysTimeTemp);
		qsDateTemp = SysTimeToQSDate(stSysTimeTemp);
		if(QSPAKSTATUS_OK != QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_QUARANTINE_QDATE, QSPAK_TYPE_DATE, &qsDateTemp, sizeof(qsDateTemp)) )
		{
			retVal = E_FAIL;
			goto Exit_Function;
		}
     }

	SYSTEMTIME stTempTime;
    FileTimeToSystemTime( &pstQuarantineSource->ftOriginalFileDateCreated, &stTempTime );
	qsDate = SysTimeToQSDate(stTempTime);
	if(QSPAKSTATUS_OK != QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_FILE_CREATED_TIME, QSPAK_TYPE_DATE, &qsDate, sizeof(qsDate)))  
    {
		retVal = E_FAIL;
		goto Exit_Function;
	}

    FileTimeToSystemTime( &pstQuarantineSource->ftOriginalFileDateAccessed, &stTempTime );
	qsDate = SysTimeToQSDate(stTempTime);
	if(QSPAKSTATUS_OK != QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_FILE_ACCESSED_TIME, QSPAK_TYPE_DATE, &qsDate, sizeof(qsDate)))  
    {
		retVal = E_FAIL;
		goto Exit_Function;
	}

    FileTimeToSystemTime( &pstQuarantineSource->ftOriginalFileDateWritten, &stTempTime );
	qsDate = SysTimeToQSDate(stTempTime);
	if(QSPAKSTATUS_OK != QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_FILE_MODIFIED_TIME, QSPAK_TYPE_DATE, &qsDate, sizeof(qsDate)))  
    {
		retVal = E_FAIL;
		goto Exit_Function;
	}

    if(QSPAKSTATUS_OK != QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_FILESIZE, QSPAK_TYPE_DWORD, 
										   &pstQuarantineSource->dwOriginalFileSize, 
										   sizeof(pstQuarantineSource->dwOriginalFileSize)))
    {
		retVal = E_FAIL;
		goto Exit_Function;
	}
    
    //
	// Get the original filename
    //

    // Free the local buffer
    if( lpszLocalBuffer )
    {
        delete[] lpszLocalBuffer;
        lpszLocalBuffer = NULL;
    }

    // Allocate the bytes necessary for the ANSI name, add 1 for the terminating NULL
    lpszLocalBuffer = (LPTSTR)new BYTE[pstQuarantineSource->dwOriginalFileNameSize + 1];
    if( !lpszLocalBuffer )
    {
        retVal = E_FAIL;
		goto Exit_Function;
    }

    // Convert the wide char name to mbcs
    WideCharToMultiByte(CP_ACP, // ANSI code page
                        0,      // no flags
                        (PWCHAR)((PBYTE)pstQuarantineSource + pstQuarantineSource->dwOriginalFileNameOffset ), // Wide char file name
                        -1,     // NULL terminated string
                        lpszLocalBuffer, // This will be the ANSI mbcs version of the file name
                        pstQuarantineSource->dwOriginalFileNameSize + 1, // buffer size
                        NULL,   // Use defualt
                        NULL);  // replacement characters

	// Store the original filename
	hrTemp = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_FILENAME_ANSI, QSPAK_TYPE_STRING, 
									lpszLocalBuffer, 
									pstQuarantineSource->dwOriginalFileNameSize);  
	if( hrTemp != QSPAKSTATUS_OK )
    {
		retVal = E_FAIL;
		goto Exit_Function;
	}


Exit_Function:

    // close files
    if (INVALID_HANDLE_VALUE != hSource)
    {
        CloseHandle( hSource );
        hSource = INVALID_HANDLE_VALUE;
    }

	if (INVALID_HANDLE_VALUE != hTempFile)
    {
        CloseHandle( hTempFile );
        hTempFile = INVALID_HANDLE_VALUE;
    }

    // Delete the temp file we made
	TruncateFile(szTempFileName);

	// Save the changes and close the QSPAK handle
	if( hQuarItem )
	{
		QsPakSaveItem(hQuarItem);
		QsPakReleaseItem(hQuarItem);
	}

	// free buffers
	if(lpszLocalBuffer)
	{
		delete[] lpszLocalBuffer;
		lpszLocalBuffer = NULL;
    }
			
	if(wcharStr)
	{
		delete[] wcharStr;
		wcharStr = NULL;
	}

	if( S_OK == retVal && !bInvalidFile )
    {
        // wipe the source file.
        SafeDeleteFile( lpszSourceFileName );

        // Scan the new Quarantine Item to update its virus information
        // We do not want an IQuarantineItem pointer back.
        retVal = ScanNewQuarantineMainFile(szQuarFileName, NULL);
    }
    else
    {
        if( strlen( szQuarFileName ) > 0 )
        {
            // wipe the destination file since it was not completed correctly
            SafeDeleteFile( szQuarFileName );
        }
    }

    if( pstQuarantineSource )
    {
        delete[] pstQuarantineSource;
        pstQuarantineSource = NULL;
    }

    return (retVal);
} // END CQuarantineDLL::ProcessIncomingFile()


// ==== Enum =================================================================
// Begin an enumeration of the quarantine folder
// Input: IEnumQuarantineItems** pEnumObj -pointer to enumeration object
// Output: HRESULT - S_OK on success, or OLE defined error code
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::Enum( IEnumQuarantineItems** pEnumObj )
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    if(!m_bInitialized)
        return E_UNINITIALIZED;

    // Create new enumeration object
    CEnumQuarantineItems* p = new CEnumQuarantineItems;
    if( p == NULL )
        return E_OUTOFMEMORY;

    // Assign the interface
    *pEnumObj = p;
    p->AddRef();

    // Initialize the enumeration
    HRESULT hr = p->Initialize( m_szQuarantineFolder );

    // If we failed, delete the object.
    if( FAILED( hr ) )
        {
        p->Release();
        *pEnumObj = p = NULL;
        }

    // Return result of initialization.
    return hr;
}


// ==== FetchQuarantineOptionsPath ============================================
// Looks in the registry to find the path of the Quarantine options file.
// Input: lpszPathBuf - pointer to buffer for the path
// Output: TRUE if the path is successfully fetched from the registry (and the
//			input buffer will contain the path), FALSE if we fail (the first
//			character of the input buffer will contain NULL)
////////////////////////////////////////////////////////////////////////////
// 4/09/98 SEDWARD - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL    CQuarantineDLL::FetchQuarantineOptionsPath(LPSTR  lpzsPathBuf, DWORD  dwBufSize)
{
    auto    BOOL        bRetVal = TRUE;
    auto    DWORD       regValueType;
    auto    HKEY        hKey = NULL;
    auto    long        lResult;

    // open the Quarantine path registry key
    lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGKEY_QUARANTINE_PATH, 0, KEY_READ, &hKey);
    if (ERROR_SUCCESS != lResult)
        {
        bRetVal = FALSE;
        goto  Bail_Function;
        }

    // get the desired path value
    lResult = RegQueryValueEx(hKey, REGVAL_QUARANTINE_OPTIONS_PATH_STR, 0
                                                            , &regValueType
                                                            , (LPBYTE)lpzsPathBuf
                                                            , &dwBufSize);
    if ((ERROR_SUCCESS != lResult)  ||  (REG_SZ != regValueType))
        {
        bRetVal = FALSE;
        goto  Bail_Function;
        }


Bail_Function:
	if( hKey )
		RegCloseKey( hKey );

	return (bRetVal);
}  // end of "CQuarantineDLL::FetchQuarantineOptionsPath"


// ==== InitializeQuarOptsObject =============================================
// Initializes a Quarantine options object and sets the pointer argument
// Input: a pointer to a pointer to a Quarantine options interface
// Output: TRUE if all goes well, FALSE if not
//
// NOTE: It the responsibility of the caller to call "Release" on the
//       pointer argument!!!
////////////////////////////////////////////////////////////////////////////
// 5/09/98 SEDWARD - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CQuarantineDLL::InitializeQuarOptsObject(IQuarantineOpts** pQuarOpts)
{
    if(pQuarOpts == NULL)
    {
        CCTRACEE(_T("CQuarantineDLL::InitializeQuarOptsObject() - IQuarantineOpts is invalid"));
        return FALSE;
    }

    auto    BOOL        bResult = TRUE;
    auto    char        szTempBuf[MAX_QUARANTINE_FILENAME_LEN];
    auto    HRESULT     hResult = S_OK;

    // Create the Quarantine options object
    *pQuarOpts = new CQuarantineOptions;

    if (*pQuarOpts == NULL)
        {
        CCTRACEE(_T("CQuarantineDLL::InitializeQuarOptsObject() - Failed to create the quarantine options object"));
        bResult = FALSE;
        goto  Exit_Function;
        }

    (*pQuarOpts)->AddRef();

    // find out where the Quarantine options file is installed
    if (FALSE == FetchQuarantineOptionsPath(szTempBuf, MAX_QUARANTINE_FILENAME_LEN))
        {
        bResult = FALSE;
        goto  Exit_Function;
        }

    // append the name of the Quarantine options file and initialize the options object
    STRCAT(szTempBuf, "\\");
    STRCAT(szTempBuf, QUARANTINE_OPTIONS_FILENAME);

	BOOL bCreate = FALSE;
	if(!::PathFileExists(szTempBuf))
		bCreate = TRUE;

    hResult = (*pQuarOpts)->Initialize(szTempBuf, bCreate);
    if (FAILED(hResult))
        {
        bResult = FALSE;
        goto  Exit_Function;
        }


Exit_Function:

    return (bResult);

}  // end of "CQuarantineDLL::InitializeQuarOptsObject"


// ==== ScanNewQuarantineMainFile ============================================
// Should be called on new Quarantine items. This function will scan the item
// and update its virus information.  It can also pass back an IQuarantineItem.
// Input:
//	 LPSTR lpszDestQuarItemPath - Name of the new file in Quarantine.
//	 IQuarantineItem **pNewItem - Used to return the IQuarantineItem* for the
//									new item.  If NULL, the item is released.
//									If not NULL, it is up to the calling
//									function to release the item.
// Return type      : HRESULT S_OK on success.
////////////////////////////////////////////////////////////////////////////
// 5/15/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
HRESULT CQuarantineDLL::ScanNewQuarantineMainFile(LPSTR lpszDestQuarItemPath,
                                                    IQuarantineItem **pNewItem )
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    HRESULT retVal;
    BOOL    bUndeletableVirFound = FALSE;

    // Now that the file is located in Quarantine, perform a scan on it
    // to update the virus information.
    IQuarantineItem* pItem;
    pItem = new CQuarantineItem;
    if( pItem == NULL )
    {
        CCTRACEE(_T("CQuarantineDLL::ScanNewQuarantineMainFile() - Failed to create quarantine item."));
        return E_OUTOFMEMORY;
    }

    // References the new item
    pItem->AddRef();

    // Scan the file to fill out the virus information
    retVal = pItem->Initialize(lpszDestQuarItemPath);
    if( FAILED( retVal ) )
    {
        CCTRACEE(_T("CQuarantineDLL::ScanNewQuarantineMainFile() - Failed to initialize the quarantine item"));
        pItem->Release();
        pItem = NULL;
        return retVal;
    }

    // If have our own quarantine scannner use it so we don't have to load another scanner
    // object for every item qurantined
    if( m_pQScanner != NULL )
    {
        retVal = DoScan(pItem);

        if( S_UNDELETABLE_VIRUS_FOUND == retVal )
        {
            bUndeletableVirFound = TRUE;
        }
        else if( FAILED( retVal ) &&
                ( E_AVAPI_VS_CANT_REPAIR != retVal ) ) // Make sure the error is not that it is
        {                                           // an unrepairable virus.
            CCTRACEE(_T("CQuarantineDLL::ScanNewQuarantineMainFile() - DoScan() failed to scan the item."));
            pItem->Release();
            return retVal;
        }
    }
    else
    {
        retVal = pItem->ScanMainFile();
        if( S_UNDELETABLE_VIRUS_FOUND == retVal )
        {
            bUndeletableVirFound = TRUE;
        }
        else if( FAILED( retVal ) &&
                ( E_AVAPI_VS_CANT_REPAIR != retVal ) ) // Make sure the error is not that it is
        {                                           // an unrepairable virus.
            CCTRACEE(_T("CQuarantineDLL::ScanNewQuarantineMainFile() - pItem->ScanMainFile() failed to scan the item."));
            pItem->Release();
            return retVal;
        }
    }

    // Make sure that the file Status is saved.  It should also be saved by ScanMainFile
    retVal = pItem->SaveItem();
    if( FAILED( retVal ) )
    {
        CCTRACEE(_T("CQuarantineDLL::ScanNewQuarantineMainFile() - Failed to save the item"));
        pItem->Release();
        return retVal;
    }

    // Check to see if the person calling the function wanted an IQuarantineItem back
    if( NULL == pNewItem )
        {
        // If they did not, then release the item
        pItem->Release();
        }
    else
        {
        // If they did, then provide it for them.
        *pNewItem = pItem;
        }

    // If an undeletable virus was found return that to the caller so they can
    // not delete the file.
    if( bUndeletableVirFound )
        {
        return S_UNDELETABLE_VIRUS_FOUND;
        }

    return S_OK;
}


// Added for NAV 2005.5/r11.5
// TODO: Check dummy file support.
// TODO: Check clearing of virus name/ID on clean scan.
HRESULT CQuarantineDLL::DoScan(IQuarantineItem* pItem)
{
    if( m_pQScanner == NULL )
    {
        CCTRACEE(_T("CQuarantineDLL::DoScan() - Invalid qurantine scanner."));
        return E_FAIL;
    }

    if( pItem == NULL )
    {
        CCTRACEE(_T("CQuarantineDLL::DoScan() - Invalid quarantine item."));
        return E_FAIL;
    }

    char szGeneratedFilename[MAX_QUARANTINE_FILENAME_LEN]  = "";
    char szFolder[MAX_QUARANTINE_FILENAME_LEN]             = "";
    DWORD dwFileType                    = 0;
    HRESULT  hr;
    HRESULT  hRetVal                    = E_UNEXPECTED;
    SCANSTATUS result;
    BOOL bFoundVirus                    = FALSE;
    BOOL bCanDeleteItem                 = TRUE;
	CSymPtr<IScanInfection> pIVirus		= NULL;

    // Save the current scan time and definitions data
    SYSTEMTIME tTemp;
	HRESULT hrGetDefsDate = m_pQScanner->GetCurrentDefinitionsDate(&tTemp);
	
	bool bDummy = false;
	if(SUCCEEDED(pItem->InitialDummyFile(&bDummy)) && bDummy)
	{
		result = SCAN_OK;

		pItem->ClearVirusName();
		pItem->ClearVirusID();
	}
	else
	{
		// Unpackage the file to the temporary directory
		BOOL bSuccess = GetQuarantinePath( szFolder, MAX_QUARANTINE_FILENAME_LEN, TARGET_QUARANTINE_TEMP_PATH );

		if( FALSE == bSuccess || szFolder[0] == '\0')
		{
			CCTRACEE(_T("CQuarantineDLL::DoScan() - Failed to get the qurantine path"));
			return E_UNABLE_TO_GET_QUARANTINE_PATH;
		}

		// If the path does not exist attempt to create it
		if(!::PathFileExists(szFolder))
			::CreateDirectory(szFolder, NULL);

		hr = pItem->UnpackageMainFile(szFolder,szGeneratedFilename, szGeneratedFilename, MAX_QUARANTINE_FILENAME_LEN,TRUE,TRUE);
		if( FAILED(hr))
		{
			CCTRACEE(_T("CQuarantineDLL::DoScan() - Unpackage item failed. Error = 0x%X."), hr);
			return hr;
		}

		result = m_pQScanner->Scan(szGeneratedFilename, &pIVirus);

		// If the Scan returned and error, then return its error
		if( result != SCAN_OK )
		{
			CCTRACEE(_T("CQuarantineDLL::DoScan() - Scan failed. Error = 0x%X."), result);
			hRetVal = result;
			goto Exit_Function;
		}
	}

    // If the file was successfully scanned, updated the date/times
    if( result == SCAN_OK )
    {
        if( SUCCEEDED(hrGetDefsDate) )
        {
            hr = pItem->SetDateOfLastScanDefs( &tTemp );
            if(FAILED(hr))
            {
                hRetVal = hr;
                CCTRACEE(_T("CQuarantineDLL::DoScan() - SetDateOfLastScanDefs failed. Error = 0x%X."), hRetVal);
                goto Exit_Function;
            }
        }

        GetSystemTime( &tTemp );
        hr = pItem->SetDateOfLastScan( &tTemp );
        if(FAILED(hr))
        {
            hRetVal = hr;
            CCTRACEE(_T("CQuarantineDLL::DoScan() - SetDateOfLastScan failed. Error = 0x%X."), hRetVal);
            goto Exit_Function;
        }
    }

    // Check to see if a virus was found
    if( result == SCAN_OK && pIVirus != NULL )
    {
        BOOL bRepairSuccessfull = FALSE;
        bFoundVirus = TRUE;

        // Update the virus information
		// Get the Virus Info object
		CSymPtr<IScanFileInfection> pFileInfection;
		if(SYM_SUCCEEDED(pIVirus->QueryInterface(IID_ScanFileInfection, (void**)&pFileInfection)))
			bCanDeleteItem = (pFileInfection->CanDelete() == true);
		else
			bCanDeleteItem = TRUE;
		
		pItem->SetVirusName(pIVirus->GetVirusName());
		pItem->SetVirusID(pIVirus->GetVirusID());

        // Add the category information to the item
        int nCatCount = pIVirus->GetCategoryCount();
        TCHAR szCategories[MAX_PATH] = {0};
        const unsigned long* pCategories = pIVirus->GetCategories();
        for( int nCatIndex=0; nCatIndex<nCatCount; nCatIndex++, pCategories++ )
        {
            if( NULL != pCategories )
            {
                TCHAR szCurCat[10] = {0};
                _stprintf(szCurCat, _T("%u "), *pCategories);
                _tcsncat(szCategories, szCurCat, MAX_PATH);
            }
            else
                break;
        }
        pItem->SetString(QSERVER_ITEM_INFO_CATEGORIES, szCategories, MAX_PATH);

        // Find out if the file is supposed to be repairable.
        hr = pItem->GetFileType(&dwFileType);
        if( FAILED(hr))
        {
            hRetVal = hr;
            CCTRACEE(_T("CQuarantineDLL::DoScan() - GetFileType failed. Error = 0x%X."), hRetVal);
            goto Exit_Function;
        }

        result = (m_pQScanner->RepairInfection(pIVirus));
        if( result != SCAN_OK )
        {
            // If the repair fails, and the file is supposed to be repairable
            // Mark it unrepairable.
            if( dwFileType & QFILE_TYPE_REPAIRABLE )
            {
                dwFileType = dwFileType - QFILE_TYPE_REPAIRABLE;

                // Since we have already encountered an error, do not report other errors
                // Update the type and save the new header.
                pItem->SetFileType(dwFileType);
            }

            if( FAILED(pItem->SaveItem()) )
                CCTRACEE(_T("CQuarantineDLL::DoScan() - SaveItem failed for unrepairable virus. Error = 0x%X."), hRetVal);

            hRetVal = E_AVAPI_VS_CANT_REPAIR;
            goto Exit_Function;
        }

        bRepairSuccessfull = TRUE;

        // Add the repairable bit
        dwFileType = dwFileType | QFILE_TYPE_REPAIRABLE;

        hr = pItem->SetFileType(dwFileType);
        if( FAILED(hr))
        {
            hRetVal = hr;
            CCTRACEE(_T("CQuarantineDLL::DoScan() - SetFileType failed. Error = 0x%X."), hRetVal);
            goto Exit_Function;
        }

        // Delete the repaired file
		TruncateFile(szGeneratedFilename);
    }
    else if(!bDummy)
    {
        // We cannot detect a virus, Use a wipe on it.
        BOOL bDeleteSuccessfull = TruncateFile(szGeneratedFilename);
        if( FALSE == bDeleteSuccessfull )
        {
            hRetVal = E_FILE_DELETE_FAILED;
            CCTRACEE(_T("CQuarantineDLL::DoScan() - TruncateFile failed to delete the file. Error = 0x%X."), hRetVal);
            goto Exit_Function;
        }

        // Clear out virus fields since we cannot detect a virus
		pItem->ClearVirusName();
		pItem->ClearVirusID();
    }

    // Update the header.
    hr = pItem->SaveItem();
    if( FAILED(hr))
    {
        hRetVal = hr;
        CCTRACEE(_T("CQuarantineDLL::DoScan() - SaveItem failed after updating header. Error = 0x%X."), hRetVal);
        goto Exit_Function;
    }

    // If there was a virus found then return S_OK
    // If there was no virus found return S_NO_VIRUS_FOUND
    HRESULT hRet = S_NO_VIRUS_FOUND;
    if( FALSE == bCanDeleteItem )
    {
        hRet = S_UNDELETABLE_VIRUS_FOUND;
    }
    else if( TRUE == bFoundVirus )
    {
        hRet = S_OK;
    }

    return hRet;

Exit_Function:

    // There was an error, wipe the virus infected file.
	TruncateFile(szGeneratedFilename);
    
	// If the error is that we cannot repair the item, and we cannot
    // delete the item, then return that we cannot delete the item.   
	if((E_AVAPI_VS_CANT_REPAIR == hRetVal) && (!bCanDeleteItem))
        return S_UNDELETABLE_VIRUS_FOUND;

    return hRetVal;
}


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineDLL::LoadQuarantineItemFromFilenameVID(LPCSTR lpszSourceFileName, DWORD dwVirusID, IQuarantineItem **pMatchingQuarItem)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	// Clear out the argument to make sure it is NULL on failure
    if(NULL == pMatchingQuarItem)
    {
        CCTRACEE(_T("CQuarantineDLL::LoadQuarantineItemFromFilenameVID() - Input item invalid."));
    }
    else if ((*pMatchingQuarItem) != NULL)
    {
        (*pMatchingQuarItem)->Release();
        (*pMatchingQuarItem) = NULL;
    }
 
	if(!m_bInitialized)
        return E_UNINITIALIZED;

	// Set up enumeration.
	IEnumQuarantineItems* pQuarEnum;
	if( FAILED(Enum(&pQuarEnum)) || pQuarEnum == NULL )
	{
		CCTRACEE(_T("CQuarantineDLL::LoadQuarantineItemFromFilenameVID() - Failed to get enumeration object"));
		return E_FAIL;
	}

	ULONG ulNumFetched;
	IQuarantineItem* pCurrentQuarItem = NULL;
	HRESULT hResult = E_FAIL;
	while(pQuarEnum->Next(1, &pCurrentQuarItem, &ulNumFetched) == NOERROR && pCurrentQuarItem != NULL )
	{
		DWORD dwCurrentVirusID = 0;
		if(SUCCEEDED(pCurrentQuarItem->GetVirusID(&dwCurrentVirusID)))
		{
			if(dwVirusID == dwCurrentVirusID)
            {
                // If VIDs are the same, compare filenames
                TCHAR* szCurrentFilename = NULL;
                DWORD dwBufSize = 0;

                // Find out how big the buffer needs to be
                hResult = pCurrentQuarItem->GetString(QSERVER_ITEM_INFO_FILENAME_ANSI, szCurrentFilename, &dwBufSize);
                if( hResult == E_BUFFER_TOO_SMALL )
                {
                    szCurrentFilename = new TCHAR[dwBufSize];

                    if( szCurrentFilename != NULL )
                    {
                        ZeroMemory(szCurrentFilename, sizeof(szCurrentFilename));

                        // Now use that buffer size to get the file name
                        hResult = pCurrentQuarItem->GetString(QSERVER_ITEM_INFO_FILENAME_ANSI, szCurrentFilename, &dwBufSize);
                        if(SUCCEEDED(hResult))
                        {
                            // Compare names
                            if(0 == _tcsicmp(lpszSourceFileName, szCurrentFilename))
                            {
                                // Assign the output item to our item, this will already have a ref count
                                // for the client so we'll leave it alone
                                *pMatchingQuarItem = pCurrentQuarItem;
                                delete[] szCurrentFilename;
                                break;
                            }
                        }
                        else
                        {
                            CCTRACEE(_T("CQuarantineDLL::LoadQuarantineItemFromFilenameVID() - FAILED(GetString(QSERVER_ITEM_INFO_FILENAME_ANSI)). Error = 0x%X."), hResult);
                        }

                        delete[] szCurrentFilename;
                    }
                    else
                    {
                        CCTRACEE(_T("CQuarantineDLL::LoadQuarantineItemFromFilenameVID() - Failed to allocate memory for file name."));
                    }
                }
                else
                {
                    CCTRACEE(_T("CQuarantineDLL::LoadQuarantineItemFromFilenameVID() - FAILED(GetString(QSERVER_ITEM_INFO_FILENAME_ANSI)). Error = 0x%X."), hResult);
                }
            }
		}

        // Release the quarantine item that doesn't match
        pCurrentQuarItem->Release();
        pCurrentQuarItem = NULL;
	}

	// Release enumeration object.
	pQuarEnum->Release();
    pQuarEnum = NULL;

	if(NULL != (*pMatchingQuarItem))
		return S_OK;
	else 
		return (FAILED(hResult) ? hResult : E_FAIL);
}

STDMETHODIMP CQuarantineDLL::LoadQuarantineItemFromUUID(UUID uniqueID, IQuarantineItem **pMatchingQuarItem)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    if(!m_bInitialized)
        return E_UNINITIALIZED;

    // Clear out the argument to make sure it is NULL on failure
    if(NULL == pMatchingQuarItem)
    {
        CCTRACEE(_T("CQuarantineDLL::LoadQuarantineItemFromUUID() - Input item invalid."));
    }
    else if( (*pMatchingQuarItem) != NULL )
    {
        (*pMatchingQuarItem)->Release();
        (*pMatchingQuarItem) = NULL;
    }

    // Set up enumeration.
    IEnumQuarantineItems* pQuarEnum = NULL;
    if( FAILED(Enum(&pQuarEnum)) || pQuarEnum == NULL )
    {
        CCTRACEE(_T("CQuarantineDLL::LoadQuarantineItemFromUUID() - Failed to get enumeration."));
        return E_FAIL;
    }

    ULONG ulNumFetched;
    IQuarantineItem* pCurrentQuarItem = NULL;
    while(pQuarEnum->Next(1, &pCurrentQuarItem, &ulNumFetched) == NOERROR  && pCurrentQuarItem != NULL )
    {
        UUID uniqueIDtoCheck = {0};
        if( SUCCEEDED( pCurrentQuarItem->GetUniqueID(uniqueIDtoCheck)) )
        {
            if(uniqueIDtoCheck == uniqueID)
            {
                // Assign the output item to our item, this will already have a ref count
                // for the client so we'll leave it alone
                *pMatchingQuarItem = pCurrentQuarItem;
                break;
            }
        }
        else
            CCTRACEE(_T("CQuarantineDLL::LoadQuarantineItemFromUUID() - Failed to get UUID for item %d."), ulNumFetched);

        // Free this object since we don't want it
        pCurrentQuarItem->Release();
        pCurrentQuarItem = NULL;
    }

    // Release enumeration object.
    pQuarEnum->Release();
    pQuarEnum = NULL;

    if( NULL != (*pMatchingQuarItem) )
        return S_OK;
    else 
        return E_FAIL;
}


// Added for NAV 2005.5/r11.5
HRESULT CQuarantineDLL::SetOriginalScanDefsDateToCurrent(HQSERVERITEM hQuarItem)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    if(!m_bInitialized)
	{
		CCTRACEE(_T("%s - !m_bInitialized"), __FUNCTION__);
		return E_UNINITIALIZED;
	}

	HRESULT hResult = E_FAIL;
	if(!m_pQScanner)
    {
        CCTRACEE(_T("CQuarantineDLL::SetOriginalScanDefsDateToCurrent() - Do not have a valid scanner object"));
        return E_FAIL;
    }

	SYSTEMTIME timeCurrent;
	hResult = m_pQScanner->GetCurrentDefinitionsDate(&timeCurrent);
	if(FAILED(hResult))
	{
		CCTRACEE(_T("CQuarantineDLL::SetOriginalScanDefsDateToCurrent() - FAILED(m_pQScanner->GetCurrentDefinitionsDate()), hResult=0x%08X"), hResult);
		return hResult;
	}

	QSPAKDATE stQSDate = SysTimeToQSDate(timeCurrent);
	QSPAKSTATUS qsResult = QsPakSetItemValue(hQuarItem, QSERVER_ITEM_INFO_ORIGINAL_SCAN_DEFS_QDATE, QSPAK_TYPE_DATE, &stQSDate, sizeof(stQSDate));  
	if(qsResult != QSPAKSTATUS_OK)
	{
		CCTRACEE(_T("CQuarantineDLL::SetOriginalScanDefsDateToCurrent() - Failed QsPakSetItemValue(QSERVER_ITEM_INFO_ORIGINAL_SCAN_DEFS_QDATE), qsResult=%d"), qsResult);
        return E_FAIL;
	}

    return S_OK;
} // END CQuarantineDLL::SetOriginalScanDefsDateToCurrent()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineDLL::TakeSystemSnapshot()
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    if(!m_bInitialized)
        return E_UNINITIALIZED;

	TCHAR szSnapShotPath[MAX_PATH * 2] = {0};
	DWORD dwBuffSize = MAX_PATH * 2;
	BOOL bSuccess = GetQuarantinePath(szSnapShotPath, dwBuffSize, TARGET_QUARANTINE_TEMP_PATH);
	if(!bSuccess || (szSnapShotPath[0] == '\0'))
	{
		CCTRACEE(_T("%s - Failed to get the qurantine path"), __FUNCTION__);
		return E_UNABLE_TO_GET_QUARANTINE_PATH;
	}

	// If the path does not exist attempt to create it
	if(!::PathFileExists(szSnapShotPath))
		::CreateDirectory(szSnapShotPath, NULL);

	_tcscpy(szSnapShotPath, szSnapShotPath);
	NameAppendFile(szSnapShotPath, SYSTEM_SNAPSHOT_FILE_NAME_FORMAT);

	CSystemSnapshot Snapper;
    HRESULT hResult = Snapper.TakeSystemSnapshot(szSnapShotPath);

    // Get back success or failure.
	if(FAILED(hResult))
	{
		CCTRACEE(_T("%s - Failed to take snapshot, hResult=0x%08X"), __FUNCTION__, hResult);
		return E_FAIL;
	}

	m_bSnapshot = true;

	return S_OK;
} // END TakeSystemSnapshot()


// Added for NAV 2005.5/r11.5
STDMETHODIMP CQuarantineDLL::AddSnapshotToQuarantineItem(IQuarantineItem *pQuarItem)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    if(!m_bInitialized)
        return E_UNINITIALIZED;

	if(NULL == pQuarItem)
    {
        CCTRACEE(_T("%s - Invalid quarantine item."), __FUNCTION__);
        return E_FAIL;
    }

	if(!m_bSnapshot)
	{
		CCTRACEE(_T("%s - No snapshot available."), __FUNCTION__);
        return E_FAIL;
	}

	// Construct snapshot path
	TCHAR szSnapshotPath[MAX_PATH * 2] = {0};
	DWORD dwBuffSize = MAX_PATH * 2;
	BOOL bSuccess = GetQuarantinePath(szSnapshotPath, dwBuffSize, TARGET_QUARANTINE_TEMP_PATH);
	if(!bSuccess || (_tcslen(szSnapshotPath) <= 0) || !::PathFileExists(szSnapshotPath))
	{
		CCTRACEE(_T("%s - Failed to get the qurantine path"), __FUNCTION__);
		return E_UNABLE_TO_GET_QUARANTINE_PATH;
	}

	NameAppendFile(szSnapshotPath, SYSTEM_SNAPSHOT_FILE_NAME_FORMAT);
	if(!(::PathFileExists(szSnapshotPath)))
	{
		CCTRACEE(_T("%s - No snapshot available."), __FUNCTION__);
        return E_FAIL;
	}
	
	// Load path of data files for this item
	TCHAR szQuarDataPath[MAX_PATH * 2] = {0};
	dwBuffSize = MAX_PATH * 2;
    HRESULT hResult = pQuarItem->CreateDataSubfolderIfNotExist(szQuarDataPath, &dwBuffSize);
	if(FAILED(hResult))
    {
        CCTRACEE(_T("%s - CreateDataSubfolderIfNotExist failed with HRESULT = 0x%X"), __FUNCTION__, hResult);
        return hResult;
    }

	NameAppendFile(szQuarDataPath, SYSTEM_SNAPSHOT_FILE_NAME_FORMAT);

	BOOL bCopied = ::CopyFile(szSnapshotPath, szQuarDataPath, FALSE);
	if(!bCopied)
	{
		DWORD dwError = GetLastError();
		CCTRACEE(_T("%s - Failed CopyFile(%s, %s), GetLastError()=%d"), __FUNCTION__, szSnapshotPath, szQuarDataPath, dwError);
		return E_FAIL;
	}

	// Write snapshot file name into QuarantineItem
	TCHAR szSnapShotName[MAX_PATH * 2] = {0};
	_tcscpy(szSnapShotName, SYSTEM_SNAPSHOT_FILE_NAME_FORMAT);
	if( FAILED(hResult = pQuarItem->SetString(QSERVER_ITEM_SYSTEM_SNAPSHOT_FILE, szSnapShotName, strlen(szSnapShotName))) )
    {
        CCTRACEE(_T("%s - Failed to set the snapshot file name. Hresult = 0x%X."), __FUNCTION__, hResult);
        return hResult;
    }
    
	if( FAILED(hResult = pQuarItem->SaveItem()) )
    {
        CCTRACEE(_T("%s - Failed to save the quarantine item. Hresult = 0x%X."), __FUNCTION__, hResult);
        return hResult;
    }

	return S_OK;
} // END AddSnapshotToQuarantineItem
