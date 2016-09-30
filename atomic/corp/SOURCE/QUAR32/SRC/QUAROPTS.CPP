
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/QUAROPTS.CPv   1.13   13 Jul 1998 10:48:32   jtaylor  $
/////////////////////////////////////////////////////////////////////////////
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
//
/////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <winbase.h>
#include "stdafx.h"
#include "QuarOpts.h"

extern LONG g_dwObjs;
extern LONG g_dwLocks;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


// ==== CQuarantineOptions::CQuarantineOptions ============================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

CQuarantineOptions::CQuarantineOptions(void)
{
    InterlockedIncrement( &g_dwObjs );
    m_bInitialized = FALSE;
    m_dwRef = 0;
}


// ==== CQuarantineOptions::~CQuarantineOptions ===========================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

CQuarantineOptions::~CQuarantineOptions(void)
{
    InterlockedDecrement( &g_dwObjs );
}




////////////////////////////////////////////////////////////////////////////
// IUnknown Implementation


// ==== CQuarantineOptions::QueryInterface ================================
//
// Function name    : CQuarantineOptions::QueryInterface
// Description      : This function will return a requested COM interface
// Return type      : STDMETHODIMP
// Argument         : REFIID riid - REFIID of interface requested
// Argument         : void** ppv - pointer to requested interface
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::QueryInterface( REFIID riid, void** ppv )
{
    *ppv = NULL;

    if( IsEqualIID( riid, IID_IUnknown )||
        IsEqualIID( riid, IID_QuarantineOpts) )
        *ppv = this;

    if( *ppv )
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return ResultFromScode( S_OK );
        }

    return ResultFromScode( E_NOINTERFACE );

}  // end of "CQuarantineOptions::QueryInterface"



// ==== CQuarantineOptions::AddRef ========================================
//
// Function name    : CQuarantineOptions::AddRef()
// Description      : Increments reference count for this object
// Return type      : ULONG
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP_(ULONG) CQuarantineOptions::AddRef()
{
    return ++m_dwRef;

}  // end of "CQuarantineOptions::AddRef"



// ==== CQuarantineOptions::Release =======================================
//
// Function name    : CQuarantineOptions::Release()
// Description      : Decrements reference count for this object
// Return type      : ULONG
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP_(ULONG) CQuarantineOptions::Release()
{
    if( --m_dwRef )
        {
        return m_dwRef;
        }

    delete this;
    return 0;

}  // end of "CQuarantineOptions::Release"









// ==== Initialize ========================================================
//
//  This function is used to initialize the Quarantine options object.  The
//  caller must call this funtion before any others.
//
//  Input:
//      lpszFileName    -- a fully-qualified path to the options file
//
//      bCreate         -- TRUE means that if the options file does not
//                         exist, a default copy will be generated
//
//  Output:
//      S_OK if all goes well, E_UNABLE_TO_READ_OPTIONS if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::Initialize(LPSTR lpszFileName, BOOL  bCreate)
{
    auto    HRESULT     retVal = S_OK;
    auto    HRESULT     tempRetVal = E_UNABLE_TO_READ_OPTIONS;


    if( TRUE == m_bInitialized )
        {
        return ERROR_ALREADY_INITIALIZED;
        }

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
    m_bInitialized = TRUE;

    // Load in the options file
    tempRetVal = ReadQuarOptsFile();
    if( FAILED(tempRetVal) )
        {
        // If we cannot read, then we were not completely initialized
        m_bInitialized = FALSE;
        retVal = tempRetVal;
        goto Exit_Function;
        }

Exit_Function:

    return (retVal);

}  // end of "CQuarantineOptions::Initialize"



// ==== ReadQuarOptsFile ==================================================
//
//  This function reads the Quarantine options file and uses its contents
//  to initialize the QUAR_OPTS_STRUCT structure.
//
//  Input:
//      nothing
//
//  Output:
//      S_OK if all goes well, E_UNEXPECTED if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::ReadQuarOptsFile(void)
{
    auto    BOOL        bResult = TRUE;
    auto    HRESULT     retVal = S_OK;

    if( FALSE == m_bInitialized )
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
        {
        retVal = E_ERROR_READING_OPTIONS;
        }

    bResult = ReadForwardingEnabled();
    if (FALSE == bResult)
        {
        retVal = E_ERROR_READING_OPTIONS;
        }

    bResult = ReadScanRepairedFiles();
    if (FALSE == bResult)
        {
        retVal = E_ERROR_READING_OPTIONS;
        }

    bResult = ReadSarcSubmissionEnabled();
    if (FALSE == bResult)
        {
        retVal = E_ERROR_READING_OPTIONS;
        }

    bResult = ReadQuarantineServer();
    if (FALSE == bResult)
        {
        retVal = E_ERROR_READING_OPTIONS;
        }

    bResult = ReadValidForwardTypes();
    if (FALSE == bResult)
        {
        retVal = E_ERROR_READING_OPTIONS;
        }

    bResult = ReadValidFileOperations(QFILE_STATUS_QUARANTINED);
    if (FALSE == bResult)
        {
        retVal = E_ERROR_READING_OPTIONS;
        }

    bResult = ReadValidFileOperations(QFILE_STATUS_SUBMITTED);
    if (FALSE == bResult)
        {
        retVal = E_ERROR_READING_OPTIONS;
        }

    bResult = ReadValidFileOperations(QFILE_STATUS_BACKUP_FILE);
    if (FALSE == bResult)
        {
        retVal = E_ERROR_READING_OPTIONS;
        }

    bResult = ReadPassword();
    if (FALSE == bResult)
        {
        retVal = E_ERROR_READING_OPTIONS;
        }

    bResult = ReadPasswordEnabled();
    if (FALSE == bResult)
        {
        retVal = E_ERROR_READING_OPTIONS;
        }

Exit_Function:
    return (retVal);

}  // end of "CQuarantineOptions::ReadQuarOptsFile"



// ==== WriteQuarOptsFile =================================================
//
//  This function writes the Quarantine options file with the values currently
//  stored in the QUAR_OPTS_STRUCT structure.
//
//  Input:
//      dwErrorBits     -- a pointer to the DWORD that will be used to set
//                         error specific flags
//
//  Output:
//      S_OK if all goes well, E_UNEXPECTED if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::WriteQuarOptsFile()
{
    auto    BOOL        bResult = TRUE;
    auto    HRESULT     retVal = S_OK;

    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    // make sure the file exists
    if (FALSE == QuarOptsFileExists(m_QuarOptsPath))
        {
        retVal = E_UNABLE_TO_FIND_OPTIONS;
        goto  Exit_Function;
        }

    // write the file data and set the DWORD error bits
    bResult = WriteScanAtStartup();
    if (FALSE == bResult)
        {
        retVal = E_ERROR_WRITING_OPTIONS;
        }

    bResult = WriteForwardingEnabled();
    if (FALSE == bResult)
        {
        retVal = E_ERROR_WRITING_OPTIONS;
        }

    bResult = WriteScanRepairedFiles();
    if (FALSE == bResult)
        {
        retVal = E_ERROR_WRITING_OPTIONS;
        }

    bResult = WriteSarcSubmissionEnabled();
    if (FALSE == bResult)
        {
        retVal = E_ERROR_WRITING_OPTIONS;
        }

    bResult = WriteQuarantineServer();
    if (FALSE == bResult)
        {
        retVal = E_ERROR_WRITING_OPTIONS;
        }

    bResult = WriteValidForwardTypes();
    if (FALSE == bResult)
        {
        retVal = E_ERROR_WRITING_OPTIONS;
        }

    bResult = WriteValidFileOperations(QFILE_STATUS_QUARANTINED);
    if (FALSE == bResult)
        {
        retVal = E_ERROR_WRITING_OPTIONS;
        }

    bResult = WriteValidFileOperations(QFILE_STATUS_SUBMITTED);
    if (FALSE == bResult)
        {
        retVal = E_ERROR_WRITING_OPTIONS;
        }

    bResult = WriteValidFileOperations(QFILE_STATUS_BACKUP_FILE);
    if (FALSE == bResult)
        {
        retVal = E_ERROR_WRITING_OPTIONS;
        }

    bResult = WritePassword();
    if (FALSE == bResult)
        {
        retVal = E_ERROR_WRITING_OPTIONS;
        }

    bResult = WritePasswordEnabled();
    if (FALSE == bResult)
        {
        retVal = E_ERROR_WRITING_OPTIONS;
        }

Exit_Function:
    return (retVal);

}  // end of "CQuarantineOptions::WriteQuarOptsFile"



// ==== GetScanAtStartup ==================================================
//
//  This function fetches the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:
//      bValue      -- a pointer to a BOOL that will receive the value
//
//  Output:
//      S_OK
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::GetScanAtStartup(BOOL*  bValue)
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    *bValue = m_QuarOpts.bScanAtStartup;
    return (S_OK);

}  // end of "CQuarantineOptions::GetScanAtStartup"



// ==== ReadScanAtStartup =================================================
//
//  This function reads the current value of the "scan at startup" option
//  from the Quarantine options file.
//
//  Input:  nothing
//
//  Output: TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::ReadScanAtStartup(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;
    auto    HRESULT     retVal = S_OK;

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
            {
            m_QuarOpts.bScanAtStartup = TRUE;
            }
        else
            {
            m_QuarOpts.bScanAtStartup = FALSE;
            }
        }

    return (bResult);

}  // end of "CQuarantineOptions::ReadScanAtStartup"



// ==== SetScanAtStartup ==================================================
//
//  This function sets the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:  nothing
//
//  Output: TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::SetScanAtStartup(BOOL  bValue)
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    m_QuarOpts.bScanAtStartup = bValue;
    return (S_OK);

}  // end of "CQuarantineOptions::SetScanAtStartup"



// ==== WriteScanAtStartup ================================================
//
//  This function writes the target QUAR_OPTS_STRUCT to the Quarantine
//  options file.
//
//  Input:  nothing
//
//  Output: a value of TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::WriteScanAtStartup(void)
{
    auto    BOOL        bResult = TRUE;
    auto    char*       szValue = NULL;


    // determine the value to write
    if (TRUE == m_QuarOpts.bScanAtStartup)
        {
        szValue = "TRUE";
        }
    else
        {
        szValue = "FALSE";
        }

    // write the string out
    bResult = WritePrivateProfileString(QOPTS_SECTION_SCAN_AT_STARTUP   // section name
                                        , QOPTS_KEY_ENABLED             // key name
                                        , szValue                       // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    return (bResult);

}  // end of "CQuarantineOptions::WriteScanAtStartup"



// ==== GetForwardingEnabled ==============================================
//
//  This function fetches the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:
//      bValue      -- a pointer to a BOOL that will receive the value
//
//  Output:
//      S_OK
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::GetForwardingEnabled(BOOL*  bValue)
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    *bValue = m_QuarOpts.bForwardingEnabled;
    return (S_OK);

}  // end of "CQuarantineOptions::GetForwardingEnabled"



// ==== ReadForwardingEnabled =============================================
//
//  This function reads the current value of the "forwarding enabled" option
//  from the Quarantine options file.
//
//  Input:  nothing
//
//  Output: TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::ReadForwardingEnabled(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(QOPTS_SECTION_FORWARDING_ENABLED // section name
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
            {
            m_QuarOpts.bForwardingEnabled = TRUE;
            }
        else
            {
            m_QuarOpts.bForwardingEnabled = FALSE;
            }
        }

    return (bResult);

}  // end of "CQuarantineOptions::ReadForwardingEnabled"



// ==== SetForwardingEnabled ==============================================
//
//  This function sets the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:  nothing
//
//  Output: TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::SetForwardingEnabled(BOOL  bValue)
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    m_QuarOpts.bForwardingEnabled = bValue;
    return (S_OK);

}  // end of "CQuarantineOptions::SetForwardingEnabled"



// ==== WriteForwardingEnabled ============================================
//
//  This function writes the target QUAR_OPTS_STRUCT to the Quarantine
//  options file.
//
//  Input:  nothing
//
//  Output: a value of TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::WriteForwardingEnabled(void)
{
    auto    BOOL        bResult = TRUE;
    auto    char*       szValue = NULL;


    // determine the value to write
    if (TRUE == m_QuarOpts.bForwardingEnabled)
        {
        szValue = "TRUE";
        }
    else
        {
        szValue = "FALSE";
        }

    // write the string out
    bResult = WritePrivateProfileString(QOPTS_SECTION_FORWARDING_ENABLED    // section name
                                        , QOPTS_KEY_ENABLED                 // key name
                                        , szValue                           // input buffer
                                        , m_QuarOptsPath);                  // location of INI file
    return (bResult);

}  // end of "CQuarantineOptions::WriteForwardingEnabled"



// ==== GetScanRepairedFiles ==============================================
//
//  This function fetches the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:
//      bValue      -- a pointer to a BOOL that will receive the value
//
//  Output:
//      S_OK
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::GetScanRepairedFiles(BOOL*  bValue)
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    *bValue = m_QuarOpts.bScanRepairedFiles;
    return (S_OK);

}  // end of "CQuarantineOptions::GetScanRepairedFiles"



// ==== ReadScanRepairedFiles =============================================
//
//  This function reads the current value of the "scan repaired files"
//  option from the Quarantine options file.
//
//  Input:  nothing
//
//  Output: TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::ReadScanRepairedFiles(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(QOPTS_SECTION_SCAN_REPAIRED_FILES    // section name
                                        , QOPTS_KEY_ENABLED                 // key name
                                        , INI_DEFAULT_STR                   // default string
                                        , m_szIniDataBuf                    // destination buffer
                                        , MAX_INI_BUF_SIZE                  // size of destination buffer
                                        , m_QuarOptsPath);                  // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        if (0 == lstrcmp("TRUE", m_szIniDataBuf))
            {
            m_QuarOpts.bScanRepairedFiles = TRUE;
            }
        else
            {
            m_QuarOpts.bScanRepairedFiles = FALSE;
            }
        }

    return (bResult);

}  // end of "CQuarantineOptions::ReadScanRepairedFiles"



// ==== SetScanRepairedFiles ==============================================
//
//  This function sets the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:  nothing
//
//  Output: TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::SetScanRepairedFiles(BOOL  bValue)
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    m_QuarOpts.bScanRepairedFiles = bValue;
    return (S_OK);

}  // end of "CQuarantineOptions::SetScanRepairedFiles"



// ==== WriteScanRepairedFiles ============================================
//
//  This function writes the target QUAR_OPTS_STRUCT to the Quarantine
//  options file.
//
//  Input:  nothing
//
//  Output: a value of TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::WriteScanRepairedFiles(void)
{
    auto    BOOL        bResult = TRUE;
    auto    char*       szValue = NULL;


    // determine the value to write
    if (TRUE == m_QuarOpts.bScanRepairedFiles)
        {
        szValue = "TRUE";
        }
    else
        {
        szValue = "FALSE";
        }

    // write the string out
    bResult = WritePrivateProfileString(QOPTS_SECTION_SCAN_REPAIRED_FILES   // section name
                                        , QOPTS_KEY_ENABLED                 // key name
                                        , szValue                           // input buffer
                                        , m_QuarOptsPath);                  // location of INI file
    return (bResult);

}  // end of "CQuarantineOptions::WriteScanRepairedFiles"



// ==== GetSarcSubmissionEnabled ==========================================
//
//  This function fetches the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:
//      bValue      -- a pointer to a BOOL that will receive the value
//
//  Output:
//      S_OK
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::GetSarcSubmissionEnabled(BOOL*  bValue)
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    *bValue = m_QuarOpts.bSarcSubmissionEnabled;
    return (S_OK);

}  // end of "CQuarantineOptions::GetSarcSubmissionEnabled"



// ==== ReadSarcSubmissionEnabled =========================================
//
//  This function reads the current value of the "SARC submission enabled"
//  option from the Quarantine options file.
//
//  Input:  nothing
//
//  Output: TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::ReadSarcSubmissionEnabled(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(QOPTS_SECTION_SARC_SUBMISSION_ENABLED    // section name
                                        , QOPTS_KEY_ENABLED                     // key name
                                        , INI_DEFAULT_STR                       // default string
                                        , m_szIniDataBuf                        // destination buffer
                                        , MAX_INI_BUF_SIZE                      // size of destination buffer
                                        , m_QuarOptsPath);                      // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        if (0 == lstrcmp("TRUE", m_szIniDataBuf))
            {
            m_QuarOpts.bSarcSubmissionEnabled = TRUE;
            }
        else
            {
            m_QuarOpts.bSarcSubmissionEnabled = FALSE;
            }
        }

    return (bResult);

}  // end of "CQuarantineOptions::ReadSarcSubmissionEnabled"



// ==== SetSarcSubmissionEnabled ==========================================
//
//  This function sets the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:  nothing
//
//  Output: TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::SetSarcSubmissionEnabled(BOOL  bValue)
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    m_QuarOpts.bSarcSubmissionEnabled = bValue;
    return (S_OK);

}  // end of "CQuarantineOptions::SetSarcSubmissionEnabled"



// ==== WriteSarcSubmissionEnabled ========================================
//
//  This function writes the target QUAR_OPTS_STRUCT to the Quarantine
//  options file.
//
//  Input:  nothing
//
//  Output: a value of TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::WriteSarcSubmissionEnabled(void)
{
    auto    BOOL        bResult = TRUE;
    auto    char*       szValue = NULL;


    // determine the value to write
    if (TRUE == m_QuarOpts.bSarcSubmissionEnabled)
        {
        szValue = "TRUE";
        }
    else
        {
        szValue = "FALSE";
        }

    // write the string out
    bResult = WritePrivateProfileString(QOPTS_SECTION_SARC_SUBMISSION_ENABLED   // section name
                                        , QOPTS_KEY_ENABLED                     // key name
                                        , szValue                               // input buffer
                                        , m_QuarOptsPath);                      // location of INI file
    return (bResult);

}  // end of "CQuarantineOptions::WriteSarcSubmissionEnabled"



// ==== GetQuarantineServer ===============================================
//
//  This function fetches the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:
//      szCharBuf [out]     -- a pointer to the buffer that will receive
//                             the current Quarantine server value
//      nBufSize [in]       -- the size of the buffer argument
//
//  Output:
//      S_OK
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::GetQuarantineServer(LPTSTR  szCharBuf
                                                        , int  nBufSize)
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    _tcsncpy(szCharBuf, m_QuarOpts.szQuarantineServer, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
    return (S_OK);

}  // end of "CQuarantineOptions::GetQuarantineServer"



// ==== ReadQuarantineServer ==============================================
//
//  This function reads the current value of the Quarantine server name
//  option from the Quarantine options file.
//
//  Input:  nothing
//
//  Output: TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::ReadQuarantineServer(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(QOPTS_SECTION_QUARANTINE_SERVER  // section name
                                        , QOPTS_KEY_QSERVER             // key name
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
        lstrcpy(m_QuarOpts.szQuarantineServer, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CQuarantineOptions::ReadQuarantineServer"



// ==== SetQuarantineServer ===============================================
//
//  This function sets the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:  nothing
//
//  Output: TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::SetQuarantineServer(LPTSTR  szCharBuf)
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    _tcsncpy(m_QuarOpts.szQuarantineServer, szCharBuf
                                            , MAX_QUARANTINE_MACHINE_NAME_LEN);
    szCharBuf[MAX_QUARANTINE_MACHINE_NAME_LEN-1] = NULL;
    return (S_OK);

}  // end of "CQuarantineOptions::SetQuarantineServer"



// ==== WriteQuarantineServer =============================================
//
//  This function writes the target QUAR_OPTS_STRUCT to the Quarantine
//  options file.
//
//  Input:  nothing
//
//  Output: a value of TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::WriteQuarantineServer(void)
{
    auto    BOOL        bResult = TRUE;

    // write the string out
    bResult = WritePrivateProfileString(QOPTS_SECTION_QUARANTINE_SERVER         // section name
                                        , QOPTS_KEY_QSERVER                     // key name
                                        , m_QuarOpts.szQuarantineServer         // input buffer
                                        , m_QuarOptsPath);                      // location of INI file
    return (bResult);

}  // end of "CQuarantineOptions::WriteQuarantineServer"



// ==== GetValidForwardTypes ==============================================
//
//  This function fetches the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:
//      dwValidTypes [out]  -- a pointer to the DWORD that will receive
//                             the current file types that may be forwarded
//
//  Output:
//      S_OK
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::GetValidForwardTypes(DWORD*  dwValidTypes)
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    *dwValidTypes = m_QuarOpts.dwValidForwardFileTypes;
    return (S_OK);

}  // end of "CQuarantineOptions::GetValidForwardTypes"



// ==== ReadValidForwardTypes =============================================
//
//  This function reads the current file types that are valid to forward to
//  a Quarantine server from the Quarantine options file.
//
//  Input:  nothing
//
//  Output: TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::ReadValidForwardTypes(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;


    // first initialize the current flag variable
    m_QuarOpts.dwValidForwardFileTypes = 0;


    // now fetch the strings from the INI file...

    // "submitted" files
    dwResult = GetPrivateProfileString(QOPTS_SECTION_VALID_FORWARD_TYPES    // section name
                                        , QOPTS_KEY_SUBMITTED               // key name
                                        , INI_DEFAULT_STR                   // default string
                                        , m_szIniDataBuf                    // destination buffer
                                        , MAX_INI_BUF_SIZE                  // size of destination buffer
                                        , m_QuarOptsPath);                  // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        goto  Exit_Function;
        }
    else
        {
        if (0 == lstrcmp("TRUE", m_szIniDataBuf))
            {
            m_QuarOpts.dwValidForwardFileTypes |= QFILE_STATUS_SUBMITTED;
            }
        else
            {
            m_QuarOpts.dwValidForwardFileTypes &= ~QFILE_STATUS_SUBMITTED;
            }
        }


    // "repaired" files
    dwResult = GetPrivateProfileString(QOPTS_SECTION_VALID_FORWARD_TYPES    // section name
                                        , QOPTS_KEY_REPAIRED                // key name
                                        , INI_DEFAULT_STR                   // default string
                                        , m_szIniDataBuf                    // destination buffer
                                        , MAX_INI_BUF_SIZE                  // size of destination buffer
                                        , m_QuarOptsPath);                  // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        goto  Exit_Function;
        }
    else
        {
        if (0 == lstrcmp("TRUE", m_szIniDataBuf))
            {
            m_QuarOpts.dwValidForwardFileTypes |= QFILE_STATUS_BACKUP_FILE;
            }
        else
            {
            m_QuarOpts.dwValidForwardFileTypes &= ~QFILE_STATUS_BACKUP_FILE;
            }
        }

    // "quarantined" files
    dwResult = GetPrivateProfileString(QOPTS_SECTION_VALID_FORWARD_TYPES    // section name
                                        , QOPTS_KEY_QUARANTINED             // key name
                                        , INI_DEFAULT_STR                   // default string
                                        , m_szIniDataBuf                    // destination buffer
                                        , MAX_INI_BUF_SIZE                  // size of destination buffer
                                        , m_QuarOptsPath);                  // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        goto  Exit_Function;
        }
    else
        {
        if (0 == lstrcmp("TRUE", m_szIniDataBuf))
            {
            m_QuarOpts.dwValidForwardFileTypes |= QFILE_STATUS_QUARANTINED;
            }
        else
            {
            m_QuarOpts.dwValidForwardFileTypes &= ~QFILE_STATUS_QUARANTINED;
            }
        }


Exit_Function:
    return (bResult);

}  // end of "CQuarantineOptions::ReadValidForwardTypes"



// ==== SetValidForwardTypes ==============================================
//
//  This function sets the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:
//      dwValidTypes    -- a DWORD bit mask flagging which file types are
//                         valid to forward to a Quarantine server
//
//  Output:
//      S_OK
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::SetValidForwardTypes(DWORD  dwValidTypes)
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    m_QuarOpts.dwValidForwardFileTypes = dwValidTypes;
    return (S_OK);

}  // end of "CQuarantineOptions::SetValidForwardTypes"



// ==== WriteValidForwardTypes ============================================
//
//  This function writes the target QUAR_OPTS_STRUCT to the Quarantine
//  options file.
//
//  Input:  nothing
//
//  Output: a value of TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::WriteValidForwardTypes(void)
{
    auto    BOOL        bRetVal = TRUE;
    auto    BOOL        bResult = TRUE;
    auto    char*       szValue = NULL;


    // "submitted" files
    if (m_QuarOpts.dwValidForwardFileTypes & QFILE_STATUS_SUBMITTED)
        {
        szValue = "TRUE";
        }
    else
        {
        szValue = "FALSE";
        }
    bResult = WritePrivateProfileString(QOPTS_SECTION_VALID_FORWARD_TYPES   // section name
                                        , QOPTS_KEY_SUBMITTED               // key name
                                        , szValue                           // input buffer
                                        , m_QuarOptsPath);                  // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }


    // "repaired" files
    if (m_QuarOpts.dwValidForwardFileTypes & QFILE_STATUS_BACKUP_FILE)
        {
        szValue = "TRUE";
        }
    else
        {
        szValue = "FALSE";
        }
    bResult = WritePrivateProfileString(QOPTS_SECTION_VALID_FORWARD_TYPES   // section name
                                        , QOPTS_KEY_REPAIRED                // key name
                                        , szValue                           // input buffer
                                        , m_QuarOptsPath);                  // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }


    // "quarantined" files
    if (m_QuarOpts.dwValidForwardFileTypes & QFILE_STATUS_QUARANTINED)
        {
        szValue = "TRUE";
        }
    else
        {
        szValue = "FALSE";
        }
    bResult = WritePrivateProfileString(QOPTS_SECTION_VALID_FORWARD_TYPES   // section name
                                        , QOPTS_KEY_QUARANTINED             // key name
                                        , szValue                           // input buffer
                                        , m_QuarOptsPath);                  // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }


    return (bRetVal);

}  // end of "CQuarantineOptions::WriteValidForwardTypes"



// ==== GetValidFileOperations ============================================
//
//  This function fetches the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:
//      dwFileOps [out]     -- a pointer to the DWORD that will receive
//                             the valid operations for the file type
//                             specified by the "index" argument
//
//      nIndex [in]         -- the desired file type; possible values are:
//
//                              QFILEOPS_INDEX_QUARANTINED
//                              QFILEOPS_INDEX_SUBMITTED
//                              QFILEOPS_INDEX_BACKUP_FILE
//
//  Output:
//      S_OK if all goes well, E_UNEXPECTED if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::GetValidFileOperations(DWORD*  dwFileOps
                                                            , int  nIndex)
{
    auto    HRESULT     retVal = S_OK;

    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;


    if (nIndex > MAX_VALID_QFILEOPS_MEMBERS)
        {
        retVal = E_UNEXPECTED;
        }
    else
        {
        *dwFileOps = m_QuarOpts.dwAllowableFileOperations[nIndex];
        }

    return (retVal);

}  // end of "CQuarantineOptions::GetValidFileOperations"



// ==== ReadValidFileOperations ===========================================
//
//  This function reads the allowable operations on a target file type from
//  from the Quarantine options file.
//
//  Input:
//      dwTargetFileStatus  -- the target file type
//
//  Output:
//      TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::ReadValidFileOperations(DWORD  dwTargetFileStatus)
{
    auto    BOOL        bResult = TRUE;
    auto    char*       szSectionName;
    auto    DWORD       dwResult = 0;
    auto    int         nIndex = 0;


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
        goto  Exit_Function;
        }
    else
        {
        if (0 == lstrcmp("TRUE", m_szIniDataBuf))
            {
            m_QuarOpts.dwAllowableFileOperations[nIndex] |= QUAR_OPTS_FILEOP_DELETE;
            }
        else
            {
            m_QuarOpts.dwAllowableFileOperations[nIndex] &= ~QUAR_OPTS_FILEOP_DELETE;
            }
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
            {
            m_QuarOpts.dwAllowableFileOperations[nIndex] |= QUAR_OPTS_FILEOP_SCAN;
            }
        else
            {
            m_QuarOpts.dwAllowableFileOperations[nIndex] &= ~QUAR_OPTS_FILEOP_SCAN;
            }
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
            {
            m_QuarOpts.dwAllowableFileOperations[nIndex] |= QUAR_OPTS_FILEOP_REPAIR;
            }
        else
            {
            m_QuarOpts.dwAllowableFileOperations[nIndex] &= ~QUAR_OPTS_FILEOP_REPAIR;
            }
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
            {
            m_QuarOpts.dwAllowableFileOperations[nIndex] |= QUAR_OPTS_FILEOP_PUT_BACK;
            }
        else
            {
            m_QuarOpts.dwAllowableFileOperations[nIndex] &= ~QUAR_OPTS_FILEOP_PUT_BACK;
            }
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
            {
            m_QuarOpts.dwAllowableFileOperations[nIndex] |= QUAR_OPTS_FILEOP_SUBMIT_TO_SARC;
            }
        else
            {
            m_QuarOpts.dwAllowableFileOperations[nIndex] &= ~QUAR_OPTS_FILEOP_SUBMIT_TO_SARC;
            }
        }


Exit_Function:
    return (bResult);

}  // end of "CQuarantineOptions::ReadValidFileOperations"



// ==== SetValidFileOperations ============================================
//
//  This function sets the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:
//      dwFileOps   -- a DWORD bitmask representing the valid operations
//                     on the target file type
//
//      nIndex      -- an index value specifying which file type to update;
//                     possible values are:
//
//                          QFILEOPS_INDEX_QUARANTINED
//                          QFILEOPS_INDEX_SUBMITTED
//                          QFILEOPS_INDEX_BACKUP_FILE
//
//  Output:
//      S_OK if all goes well, E_UNEXPECTED if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::SetValidFileOperations(DWORD  dwFileOps
                                                            , int  nIndex)
{
    auto    HRESULT     retVal = S_OK;

    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    if (nIndex > MAX_VALID_QFILEOPS_MEMBERS)
        {
        retVal = E_UNEXPECTED;
        }
    else
        {
        m_QuarOpts.dwAllowableFileOperations[nIndex] = dwFileOps;
        }

    return (retVal);

}  // end of "CQuarantineOptions::SetValidFileOperations"



// ==== WriteValidFileOperations ==========================================
//
//  This function writes the target QUAR_OPTS_STRUCT to the Quarantine
//  options file.
//
//  Input:
//      dwTargetFileStatus  -- the target file status
//
//  Output:
//      TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::WriteValidFileOperations(DWORD  dwTargetFileStatus)
{
    auto    BOOL        bResult = TRUE;
    auto    char*       szSectionName = NULL;
    auto    char*       szValue = NULL;
    auto    int         nIndex = 0;


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

        default:
            nIndex = -1;
            break;
        }


    // now write the allowable operations for the target file status...

    // delete
    if (m_QuarOpts.dwAllowableFileOperations[nIndex] & QUAR_OPTS_FILEOP_DELETE)
        {
        szValue = "TRUE";
        }
    else
        {
        szValue = "FALSE";
        }
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_DELETE              // key name
                                        , szValue                       // input buffer
                                        , m_QuarOptsPath);              // location of INI file


    // scan
    if (m_QuarOpts.dwAllowableFileOperations[nIndex] & QUAR_OPTS_FILEOP_SCAN)
        {
        szValue = "TRUE";
        }
    else
        {
        szValue = "FALSE";
        }
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_SCAN                // key name
                                        , szValue                       // input buffer
                                        , m_QuarOptsPath);              // location of INI file


    // repair
    if (m_QuarOpts.dwAllowableFileOperations[nIndex] & QUAR_OPTS_FILEOP_REPAIR)
        {
        szValue = "TRUE";
        }
    else
        {
        szValue = "FALSE";
        }
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_REPAIR              // key name
                                        , szValue                       // input buffer
                                        , m_QuarOptsPath);              // location of INI file


    // put back
    if (m_QuarOpts.dwAllowableFileOperations[nIndex] & QUAR_OPTS_FILEOP_PUT_BACK)
        {
        szValue = "TRUE";
        }
    else
        {
        szValue = "FALSE";
        }
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_PUTBACK             // key name
                                        , szValue                       // input buffer
                                        , m_QuarOptsPath);              // location of INI file


    // submit to sarc
    if (m_QuarOpts.dwAllowableFileOperations[nIndex] & QUAR_OPTS_FILEOP_SUBMIT_TO_SARC)
        {
        szValue = "TRUE";
        }
    else
        {
        szValue = "FALSE";
        }
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_SUBMIT_TO_SARC      // key name
                                        , szValue                       // input buffer
                                        , m_QuarOptsPath);              // location of INI file


    return (bResult);

}  // end of "CQuarantineOptions::WriteValidFileOperations"



// ==== GetPassword =======================================================
//
//  This function fetches the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:
//      szCharBuf [out]     -- a pointer to the buffer that will receive
//                             the current Quarantine server value
//      nBufSize [in]       -- the size of the buffer argument
//
//  Output:
//      S_OK
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::GetPassword(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_QuarOpts.szPassword, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
    return (S_OK);

}  // end of "CQuarantineOptions::GetPassword"



// ==== ReadPassword ======================================================
//
//  This function reads the current value of the Quarantine password from
//  the Quarantine options file.
//
//  Input:  nothing
//
//  Output: TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::ReadPassword(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(QOPTS_SECTION_QUARANTINE_PASSWORD    // section name
                                        , QOPTS_KEY_PASSWORD                // key name
                                        , INI_DEFAULT_STR                   // default string
                                        , m_szIniDataBuf                    // destination buffer
                                        , MAX_INI_BUF_SIZE                  // size of destination buffer
                                        , m_QuarOptsPath);                  // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_QuarOpts.szPassword, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CQuarantineOptions::ReadPassword"



// ==== SetPassword =======================================================
//
//  This function sets the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:  nothing
//
//  Output: TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::SetPassword(LPTSTR  szCharBuf)
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    _tcsncpy(m_QuarOpts.szPassword, szCharBuf, MAX_QUAR_PASSWORD_LEN);
    szCharBuf[MAX_QUAR_PASSWORD_LEN-1] = NULL;
    return (S_OK);

}  // end of "CQuarantineOptions::SetPassword"



// ==== WritePassword =====================================================
//
//  This function writes the target QUAR_OPTS_STRUCT to the Quarantine
//  options file.
//
//  Input:  nothing
//
//  Output: a value of TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::WritePassword(void)
{
    auto    BOOL        bResult = TRUE;

    // write the string out
    bResult = WritePrivateProfileString(QOPTS_SECTION_QUARANTINE_PASSWORD   // section name
                                        , QOPTS_KEY_PASSWORD                // key name
                                        , m_QuarOpts.szPassword             // input buffer
                                        , m_QuarOptsPath);                  // location of INI file
    return (bResult);

}  // end of "CQuarantineOptions::WritePassword"



// ==== GetPasswordEnabled ================================================
//
//  This function fetches the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:
//      szCharBuf [out]     -- a pointer to the buffer that will receive
//                             the current Quarantine server value
//      nBufSize [in]       -- the size of the buffer argument
//
//  Output:
//      S_OK
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::GetPasswordEnabled(BOOL*  bValue)
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    *bValue = m_QuarOpts.bPasswordEnabled;
    return (S_OK);

}  // end of "CQuarantineOptions::GetPasswordEnabled"



// ==== ReadPasswordEnabled ===============================================
//
//  This function reads the current value of the Quarantine password from
//  the Quarantine options file.
//
//  Input:  nothing
//
//  Output: TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::ReadPasswordEnabled(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;
    auto    HRESULT     retVal = S_OK;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(QOPTS_SECTION_QUARANTINE_PASSWORD    // section name
                                        , QOPTS_KEY_ENABLED                 // key name
                                        , INI_DEFAULT_STR                   // default string
                                        , m_szIniDataBuf                    // destination buffer
                                        , MAX_INI_BUF_SIZE                  // size of destination buffer
                                        , m_QuarOptsPath);                  // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        if (0 == lstrcmp("TRUE", m_szIniDataBuf))
            {
            m_QuarOpts.bPasswordEnabled = TRUE;
            }
        else
            {
            m_QuarOpts.bPasswordEnabled = FALSE;
            }
        }

    return (bResult);

}  // end of "CQuarantineOptions::ReadPasswordEnabled"



// ==== SetPasswordEnabled ================================================
//
//  This function sets the current value of the target QUAR_OPTS_STRUCT
//  structure member.
//
//  Input:  nothing
//
//  Output: TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineOptions::SetPasswordEnabled(BOOL  bValue)
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    m_QuarOpts.bPasswordEnabled = bValue;
    return (S_OK);

}  // end of "CQuarantineOptions::SetPasswordEnabled"



// ==== WritePasswordEnabled ==============================================
//
//  This function writes the target QUAR_OPTS_STRUCT to the Quarantine
//  options file.
//
//  Input:  nothing
//
//  Output: a value of TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::WritePasswordEnabled(void)
{
    auto    BOOL        bResult = TRUE;
    auto    char*       szValue = NULL;


    // determine the value to write
    if (TRUE == m_QuarOpts.bPasswordEnabled)
        {
        szValue = "TRUE";
        }
    else
        {
        szValue = "FALSE";
        }

    // write the string out
    bResult = WritePrivateProfileString(QOPTS_SECTION_QUARANTINE_PASSWORD   // section name
                                        , QOPTS_KEY_ENABLED                 // key name
                                        , szValue                           // input buffer
                                        , m_QuarOptsPath);                  // location of INI file
    return (bResult);

}  // end of "CQuarantineOptions::WritePasswordEnabled"



// ==== QuarOptsFileExists ================================================
//
//  This function simply checks to see if the specified Quarantine options
//  file exists or not.
//
//  Input:
//      lpszQuarOptsPath    -- a fully-qualified path to the options file
//
//  Output:
//      TRUE if the specified file exists, FALSE if it does not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::QuarOptsFileExists(LPTSTR  lpszQuarOptsPath)
{
    auto    DWORD       dwTempAttributes = 0;
    auto    BOOL        bResult = TRUE;

    // see if the data file really exists
    dwTempAttributes = GetFileAttributes(lpszQuarOptsPath);
    if ((0xFFFFFFFF == dwTempAttributes) ||
        (FILE_ATTRIBUTE_DIRECTORY & dwTempAttributes))
        {
        bResult = FALSE;
        }

    return (bResult);

}  // end of "CQuarantineOptions::QuarOptsFileExists"



// ==== CreateDefaultQuarOptsDatFile ======================================
//
//  This function is used to generate a default DAT file.
//
//  Input:
//      lpszFileName    - a fully-qualified path to the QuarOpts.dat file
//
//  Output:
//      TRUE if all goes well, FALSE if not
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL    CQuarantineOptions::CreateDefaultQuarOptsDatFile(LPTSTR  lpszFileName)
{
    auto    BOOL        bResult = TRUE;
    auto    BOOL        bRetVal = TRUE;
    auto    char*       szSectionName = NULL;
    auto    HANDLE      fileHandle = INVALID_HANDLE_VALUE;


    // create the file
    fileHandle = CreateFile(lpszFileName                        // name of the file
                                , GENERIC_WRITE                 // access mode
                                , 0                             // no sharing
                                , NULL                          // no secuity
                                , CREATE_ALWAYS                 // always create the file
                                , FILE_FLAG_SEQUENTIAL_SCAN     // flags and attributes
                                , NULL);                        // no template attributes

    if (INVALID_HANDLE_VALUE == fileHandle)
        {
        bRetVal = FALSE;
        goto  Exit_Function;
        }
    else
        {
        CloseHandle(fileHandle);
        }



    // write out the default values...

    // password
    bResult = WritePrivateProfileString(QOPTS_SECTION_QUARANTINE_PASSWORD   // section name
                                        , QOPTS_KEY_PASSWORD                // key name
                                        , DEFAULT_PASSWORD                  // input buffer
                                        , m_QuarOptsPath);                  // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // scan automatically at startup
    bResult = WritePrivateProfileString(QOPTS_SECTION_SCAN_AT_STARTUP   // section name
                                        , QOPTS_KEY_ENABLED             // key name
                                        , DEFAULT_SCAN_AT_STARTUP       // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // enable forwarding of files
    bResult = WritePrivateProfileString(QOPTS_SECTION_FORWARDING_ENABLED    // section name
                                        , QOPTS_KEY_ENABLED                 // key name
                                        , DEFAULT_FORWARDING_ENABLED        // input buffer
                                        , m_QuarOptsPath);                  // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // scan repaired files
    bResult = WritePrivateProfileString(QOPTS_SECTION_SCAN_REPAIRED_FILES   // section name
                                        , QOPTS_KEY_ENABLED                 // key name
                                        , DEFAULT_SCAN_REPAIRED_FILES       // input buffer
                                        , m_QuarOptsPath);                  // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // enable submission to SARC
    bResult = WritePrivateProfileString(QOPTS_SECTION_SARC_SUBMISSION_ENABLED   // section name
                                        , QOPTS_KEY_ENABLED                     // key name
                                        , DEFAULT_SARC_SUBMISSION_ENABLED       // input buffer
                                        , m_QuarOptsPath);                      // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // specification of the Quarantine server (?)
    bResult = WritePrivateProfileString(QOPTS_SECTION_QUARANTINE_SERVER         // section name
                                        , QOPTS_KEY_QSERVER                     // key name
                                        , DEFAULT_SARC_QUARANTINE_SERVER        // input buffer
                                        , m_QuarOptsPath);                      // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }



    // valid file types to forward...

    // submitted files
    bResult = WritePrivateProfileString(QOPTS_SECTION_VALID_FORWARD_TYPES   // section name
                                        , QOPTS_KEY_SUBMITTED               // key name
                                        , DEFAULT_FORWARD_SUBMITTED         // input buffer
                                        , m_QuarOptsPath);                  // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }


    // repaired files
    bResult = WritePrivateProfileString(QOPTS_SECTION_VALID_FORWARD_TYPES   // section name
                                        , QOPTS_KEY_REPAIRED                // key name
                                        , DEFAULT_FORWARD_REPAIRED          // input buffer
                                        , m_QuarOptsPath);                  // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }


    // quarantined files
    bResult = WritePrivateProfileString(QOPTS_SECTION_VALID_FORWARD_TYPES   // section name
                                        , QOPTS_KEY_QUARANTINED             // key name
                                        , DEFAULT_FORWARD_QUARANTINED       // input buffer
                                        , m_QuarOptsPath);                  // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }



    // valid operation on submitted files...

    // Quarantined files...
    szSectionName = QOPTS_SECTION_VALID_OPERATIONS_QUARANTINED;

    // delete
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_DELETE              // key name
                                        , DEFAULT_QUARANTINED_DELETE    // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // scan
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_SCAN                // key name
                                        , DEFAULT_QUARANTINED_SCAN      // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // repair
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_REPAIR              // key name
                                        , DEFAULT_QUARANTINED_REPAIR    // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // put back
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_PUTBACK             // key name
                                        , DEFAULT_QUARANTINED_PUT_BACK  // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // submit to sarc
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_SUBMIT_TO_SARC      // key name
                                        , DEFAULT_QUARANTINED_SUBMIT    // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }



    // Submitted files...
    szSectionName = QOPTS_SECTION_VALID_OPERATIONS_SUBMITTED;

    // delete
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_DELETE              // key name
                                        , DEFAULT_SUBMITTED_DELETE      // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // scan
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_SCAN                // key name
                                        , DEFAULT_SUBMITTED_SCAN        // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // repair
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_REPAIR              // key name
                                        , DEFAULT_SUBMITTED_REPAIR      // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // put back
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_PUTBACK             // key name
                                        , DEFAULT_SUBMITTED_PUT_BACK    // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // submit to sarc
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_SUBMIT_TO_SARC      // key name
                                        , DEFAULT_SUBMITTED_SUBMIT      // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }



    // Repaired files...
    szSectionName = QOPTS_SECTION_VALID_OPERATIONS_REPAIRED;

    // delete
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_DELETE              // key name
                                        , DEFAULT_REPAIRED_DELETE       // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // scan
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_SCAN                // key name
                                        , DEFAULT_REPAIRED_SCAN         // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // repair
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_REPAIR              // key name
                                        , DEFAULT_REPAIRED_REPAIR       // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // put back
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_PUTBACK             // key name
                                        , DEFAULT_REPAIRED_PUT_BACK     // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // submit to sarc
    bResult = WritePrivateProfileString(szSectionName                   // section name
                                        , QOPTS_KEY_SUBMIT_TO_SARC      // key name
                                        , DEFAULT_REPAIRED_SUBMIT       // input buffer
                                        , m_QuarOptsPath);              // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }



Exit_Function:

    return (bRetVal);

}  // end of "CQuarantineOptions::CreateDefaultQuarOptsDatFile"

