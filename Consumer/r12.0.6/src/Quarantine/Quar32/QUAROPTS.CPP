//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// QuarOpts.cpp: implementation of the CQuarantineOptions class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/QUAROPTS.CPv  $
// 
//    Rev 1.13   13 Jul 1998 10:48:32   jtaylor
// Changed functions using tscncpy to make sure they NULL terminate the strings.
//
//    Rev 1.12   11 May 1998 13:13:04   jtaylor
// -- added E_ERROR_READING_OPTIONS to ReadQuarOptsFile()
// -- added E_ERROR_WRITING_OPTIONS to WriteQuarOptsFile()
//
//    Rev 1.11   08 May 1998 13:15:46   SEDWARD
// Reversed previous change of 'm_bInitialized' variable, it was causing
// 'Initialize()' to always fail!
//
//    Rev 1.10   07 May 1998 20:53:32   SEDWARD
// Do not set 'm_bInitialized' to TRUE until ReadQuarOptsFIle has
// successfully returned in 'Initialize()'.
//
//    Rev 1.9   21 Apr 1998 14:26:24   SEDWARD
// Put the 'scan at startup' stuff back in; note that this really refers to
// the option to be prompted for a scan at startup if newer defs are found.
//
//    Rev 1.8   20 Apr 1998 22:27:52   SEDWARD
// Removed the 'scan at startup' stuff.
//
//    Rev 1.7   10 Apr 1998 18:16:06   JTaylor
// Fixed an initializtion order problem.
//
//    Rev 1.6   10 Apr 1998 12:36:22   SEDWARD
// Updated HRESULT values.
//
//    Rev 1.5   07 Apr 1998 22:37:08   SEDWARD
// Added CreateDefaultQuarOptsDatFile(); Initialize now calls this if necessary.
//
//    Rev 1.4   06 Apr 1998 20:55:12   JTaylor
// Fixed an error in initialize.
//
//    Rev 1.3   06 Apr 1998 19:25:22   JTaylor
// Added error codes to options.  Made initialize load the options, fixed an options check to see if options file exists to not return true if it is a directory.  Added Quarantine path validation.  Fixed header style for Quaropts.
//
//    Rev 1.2   01 Apr 1998 12:03:48   SEDWARD
// Added 'password enabled' functions.
//
//    Rev 1.1   30 Mar 1998 19:50:38   SEDWARD
// Added password stuff.
//
//    Rev 1.0   16 Mar 1998 18:50:16   SEDWARD
// Initial revision.
/////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <winbase.h>
#include "stdafx.h"
#include "QuarOpts.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction

//  Function created: 3/98, SEDWARD
CQuarantineOptions::CQuarantineOptions(void)
{
    m_bInitialized = false;
}

//  Function created: 3/98, SEDWARD
CQuarantineOptions::~CQuarantineOptions(void)
{
}

//////////////////////////////////////////////////////////////////////
// Used to initialize the Quarantine options object.  The caller MUST call
// this funtion before any others.
// Input:
//      lpszFileName - a fully-qualified path to the options file
//      bCreate      - TRUE means that if the options file does not
//                     exist, a default copy will be generated
//  Output: S_OK if all goes well, E_UNABLE_TO_READ_OPTIONS if not
//  Function created: 3/98, SEDWARD
//////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineOptions::Initialize(LPSTR lpszFileName, BOOL  bCreate)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	HRESULT retVal = S_OK;

	if(m_bInitialized)
		return ERROR_ALREADY_INITIALIZED;

	memset( (void*) &m_QuarOpts, 0, sizeof(m_QuarOpts) );

	// make sure the file exists
	if ((NULL != lpszFileName)  &&  (NULL != *lpszFileName))
	{
		if (QuarOptsFileExists(lpszFileName))
		{
			lstrcpy(m_QuarOptsPath, lpszFileName);
		}
		else if (TRUE == bCreate)   // user wants a default DAT file
		{
			if (FALSE == CreateDefaultQuarOptsDatFile(lpszFileName))
			{
				retVal = E_UNABLE_TO_READ_OPTIONS;
				goto  Exit_Function;
			}
			else    // save the full path of the new DAT file
			{
				lstrcpy(m_QuarOptsPath, lpszFileName);
			}
		}
		else    // file does not exist and the user does not want a default copy
		{
			retVal = E_UNABLE_TO_READ_OPTIONS;
			goto  Exit_Function;
		}
	}
	else    // bogus filename argument
	{
		retVal = E_UNABLE_TO_READ_OPTIONS;
		goto  Exit_Function;
	}

	// We are now almost initialized.  We are initialized once the read succeeds.
	m_bInitialized = true;

	// Load in the options file
	HRESULT tempRetVal = E_UNABLE_TO_READ_OPTIONS;
	tempRetVal = ReadQuarOptsFile();
	if( FAILED(tempRetVal) )
	{
		// If we cannot read, then we were not completely initialized
		m_bInitialized = false;
		retVal = tempRetVal;
		goto Exit_Function;
	}

Exit_Function:

	return (retVal);
}  // END Initialize()


//////////////////////////////////////////////////////////////////////
// Reads the Quarantine options file and uses its contents to initialize the
// QUAR_OPTS_STRUCT structure.
//  Output: S_OK if all goes well, E_UNEXPECTED if not
//  Function created: 3/98, SEDWARD
//////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineOptions::ReadQuarOptsFile(void)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	BOOL    bResult = TRUE;
	HRESULT retVal = S_OK;

	if(!m_bInitialized)
		return E_UNINITIALIZED;

	// make sure the file exists
	if (FALSE == QuarOptsFileExists(m_QuarOptsPath))
	{
		retVal = E_UNABLE_TO_FIND_OPTIONS;
		goto  Exit_Function;
	}

	// read in the file data and set the DWORD error bits
	bResult = ReadScanAtStartup();
	if (FALSE == bResult)
		retVal = E_ERROR_READING_OPTIONS;

	bResult = ReadValidFileOperations(QFILE_STATUS_QUARANTINED);
	if (FALSE == bResult)
		retVal = E_ERROR_READING_OPTIONS;

	bResult = ReadValidFileOperations(QFILE_STATUS_SUBMITTED);
	if (FALSE == bResult)
		retVal = E_ERROR_READING_OPTIONS;

	bResult = ReadValidFileOperations(QFILE_STATUS_BACKUP_FILE);
	if (FALSE == bResult)
		retVal = E_ERROR_READING_OPTIONS;

	bResult = ReadValidFileOperations(QFILE_STATUS_THREAT_BACKUP);
	if (FALSE == bResult)
		retVal = E_ERROR_READING_OPTIONS;

Exit_Function:
	return retVal;

}  // END ReadQuarOptsFile()


//////////////////////////////////////////////////////////////////////
//  This function writes the Quarantine options file with the values currently
//  stored in the QUAR_OPTS_STRUCT structure.
//  Input:
//      dwErrorBits     -- a pointer to the DWORD that will be used to set
//                         error specific flags
//  Output: S_OK if all goes well, E_UNEXPECTED if not
//  Function created: 3/98, SEDWARD
//////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineOptions::WriteQuarOptsFile()
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);
	
	if(!m_bInitialized)
		return E_UNINITIALIZED;

	// make sure the file exists
	HRESULT retVal = S_OK;
	if (FALSE == QuarOptsFileExists(m_QuarOptsPath))
	{
		retVal = E_UNABLE_TO_FIND_OPTIONS;
		goto  Exit_Function;
	}

	// write the file data and set the DWORD error bits
	BOOL bResult = TRUE;
	bResult = WriteScanAtStartup();
	if (FALSE == bResult)
		retVal = E_ERROR_WRITING_OPTIONS;

	bResult = WriteValidFileOperations(QFILE_STATUS_QUARANTINED);
	if (FALSE == bResult)
		retVal = E_ERROR_WRITING_OPTIONS;

	bResult = WriteValidFileOperations(QFILE_STATUS_SUBMITTED);
	if (FALSE == bResult)
		retVal = E_ERROR_WRITING_OPTIONS;

	bResult = WriteValidFileOperations(QFILE_STATUS_BACKUP_FILE);
	if (FALSE == bResult)
		retVal = E_ERROR_WRITING_OPTIONS;

	bResult = WriteValidFileOperations(QFILE_STATUS_THREAT_BACKUP);
	if (FALSE == bResult)
		retVal = E_ERROR_WRITING_OPTIONS;

Exit_Function:
	return retVal;
}  // END WriteQuarOptsFile()


//////////////////////////////////////////////////////////////////////
//  This function fetches the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//  Input: bValue - a pointer to a BOOL that will receive the value
//  Output: S_OK
//  Function created: 3/98, SEDWARD
//////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineOptions::GetScanAtStartup(BOOL*  bValue)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	if(!m_bInitialized)
		return E_UNINITIALIZED;

	*bValue = m_QuarOpts.bScanAtStartup;
	return S_OK;
}  // END GetScanAtStartup()


//////////////////////////////////////////////////////////////////////
// Reads the current value of the "scan at startup" option from the Quarantine
// options file.
//  Output: TRUE if all goes well, FALSE if not
//  Function created: 3/98, SEDWARD
//////////////////////////////////////////////////////////////////////
BOOL CQuarantineOptions::ReadScanAtStartup()
{
    BOOL    bResult = TRUE;
    DWORD   dwResult = 0;
    HRESULT retVal = S_OK;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(QOPTS_SECTION_SCAN_AT_STARTUP    // section name
                                        , QOPTS_KEY_ENABLED             // key name
                                        , INI_DEFAULT_STR               // default string
                                        , m_szIniDataBuf                // destination buffer
                                        , MAX_INI_BUF_SIZE              // size of destination buffer
                                        , m_QuarOptsPath);              // location of INI file

	if (_tcslen(m_szIniDataBuf) != dwResult)
	{
		bResult = FALSE;
	}
	else
	{
		if (0 == lstrcmp("TRUE", m_szIniDataBuf))
			m_QuarOpts.bScanAtStartup = TRUE;
		else
			m_QuarOpts.bScanAtStartup = FALSE;
	}

    return bResult;
}  // END ReadScanAtStartup()


//////////////////////////////////////////////////////////////////////
// Sets the current value of the target QUAR_OPTS_STRUCT structure member.
//  Output: TRUE if all goes well, FALSE if not
//  Function created: 3/98, SEDWARD
//////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineOptions::SetScanAtStartup(BOOL  bValue)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    if(!m_bInitialized)
        return E_UNINITIALIZED;

    m_QuarOpts.bScanAtStartup = bValue;
    return S_OK;
}  // END SetScanAtStartup()


//////////////////////////////////////////////////////////////////////
//  Writes the target QUAR_OPTS_STRUCT to the Quarantine options file.
//  Output: a value of TRUE if all goes well, FALSE if not
//  Function created: 3/98, SEDWARD
//////////////////////////////////////////////////////////////////////
BOOL    CQuarantineOptions::WriteScanAtStartup()
{
    BOOL  bResult = TRUE;
    char* szValue = NULL;

    // determine the value to write
    if (TRUE == m_QuarOpts.bScanAtStartup)
        szValue = "TRUE";
    else
        szValue = "FALSE";

    // write the string out
    bResult = WritePrivateProfileString(QOPTS_SECTION_SCAN_AT_STARTUP   // section name
                                        , QOPTS_KEY_ENABLED             // key name
                                        , szValue                       // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    return bResult;
}  // END WriteScanAtStartup()


//////////////////////////////////////////////////////////////////////
// Fetches the current value of the target QUAR_OPTS_STRUCT structure member.
// Input:
//   dwFileOps [out] - a pointer to the DWORD that will receive the valid
//						operations for the file type specified by the "index" 
//						argument
//   nIndex [in]     - desired file type. Possible values:
//                       QFILEOPS_INDEX_QUARANTINED
//                       QFILEOPS_INDEX_SUBMITTED
//                       QFILEOPS_INDEX_BACKUP_FILE
//  Output: S_OK if all goes well, E_UNEXPECTED if not
//  Function created: 3/98, SEDWARD
//////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineOptions::GetValidFileOperations(DWORD* dwFileOps,
                                                        int nIndex)
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    HRESULT retVal = S_OK;

    if(!m_bInitialized)
        return E_UNINITIALIZED;

    if (nIndex > MAX_VALID_QFILEOPS_MEMBERS)
        retVal = E_UNEXPECTED;
    else
        *dwFileOps = m_QuarOpts.dwAllowableFileOperations[nIndex];

    return retVal;
}  // END GetValidFileOperations()


//////////////////////////////////////////////////////////////////////
// Reads the allowable operations on a target file type from the Quarantine 
// options file.
// Input: dwTargetFileStatus - the target file type
//  Output: TRUE if all goes well, FALSE if not
//  Function created: 3/98, SEDWARD
//////////////////////////////////////////////////////////////////////
BOOL CQuarantineOptions::ReadValidFileOperations(DWORD dwTargetFileStatus)
{
    BOOL  bResult = TRUE;
    char* szSectionName;
    DWORD dwResult = 0;
    int   nIndex = 0;

    // determine the target status value
	switch (dwTargetFileStatus)
	{
	case   QFILE_STATUS_QUARANTINED:
		szSectionName = QOPTS_SECTION_VALID_OPERATIONS_QUARANTINED;
		nIndex = QFILEOPS_INDEX_QUARANTINED;
		break;

	case   QFILE_STATUS_SUBMITTED:
		szSectionName = QOPTS_SECTION_VALID_OPERATIONS_SUBMITTED;
		nIndex = QFILEOPS_INDEX_SUBMITTED;
		break;

	case   QFILE_STATUS_BACKUP_FILE:
		szSectionName = QOPTS_SECTION_VALID_OPERATIONS_REPAIRED;
		nIndex = QFILEOPS_INDEX_BACKUP_FILE;
		break;

	case   QFILE_STATUS_THREAT_BACKUP:
		szSectionName = QOPTS_SECTION_VALID_OPERATIONS_SECURITY_RISKS;
		nIndex = QFILEOPS_INDEX_THREAT_BACKUP_FILE;
		break;

	default:
		nIndex = -1;
		break;
	}

    // initialize the output DWORD
    m_QuarOpts.dwAllowableFileOperations[nIndex] = 0;

    // now get the allowable operations for the target file status...

    // delete
    dwResult = GetPrivateProfileString(szSectionName        // section name
                                        , QOPTS_KEY_DELETE  // key name
                                        , INI_DEFAULT_STR   // default string
                                        , m_szIniDataBuf    // destination buffer
                                        , MAX_INI_BUF_SIZE  // size of destination buffer
                                        , m_QuarOptsPath);  // location of INI file
	if (_tcslen(m_szIniDataBuf) != dwResult)
	{
		bResult = FALSE;
		goto Exit_Function;
	}
	else
	{
		if (0 == lstrcmp("TRUE", m_szIniDataBuf))
			m_QuarOpts.dwAllowableFileOperations[nIndex] |= QUAR_OPTS_FILEOP_DELETE;
		else
			m_QuarOpts.dwAllowableFileOperations[nIndex] &= ~QUAR_OPTS_FILEOP_DELETE;
	}

    // scan
    dwResult = GetPrivateProfileString(szSectionName        // section name
                                        , QOPTS_KEY_SCAN    // key name
                                        , INI_DEFAULT_STR   // default string
                                        , m_szIniDataBuf    // destination buffer
                                        , MAX_INI_BUF_SIZE  // size of destination buffer
                                        , m_QuarOptsPath);  // location of INI file
	if (_tcslen(m_szIniDataBuf) != dwResult)
	{
		bResult = FALSE;
		goto  Exit_Function;
	}
	else
	{
		if (0 == lstrcmp("TRUE", m_szIniDataBuf))
			m_QuarOpts.dwAllowableFileOperations[nIndex] |= QUAR_OPTS_FILEOP_SCAN;
		else
			m_QuarOpts.dwAllowableFileOperations[nIndex] &= ~QUAR_OPTS_FILEOP_SCAN;
	}

    // repair
    dwResult = GetPrivateProfileString(szSectionName        // section name
                                        , QOPTS_KEY_REPAIR  // key name
                                        , INI_DEFAULT_STR   // default string
                                        , m_szIniDataBuf    // destination buffer
                                        , MAX_INI_BUF_SIZE  // size of destination buffer
                                        , m_QuarOptsPath);  // location of INI file
	if (_tcslen(m_szIniDataBuf) != dwResult)
	{
		bResult = FALSE;
		goto  Exit_Function;
	}
	else
	{
		if (0 == lstrcmp("TRUE", m_szIniDataBuf))
			m_QuarOpts.dwAllowableFileOperations[nIndex] |= QUAR_OPTS_FILEOP_REPAIR;
		else
			m_QuarOpts.dwAllowableFileOperations[nIndex] &= ~QUAR_OPTS_FILEOP_REPAIR;
	}

    // put back
    dwResult = GetPrivateProfileString(szSectionName        // section name
                                        , QOPTS_KEY_PUTBACK // key name
                                        , INI_DEFAULT_STR   // default string
                                        , m_szIniDataBuf    // destination buffer
                                        , MAX_INI_BUF_SIZE  // size of destination buffer
                                        , m_QuarOptsPath);  // location of INI file
	if (_tcslen(m_szIniDataBuf) != dwResult)
	{
		bResult = FALSE;
		goto  Exit_Function;
	}
	else
	{
		if (0 == lstrcmp("TRUE", m_szIniDataBuf))
			m_QuarOpts.dwAllowableFileOperations[nIndex] |= QUAR_OPTS_FILEOP_PUT_BACK;
		else
			m_QuarOpts.dwAllowableFileOperations[nIndex] &= ~QUAR_OPTS_FILEOP_PUT_BACK;
	}

    // submit to sarc
    dwResult = GetPrivateProfileString(szSectionName                // section name
                                        , QOPTS_KEY_SUBMIT_TO_SARC  // key name
                                        , INI_DEFAULT_STR           // default string
                                        , m_szIniDataBuf            // destination buffer
                                        , MAX_INI_BUF_SIZE          // size of destination buffer
                                        , m_QuarOptsPath);          // location of INI file
	if (_tcslen(m_szIniDataBuf) != dwResult)
	{
		bResult = FALSE;
		goto  Exit_Function;
	}
	else
	{
		if (0 == lstrcmp("TRUE", m_szIniDataBuf))
			m_QuarOpts.dwAllowableFileOperations[nIndex] |= QUAR_OPTS_FILEOP_SUBMIT_TO_SARC;
		else
			m_QuarOpts.dwAllowableFileOperations[nIndex] &= ~QUAR_OPTS_FILEOP_SUBMIT_TO_SARC;
	}


Exit_Function:
    return (bResult);

}  // END ReadValidFileOperations()


//////////////////////////////////////////////////////////////////////
// Sets the current value of the target QUAR_OPTS_STRUCT structure member.
// Input:
//   dwFileOps - DWORD bitmask representing the valid operations on the target
//					file type
//   nIndex    - index value specifying which file type to update;
//               possible values:
//               QFILEOPS_INDEX_QUARANTINED
//               QFILEOPS_INDEX_SUBMITTED
//               QFILEOPS_INDEX_BACKUP_FILE
//  Output: S_OK if all goes well, E_UNEXPECTED if not
//  Function created: 3/98, SEDWARD
//////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineOptions::SetValidFileOperations(DWORD dwFileOps
                                                            , int  nIndex)
{
	ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

	HRESULT retVal = S_OK;

	if(!m_bInitialized)
		return E_UNINITIALIZED;

	if (nIndex > MAX_VALID_QFILEOPS_MEMBERS)
		retVal = E_UNEXPECTED;
	else
		m_QuarOpts.dwAllowableFileOperations[nIndex] = dwFileOps;

	return retVal;
}  // END SetValidFileOperations()


//////////////////////////////////////////////////////////////////////
// Writes the target QUAR_OPTS_STRUCT to the Quarantine options file.
// Input: dwTargetFileStatus - the target file status
// Output: TRUE if all goes well, FALSE if not
//  Function created: 3/98, SEDWARD
//////////////////////////////////////////////////////////////////////
BOOL    CQuarantineOptions::WriteValidFileOperations(DWORD  dwTargetFileStatus)
{
    BOOL  bResult = TRUE;
    char* szSectionName = NULL;
    char* szValue = NULL;
    int   nIndex = 0;

    // determine the target status value and index
	switch (dwTargetFileStatus)
	{
	case   QFILE_STATUS_QUARANTINED:
		szSectionName = QOPTS_SECTION_VALID_OPERATIONS_QUARANTINED;
		nIndex = QFILEOPS_INDEX_QUARANTINED;
		break;

	case   QFILE_STATUS_SUBMITTED:
		szSectionName = QOPTS_SECTION_VALID_OPERATIONS_SUBMITTED;
		nIndex = QFILEOPS_INDEX_SUBMITTED;
		break;

	case   QFILE_STATUS_BACKUP_FILE:
		szSectionName = QOPTS_SECTION_VALID_OPERATIONS_REPAIRED;
		nIndex = QFILEOPS_INDEX_BACKUP_FILE;
		break;

	case   QFILE_STATUS_THREAT_BACKUP:
		szSectionName = QOPTS_SECTION_VALID_OPERATIONS_SECURITY_RISKS;
		nIndex = QFILEOPS_INDEX_THREAT_BACKUP_FILE;
		break;

	default:
		nIndex = -1;
		break;
	}


    // now write the allowable operations for the target file status...

    // delete
    if (m_QuarOpts.dwAllowableFileOperations[nIndex] & QUAR_OPTS_FILEOP_DELETE)
        szValue = "TRUE";
    else
        szValue = "FALSE";

    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_DELETE              // key name
                                        , szValue                       // input buffer
                                        , m_QuarOptsPath);              // location of INI file


    // scan
    if (m_QuarOpts.dwAllowableFileOperations[nIndex] & QUAR_OPTS_FILEOP_SCAN)
        szValue = "TRUE";
    else
        szValue = "FALSE";

	bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_SCAN                // key name
                                        , szValue                       // input buffer
                                        , m_QuarOptsPath);              // location of INI file


    // repair
    if (m_QuarOpts.dwAllowableFileOperations[nIndex] & QUAR_OPTS_FILEOP_REPAIR)
        szValue = "TRUE";
    else
        szValue = "FALSE";

    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_REPAIR              // key name
                                        , szValue                       // input buffer
                                        , m_QuarOptsPath);              // location of INI file


    // put back
    if (m_QuarOpts.dwAllowableFileOperations[nIndex] & QUAR_OPTS_FILEOP_PUT_BACK)
        szValue = "TRUE";
    else
        szValue = "FALSE";

    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_PUTBACK             // key name
                                        , szValue                       // input buffer
                                        , m_QuarOptsPath);              // location of INI file


    // submit to sarc
    if (m_QuarOpts.dwAllowableFileOperations[nIndex] & QUAR_OPTS_FILEOP_SUBMIT_TO_SARC)
        szValue = "TRUE";
    else
        szValue = "FALSE";

    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_SUBMIT_TO_SARC      // key name
                                        , szValue                       // input buffer
                                        , m_QuarOptsPath);              // location of INI file

    return bResult;
}  // END WriteValidFileOperations()


//////////////////////////////////////////////////////////////////////
// Checks if the specified Quarantine options file exists or not.
// Input: lpszQuarOptsPath - a fully-qualified path to the options file
//  Function created: 3/98, SEDWARD
//////////////////////////////////////////////////////////////////////
BOOL CQuarantineOptions::QuarOptsFileExists(LPTSTR lpszQuarOptsPath)
{
    BOOL bResult = TRUE;

    // see if the data file really exists
    DWORD dwTempAttributes = GetFileAttributes(lpszQuarOptsPath);
    if((INVALID_FILE_ATTRIBUTES == dwTempAttributes) 
		|| ((FILE_ATTRIBUTE_DIRECTORY & dwTempAttributes) == FILE_ATTRIBUTE_DIRECTORY))
	{
		bResult = FALSE;
	}

    return bResult;
}  // END QuarOptsFileExists()


//////////////////////////////////////////////////////////////////////
// Generate a default DAT file.
// Input: lpszFileName - a fully-qualified path to the QuarOpts.dat file
//  Output: TRUE if all goes well, FALSE if not
//  Function created: 4/98, SEDWARD
//////////////////////////////////////////////////////////////////////
BOOL CQuarantineOptions::CreateDefaultQuarOptsDatFile(LPTSTR lpszFileName)
{
    BOOL   bResult = TRUE;
    BOOL   bRetVal = TRUE;
    char*  szSectionName = NULL;
    HANDLE fileHandle = INVALID_HANDLE_VALUE;

    // create the file
    fileHandle = CreateFile(lpszFileName                        // name of the file
                                , GENERIC_WRITE                 // access mode
                                , 0                             // no sharing
                                , NULL                          // no secuity
                                , CREATE_ALWAYS                 // always create the file
                                , FILE_FLAG_SEQUENTIAL_SCAN     // flags and attributes
                                , NULL);                        // no template attributes
	DWORD dwLastError = GetLastError();

	if(INVALID_HANDLE_VALUE == fileHandle)
	{
		CCTRACEE(_T("%s - Failed to create new default options file. GetLastError()=%d"), __FUNCTION__, dwLastError);

		bRetVal = FALSE;
		goto  Exit_Function;
	}
	else
	{
		CloseHandle(fileHandle);
		lstrcpy(m_QuarOptsPath, lpszFileName);
	}


	// write out the default values...

    // scan automatically at startup
    bResult = WritePrivateProfileString(QOPTS_SECTION_SCAN_AT_STARTUP   // section name
                                        , QOPTS_KEY_ENABLED             // key name
                                        , DEFAULT_SCAN_AT_STARTUP       // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        bRetVal = FALSE;


	// valid operation on submitted files...

    // Quarantined files...
    szSectionName = QOPTS_SECTION_VALID_OPERATIONS_QUARANTINED;

    // delete
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_DELETE              // key name
                                        , DEFAULT_QUARANTINED_DELETE    // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        bRetVal = FALSE;

    // scan
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_SCAN                // key name
                                        , DEFAULT_QUARANTINED_SCAN      // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        bRetVal = FALSE;

    // repair
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_REPAIR              // key name
                                        , DEFAULT_QUARANTINED_REPAIR    // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        bRetVal = FALSE;

    // put back
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_PUTBACK             // key name
                                        , DEFAULT_QUARANTINED_PUT_BACK  // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        bRetVal = FALSE;

    // submit to sarc
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_SUBMIT_TO_SARC      // key name
                                        , DEFAULT_QUARANTINED_SUBMIT    // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        bRetVal = FALSE;


    // Submitted files...
    szSectionName = QOPTS_SECTION_VALID_OPERATIONS_SUBMITTED;

    // delete
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_DELETE              // key name
                                        , DEFAULT_SUBMITTED_DELETE      // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        bRetVal = FALSE;

    // scan
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_SCAN                // key name
                                        , DEFAULT_SUBMITTED_SCAN        // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        bRetVal = FALSE;

    // repair
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_REPAIR              // key name
                                        , DEFAULT_SUBMITTED_REPAIR      // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        bRetVal = FALSE;

    // put back
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_PUTBACK             // key name
                                        , DEFAULT_SUBMITTED_PUT_BACK    // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        bRetVal = FALSE;

    // submit to sarc
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_SUBMIT_TO_SARC      // key name
                                        , DEFAULT_SUBMITTED_SUBMIT      // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        bRetVal = FALSE;


    // Repaired files...
    szSectionName = QOPTS_SECTION_VALID_OPERATIONS_REPAIRED;

    // delete
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_DELETE              // key name
                                        , DEFAULT_REPAIRED_DELETE       // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        bRetVal = FALSE;

    // scan
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_SCAN                // key name
                                        , DEFAULT_REPAIRED_SCAN         // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        bRetVal = FALSE;

    // repair
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_REPAIR              // key name
                                        , DEFAULT_REPAIRED_REPAIR       // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        bRetVal = FALSE;

    // put back
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_PUTBACK             // key name
                                        , DEFAULT_REPAIRED_PUT_BACK     // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        bRetVal = FALSE;

    // submit to sarc
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_SUBMIT_TO_SARC      // key name
                                        , DEFAULT_REPAIRED_SUBMIT       // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        bRetVal = FALSE;

	// Threat backup files...
	szSectionName = QOPTS_SECTION_VALID_OPERATIONS_SECURITY_RISKS;

	// delete
	bResult = WritePrivateProfileString(szSectionName                   // section name
		, QOPTS_KEY_DELETE              // key name
		, DEFAULT_THREAT_BACKUP_DELETE    // input buffer
		, m_QuarOptsPath);              // location of INI file
	if (FALSE == bResult)
		bRetVal = FALSE;

	// scan
	bResult = WritePrivateProfileString(szSectionName                   // section name
		, QOPTS_KEY_SCAN                // key name
		, DEFAULT_THREAT_BACKUP_SCAN      // input buffer
		, m_QuarOptsPath);              // location of INI file
	if (FALSE == bResult)
		bRetVal = FALSE;

	// repair
	bResult = WritePrivateProfileString(szSectionName                   // section name
		, QOPTS_KEY_REPAIR              // key name
		, DEFAULT_THREAT_BACKUP_REPAIR    // input buffer
		, m_QuarOptsPath);              // location of INI file
	if (FALSE == bResult)
		bRetVal = FALSE;

	// put back
	bResult = WritePrivateProfileString(szSectionName                   // section name
		, QOPTS_KEY_PUTBACK             // key name
		, DEFAULT_THREAT_BACKUP_PUT_BACK  // input buffer
		, m_QuarOptsPath);              // location of INI file

	if (FALSE == bResult)
		bRetVal = FALSE;

	// submit to sarc
	bResult = WritePrivateProfileString(szSectionName                   // section name
		, QOPTS_KEY_SUBMIT_TO_SARC      // key name
		, DEFAULT_THREAT_BACKUP_SUBMIT    // input buffer
		, m_QuarOptsPath);              // location of INI file
	if (FALSE == bResult)
		bRetVal = FALSE;

Exit_Function:
	return (bRetVal);
}  // END CreateDefaultQuarOptsDatFile()
