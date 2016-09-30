////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/quarantinedll.cpv   1.69   08 Jul 1998 21:06:02   jtaylor  $
////////////////////////////////////////////////////////////////////////////
//
// QuarantineDLL.cpp - Contains implementation for CQuarantineDLL
//
//
//
//
////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/quarantinedll.cpv  $
// 
//    Rev 1.69   08 Jul 1998 21:06:02   jtaylor
// Updated to Xor the contents of Quarantine files by 0xff.  No longer delete non Qfiles in Incoming directory.
//
//    Rev 1.68   08 Jul 1998 17:29:24   jtaylor
// Fixed the GetScanningContext function to restore the original GetConfigInfo function.
//
//    Rev 1.67   08 Jul 1998 12:25:50   jtaylor
// Changed the Quarantine file signature and updated the code to work with both signatures and upgrade the old signature.
//
//    Rev 1.66   02 Jul 1998 17:16:36   jtaylor
// changed the isOkayToForward files to make it reload the server name.
//
//    Rev 1.65   01 Jul 1998 21:14:34   jtaylor
// Added time conversion for 95 from localtime to UTC.
//
//    Rev 1.64   01 Jul 1998 17:29:30   jtaylor
// Updated the functions to prevent file time manipulation from failing a task.
//
//    Rev 1.63   22 Jun 1998 20:49:42   jtaylor
// Added support for setting the Quarantine bloodhound level to 3.
//
//    Rev 1.62   11 Jun 1998 13:46:46   DBuches
// Updated calls to TransferFile().  Now needs filenames passed in.
//
//    Rev 1.61   08 Jun 1998 19:47:24   SEDWARD
// Added support for the callback function in ForwardAllFiles().
//
//    Rev 1.60   03 Jun 1998 14:59:40   jtaylor
// Changed date/time added to Quarantine back to GMT.
//
//    Rev 1.59   01 Jun 1998 20:06:36   SEDWARD
// Added 'NeedToForwardFiles' and 'IsValidForwardFile'.
//
//    Rev 1.58   28 May 1998 21:52:44   jtaylor
// Added NULL setting to AddFileToQuarantine.
//
//    Rev 1.57   28 May 1998 18:55:50   jtaylor
// Added call to ForwardAllFiles inside ProcessAllIncomingFiles.
//
//    Rev 1.56   26 May 1998 19:41:12   jtaylor
// Modified ProcessAllIncomingFiles to not stop when it encounters an error processing a file.
//
//    Rev 1.55   21 May 1998 12:49:12   jtaylor
// Added support for undeletable virus return on add file to quarantine.
//
//    Rev 1.54   20 May 1998 22:27:02   SEDWARD
// In the 'ForwardAllFiles' function, we now break out of the loop if
// 'ForwardToServer' fails.
//
//    Rev 1.53   20 May 1998 22:07:12   jtaylor
// Fixed invitial value bug for 2 file handles.
//
//    Rev 1.52   20 May 1998 18:05:56   jtaylor
// Fixed ProcessIncomingFile.  Added delete on failed destination, and fixed delete on source when success.
//
//    Rev 1.51   18 May 1998 21:22:18   jtaylor
// Removed duplicate code, improved modularity and readability.
//
//    Rev 1.50   15 May 1998 18:40:10   jtaylor
// Cleaned up code, moved security functions to global location.
// Moved compressed file check to global location.
//
//    Rev 1.49   12 May 1998 13:09:32   DBuches
// 1st pass at support for NT file security.
//
//    Rev 1.48   11 May 1998 13:12:16   jtaylor
// -- added checks in ForwardAllFiles()
// -- changed FetchQuarantineServer() to call InitializeQuarOptsObject()
// -- added InitializeQuarOptsObject() function
// -- added IsOkayToForwardFiles() function
//
//    Rev 1.47   08 May 1998 21:28:50   jtaylor
// Changed the file transfer buffers in AddFile and ProcessIncomingFile to
// not be on the stack.
//
//    Rev 1.46   05 May 1998 18:05:56   SEDWARD
// Oops! Forgot to specify server name in last bug fix...
//
//    Rev 1.45   05 May 1998 18:00:04   SEDWARD
// Fixed forwarding of backup files in 'ForwardAllFiles().'
//
//    Rev 1.44   27 Apr 1998 22:41:50   jtaylor
// Added more debugging code.
//
//    Rev 1.43   27 Apr 1998 17:32:38   jtaylor
// Changed the dateQuarantined header structure in AP to a SYSTEMTIME.
//
//    Rev 1.42   27 Apr 1998 15:40:52   jtaylor
// Fixed ProcessAllIncomingFiles.
// Fixed ProcessIncoming files to load the AP header from the AP file.
//
//    Rev 1.41   26 Apr 1998 22:29:04   jtaylor
// Added ProcessAllIncomingFiles function.
// Added a check on ProcessIncomingFile to return success if the file is not
//  a valid QuarantineFile.
//
//    Rev 1.40   26 Apr 1998 19:54:02   jtaylor
// Changed the names of the Quarantine Header variables.
// Upgraded the AP header processing code to the new AP date format (FILETIME)
//
//    Rev 1.39   26 Apr 1998 00:00:30   jtaylor
// Added support for preserving the original files date/times.
//
//    Rev 1.38   25 Apr 1998 23:00:00   jtaylor
// Added ProcessIncomingFile to IQuarantineDLL interface and class.
//
//    Rev 1.37   25 Apr 1998 19:52:50   jtaylor
// Removed .EXE from the compressed file extension list.
//
//    Rev 1.36   24 Apr 1998 21:05:00   jtaylor
// Added new functions for getting/setting original file date/time
// Added code to check to see if the file is compressed.
//
//    Rev 1.35   24 Apr 1998 14:45:52   DBuches
// Added code to save off domain name to header.
//
//    Rev 1.34   22 Apr 1998 15:11:34   JTaylor
// Added check to make sure we do not ennable AP if we did not dissable it.
//
//    Rev 1.33   20 Apr 1998 20:40:50   JTaylor
// Fixed a possible memory leak.
//
//    Rev 1.32   20 Apr 1998 10:59:02   JTaylor
// Added free for header buffer and check to make sure memory allocated correctl
// Added check to make sure that the file copy copied the whole file.
//
//    Rev 1.31   18 Apr 1998 23:07:32   JTaylor
// Added support for returning an IQuarantineItem * to the new quarantine file
// from AddFileToQuarantine.
//
//    Rev 1.30   18 Apr 1998 17:35:14   JTaylor
// Added GUID creation to the AddFileToQuarantine method."
//
//    Rev 1.29   15 Apr 1998 15:18:14   JTaylor
// Do not report error when adding a virus infected file that cannot be repaired
// Added an additional SaveItem to make sure the contents are saved.
//
//    Rev 1.28   10 Apr 1998 18:23:54   JTaylor
// added a call to set the files type after creation.
//
//    Rev 1.27   10 Apr 1998 18:16:58   JTaylor
// Added an initialization for the defutils object to NULL.
//
//    Rev 1.26   10 Apr 1998 12:36:06   SEDWARD
// Updated HRESULT values.
//
//    Rev 1.25   10 Apr 1998 00:04:10   SEDWARD
// Added FetchQuarantineServeName and FetchQuarantineOptionsPath.
//
//    Rev 1.24   07 Apr 1998 22:35:54   SEDWARD
// Partially filled in ForwardAllFiles.
//
//    Rev 1.23   06 Apr 1998 20:35:02   JTaylor
// Added comments on argument or return type expectations for GetDefinitionsDate and AddFileToQuarantine.
//
//    Rev 1.22   06 Apr 1998 19:25:22   JTaylor
// Added error codes to options.  Made initialize load the options, fixed an options check to see if options file exists to not return true if it is a directory.  Added Quarantine path validation.  Fixed header style for Quaropts.
//
//    Rev 1.21   02 Apr 1998 19:26:00   JTaylor
// Added support for variable length header buffer
//
//    Rev 1.20   01 Apr 1998 19:03:44   JTaylor
// fixed the initialize call and stopped the scanning engine in the destructor.
//
//    Rev 1.19   30 Mar 1998 21:01:34   JTaylor
// Enhanced error handling ability.
//
//    Rev 1.18   30 Mar 1998 19:52:50   JTaylor
// Update the method for enabling/dissabling AP.
//
//    Rev 1.17   26 Mar 1998 19:05:06   JTaylor
// Updated the error conditions in the destructor.  Updated a few comments.
//
//    Rev 1.16   25 Mar 1998 21:50:44   JTaylor
// Added more error checking, changed string functions to str.
//
//    Rev 1.15   24 Mar 1998 17:54:58   JTaylor
// Added code to dissable and enable AP for this process.  This allows us to manipulate virus infected files.
//
//    Rev 1.14   16 Mar 1998 18:45:32   SEDWARD
// Include 'Quar32.h'.
//
//    Rev 1.13   13 Mar 1998 22:29:50   JTaylor
// Added initialize member to IQuarantineDLL.  Added checks to verify that it was initialized.
//
//    Rev 1.12   11 Mar 1998 22:42:24   JTaylor
// Added file type and file status arguments to AddFileToQuarantine.
//
//    Rev 1.11   11 Mar 1998 18:57:26   JTaylor
// Changed the interface pointer on the AddFileToQuarantine to a this pointer
//
//    Rev 1.10   11 Mar 1998 18:45:50   SEDWARD
// Changed 'fileStatus' to 'dwFileStatus'.
//
//    Rev 1.9   11 Mar 1998 18:15:20   JTaylor
// Switched to core for file name maniputlation functions in AddFileToQuarantine.
//
//    Rev 1.8   11 Mar 1998 17:42:22   JTaylor
// Updated the add function to contain the code for adding and scanning for viruses.
//
//    Rev 1.7   11 Mar 1998 15:43:20   JTaylor
// Added GetCurrentDefinitionsDate to get a SYSTEMTIME with the current definitions date.
//
//    Rev 1.6   10 Mar 1998 23:41:14   JTaylor
// Added code to get the real quarantine path in the constructor, changed the argument of GetScanningContext, fixed a bug in GetScanningContext.
//
//    Rev 1.5   06 Mar 1998 19:35:08   JTaylor
// Added virus definitions usage and virus scanning initialization.
//
//    Rev 1.4   02 Mar 1998 07:58:34   DBuches
// Fixed AddFileToQuarantine().  S_FALSE is a success code.  Function now retruns E_FAIL.
//
//    Rev 1.3   27 Feb 1998 01:29:18   jtaylor
// Improved the implementation of AddFileToQuarantine
//
//    Rev 1.1   25 Feb 1998 15:09:24   DBuches
// Implemented CQuarantineDLL::Enum().
//
//    Rev 1.0   24 Feb 1998 17:33:12   DBuches
// Initial revision.
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Quar32.h"
#include "QuarantineDLL.h"
#include "EnumQuarantineItems.h"
#include "QuarAdd.h"
#include "xapi.h"
#include "navcb.h"
#include "er.h"
#include "apcomm.h"
#include "global.h"

// Global object count
extern LONG g_dwObjs;

////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::CQuarantineDLL
//
// Description      : Constructor
//
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CQuarantineDLL::CQuarantineDLL()
{
    InterlockedIncrement( &g_dwObjs );
    m_pDefObject = NULL;
    m_dwRef = 0;
    m_hContext = NULL;
    m_bInitialized = FALSE;
    m_hAPDll = NULL;
    m_bDissabledAP = FALSE;
}



////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::~CQuarantineDLL
//
// Description      : Destructor
//
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
// 3/24/98 JTAYLOR - Code added to dissable AP for this process
// 3/30/98 JTAYLOR - Moved Dissable AP code
// 4/22/98 JTAYLOR - Only enable AP if we were initialized (i.e. AP dissabled)
////////////////////////////////////////////////////////////////////////////
CQuarantineDLL::~CQuarantineDLL()
    {
    InterlockedDecrement( &g_dwObjs );

    // If AP was dissabled Enable AP for this process.
    if( TRUE == m_bInitialized )
        {
        APProtectProcess(TRUE);
        }

    // Destroy the CDefUtils object
    if(m_pDefObject)
        delete m_pDefObject;

    if( NULL != m_hContext )
        {
        VLScanClose( m_hContext );
        m_hContext = NULL;
        }
    }



///////////////////////////////////////////////////////////////////
// IUnknown implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function name    : CQuarantineDLL::QueryInterface
// Description      : This function will return a requested COM interface
// Return type      : STDMETHODIMP
// Argument         : REFIID riid - REFIID of interface requested
// Argument         : void** ppv - pointer to requested interface
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::QueryInterface( REFIID riid, void** ppv )
{
    *ppv = NULL;

    if( IsEqualIID( riid, IID_IUnknown )||
        IsEqualIID( riid, IID_QuarantineDLL) )
        *ppv = this;

    if( *ppv )
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return ResultFromScode( S_OK );
        }

    return ResultFromScode( E_NOINTERFACE );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name    : CQuarantineDLL::AddRef()
// Description      : Increments reference count for this object
// Return type      : ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CQuarantineDLL::AddRef()
{
    return ++m_dwRef;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name    : CQuarantineDLL::Release()
// Description      : Decrements reference count for this object
// Return type      : ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CQuarantineDLL::Release()
{
    if( --m_dwRef )
        return m_dwRef;

    delete this;
    return 0;
}


////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::GetVersion()
//
// Description      : Returns the version number of this object
//
// Return type      : UINT
//
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_( UINT ) CQuarantineDLL::GetVersion()
{
    return 1;
}


////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::Initialize
//
// Description      : Initialize the Quarantine Class
//
// Return type      : HRESULT S_OK on success.
//
////////////////////////////////////////////////////////////////////////////
// 3/13/98 JTAYLOR - Function created / Header added.
// 3/24/98 JTAYLOR - Code added to dissable AP for this process
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::Initialize( )
    {
    auto    DWORD       dwTempAttributes = 0;

    if( TRUE == m_bInitialized )
        return ERROR_ALREADY_INITIALIZED;

    BOOL bSuccess = GetQuarantinePath( m_szQuarantineFolder, MAX_PATH, TARGET_QUARANTINE_PATH );

    if( FALSE == bSuccess || m_szQuarantineFolder == "")
        {
        return E_UNABLE_TO_GET_QUARANTINE_PATH;
        }

    // Check to make sure that the Quarantine Path is valid.
    dwTempAttributes = GetFileAttributes(m_szQuarantineFolder);

    if ((0xFFFFFFFF == dwTempAttributes) ||
        !(FILE_ATTRIBUTE_DIRECTORY & dwTempAttributes))
        {
        return E_QUARANTINE_DIRECTORY_INVALID;
        }

    // get the name of the Quarantine server
    if (FALSE == FetchQuarantineServerName())
        {
        return E_UNABLE_TO_READ_OPTIONS;
        }

    // Create the def utils object
    m_pDefObject = new CDefUtils;
    if (NULL == m_pDefObject)
        {
         return E_OUTOFMEMORY;
        }

    if( FALSE == m_pDefObject->InitWindowsApp(QUARANTINE_APP_ID))
        {
        return E_UNABLE_TO_INITIALIZE_DEFUTILS;
        }

    // If we are not using the newest defs then update our defs
    if(!m_pDefObject->DefsAreNewest())
        {
        BOOL bDefsChanged = FALSE;

        // Start using the new defs
        if( FALSE == m_pDefObject->UseNewestDefs(&bDefsChanged))
            {
            return E_UNABLE_TO_USE_NEWEST_DEFS;
            }
        }

    // Dissable AP for this process.
    APProtectProcess(FALSE);

    m_bInitialized = TRUE;

    return S_OK;
    }


////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::GetQuarantineFolder
//
// Description      : Gets the quarantine folder
//
// Return type      : HRESULT S_OK on success.
//
// Argument         : LPSTR lpszFolder - buffer to recive folder name
// Argument         : UINT uBufferSize - size of buffer
//
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::GetQuarantineFolder( LPSTR lpszFolder,
    UINT uBufferSize )
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    if( NULL == lstrcpyn( lpszFolder, m_szQuarantineFolder, uBufferSize ))
        return E_COPY_FAILED;

    return S_OK;
}



////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::GetServerName
//
// Description      : Retrieves the quarantine server name
//
// Return type      : HRESULT - S_OK on success
//
// Argument         : LPSTR lpszName    - buffer to recive server name
// Argument         : UINT uBufferSize  - size of buffer
//
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::GetServerName( LPSTR lpszName,
    UINT uBufferSize )
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    if( NULL == lstrcpyn( lpszName, m_szServerName, uBufferSize ))
        return E_COPY_FAILED;

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::GetCurrentDefinitionsDate
//
// Description      : Retrieves the date of the current definitions for Quarantine
//                    This Puts the correct day of the defs in the SYSTEMTIME and
//                    Zeros out the time fields then puts the revision of the defs
//                    into the hours slot.
//
// Return type      : HRESULT - S_OK on success
//
// Argument         : SYSTEMTIME *lptTime -- Pointer to a structure receiving
//                                           the date.
//
////////////////////////////////////////////////////////////////////////////
// 3/6/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::GetCurrentDefinitionsDate( SYSTEMTIME *lptTime )
    {
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    if (NULL == m_pDefObject)
        {
         return E_DEFUTILS_NOT_CREATED;
        }

    if( FALSE == m_pDefObject->InitWindowsApp(QUARANTINE_APP_ID))
        {
        return E_UNABLE_TO_INITIALIZE_DEFUTILS;
        }

    // get the date of the current defs.
    WORD wYear, wMonth, wDay;
    DWORD dwRev;
    if( FALSE == m_pDefObject->GetCurrentDefsDate(&wYear, &wMonth, &wDay, &dwRev))
        return E_UNABLE_TO_GET_CURRENT_DEFS_DATE;

    lptTime->wYear = wYear;
    lptTime->wMonth = wMonth;
    lptTime->wDay = wDay;
    lptTime->wHour = (WORD)dwRev;
    lptTime->wMinute = 0;
    lptTime->wSecond = 0;
    lptTime->wMilliseconds = 0;
    lptTime->wDayOfWeek = 0;

    return S_OK;
    }

////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::GetScanningContext
//
// Description      : Retrieves the Quarantine scanning context
//                    This value is needed for QuarantineItems to scan/repair
//
// Return type      : HRESULT - S_OK on success
//
// Argument         : HVCONTEXT hContext -Pointer to reference scanning context
//
////////////////////////////////////////////////////////////////////////////
// 3/6/98 JTAYLOR - Function created / Header added.
// 6/22/98 JTAYLOR - Added function to initialize the config callback.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::GetScanningContext( HVCONTEXT *lphContext )
    {
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    // If the scanning engine has not been initialized, do so now.
    if( NULL == m_hContext )
        {
        auto    char    szDatFile [SYM_MAX_PATH + 1];
        auto    char    szInfFile [SYM_MAX_PATH + 1];
        auto    char    szDefDir[_MAX_PATH+1];

        // get the current definition directory
        if (NULL == m_pDefObject)
            {
            SYM_ASSERT(FALSE);
            return E_DEFUTILS_NOT_CREATED;
            }

        if( FALSE == m_pDefObject->InitWindowsApp(QUARANTINE_APP_ID))
            {
            SYM_ASSERT(FALSE);
            return E_UNABLE_TO_INITIALIZE_DEFUTILS;
            }

        if(!m_pDefObject->GetCurrentDefs(szDefDir, _MAX_PATH+1))
            {
            SYM_ASSERT(FALSE);
            return E_UNABLE_TO_GET_CURRENT_DEFS_DIR;
            }

        // Set up the callbacks
        NAVSetDefsPath( szDefDir );

        // Copy the current callback function
        VOID (WINAPI FAR *fpOrigCallback)
        (
            LPTSTR                              lpszID,
            LPVOID                              lpvResult,
            UINT                                uMaxResultLen,
            LPBOOL                              lpbImplemented,
            LPVOID                              lpvCookie
        ) = NULL;

       fpOrigCallback = (&gstNAVCallBacks)->lpNAVEXCallBacks->GetConfigInfo;

        // Set the callback for bloodhound scanning to the local function
        (&gstNAVCallBacks)->lpNAVEXCallBacks->GetConfigInfo = QuarantineGetConfigInfo;

        m_hContext = VLScanInit(1, &gstNAVCallBacks, 0);

        // Make sure we have a valid context
        if( m_hContext == NULL )
            {
            SYM_ASSERT(FALSE);

            // restore the callback function
            (&gstNAVCallBacks)->lpNAVEXCallBacks->GetConfigInfo = fpOrigCallback;

            return E_UNABLE_TO_INIT_AVAPI;
            }

        // Copy the definitions directories
        strcpy(szDatFile,szDefDir);
        strcpy(szInfFile,szDefDir);

        // Append the filenames
        NameAppendFile(szInfFile, "VIRSCAN.INF");
        NameAppendFile(szDatFile, "VIRSCAN1.DAT");

        VSTATUS vStat;
        // Register the Virus Database
        if (vStat = VLRegisterVirusDB( m_hContext,
                                       szDatFile,
                                       szInfFile,
                                       "") != VS_OK )
            {
            SYM_ASSERT(FALSE);
            // restore the callback function
            (&gstNAVCallBacks)->lpNAVEXCallBacks->GetConfigInfo = fpOrigCallback;

            return MapVStatus(vStat);
            }

        // restore the callback function
        (&gstNAVCallBacks)->lpNAVEXCallBacks->GetConfigInfo = fpOrigCallback;
        }

    *lphContext = m_hContext;

    return S_OK;
    }

////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::SetServerName
//
// Description      : Sets the name of the server
//
// Return type      : HRESULT - S_OK on success
//
// Argument         : LPSTR lpszName - new server name
//
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::SetServerName( LPSTR lpszName )
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    return E_NOTIMPL;
}



////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::AddFileToQuarantine
//
// Description      : Addes a file to the quarantine system.
//
// Return type      : HRESULT - S_OK on success, E_FAIL on error.
//
// Argument         : LPSTR lpszFileName -- Path and filename of the file to Quarantine
//                                          This should be a long ANSI filename.
//                    DWORD dwFileType -- The type of file (compressed...)
//                    DWORD dwFileStatus -- The status of the file (Quarantine, backup...)
//                    IQuarantineItem **pNewItem  -- This is a buffer for returning a
//                              Interface to the new QuarantineItem.  This is used for changing
//                              the file information (for example, SetStatus).
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
STDMETHODIMP CQuarantineDLL::AddFileToQuarantine( LPSTR lpszSourceFileName, DWORD dwFileType, DWORD dwFileStatus, IQuarantineItem **pNewItem )
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    char                szDestFileName[MAX_QUARANTINE_FILENAME_LEN];
    DWORD               dwBytesWritten;
    DWORD               dwBytesRead;
    HANDLE              hSource = INVALID_HANDLE_VALUE;
    QFILE_HEADER_STRUCT stQuarantineHeader;
    HRESULT             retVal;
    HRESULT             hrTemp;
    BOOL                bResult=FALSE;
    HANDLE              hFile=INVALID_HANDLE_VALUE;
    FILETIME            ftCreation;
    FILETIME            ftAccess;
    FILETIME            ftLastWrite;
    LPBYTE              lpSecurityDesc = NULL;

    // Clear out the argument to make sure it is NULL on failure
    if( NULL != pNewItem )
        {
        *pNewItem = NULL;
        }

    // Create a new file in Quarantine and prepare the header structure
    hrTemp = SetupQuarantineFile( &stQuarantineHeader,
                                   lpszSourceFileName,
                                   m_szQuarantineFolder,
                                   dwFileType,
                                   dwFileStatus,
                                   &hFile,
                                   szDestFileName,
                                   MAX_QUARANTINE_FILENAME_LEN );
    if( FAILED( hrTemp ) )
        {
        retVal = hrTemp;
        goto  Exit_Function;
        }

    // Store the original filename
    strcpy(stQuarantineHeader.FileInfo.szOriginalAnsiFilename,lpszSourceFileName);

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

    // Make sure we preserve security on NT systems.
    if( FAILED( GetFileSecurityDesc( lpszSourceFileName, &lpSecurityDesc ) ) )
        {
        retVal = E_FILE_CREATE_FAILED;
        goto  Exit_Function;
        }

    // Save off original file size.
    stQuarantineHeader.FileInfo.dwOriginalFileSize = GetFileSize( hSource, NULL );

    // Save off the current time
    GetSystemTime( &stQuarantineHeader.FileInfo.stDateQuarantined );

    // Save the original file time information.
    bResult = GetFileTime( hSource, &ftCreation, &ftAccess, &ftLastWrite );
    if( FALSE != bResult )
        {
        // Copy the original date/times of the source files.
        bResult = FileTimeToSystemTime( &ftCreation,  &stQuarantineHeader.FileInfo.stOriginalFileDateCreated  );
        bResult = FileTimeToSystemTime( &ftAccess,    &stQuarantineHeader.FileInfo.stOriginalFileDateAccessed );
        bResult = FileTimeToSystemTime( &ftLastWrite, &stQuarantineHeader.FileInfo.stOriginalFileDateWritten  );
        }

    // Write out the header and zero out the header region of the QuarantineFile.
    retVal = WriteHeaderBuffer( &stQuarantineHeader, hFile );
    if( FAILED( retVal ) )
        {
        goto  Exit_Function;
        }

    // Transfer the orignal file into the new Quarantine file.
    retVal = TransferFile( lpszSourceFileName, hSource, szDestFileName, hFile );
    if( FAILED( retVal ) )
        {
        goto  Exit_Function;
        }

    // Make sure the the entire file was copied.  The new file should be the
    // length of the header buffer + the length of the original file.
    dwBytesRead = GetFileSize( hSource, NULL );
    dwBytesWritten = GetFileSize( hFile, NULL );
    if( dwBytesRead + stQuarantineHeader.dwHeaderBytes != dwBytesWritten )
        {
        retVal = E_FILE_WRITE_FAILED;
        goto  Exit_Function;
        }

    retVal = S_OK;

Exit_Function:

    // close files
    if (INVALID_HANDLE_VALUE != hSource)
        {
        CloseHandle( hSource );
        hSource = INVALID_HANDLE_VALUE;
        }

    if (INVALID_HANDLE_VALUE != hFile)
        {
        CloseHandle( hFile );
        hFile = INVALID_HANDLE_VALUE;

        // Preserve file security
        SetFileSecurityDesc( szDestFileName, lpSecurityDesc );
        }

    // Free security descriptor.
    if( NULL != lpSecurityDesc )
        {
        delete [] lpSecurityDesc;
        lpSecurityDesc = NULL;
        }

    if( S_OK == retVal )
        {
        // Scan the new Quarantine Item to update its virus information
        retVal = ScanNewQuarantineItem( szDestFileName, pNewItem );
        }

    return (retVal);
}



////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::ProcessAllIncomingFiles
//
// Description      : This function iterates through the Quarantine Incoming
//                    directory and calls ProcessIncomingFile on each file
//                    in it.
//                    After it is done with that, it calls ForwardAllFiles
//
// Return type      : HRESULT - S_OK on success
//                              E_UNINITIALIZED -- if the IQuarantineDLL is not initialized
//                              E_FAIL -- if there is any problem processing files.
//
////////////////////////////////////////////////////////////////////////////
// 4/26/98 JTAYLOR - Function created / Header added.
// 4/27/98 JTAYLOR - Updated to acutally support file searching.
// 5/26/98 JTAYLOR - Changed process to not stop on first error.
// 5/28/98 JTAYLOR - Added a call to ForwardAllFiles.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::ProcessAllIncomingFiles()
{
    if( FALSE == m_bInitialized )
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

    if( FAILED( ForwardAllFiles( ) ) );
        {
        bResult = FALSE;
        }

    if( FALSE == bResult )
        return E_FAIL;

    return S_OK;
}



////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::ProcessIncomingFile
//
// Description      : Processes a file from the incoming directory and adds
//                    it to the Quarantine directory.  This also upgrades the
//                    header used to insert the file to the full Quarantine
//                    header.  This function deletes the source file.
//
// Return type      : HRESULT - S_OK on success
//
// Argument         : LPSTR lpszFileName -- Path and filename of the file to process
//                                          This should be a long ANSI filename.
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
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    char                szDestFileName[MAX_QUARANTINE_FILENAME_LEN] = "";
    DWORD               dwBytesWritten;
    DWORD               dwBytesRead;
    DWORD               dwFileType;
    DWORD               dwFileStatus;
    HANDLE              hSource;
    QFILE_HEADER_STRUCT stQuarantineHeader;
    QFILE_AP_HEADER_STRUCT stQuarantineHeaderSource;
    HRESULT             retVal;
    HRESULT             hrTemp;
    BOOL                bResult=FALSE;
    HANDLE              hFile=NULL;
    BOOL                bInvalidFile = FALSE;

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

    // Read in the original AP header.
    bResult = ReadFile(hSource, &stQuarantineHeaderSource, sizeof(stQuarantineHeaderSource), &dwBytesRead, NULL);

    if( FALSE == bResult )
        {
        retVal = E_FILE_READ_FAILED;
        goto  Exit_Function;
        }

    // If the header signature is wrong, then return S_OK.
    if( QUARANTINE_SIGNATURE != stQuarantineHeaderSource.dwSignature )
        {
        bInvalidFile = TRUE;
        retVal = S_OK;
        goto Exit_Function;
        }

    // Get the file status and type.
    dwFileStatus = stQuarantineHeaderSource.FileInfo.dwFileStatus;
    dwFileType = stQuarantineHeaderSource.FileInfo.dwFileType;

    // Create a new file in Quarantine and prepare the header structure
    hrTemp = SetupQuarantineFile( &stQuarantineHeader,
                                   lpszSourceFileName,
                                   m_szQuarantineFolder,
                                   dwFileType,
                                   dwFileStatus,
                                   &hFile,
                                   szDestFileName,
                                   MAX_QUARANTINE_FILENAME_LEN );
    if( FAILED( hrTemp ) )
        {
        retVal = hrTemp;
        goto  Exit_Function;
        }

    // copy the file status and type
    stQuarantineHeader.FileInfo.dwFileStatus = stQuarantineHeaderSource.FileInfo.dwFileStatus;
    stQuarantineHeader.FileInfo.dwFileType = stQuarantineHeaderSource.FileInfo.dwFileType;

    // Copy the original date/times for the file
    stQuarantineHeader.FileInfo.stDateQuarantined = stQuarantineHeaderSource.FileInfo.stDateQuarantined;

    // If we are on Windows 95, then translate the time that AP provided to a GMT time.
    if( !IsWinNT() )
        {

        // Get the information on the current timezone
        TIME_ZONE_INFORMATION tzInfo;
        GetTimeZoneInformation( &tzInfo );

        // Get the date the file was quarantined as a FILETIME
        FILETIME tDateQuarantined;
        SystemTimeToFileTime( &stQuarantineHeader.FileInfo.stDateQuarantined,
                              &tDateQuarantined );

        // Convert the filetime to a LONGLONG
        LONGLONG nFileTime;
        nFileTime = tDateQuarantined.dwHighDateTime;
        nFileTime <<= 32;
        nFileTime += tDateQuarantined.dwLowDateTime;

        // Adjust for timezone and daylight savings time
        LONGLONG nFileBias;
        // (Timezone difference in minutes + Daylight savings time in minutes ) * seconds in minute * 100 nanosecond intervals in a minute
        nFileBias = ( (LONGLONG)tzInfo.Bias + (LONGLONG)tzInfo.DaylightBias ) * (LONGLONG)60 * (LONGLONG)10000000;
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
        FileTimeToSystemTime( &tDateQuarantined,
                              &stQuarantineHeader.FileInfo.stDateQuarantined );
        }

    bResult = FileTimeToSystemTime( &stQuarantineHeaderSource.FileInfo.ftOriginalFileDateCreated,
                                    &stQuarantineHeader.FileInfo.stOriginalFileDateCreated );

    bResult = FileTimeToSystemTime( &stQuarantineHeaderSource.FileInfo.ftOriginalFileDateAccessed,
                                    &stQuarantineHeader.FileInfo.stOriginalFileDateAccessed );

    bResult = FileTimeToSystemTime( &stQuarantineHeaderSource.FileInfo.ftOriginalFileDateWritten,
                                    &stQuarantineHeader.FileInfo.stOriginalFileDateWritten );

    stQuarantineHeader.FileInfo.dwOriginalFileSize = stQuarantineHeaderSource.FileInfo.dwOriginalFileSize;
    strcpy( stQuarantineHeader.FileInfo.szOriginalAnsiFilename,
            stQuarantineHeaderSource.FileInfo.szOriginalAnsiFilename );

    // Write out the header and zero out the header region of the QuarantineFile.
    retVal = WriteHeaderBuffer( &stQuarantineHeader, hFile );
    if( FAILED( retVal ) )
        {
        goto  Exit_Function;
        }

    // Seek the hSource to the end of the header.
    if( 0xFFFFFFFF == SetFilePointer(hSource, stQuarantineHeaderSource.dwHeaderBytes, NULL, FILE_BEGIN ) )
        {
        retVal = E_FILE_READ_FAILED;
        goto  Exit_Function;
        }

    // Transfer the orignal file into the new Quarantine file.
    retVal = TransferFile( lpszSourceFileName, hSource, szDestFileName, hFile );
    if( FAILED( retVal ) )
        {
        goto  Exit_Function;
        }

    // Make sure the the entire file was copied.  The new file should be the
    // length of the header buffer + the length of the original file.
    // we have to rely on the header information for this bytes read.
    dwBytesRead = stQuarantineHeaderSource.FileInfo.dwOriginalFileSize;
    dwBytesWritten = GetFileSize( hFile, NULL );
    if( dwBytesRead + stQuarantineHeader.dwHeaderBytes != dwBytesWritten )
        {
        return E_FILE_WRITE_FAILED;
        }

    retVal = S_OK;

Exit_Function:

    // close files
    if (INVALID_HANDLE_VALUE != hSource)
        {
        CloseHandle( hSource );
        hSource = INVALID_HANDLE_VALUE;
        }

    if (INVALID_HANDLE_VALUE != hFile)
        {
        CloseHandle( hFile );
        hFile = INVALID_HANDLE_VALUE;
        }

    if( S_OK == retVal && !bInvalidFile )
        {
        // wipe the source file.
        FileWipe( lpszSourceFileName, 1 );

        // Scan the new Quarantine Item to update its virus information
        // We do not want an IQuarantineItem pointer back.
        retVal = ScanNewQuarantineItem( szDestFileName, NULL );
        }
    else
        {
        if( strlen( szDestFileName ) > 0 )
            {
            // wipe the destination file since it was not completed correctly
            FileWipe( szDestFileName, 1 );
            }
        }

    return (retVal);
}



////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::DeleteAllFiles
//
// Description      : Removes all files from the quarantine directory
//
// Return type      : HRESULT - S_OK on success
//
//
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::DeleteAllFiles()
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    return E_NOTIMPL;
}



////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::ForwardAllFiles
//
// Description      : Forwards all files on this system to the quarantine server
//
// Return type      : HRESULT - S_OK on success
//
//
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::ForwardAllFiles(FPFORWARDPROGRESS  lpfnForwardProgress)
{
    // make sure we're initialized
    if ( FALSE == m_bInitialized )
        {
        return E_UNINITIALIZED;
        }

    // make sure it makes sense to forward files
    if (FALSE == IsOkayToForwardFiles())
        {
        return  (E_UNABLE_TO_FORWARD_QUARANTINE_ITEM);
        }


    // allocate an array to temporarily store IQuarantineItem pointers
    auto    int                 numFiles;
    auto    IQuarantineItem**   arQItemPtrs;
    numFiles = NumFilesInQuarantine();
    if (numFiles < 0)
        {
        return  (E_UNABLE_TO_FORWARD_QUARANTINE_ITEM);
        }

    arQItemPtrs = new IQuarantineItem*[numFiles];
    if (NULL == arQItemPtrs)
        {
        return  (E_UNABLE_TO_FORWARD_QUARANTINE_ITEM);
        }

    // set up the enumeration
    auto    IEnumQuarantineItems*   pEnum;
    if( FAILED( this->Enum( &pEnum ) ) )
        {
        SYM_ASSERT(FALSE);
        return E_UNABLE_TO_CREATE_QUARANTINE_ITEM;
        }


    auto    BOOL                bContinueLoop = TRUE;
    auto    DWORD               dwBytesDelivered = 0;
    auto    DWORD               dwFileSize = 0;
    auto    DWORD               dwTotalBytes = 0;
    auto    int                 numForwardItems = 0;
    auto    IQuarantineItem*    pItem = NULL;
    auto    ULONG               uFetched;

    // loop and add valid forward items to the Quarantine item array
    while ((TRUE == bContinueLoop)  &&  (NOERROR == pEnum->Next(1, &pItem, &uFetched)))
        {
        if (TRUE == IsValidForwardFile(pItem))
            {
            // add the current item to the array
            arQItemPtrs[numForwardItems] = pItem;
            ++numForwardItems;

            // if the callback function is non-null, get the number of bytes we'll
            // send and update the total
            if (NULL != lpfnForwardProgress)
                {
                pItem->GetOriginalFilesize(&dwFileSize);
                dwTotalBytes += (dwFileSize + DEFAULT_HEADER_BUFFER_SIZE);
                }
            }
        }


    // now that we've got an array of items to forward, do so
    auto    int     nIndex;
    for (nIndex = 0; nIndex < numForwardItems; ++nIndex)
        {
        pItem = arQItemPtrs[nIndex];
        if (FAILED(pItem->ForwardToServer(m_szServerName, lpfnForwardProgress
                                        , &dwBytesDelivered, &dwTotalBytes)))
            {
            // if we fail to forward, break and try again next time
            break;
            }
        }

    // free memory allocated for the items array
    if (NULL != arQItemPtrs)
        {
        delete []  arQItemPtrs;
        }

    // Release enumeration object.
    pEnum->Release();
    return  (S_OK);

}  // end of "CQuarantineDLL::ForwardAllFiles"



////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::Enum
//
// Description      : Begin an enumeration of the quarantine folder
//
// Return type      : HRESULT - S_OK on success, or OLE defined error code
//
// Argument         : IEnumQuarantineItems** pEnumObj -pointer to enumeration
//                    object
//
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::Enum( IEnumQuarantineItems** pEnumObj )
{
    if( FALSE == m_bInitialized )
        return E_UNINITIALIZED;

    // Create new enumeration object
    CEnumQuarantineItems* p = new CEnumQuarantineItems;
    if( p == NULL )
        return E_OUTOFMEMORY;

    // Make sure we support the correct interface
    HRESULT hr = p->QueryInterface( IID_EnumQuarantineItems, (LPVOID*) pEnumObj );
    if( FAILED( hr ) )
        {
        delete p;
        return E_NOINTERFACE;
        }

    // Initialize the enumeration
    hr = p->Initialize( m_szQuarantineFolder );

    // If we failed, delete the object.
    if( FAILED( hr ) )
        {
        p->Release();
        }

    // Return result of initialization.
    return hr;
}


////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::APProtectProcess
//
// Description      : This function loads and unloads the AP dll and
//                    checks to see if the AP is running.  If AP is running
//                    it will enable/dissable AP for this process.
//
// Return type      : HRESULT - S_OK on success
//
// Argument         : BOOL bEnable -- TRUE - If ap running Enable AP and unload DLL.
//                                 -- FALSE- Load DLL If AP running Dissable AP.
//
////////////////////////////////////////////////////////////////////////////
// 3/30/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::APProtectProcess(BOOL bEnable)
{
    //HINSTANCE       m_hAPDll        = NULL;
    //BOOL            m_bDissabledAP  = FALSE;

    // Do not allow the user to call Dissable when we have a valid g_hDLL.
    TCHAR szAPDLL[256] = _T("navap32.dll");
    // function prototype typedef
    typedef BOOL (WINAPI *FunctionPointer) ();
    typedef BOOL (WINAPI *LoadedFunction) (PDWORD pArg);

    FunctionPointer    fpDissableAP = NULL;
    FunctionPointer    fpEnableAP = NULL;
    LoadedFunction     fpAPLoaded = NULL;

    BOOL        veryTemp;
    BOOL        bAPRunning = FALSE;
    DWORD       dwEmpty = 0;

    if ( !m_hAPDll )
        {
        m_hAPDll = LoadLibrary(szAPDLL);
        if( !m_hAPDll )
            {
            return E_UNABLE_TO_LOAD_NAVAP32;
            }
        }

    // Load the function pointers
    fpDissableAP = (FunctionPointer)GetProcAddress(m_hAPDll, szNAVAPUnprotectProcess);
    fpEnableAP =   (FunctionPointer)GetProcAddress(m_hAPDll, szNAVAPProtectProcess);
    fpAPLoaded =   (LoadedFunction) GetProcAddress(m_hAPDll, szNAVAPGetEnabledDisabled);

    // Check to make sure all of the functions are valid.  This should never fail.
    if( NULL == fpDissableAP ||
        NULL == fpEnableAP ||
        NULL == fpAPLoaded )
        {
        // bugbug -- There should probably be an error code just for this.
        return E_UNABLE_TO_LOAD_NAVAP32;
        }

    // Check to see if AP is running
    bAPRunning = fpAPLoaded(&dwEmpty);

    // If AP is not running then we should return successfully.
    if( FALSE == bAPRunning )
        {
        if( TRUE == bEnable )
            {
            FreeLibrary(m_hAPDll);
            m_hAPDll = NULL;
            }
        return S_OK;
        }
    else
        {
        // If we are enabling AP then enable it and unload the DLL.
        if( TRUE == bEnable )
            {
            veryTemp = fpEnableAP();
            if( FALSE == veryTemp )
                return E_UNABLE_TO_ENABLE_AP;
            m_bDissabledAP = FALSE;
            FreeLibrary(m_hAPDll);
            m_hAPDll = NULL;
            }
        else
            {
            veryTemp = fpDissableAP();
            if( FALSE == veryTemp )
                return E_UNABLE_TO_DISSABLE_AP;
            m_bDissabledAP = TRUE;
            }
        }

    return S_OK;
    }



////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::FetchQuarantineServerName
//
// Description      : This function hunts down the location of the Quarantine
//                    options file and reads the server name from it so the
//                    corresponding member of this object can be propertly
//                    initialized.
//
// Return type      : TRUE if we successfully initialize the "m_szServerName"
//                    member, FALSE if we cannot
//
// Argument         : nothing
//
////////////////////////////////////////////////////////////////////////////
// 4/09/98 SEDWARD - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL    CQuarantineDLL::FetchQuarantineServerName(void)
{
    auto    BOOL                bResult = TRUE;
    auto    HRESULT             hResult = S_OK;
    auto    IQuarantineOpts*    pQuarOpts = NULL;


    // initialize the quarantine options pointer
    if (FALSE == InitializeQuarOptsObject(&pQuarOpts))
        {
        bResult = FALSE;
        goto  Exit_Function;
        }

    // finally, get the name of the Quarantine server
    hResult = pQuarOpts->GetQuarantineServer(m_szServerName, MAX_PATH);
    if (FAILED(hResult))
        {
        bResult = FALSE;
        goto  Exit_Function;
        }


Exit_Function:
    if (NULL != pQuarOpts)
        {
        pQuarOpts->Release();
        pQuarOpts = NULL;
        }

    return (bResult);

}  // end of "CQuarantineDLL::FetchQuarantineServerName"



////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::FetchQuarantineOptionsPath
//
// Description      : This function looks in the registry to find the path
//                    where the Quarantine options file is located.
//
// Return type      : TRUE if the path is successfully fetched from the
//                    registry (and the input buffer will contain the
//                    path), FALSE if we fail (and the first character of the
//                    input buffer will contain NULL)
//
// Argument         : lpszPathBuf - a pointer to a buffer where the path will
//                    be stored
//
////////////////////////////////////////////////////////////////////////////
// 4/09/98 SEDWARD - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL    CQuarantineDLL::FetchQuarantineOptionsPath(LPSTR  lpzsPathBuf, DWORD  dwBufSize)
{
    auto    BOOL        bRetVal = TRUE;
    auto    DWORD       regValueType;
    auto    HKEY        hKey;
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
    return (bRetVal);


}  // end of "CQuarantineDLL::FetchQuarantineOptionsPath"



////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::InitializeQuarOptsObject
//
// Description      : This function initializes a Quarantine options object
//                    and sets the pointer argument accordingly
//
// Return type      : TRUE if all goes well, FALSE if not
//
// Argument         : a pointer to a pointer to a Quarantine options interface
//
// NOTE: it the responsibility of the caller to call "Release" on the
//       pointer argument!!!
//
////////////////////////////////////////////////////////////////////////////
// 5/09/98 SEDWARD - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL    CQuarantineDLL::InitializeQuarOptsObject(IQuarantineOpts**  pQuarOpts)
{
    auto    BOOL        bResult = TRUE;
    auto    char        szTempBuf[MAX_QUARANTINE_FILENAME_LEN];
    auto    HRESULT     hResult = S_OK;


    // get an interface pointer to the Quarantine options object
    hResult = CoCreateInstance(CLSID_QuarantineOpts, NULL, CLSCTX_INPROC_SERVER
                                                            , IID_QuarantineOpts
                                                            , (LPVOID*)pQuarOpts);
    if (FAILED(hResult))
        {
        *pQuarOpts = NULL;
        bResult = FALSE;
        goto  Exit_Function;
        }

    // find out where the Quarantine options file is installed
    if (FALSE == FetchQuarantineOptionsPath(szTempBuf, MAX_QUARANTINE_FILENAME_LEN))
        {
        bResult = FALSE;
        goto  Exit_Function;
        }

    // append the name of the Quarantine options file and initialize the options object
    STRCAT(szTempBuf, "\\");
    STRCAT(szTempBuf, QUARANTINE_OPTIONS_FILENAME);
    hResult = (*pQuarOpts)->Initialize(szTempBuf);
    if (FAILED(hResult))
        {
        bResult = FALSE;
        goto  Exit_Function;
        }


Exit_Function:

    return (bResult);

}  // end of "CQuarantineDLL::InitializeQuarOptsObject"



////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::IsOkayToForwardFiles
//
// Description      : This function determines if it is okay to go ahead and
//                    forward files from a client Quarantine to a server.
//
// Return type      : TRUE if it's okay to forward, FALSE if it is not
//
// Arguments        : none
//
////////////////////////////////////////////////////////////////////////////
// 5/09/98 SEDWARD - Function created / Header added.
// 7/02/98 JTAYLOR - Changed the function to load the server name.
////////////////////////////////////////////////////////////////////////////
BOOL    CQuarantineDLL::IsOkayToForwardFiles(void)
{
    auto    BOOL                bResult = TRUE;
    auto    BOOL                bTempBool;
    auto    HRESULT             hResult = S_OK;
    auto    IQuarantineOpts*    pQuarOpts = NULL;

    // initialize the quarantine options pointer
    if (FALSE == InitializeQuarOptsObject(&pQuarOpts))
        {
        bResult = FALSE;
        goto  Exit_Function;
        }

    // finally, see if forwarding is enabled
    hResult = pQuarOpts->GetForwardingEnabled(&bTempBool);
    if ((FAILED(hResult))  ||  (FALSE == bTempBool))
        {
        bResult = FALSE;
        goto  Exit_Function;
        }

    // If forwarding is enabled, read the server name.
    hResult = pQuarOpts->GetQuarantineServer(m_szServerName, MAX_PATH);
    if (FAILED(hResult))
        {
        bResult = FALSE;
        goto  Exit_Function;
        }

    // if the server specification is empty, don't bother trying to forward
    if ((NULL == *m_szServerName)  ||  (0 == strlen(m_szServerName)))
        {
        bResult = FALSE;
        goto  Exit_Function;
        }

Exit_Function:

    if (NULL != pQuarOpts)
        {
        pQuarOpts->Release();
        pQuarOpts = NULL;
        }

    return (bResult);

}  // end of "CQuarantineDLL::IsOkayToForwardFiles"

////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::SetupQuarantineFile
//
// Description      : This function creates a new file in Quarantine,
//                    Clears the header, and populates fields in it.
//
// Argument         : LPSTR lpszSourceFileName - file being added to Quarantine
//                       or processed from AP Quarantine file.  This is only
//                       used to get the extension of the source file.
// Argument         : LPSTR lpszQuarFileName - file being added to Quarantine
//                       or processed from AP Quarantine file.
// Argument         : DWORD dwFileType - The type of the file.
// Argument         : DWORD dwFileStatus - The status of the file
// Argument         : LPSTR lpszQuarFileName - The new filename in Quarantine
// Argument         : DWORD dwBuffSize - The size of the new filename buffer
// Argument         : LPHANDLE lpNewQuarFileHandle - The buffer for a handle to
//                       the new Quarantine file.
//
// Return type      : HRESULT S_OK on success.
//
////////////////////////////////////////////////////////////////////////////
// 5/15/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::SetupQuarantineFile( LPQFILE_HEADER_STRUCT lpQuarantineHeader,
                                                  LPSTR lpszSourceFileName,
                                                  LPSTR lpszQuarDirectory,
                                                  DWORD dwFileType,
                                                  DWORD dwFileStatus,
                                                  LPHANDLE lpNewQuarFileHandle,
                                                  LPSTR lpszQuarFileName,
                                                  DWORD dwBuffSize )
{
    char                szFileExtension[MAX_QUARANTINE_EXTENSION_LENGTH];
    HRESULT             hrRetVal = S_OK;
    HRESULT             hrTemp = S_OK;
    DWORD               dwLocalBufferSize;

    // Clear the header buffer
    memset(lpQuarantineHeader,0,sizeof(QFILE_HEADER_STRUCT));

    if (NOERR != NameReturnExtensionEx(lpszSourceFileName,
                                       szFileExtension,
                                       MAX_QUARANTINE_EXTENSION_LENGTH))
        {
        hrRetVal = E_FILE_FILENAME_MANIPULATION_FAILED;
        goto  Exit_Function;
        }

    lpQuarantineHeader->dwSignature = QUARANTINE_SIGNATURE;
    lpQuarantineHeader->dwVersion = QUARANTINE_VERSION;

    // Check to see if the header is greater then the default size.
    if( sizeof(QFILE_HEADER_STRUCT) > DEFAULT_HEADER_BUFFER_SIZE )
        {
        lpQuarantineHeader->dwHeaderBytes = sizeof(QFILE_HEADER_STRUCT);
        }
    else
        {
        lpQuarantineHeader->dwHeaderBytes = DEFAULT_HEADER_BUFFER_SIZE;
        }

    // Create a new file in Quarantine
    *lpNewQuarFileHandle = CreateUniqueFile(lpszQuarDirectory, szFileExtension, lpszQuarFileName);

    // If the file handle is invalid, return an error.
    if( INVALID_HANDLE_VALUE == *lpNewQuarFileHandle )
        {
        hrRetVal = E_FILE_CREATE_FAILED;
        goto  Exit_Function;
        }

    // save the name of the QuarantineFile
    if( NOERR != NameReturnFile(lpszQuarFileName,lpQuarantineHeader->FileInfo.szCurrentFilename))
        {
        hrRetVal = E_FILE_FILENAME_MANIPULATION_FAILED;
        goto  Exit_Function;
        }

    // bugbug -- Make sure the the file status is valid.
    // set the status of the new file
    lpQuarantineHeader->FileInfo.dwFileStatus = dwFileStatus;

    // bugbug -- Make sure the the file type is valid.
    // set the type of the new file
    lpQuarantineHeader->FileInfo.dwFileType = dwFileType;
    // If the file is compressed mark it so.
    if( TRUE == IsFileCompressed( szFileExtension ) )
        {
        lpQuarantineHeader->FileInfo.dwFileType |= QFILE_TYPE_COMPRESSED;
        }

    // Save off the user name
    dwLocalBufferSize = MAX_QUARANTINE_OWNER_NAME_LEN;
    GetUserName( lpQuarantineHeader->FileInfo.szOriginalOwnerName, &dwLocalBufferSize );

    // Save off the computer name
    dwLocalBufferSize = MAX_QUARANTINE_MACHINE_NAME_LEN;
    GetComputerName( lpQuarantineHeader->FileInfo.szOriginalMachineName, &dwLocalBufferSize );

    // get domain name here
    GetDomainName( lpQuarantineHeader->FileInfo.szOriginalMachineDomain, MAX_QUARANTINE_MACHINE_DOMAIN_LEN );

    // Fill in the GUID field of the header.
    hrTemp = CoCreateGuid( &( lpQuarantineHeader->FileInfo.uniqueID ) );
    if( FAILED( hrTemp ) )
        {
        hrRetVal = E_UNABLE_TO_CREATE_GUID;
        goto  Exit_Function;
        }

Exit_Function:
// bugbug -- error cleanup conditions.  Add comments above as well so the user knows
// what to expect.
    return hrRetVal;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::WriteHeaderBuffer
//
// Description      : This function writes out the header buffer for a
//                    Quarantine file.  It also clears out the rest of the
//                    header buffer, filling it with zeros.
//                    It assumes that the file was created with write permissions.
//
// Argument         : LPQFILE_HEADER_STRUCT lpQuarantineHeader - This is the
//                       header structure that should be placed at the top
//                       of the file.
// Argument         : LPHANDLE lpNewQuarFileHandle - The handle to the new
//                       Quarantine file.  This file should be open already.
//
// Return type      : HRESULT S_OK on success.
//
////////////////////////////////////////////////////////////////////////////
// 5/15/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::WriteHeaderBuffer( LPQFILE_HEADER_STRUCT lpQuarantineHeader,
                                                HANDLE NewQuarFileHandle )
{
    BOOL    bResult = FALSE;
    int     nBufferLeft = 0;
    int     nResult = 0;
    LPBYTE  pHeaderBuff = NULL;
    DWORD   dwBytesWritten = 0;

    // Write out the header struct
    bResult = WriteFile(NewQuarFileHandle, lpQuarantineHeader, SQFILE_HEADER_STRUCT, &dwBytesWritten, NULL);
    if( FALSE == bResult )
        {
        return E_FILE_WRITE_FAILED;
        }

    // Find out how much buffer room is left.
    nBufferLeft = lpQuarantineHeader->dwHeaderBytes - SQFILE_HEADER_STRUCT;

    // If there is more header buffer than the header requires....
    if( nBufferLeft > 0 )
        {
        pHeaderBuff = new BYTE[nBufferLeft];
        if( NULL == pHeaderBuff )
            {
            return E_OUTOFMEMORY;
            }

        // Zero out the buffer
        memset(pHeaderBuff,0,nBufferLeft);

        // write out the rest of the header buffer
        bResult = WriteFile( NewQuarFileHandle, pHeaderBuff, nBufferLeft, &dwBytesWritten, NULL);

        // Free the buffer
        if( NULL != pHeaderBuff )
            {
            delete []pHeaderBuff;
            pHeaderBuff = NULL;
            }

        // Make sure the file write succeeded.
        if ( ( FALSE == bResult ) ||
             ( (int)dwBytesWritten != nBufferLeft ) )
            {
            return E_FILE_WRITE_FAILED;
            }
        }

    return S_OK;
}


////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::ScanNewQuarantineItem
//
// Description      : This function should be called on new Quarantine items
//                    It will scan the item and update its virus information.
//                    It can also pass back a IQuarantineItem.
//
// Argument         : LPSTR lpszDestFileName - This is the name of the new file
//                       in Quarantine.
// Argument         : IQuarantineItem **pNewItem - This is used to return the
//                       IQuarantineItem * for the new item.  If it is NULL,
//                       the item is released.  If it is not NULL, it is up
//                       to the calling function to release the item.
//
// Return type      : HRESULT S_OK on success.
//
////////////////////////////////////////////////////////////////////////////
// 5/15/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::ScanNewQuarantineItem( LPSTR lpszDestFileName,
                                                    IQuarantineItem **pNewItem )
{
    HRESULT retVal;
    BOOL    bUndeletableVirFound = FALSE;

    // Now that the file is located in Quarantine, perform a scan on it
    // to update the virus information.
    IQuarantineItem* pItem;
    retVal = CoCreateInstance( CLSID_QuarantineItem,
                               NULL,
                               CLSCTX_INPROC_SERVER,
                               IID_QuarantineItem,
                               (LPVOID*)&pItem);
    if( FAILED( retVal ) )
        return retVal;

    // Scan the file to fill out the virus information
    retVal = pItem->Initialize(lpszDestFileName);
    if( FAILED( retVal ) )
    {
        pItem->Release();
        return retVal;
    }

    retVal = pItem->ScanItem(this);
    if( S_UNDELETABLE_VIRUS_FOUND == retVal )
        {
        bUndeletableVirFound = TRUE;
        }
    else if( FAILED( retVal ) &&
             ( E_AVAPI_VS_CANT_REPAIR != retVal ) ) // Make sure the error is not that it is
        {                                           // an unrepairable virus.
        pItem->Release();
        return retVal;
        }

    // Make sure that the file Status is saved.  It should also be saved by ScanItem
    retVal = pItem->SaveItem();
    if( FAILED( retVal ) )
        {
        pItem->Release();
        return retVal;
        }

    // Check to see if the person calling the function wanted an IQuarantineItem back
    if( NULL == pNewItem )
        {
        // If they did not, then release the item
        retVal = pItem->Release();
        if( FAILED( retVal ) )
            return retVal;
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



////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::NeedToForwardFiles
//
// Description      : This function permits a client to determine if any
//                    files need to be forwarded.
//
// Return type      : HRESULT - S_OK on success; if any files need to be
//                    forwarded, the BOOL argument is set to TRUE, else
//                    it is set to FALSE.
//
////////////////////////////////////////////////////////////////////////////
// 6/1/98 SEDWARD - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineDLL::NeedToForwardFiles(BOOL*  bNeedToForward)
{
    auto    IQuarantineOpts*    pQuarOpts = NULL;

    // make sure we're initialized
    if ( FALSE == m_bInitialized )
        {
        return E_UNINITIALIZED;
        }

    // see if it even makes sense to try to forward
    if (FALSE == IsOkayToForwardFiles())
        {
        return  (E_UNABLE_TO_FORWARD_QUARANTINE_ITEM);
        }

    // set up enumeration
    auto    IEnumQuarantineItems*   pEnum;
    if( FAILED( this->Enum( &pEnum ) ) )
        {
        SYM_ASSERT(FALSE);
        return E_UNABLE_TO_CREATE_QUARANTINE_ITEM;
        }

    // fetch items and check out their status
    auto    BOOL                bContinueLoop = TRUE;
    auto    IQuarantineItem*    pItem;
    auto    ULONG               uFetched;

    while ((TRUE == bContinueLoop)  &&  (NOERROR == pEnum->Next(1, &pItem, &uFetched)))
        {
        // if any 'non-backup' files are encountered, break out of the loop
        if (TRUE == IsValidForwardFile(pItem))
            {
            bContinueLoop = FALSE;
            break;
            }
        }

    // set the return value argument
    *bNeedToForward = !bContinueLoop;

    // Release enumeration object.
    pEnum->Release();
    return (S_OK);

}  // end of "CQuarantineDLL::NeedToForwardFiles"



////////////////////////////////////////////////////////////////////////////
// Function name    : CQuarantineDLL::IsValidForwardFile
//
// Description      : This function queries the Quarantine item argument for
//                    its status, and then determines if the file is valid
//                    to be forwarded.
//
// Return type      : TRUE if the Quarantine item should be forwarded, FALSE
//                    if not.
//
////////////////////////////////////////////////////////////////////////////
// 6/1/98 SEDWARD - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CQuarantineDLL::IsValidForwardFile(IQuarantineItem*  pItem)
{
    auto    BOOL        bForwardIsValid;
    auto    DWORD       dwStatus = 0;

    // get the status of the current item
    if( FAILED( pItem->GetFileStatus( &dwStatus ) ) )
        {
        bForwardIsValid = FALSE;
        }
    else
        {
        // if the Quarantine item is a 'non-backup' file, return TRUE
        switch( dwStatus )
            {
            case QFILE_STATUS_QUARANTINED:
            case QFILE_STATUS_SUBMITTED:
                bForwardIsValid = TRUE;
                break;

            default:
                bForwardIsValid = FALSE;
                break;
            }
        }

    return (bForwardIsValid);

}  // end of "CQuarantineDLL::IsValidForwardFile"
