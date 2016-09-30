////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/scandeliverdll.cpv   1.65   10 Jul 1998 15:24:28   jtaylor  $
////////////////////////////////////////////////////////////////////////////
//
// ScanDeliverDLL.cpp - Contains implementation for CScanDeliverDLL
//
//
////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/scandeliverdll.cpv  $
// 
//    Rev 1.65   10 Jul 1998 15:24:28   jtaylor
// Changed the email subject text.
// 
//    Rev 1.64   10 Jul 1998 12:07:20   SEDWARD
// Display the corporate page if the NAV corporate value is in the registry
// (fix for #118353, we used to look for the AdminTools corporate value
// instead).
//
//    Rev 1.63   08 Jul 1998 22:58:24   jtaylor
// Changed the date submitted to GMT.
//
//    Rev 1.62   01 Jul 1998 15:37:48   jtaylor
// Added a body to the email sent to SARC.
//
//    Rev 1.61   30 Jun 1998 21:46:16   jtaylor
// Updated the error reporting on DeliverFilesToSARC.  Started the message body in SendToSarc.
//
//    Rev 1.59   14 Jun 1998 18:27:10   SEDWARD
// Only add the corporate page to the S&D wizard if we find the "AdmTools"
// corporate features value in the registry.
//
//    Rev 1.58   12 Jun 1998 12:56:16   jtaylor
// Re-enabled the trialware check.
// Changed the rejection limit reason max length to 1000 characters
// Changed the trivial file filter code to get a rejection reason from SARC.
//
//    Rev 1.56   11 Jun 1998 02:02:48   jtaylor
// Added support for dissabling Scan and Deliver when a trial copy of NAV expires.
//
//    Rev 1.55   10 Jun 1998 23:21:46   jtaylor
// Added a function called TriggerAutodial which tries to establish and email connection to the SARC mailserver in order to trigger the IE4 AutoDial feature.
//
//    Rev 1.54   10 Jun 1998 21:50:42   jtaylor
// Added support for enhanced network connection detection and retry connection dialog.
//
//    Rev 1.53   08 Jun 1998 22:50:40   jtaylor
// Added support for the new package format for platform ids.
//
//    Rev 1.52   08 Jun 1998 21:26:46   jtaylor
// Changed the call to load the nav install path to use the global function.
//
//    Rev 1.51   08 Jun 1998 20:44:30   jtaylor
// Changed Resource dLL to be dynamically loaded.
//
//    Rev 1.50   05 Jun 1998 14:58:18   jtaylor
// Filled out more package fields.
//
//    Rev 1.49   03 Jun 1998 14:26:56   jtaylor
// Added resource initialization call.
//
//    Rev 1.48   29 May 1998 15:28:20   jtaylor
// Changed trivial file filter failure to allow submission of the file.
//
//    Rev 1.47   28 May 1998 18:28:48   jtaylor
// Changed two Strcats to a NameAppendFile.
//
//    Rev 1.46   26 May 1998 13:45:00   SEDWARD
// Added call to 'InitDbcsVars' function.
//
//    Rev 1.45   20 May 1998 23:23:32   jtaylor
// Added support for all files clean.
//
//    Rev 1.44   19 May 1998 19:12:00   jtaylor
// Fixed a problem with an infinite loop on error sending to SARC.
//
//    Rev 1.43   19 May 1998 16:02:42   SEDWARD
// Changed 'IsFileOfTheDayModeValid' so it returns a DWORD instead of a BOOL.
//
//    Rev 1.42   19 May 1998 14:40:26   jtaylor
// Fixed the smtp server, email address and socket server address to be dynamic.
//
//    Rev 1.41   15 May 1998 16:30:06   jtaylor
// Removed some commented code.
//
//    Rev 1.40   14 May 1998 16:19:00   jtaylor
// Added code to present the correct priority to the SARC socket server.
// Added code to read the socket server from the string table.
// Added code to hard code the socket server to Brian's computer.
// Fixed KEY_ALL_ACCESS errors.
//
//    Rev 1.39   01 May 1998 13:40:26   jtaylor
// Fixed a bug in two GetVersion calls.  They were failing to check the WinNT/95
// correctly.
//
//    Rev 1.38   30 Apr 1998 22:02:38   jtaylor
// In SendToSarc, improved cancel support, and file close on errors.
//
//    Rev 1.37   30 Apr 1998 20:21:58   jtaylor
// Added more error handling, and added progress dialog to SendToSarc.
//
//    Rev 1.36   01 May 1998 22:19:58   jtaylor
// Added progress dialog control.
//
//    Rev 1.35   27 Apr 1998 19:50:30   SEDWARD
// Add the corporate page to the property sheet only if running in corporate
// mode.
//
//    Rev 1.34   27 Apr 1998 00:47:50   jtaylor
// Added tracking to check and see if a file was sent to SARC.
//
//    Rev 1.33   27 Apr 1998 00:24:54   jtaylor
// Enhanced File of the Day support.
// Fixed double quotes in the resources.
//
//    Rev 1.32   26 Apr 1998 21:59:44   jtaylor
// Added an error code for maximum submission size.
//
//    Rev 1.31   26 Apr 1998 17:28:16   SEDWARD
// Moved SARC calls to ReviewPg.cpp.
//
//    Rev 1.30   26 Apr 1998 02:15:34   jtaylor
// Added support for maintaining the original file date/time when stripping content.
//
//    Rev 1.29   26 Apr 1998 01:06:08   jtaylor
// Added support for reading trivial file rejection reasons from the string table.
//
//    Rev 1.28   25 Apr 1998 18:38:24   jtaylor
// Added malicious code filter from SARC.
//
//    Rev 1.27   24 Apr 1998 23:26:28   jtaylor
// fixed a return value on SMTP transfer call.
//
//    Rev 1.26   24 Apr 1998 18:36:00   jtaylor
// Updated call to getstripfilecontent
//
//    Rev 1.25   24 Apr 1998 18:19:46   SEDWARD
// -- include FOTDPg.h and TIme.h
// -- removed the "clean files" property page from LaunchUI()
// -- added support for "file of the day" mode
//
//    Rev 1.24   24 Apr 1998 17:08:24   jtaylor
// Added Filesize limitation of 10MB for submission packet.
// Added code for using the SARC api to check this, but switched
// to checking the package file when the API failed.
//
//    Rev 1.23   24 Apr 1998 00:36:32   jtaylor
// Added CheckBackendStatus.  Made the SARC API functions public (so the UI can
// call them).
//
//    Rev 1.22   23 Apr 1998 17:55:54   jtaylor
// Cleaned up the temporary file functions a little more.
//
//    Rev 1.21   23 Apr 1998 17:46:58   jtaylor
// Created a new function for generating uninque files in Quarantine directory.
// Implemented calls to that function.
//
//    Rev 1.20   23 Apr 1998 16:22:28   jtaylor
// Added support for file stripping.
//
//    Rev 1.19   22 Apr 1998 14:03:06   JTaylor
// removed the long filenames from create package
//
//    Rev 1.18   21 Apr 1998 20:25:42   JTaylor
// Made the mime encoded file be a temporary file and added a delete for it.
//
//    Rev 1.17   20 Apr 1998 21:52:28   JTaylor
// Mark submitted items as submitted and save them.
//
//    Rev 1.16   20 Apr 1998 20:37:28   SEDWARD
// Restored return value for LaunchUI() (sorry Jacob!).
//
//    Rev 1.15   20 Apr 1998 18:07:40   SEDWARD
// Added include files for new wizard panels, added new wizard panel objects
// to the property sheet.
//
//    Rev 1.14   20 Apr 1998 10:01:54   JTaylor
// Improved error handling in CreatePackage
//
//    Rev 1.13   20 Apr 1998 01:06:12   JTaylor
// Fixed a bug where a file being processed may be left on the drive.
//
//    Rev 1.12   20 Apr 1998 00:22:34   JTaylor
// Added more error handling.  Made function calls more robust.
// The SendToSarc function still needs more help with the error conditions.
//
//    Rev 1.11   19 Apr 1998 22:14:06   JTaylor
// Added support for SMTP deliver to CScanDeliverDLL
// Added a new return type to LaunchUI, it returns TRUE if the user finished
//   the wizard panels (i.e. no cancel)
// Cleared a variable before the call to UnpackageItem.
//
//    Rev 1.10   18 Apr 1998 22:47:18   JTaylor
// Added conversions for all filenames to short filenames for packaging api
// Removed stub progress function.
// Added closeHandle call on filename generated by CreateUniqueFile.
//
//    Rev 1.9   18 Apr 1998 19:59:10   JTaylor
// Added packaging API call and function CreatePackage.
//
//    Rev 1.8   16 Apr 1998 23:50:12   JTaylor
// Added a string for files rejected by having no detecable virus.
//
//    Rev 1.7   16 Apr 1998 23:46:14   JTaylor
// removed a second declaratin of bResult.
//
//    Rev 1.6   16 Apr 1998 23:40:18   JTaylor
// Added greater support for filtering files and increased the data structures for tracking files.
//
//    Rev 1.5   15 Apr 1998 23:47:00   SEDWARD
// Added LaunchUI() and GetScanConfigOptionsPath().
//
//    Rev 1.4   14 Apr 1998 22:54:56   JTaylor
// Added implementation for maintaining strings and item pointer arrays for the items marked for submission.  (accepted/rejected lists).
//
//    Rev 1.3   14 Apr 1998 19:50:04   JTaylor
// Added a necessary initialize to the internal reference count of CScanDeliverDll.
//
//    Rev 1.2   14 Apr 1998 19:31:20   JTaylor
// Removed some unneccessary comments.
//
//    Rev 1.1   13 Apr 1998 15:52:38   SEDWARD
// Changed 'DeliverFileToSarc' to 'DeliverFilesToSarc'.
//
//    Rev 1.0   13 Apr 1998 15:15:42   JTaylor
// Initial revision.
//
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#define INITIIDS
#include "navtrust.h"
#include "ccSettingsInterface.h"
#include "NAVDefutilsLoader.h"

#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"

#include "ccStreamInterface.h"
#include "ccSymMemoryStreamImpl.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("Scandlvr"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

// Module lifetime management for trust cache
#include "ccSymModuleLifetimeMgrHelper.h"
cc::IModuleLifetimeMgrPtr g_ModuleManager;
IMPLEMENT_MODULEMANAGER(::g_ModuleManager);

#define INITGUID
#include "ScanDeliverDLL.h"
#include "ScanDlvr.h"
#include "ScanWiz.h"

// SARC Scan and Deliver APIs
#include "SDSendMe.h"
#include "SDStrip.h"
#include "SDSocket.h"
#include "SDFilter.h"

#include "Quaradd.h"
#include "global.h"
#include "smtpmail.h"
#include "AcceptPg.h"
#include "RejectPg.h"
#include "CorpPg.h"
#include "ReviewPg.h"
#include "WrapUpPg.h"
#include "WelcomPg.h"
#include "SendAddlInfoPg.h"
#include "User1Pg.h"
#include "User2Pg.h"
#include "User3Pg.h"
#include "FOTDPg.h"
#include "Time.h"
#include "AVRESBranding.h"

#include "QItemEZ.h"

// Include the Scan and Deliver resource files.
#include "sdrcinit.h"

// Progress dialog
#include "ProgDlg.h"

// NAV Versioning info
#include "StahlSoft.h"
#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"
#include "NAVVersion.h"

// QSPack fields
#include "qsfields.h"

// Returns true if running on a 64bit os
bool Is64BitOS(VOID);

// Global object count
extern LONG g_dwObjs;

// Global Configurations object
extern CScanDeliverConfiguration   g_ConfigInfo;

// The tag for SARC to know that at least one of the files
// in the package are from OEH and should be handled
// differently.
//
// DON'T TRANSLATE THIS TAG !!!!!!!!!!
//
#define OEH_TAG _T("FROM : OEH,1.0\n")

////////////////////////////////////////////////////////////////////////////
// Function name    : GetCurrentDefinitionsDate
//
// Description      : Retrieves the date of the current definitions for Quarantine
//                    This Puts the correct day of the defs in the SYSTEMTIME and
//                    Zeros out the time fields then puts the revision of the defs
//                    into the hours slot.
//
////////////////////////////////////////////////////////////////////////////
HRESULT GetCurrentDefinitionsDate( SYSTEMTIME *lptTime )
{
    if( lptTime == NULL )
    {
        CCTRACEE("GetCurrentDefinitionsDate() - Invalid pointer.");
        return E_FAIL;
    }

    DefUtilsLoader DefUtilsLoader;

    { // Scope the object
        IDefUtilsPtr pIDefUtils;
        if( SYM_FAILED(DefUtilsLoader.CreateObject(pIDefUtils.m_p)) )
        {
            CCTRACEE("GetCurrentDefinitionsDate() - Failed to create the defutils loader object");
            return E_FAIL;
        }

        // Initialize defutils
        if( pIDefUtils->InitWindowsApp( _T( QUARANTINE_APP_ID ) ) == false )
        {
            CCTRACEE("GetCurrentDefinitionsDate() - Failed to initialize defutils");
            return E_FAIL;
        }

        // Get current defs being used by quarantine
        DWORD dwTmpRev = 0;
        if( pIDefUtils->GetCurrentDefsDate(&lptTime->wYear, &lptTime->wMonth, &lptTime->wDay, &dwTmpRev) == false )
        {
            CCTRACEE("GetCurrentDefinitionsDate() - Failed to get the definitions date");
            return E_FAIL;
        }

        // Set the revision
        lptTime->wHour = (WORD)dwTmpRev;

        // Clear unused fields
        lptTime->wMinute = 0;
        lptTime->wSecond = 0;
        lptTime->wMilliseconds = 0;
        lptTime->wDayOfWeek = 0;
    } // End scope

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : UpdateProgress
//
// Description      : This function is used to update the progress control
//                    passed in as lpvProgress, to the nPercent mark.
//
////////////////////////////////////////////////////////////////////////////
// 4/11/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()
HRESULT CScanDeliverProgressImpl::UpdateProgress(DWORD_PTR dwpProgressCookie, int  nPercentageComplete)
{
    // This pointer always needs to be valid.
    ASSERT(dwpProgressCookie);

    // Advance progress dialog
    ((CProgressDlg *)dwpProgressCookie)->SetPos( nPercentageComplete );

    return !((CProgressDlg *)dwpProgressCookie)->CheckCancelButton() ? S_OK : E_FAIL;
};

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::CScanDeliverDLL
//
// Description      : Constructor
//
////////////////////////////////////////////////////////////////////////////
// 4/11/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CScanDeliverDLL::CScanDeliverDLL()
{
    m_dwRef =0;
    InterlockedIncrement( &g_dwObjs );
    m_nAcceptedFiles = 0;
    m_nRejectedFiles = 0;
    m_szPackageFilename[0] = NULL;
    m_bInFileOfTheDayMode = FALSE;
    m_bPackageSent = FALSE;

	CBrandingRes BrandRes;
	m_csProductName = BrandRes.ProductName();
}



////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::~CScanDeliverDLL
//
// Description      : Destructor
//
////////////////////////////////////////////////////////////////////////////
// 4/11/98 JTAYLOR - Function created / Header added.
// 4/19/98 JTAYLOR - Added code to release memory.
////////////////////////////////////////////////////////////////////////////
CScanDeliverDLL::~CScanDeliverDLL()
{
    InterlockedDecrement( &g_dwObjs );

    // Explicitly release submission data pointer before
    //  we unload the Dll that implements it.
    m_pPreSubmissionData.Release();

    // clear out temp files
    CleanupTemporaryFiles();
}

///////////////////////////////////////////////////////////////////
// IUnknown implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function name    : CScanDeliverDLL::QueryInterface
// Description      : This function will return a requested COM interface
// Return type      : STDMETHODIMP
// Argument         : REFIID riid - REFIID of interface requested
// Argument         : void** ppv - pointer to requested interface
//
////////////////////////////////////////////////////////////////////////////////
// 4/11/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::QueryInterface( REFIID riid, void** ppv )
{
    *ppv = NULL;

    if( IsEqualIID( riid, IID_IUnknown )||
        IsEqualIID( riid, IID_ScanDeliverDLL) )
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
// Function name    : CScanDeliverDLL::AddRef()
// Description      : Increments reference count for this object
// Return type      : ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 4/11/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CScanDeliverDLL::AddRef()
{
    return ++m_dwRef;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name    : CScanDeliverDLL::Release()
// Description      : Decrements reference count for this object
// Return type      : ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 4/11/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CScanDeliverDLL::Release()
{
    if( --m_dwRef )
        return m_dwRef;

    delete this;
    return 0;
}


////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::GetVersion()
//
// Description      : Returns the version number of this object
//
// Return type      : UINT
//
////////////////////////////////////////////////////////////////////////////
// 4/11/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_( UINT ) CScanDeliverDLL::GetVersion()
{
    return 1;
}


////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::DeliverFilesToSarc()
//
// Description      : This function starts the file delivery process
//
// Return type      :
//
////////////////////////////////////////////////////////////////////////////
// 4/11/98 JTAYLOR - Function created / Header added.
// 4/15/98 JTAYLOR - Added calls to SARC APIs and file gathering/filtering
// 4/20/98 JTAYLOR - Added more error handling
// 4/20/98 JTAYLOR - Fixed a bug where a file being processed may be left
//                   on the drive.
// 4/20/98 JTAYLOR - Added and Update status on items successfully sent to SARC
// 4/25/98 JTAYLOR - Added malicious code filter.
// 5/01/98 JTAYLOR - Fixed problem with GetVersion call.
// 5/29/98 JTAYLOR - Changed trivial file filter failure to allow submission.
// 6/04/98 JTAYLOR - Removed unneccesary iQuarantineDll creation.
// 6/08/98 JTAYLOR - Changed the resource DLL to be dynamically loaded.
// 6/08/98 JTAYLOR - Changed the NAV path to use the global function.
// 6/11/98 JTAYLOR - Added dissable for expired trialware.
// 6/12/98 JTAYLOR - Added rejection reason from malicious code filter
// 6/30/98 JTAYLOR - Added E_UNABLE_TO_INITIALIZE error code for Quarantine
//                      to display a message for.
//                 - Improved the error reporting logic
// 7/08/98 JTAYLOR - Changed the date submitted to GMT
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::DeliverFilesToSarc( IQuarantineItem **pQuarantineItemArray, int nNumberItems )
    {
    // since this module displays the wizard panels, we need to swap module states
    // in order to access our resources
    AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

    auto int     nIndex = 0;
    auto int     i = 0;
    auto int     nStartup = 0;
    
    CString      cszGeneratedFilename;
    CString      cszTempFolder;
    CString      cszNavFolder;
    CString      cszDefsFolder;
    CString      cszScanConfigPath;
    CString      szStatusText;
    CString      cszTempString;

    auto DWORD   dwErrorBits                                           = 0;
    auto CString szRejectionReason;
    auto int     nMaxRejectionReasonLength                             = 1000;
    auto IQuarantineItem *pIQuarItem                                   = NULL;
    auto HRESULT hr                                                    = E_FAIL;
    auto BOOL    bResult                                               = FALSE;
    auto HRESULT hrRetVal                                              = E_FAIL;
    auto ViralSubmissionFilter *pVirFilter                             = NULL;
    // WJORDAN 2-5-03: Removing dependencies on the scan and deliver filtering dll (sdflt32i.dll)
    //auto CMaliciousCodeFilter *pFileFilter                             = NULL;

    HINSTANCE hResources = NULL;

    m_bPackageSent = FALSE;

    // Get the installation directory
    bResult = GetMainNavDir( cszNavFolder.GetBuffer(MAX_QUARANTINE_FILENAME_LEN));
    cszNavFolder.ReleaseBuffer();

    // Make sure the get path function suceeded.
    if( FALSE == bResult || !cszNavFolder.GetLength())
    {
        return E_UNABLE_TO_INITIALIZE;
    }

    // dynamically load the DLL
    CString cszResourceDLL;
    DWORD dwSize = cszNavFolder.GetLength() + strlen(szDLLName) + 2;

    // Create the name of the resource DLL.
    PathCombine( cszResourceDLL.GetBuffer(dwSize), cszNavFolder, szDLLName );
    cszResourceDLL.ReleaseBuffer();

    // function prototype typedef
    typedef BOOL (WINAPI *InitFunctionPointer) ();

    InitFunctionPointer     fpInitResources = NULL;

    // Verify the signature
    if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(cszResourceDLL) )
        return E_UNABLE_TO_INITIALIZE;

    hResources = LoadLibraryEx(cszResourceDLL, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if( !hResources )
    {
        return E_UNABLE_TO_INITIALIZE;
    }

    // Load the function pointers
    fpInitResources = (InitFunctionPointer)GetProcAddress(hResources, szInitScanDeliverResources );

    // Check to make sure all of the functions are valid.  This should never fail.
    if( NULL == fpInitResources )
    {
        return E_UNABLE_TO_INITIALIZE;
    }

    // Make sure that the resource DLL is loaded
    fpInitResources( );

    // Make sure that the arguments are marginally valid
    if( ( 0 >= nNumberItems ) ||
        ( NULL == pQuarantineItemArray ) )
    {
        return E_INVALIDARG;
    }

    // Initialize some global DBCS information.
    InitDbcsVars();

    // Clear the accepted and rejected lists. These should be null.
    m_AcceptedFileInterfacePointers.RemoveAll();
    m_AcceptedFileNames.RemoveAll();
    m_RejectedFileInterfacePointers.RemoveAll();
    m_RejectedFileStrings.RemoveAll();

    // Create progress dialog
    CProgressDlg progress;
    progress.Create();
    progress.SetRange(0, nNumberItems );
    // Load the status text string
    szStatusText.LoadString(IDS_PROGRESS_ANALYZING);
    progress.SetStatus(szStatusText);

    // get the temporary Quarantine Directory for unpackaging
    bResult = GetPathFromRegKey( HKEY_LOCAL_MACHINE,
                                 REGKEY_QUARANTINE_TEMP_PATH,
                                 REGVAL_QUARANTINE_TEMP_PATH,
                                 cszTempFolder.GetBuffer(MAX_QUARANTINE_FILENAME_LEN),
                                 MAX_QUARANTINE_FILENAME_LEN);
    cszTempFolder.ReleaseBuffer();
    if( FALSE == bResult )
    {
        hrRetVal = E_QUARANTINE_DIRECTORY_INVALID;
        goto Exit_Function;
    }

    // get the definitions Directory
    bResult = GetPathFromRegKey( HKEY_LOCAL_MACHINE,
                                 REGKEY_NAV_DEFINITIONS_PATH,
                                 REGVAL_NAV_DEFINITIONS_PATH,
                                 cszDefsFolder.GetBuffer(MAX_QUARANTINE_FILENAME_LEN),
                                 MAX_QUARANTINE_FILENAME_LEN);
    cszDefsFolder.ReleaseBuffer();
    if( FALSE == bResult )
    {
        hrRetVal = E_QUARANTINE_DIRECTORY_INVALID;
        goto Exit_Function;
    }

    // get the Scan Config Options Directory
    bResult = GetPathFromRegKey( HKEY_LOCAL_MACHINE,
                                 REGKEY_QUARANTINE_PATH,
                                 REGVAL_QUARANTINE_OPTIONS_PATH_STR,
                                 cszScanConfigPath.GetBuffer(MAX_QUARANTINE_FILENAME_LEN),
                                 MAX_QUARANTINE_FILENAME_LEN);
    cszScanConfigPath.ReleaseBuffer();
    if( FALSE == bResult )
    {
        hrRetVal = E_QUARANTINE_DIRECTORY_INVALID;
        goto Exit_Function;
    }

    // Create the SARC filter APIs
    pVirFilter = ViralSubmissionFilterFactory::CreateInstance();
    if( NULL == pVirFilter )
    {
        hrRetVal = E_UNABLE_TO_CREATE_VIRUS_FILTER;
        goto Exit_Function;
    }

    // Start the virus filter
    nStartup = pVirFilter->StartUp(cszNavFolder, cszDefsFolder, FILTER_TYPE_SELF_DETERMINATION );
    if( INIT_STATUS_NO_ERROR != nStartup )
    {
        hrRetVal = E_UNABLE_TO_START_VIRUS_FILTER;
        goto Exit_Function;
    }

    // WJORDAN 2-5-03: Removing dependencies on the scan and deliver filtering dll (sdflt32i.dll)
    /* Create the SARC file filter APIs
    pFileFilter = CMaliciousCodeFilterFactory::CreateInstance();
    if( NULL == pFileFilter )
        {
        hrRetVal = E_UNABLE_TO_CREATE_FILE_MALICIOUSCODEFILTER;
        goto Exit_Function;
        }
    

    // Start the virus filter
    nStartup = pFileFilter->StartUp( cszNavFolder );
    if( INIT_STATUS_NO_ERROR != nStartup )
        {
        hrRetVal = E_UNABLE_TO_CREATE_FILE_MALICIOUSCODEFILTER;
        goto Exit_Function;
        }
    */

    // Add each item to the accepted or rejected list.
    for( i = 0; i < nNumberItems; i++ )
    {
        BOOL bWantSubmission = FALSE;

        // Clear the Generated Filename.  Having a non-empty value will confuse UnpackageItem
        cszGeneratedFilename.Empty();

        IQuarantineItem *pQuarantineItem = pQuarantineItemArray[i];
        hr = pQuarantineItem->UnpackageMainFile((LPSTR)(LPCSTR)cszTempFolder,
                                             NULL,
                                             cszGeneratedFilename.GetBuffer(MAX_QUARANTINE_FILENAME_LEN),
                                             MAX_QUARANTINE_FILENAME_LEN,
                                             FALSE,
                                             TRUE);
        cszGeneratedFilename.ReleaseBuffer();
        if( FAILED( hr ) )
        {
            hrRetVal = hr;
            goto Exit_Function;
        }

		DWORD dwVirID = NULL;
        hr = pQuarantineItem->GetVirusID(&dwVirID);
        if( FAILED( hr ) )
        {
            // non-fatal, this could be an item that has no VID,
            // such as an item manually placed into quarantine
        }

        DWORD dwFileType;
        hr = pQuarantineItem->GetFileType(&dwFileType);
        if( FAILED( hr ) )
        {
            hrRetVal = hr;
            goto Exit_Function;
        }

        // reject compressed files here.
        if( dwFileType & QFILE_TYPE_COMPRESSED )
        {
            szRejectionReason.Format( IDS_TRIVIAL_NO_COMPRESSED, m_csProductName );
            bWantSubmission = FALSE;
        }
        else
        {
            // We want WarChild or OEH files. They don't have N30 structures so we need
            // to intercept here.
            //
            if ( dwFileType & QFILE_TYPE_WARCHILD || dwFileType & QFILE_TYPE_OEH )
            {
                bWantSubmission = TRUE;
            }
            else
            {
                // The SARC api should only be called if there is a virus.
                // Find out if there is a non-zero N30 structure
                if(dwVirID != 0)
                {
                    // Hacking up a bogus N30 struct for a quicky fix here...
                    N30 cN30;
                    ZeroMemory(&cN30, sN30);
                    cN30.wVirID = (WORD)dwVirID;
                    CString cszVirusName;
                    QItemEZ::GetThreatName(pQuarantineItem, cszVirusName);
                    
                    _tcsncpy(cN30.lpVName, cszVirusName, N30_SIZE_VIRUSNAME - 1);

                    // Need to stick something in the wControl and wControl2 members...
                    cN30.wControl = 0;  // need to put something reflecting the original 
                    // intent of bitCTRL_NOTCLEAN and bitINFO_AGENT
                    //
                    cN30.wControl2 = 0; // need to put something reflecting the original 
                    // intent of N30_CTRL2_NO_GENERIC

                    // Find out if SARC would like to see the file.
                    int nResponse = pVirFilter->AllowSubmission(cszGeneratedFilename,
                        &cN30,
                        dwFileType & QFILE_TYPE_REPAIRABLE,
                        szRejectionReason.GetBuffer(nMaxRejectionReasonLength));
                    // Clean up the buffer for the rejection reason.
                    szRejectionReason.ReleaseBuffer();

                    if( SUBMIT_STATUS_ALLOW_SUBMISSION == nResponse )
                    {
                        bWantSubmission = TRUE;
                    }
                }
                else
                {
                    // Clear the rejection reason.
                    szRejectionReason.Empty();
                    
                    // load the default rejection string
                    szRejectionReason.LoadString( IDS_TRIVIAL_POSSIBLE_THREAT );

                    //
                    // WJORDAN 2-5-03: Removing dependencies on the scan and deliver filtering dll (sdflt32i.dll)
                    //

                    // We should now try to use the trivial file rejection fileter
                    // this will allow us to determine more information to display to the
                    // user about the rejected file.
                    //int nResponse = pFileFilter->Filter( szGeneratedFilename, szRejectionReason.GetBuffer(nMaxRejectionReasonLength) );
                    // Clean up the buffer for the rejection reason.
                    //szRejectionReason.ReleaseBuffer();

                    /* If an error occurs, then accept the files.
                    if( ( FILTER_FILE_NOT_FOUND_ERROR == nResponse ) ||
                    ( FILTER_MALLOC_ERROR == nResponse )         ||
                    ( FILTER_GENERAL_FILE_ERROR == nResponse ) )
                    {
                    // There was an error filtering a file.
                    bWantSubmission = TRUE;
                    }*/
                }
            }

        }

        if( TRUE == bWantSubmission )
        {
            // Create a string referencing the filename.
            cszTempString = cszGeneratedFilename;

            // Clear the Generated Filename.
            cszGeneratedFilename.Empty();

            // If the file is accepted, then add it to the accepted list
            // Also store the unpackaged filename for use later.
            nIndex = m_AcceptedFileInterfacePointers.Add(pQuarantineItemArray[i]);
            m_AcceptedFileNames.SetAtGrow(nIndex, cszTempString);
            m_nAcceptedFiles++;
            
            // Add the temp file to the delete list for later removal
            m_lstTemporaryFiles.push_back(cszTempString);
        }
        else
        {
            cszTempString = szRejectionReason;

            // If the file is rejected then add it to the rejected list.
            // Save the rejection reason for later, and delete the rejected file.
            nIndex = m_RejectedFileInterfacePointers.Add(pQuarantineItemArray[i]);
            m_RejectedFileStrings.SetAtGrow( nIndex, cszTempString );
            m_nRejectedFiles++;
            FileZeroAndDelete( cszGeneratedFilename, 1 );

            // Clear the Generated Filename.
            cszGeneratedFilename.Empty();
        }

        // Advance progress dialog
        progress.SetPos( i );

        if( progress.CheckCancelButton() )
            {
            hrRetVal = E_USER_CANCELLED;
            goto Exit_Function;
            }
        }

    // Finish creating the scan config options object.
    PathAppend( cszScanConfigPath.GetBuffer(cszScanConfigPath.GetLength() + _tcslen(SCAN_CONFIG_DAT_FILENAME) + 2), SCAN_CONFIG_DAT_FILENAME );
    cszScanConfigPath.ReleaseBuffer();

    bResult = g_ConfigInfo.Initialize((LPSTR)(LPCSTR)cszScanConfigPath, TRUE); // TRUE means create the DAT file if it does not exist
    if( FALSE == bResult )
    {
        hrRetVal = E_UNABLE_TO_CREATE_OPTIONS_OBJECT;
        goto Exit_Function;
    }

    bResult = g_ConfigInfo.ReadConfigFile(&dwErrorBits);
    if( FALSE == bResult )
    {
        hrRetVal = E_UNABLE_TO_READ_OPTIONS;
        goto Exit_Function;
    }

    // Remove progress window
    progress.DestroyWindow();

    // display the wizard panels
    LaunchUI();

    // If the wizard panels sent a package to SARC, then update the
    // IQuarantineItems.
    if( TRUE == m_bPackageSent )
    {
        // Get the current date/time
        SYSTEMTIME stDateSubmitted;
        GetSystemTime( &stDateSubmitted );

        // Since the send to SARC was successfull, mark them as sent.
        for( i = 0; i < m_nAcceptedFiles; i++ )
        {
            // Update the status of the file as sent to SARC.
            pIQuarItem = m_AcceptedFileInterfacePointers[i];

			// NAV 2006 LKurahashi: No longer have a Submitted view in QConsole.
			// Add submitted status bit instead of replacing existing status.
			DWORD dwFileType;
			hr = pIQuarItem->GetFileStatus(&dwFileType);
			if(FAILED(hr))
			{
				if(QItemEZ::THREAT_VIRAL == QItemEZ::IsViral(pIQuarItem))
					dwFileType = QFILE_STATUS_QUARANTINED;
				else
					dwFileType = QFILE_STATUS_THREAT_BACKUP;
			}
			dwFileType = dwFileType | QFILE_STATUS_SUBMITTED;
			hr = pIQuarItem->SetFileStatus(dwFileType);
			if(FAILED(hr))
			{
				hrRetVal = hr;
				goto Exit_Function;
			}

            // Update the time the files were sent to SARC
            hr = pIQuarItem->SetDateSubmittedToSARC( &stDateSubmitted );
            if( FAILED( hr ) )
            {
                hrRetVal = hr;
                goto Exit_Function;
            }

            hr = pIQuarItem->SaveItem();
            if( FAILED( hr ) )
            {
                hrRetVal = hr;
                goto Exit_Function;
            }
        }
    }

    hrRetVal = S_OK;

Exit_Function:

    if( S_OK != hrRetVal &&
        E_USER_CANCELLED != hrRetVal &&
        E_OUTOFMEMORY != hrRetVal )
    {
        CString szError;
        CString szErrorCaption;
        szErrorCaption.LoadString( IDS_CAPTION_STANDARD_MESGBOX );

        if( TRUE == m_bPackageSent )
        {
            szError.LoadString( IDS_ERROR_UPDATING_ITEMS );
        }
        else
        {
            szError.Format( IDS_ERROR_STARTING_SUBMISSION, m_csProductName );
        }

        MessageBox( NULL, szError, szErrorCaption, MB_OK | MB_ICONERROR );
    }

    // Remove progress window
    progress.DestroyWindow();

    // Clean up the virus filter -- note these have no return values to check
    if( NULL != pVirFilter )
    {
        pVirFilter->ShutDown();
        pVirFilter->Release();
    }

    //
    // WJORDAN 2-5-03: Removing dependencies on the scan and deliver filtering dll (sdflt32i.dll)
    //
    /* Clean up the file filter -- note these have no return values to check
    if( NULL != pFileFilter )
        {
        pFileFilter->ShutDown();
        pFileFilter->Release();
        }
    */

    // clear out temp files
    CleanupTemporaryFiles();

    // Clear the accepted and rejected lists.
    m_AcceptedFileInterfacePointers.RemoveAll();
    m_AcceptedFileNames.RemoveAll();
    m_RejectedFileInterfacePointers.RemoveAll();
    m_RejectedFileStrings.RemoveAll();

    FreeLibrary(hResources);


    return hrRetVal;
}


////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::GetNumberRejectedFiles()
//
// Description      : This function is used to retrieve the number of rejected
//                    files in this object.
//
// Return type      : int, this is the number of files rejected.
//
////////////////////////////////////////////////////////////////////////////
// 4/14/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
int CScanDeliverDLL::GetNumberRejectedFiles()
{
    return m_nRejectedFiles;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::GetNumberAcceptedFiles()
//
// Description      : This function is used to retrieve the number of accepted
//                    files in this object.
//
// Return type      : int, this is the number of files accepted.
//
////////////////////////////////////////////////////////////////////////////
// 4/14/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
int CScanDeliverDLL::GetNumberAcceptedFiles()
{
    return m_nAcceptedFiles;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::GetRejectionReason
//
// Description      : This function is used to retrieve the string containing
//                    the reason why files where rejected.
//
// Return type      : CString *, this is the reason.  This may be NULL.
//
////////////////////////////////////////////////////////////////////////////
// 4/14/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CString CScanDeliverDLL::GetRejectionReason(int nIndex)
{
    if( ( nIndex < 0 ) ||
        ( nIndex > m_nRejectedFiles ))
    {
        return _T("");
    }

    return m_RejectedFileStrings.GetAt(nIndex);
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::GetAcceptedQuarantineItem
//
// Description      : This function is used to retrieve the IQuarantineItem
//                    interface for the index.
//
// Return type      : IQuarantineItem *, The item, this may be NULL
//
////////////////////////////////////////////////////////////////////////////
// 4/14/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
IQuarantineItem * CScanDeliverDLL::GetAcceptedQuarantineItem(int nIndex)
{
    if( ( nIndex < 0 ) ||
        ( nIndex > m_nAcceptedFiles ))
        {
        return NULL;
        }

    return m_AcceptedFileInterfacePointers.GetAt(nIndex);
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::GetRejectedQuarantineItem
//
// Description      : This function is used to retrieve the IQuarantineItem
//                    interface for the index.
//
// Return type      : IQuarantineItem *, The item, this may be NULL
//
////////////////////////////////////////////////////////////////////////////
// 4/14/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
IQuarantineItem * CScanDeliverDLL::GetRejectedQuarantineItem(int nIndex)
{
    if( ( nIndex < 0 ) ||
        ( nIndex > m_nRejectedFiles ))
        {
        return NULL;
        }

    return m_RejectedFileInterfacePointers.GetAt(nIndex);
}

////////////////////////////////////////////////////////////////////////////
// Function name    : LaunchUI
//
// Description      : This function displays the series of wizard panels that
//                    walks the user through the submission process
//
// Return type      : TRUE -- if the user selected OK
//                    FALSE -- if the user hit cancel
//
////////////////////////////////////////////////////////////////////////////
// 4/15/98 SEDWAD - Function created / Header added.
// 4/19/98 JTAYLOR - Added return value
// 4/27/98 JTAYLOR - Added code to support switching to File of the Day mode.
////////////////////////////////////////////////////////////////////////////
BOOL    CScanDeliverDLL::LaunchUI(void)
{
    auto    CScanDeliverWizard*         pWizSheet;
    auto    CWelcomePropertyPage        wizWelcome;
    auto    CAcceptedFilesPropertyPage  wizAcceptedFiles(this);
    auto    CRejectedFilesPropertyPage  wizRejectedFiles(this);
    auto    CFileOfTheDayPropertyPage   wizFileOfTheDay(this);
    auto    CSendAddlInfoPropertyPage   wizSendAddlInfo;
    auto    CUserInfo1PropertyPage      wizUserInfo1;
    auto    CUserInfo2PropertyPage      wizUserInfo2;
    auto    CUserInfo3PropertyPage      wizUserInfo3;
    auto    CorpInfoPropertyPage        wizCorporateInfo;
    auto    CReviewPropertyPage         wizReview(this);
    auto    CWrapUpPropertyPage         wizWrapUp;
    auto    int                         nResult;

    // create an instance of the property sheet
    pWizSheet = new CScanDeliverWizard(IDS_SCAN_WIZARD_CAPTION);

    // add the property pages to the property sheet
    pWizSheet->AddPage(&wizWelcome);

    // see if it is appropriate to show the dialog for "File of the Day" mode
    if (FOTD_IS_VALID == IsFileOfTheDayModeValid())
        {
        if (TRUE == ConvertToFileOfTheDayMode())
            {
            pWizSheet->AddPage(&wizFileOfTheDay);
            }
        else
            {
            pWizSheet->AddPage(&wizRejectedFiles);
            }
        }
    else
        {
        if (GetNumberAcceptedFiles() > 0)
            {
            pWizSheet->AddPage(&wizAcceptedFiles);
            }

        if (GetNumberRejectedFiles() > 0)
            {
            pWizSheet->AddPage(&wizRejectedFiles);
            }
        }

    // finish adding property pages
    pWizSheet->AddPage(&wizSendAddlInfo);
    pWizSheet->AddPage(&wizUserInfo1);
    pWizSheet->AddPage(&wizUserInfo2);
    pWizSheet->AddPage(&wizUserInfo3);

    // add the corporate page only if we're running in "corporate mode"
    if (TRUE == g_ConfigInfo.IsCorporateMode())
        {
        pWizSheet->AddPage(&wizCorporateInfo);
        }

    pWizSheet->AddPage(&wizReview);
    pWizSheet->AddPage(&wizWrapUp);

    // place the sheet in "wizard" mode and display the wizard panels
    pWizSheet->SetWizardMode();
    nResult = pWizSheet->DoModal();

    // free up alloated memory
    delete  pWizSheet;

    // if the user hit OK then return TRUE
    if (ID_WIZFINISH == nResult)
        {
        return TRUE;
        }

    return FALSE;

}  // end of "CScanDeliverDLL::LaunchUI"

void MakeBackupForTesting(LPCTSTR szSubmission);

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::CreatePackage()
//
// Description      : This function fills out the package info and
//                    Adds all the files to the package, creates the
//                    .zip file.
//
// Return type      : TRUE for success
//                    FALSE otherwise
//
////////////////////////////////////////////////////////////////////////////
// 4/18/98 JTAYLOR - Function created / Header added.
// 4/18/98 JTAYLOR - Added close of file created with CreateUniqueFile.
//                   Added conversion of filenames to Short filenames.
// 4/20/98 JTAYLOR - Added more error handling
// 4/20/98 JTAYLOR - Improved error handling
// 4/22/98 JTAYLOR - Removed short filename creation
// 4/23/98 JTAYLOR - Added support for file stripping
// 4/23/98 JTAYLOR - Moved temp file creation to its own function
// 4/23/98 JTAYLOR - Fixed a bug involving redeclarations of bResult.
// 4/24/98 JTAYLOR - Added 10 MB filesize limitation.
// 4/26/98 JTAYLOR - Added code to preserve original filetimes when stripping
//                   file content.
// 4/26/98 JTAYLOR - Added specific error code for submission > max packet size.
// 5/01/98 JTAYLOR - Fixed problem with GetVersion call.
// 5/06/98 JTAYLOR - Added max buff size.
// 6/04/98 JTAYLOR - Added support for remaining package fields
// 6/08/98 JTAYLOR - Changed the NAV path to use the global function.
// 6/08/98 JTAYLOR - Added support for the platform field.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::CreatePackage( HWND hWnd )
{
    BOOL bResult = FALSE;
	ISNDSubmission::FIELD stField;
	DWORD   dwCharsCopied                                         = 0;
	DWORD   dwHighSize                                            = 0;
	DWORD   dwLowSize                                             = 0;
	HANDLE  hPackage                                              = INVALID_HANDLE_VALUE;
	HRESULT hrRetVal                                              = E_FAIL;
    HRESULT hr;
	int     i                                                     = 0;
	CString szStatusText;

    SYMRESULT sr = SYM_ERROR;
    
    ISNDSubmissionPtr pSubmission;
    ISNDSamplePtr pSample;
    ISNDDataBufferPtr pSubmissionData;
    ISNDProgressPtr pProgress = new CScanDeliverProgressImpl;


    CWnd *pWindow = CWnd::FromHandle( hWnd );
	if( NULL == pWindow )
	{
		return E_INVALIDARG;
	}

    // Create progress dialog
	CProgressDlg progress;
	progress.Create(pWindow);
	progress.SetRange(0, m_nAcceptedFiles );

	// Load the status text string
	szStatusText.LoadString(IDS_PROGRESS_CREATING_PACKAGE);
	progress.SetStatus(szStatusText);

	// Create a unique filename in the Quarantine TEMP directory.
	hrRetVal = CreateNewFileInQuarantineTemp( _T(".zip"), m_szPackageFilename, MAX_QUARANTINE_FILENAME_LEN );
	if( FAILED( hrRetVal ) )
	{
		hrRetVal = E_FILE_CREATE_FAILED;
		goto Exit_Function;
	}

    // Add to list of files to be cleaned up
    m_lstTemporaryFiles.push_back(m_szPackageFilename);

     sr = SNDSubmissionDLL_Loader::CreateObject (GETMODULEMGR(), &pSubmission ); 
    if(SYM_FAILED(sr))
        // couldn't create ISNDSubmission object
        goto Exit_Function;
    
    hrRetVal = pSubmission->Reset(m_szPackageFilename);
    if(FAILED(hrRetVal))
        goto Exit_Function;

    if(!AddGeneralInformationToSubmission(pSubmission))
        goto Exit_Function;

    if(!AddSamplesToSubmission(pSubmission))
        goto Exit_Function;

	progress.SetRange(0, 100 );

	// Load the status text string
	szStatusText.LoadString(IDS_PROGRESS_COMPRESSING);
	progress.SetStatus(szStatusText);

	// Reset the position to 0%
	progress.SetPos( 0 );

    // init the stream for 
    m_pPreSubmissionData.Release();
    m_pPreSubmissionData.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());

    // Zip up the package
	hr = pSubmission->Finish( pProgress,
		(DWORD_PTR)&progress,
		0,
		m_pPreSubmissionData,
		&stField);

	if( FAILED(hr) )
	{
		hrRetVal = E_UNABLE_TO_FINISH_PACKAGE;
		goto Exit_Function;
	}

	// Open the package file
	hPackage = CreateFile(m_szPackageFilename,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if( INVALID_HANDLE_VALUE == hPackage )
	{
		hrRetVal = E_FILE_CREATE_FAILED;
		goto Exit_Function;
	}

	// get the filesize
	dwLowSize = GetFileSize( hPackage, &dwHighSize );
	// If there are more that 10 MB of files, return failure
	if( ( dwHighSize > 0 ) ||
		( dwLowSize > 10 * 1024 * 1024 ) )
	{
		hrRetVal = E_PACKAGE_OVER_MAX_SUBMISSION_SIZE;
		goto Exit_Function;
	}

	// Close the file handle
	if( FALSE == CloseHandle(hPackage) )
	{
		hrRetVal = E_FILE_CLOSE_FAILED;
		goto Exit_Function;
	}
	else
	{
		hPackage = INVALID_HANDLE_VALUE;
	}

	/*
	// This is the code to use if SARC fixes GetTotalFileSizeCount.
	// Otherwise, we can use the code above to check the compressed package file size.
	// Now make sure that the submission size is < 10 MegaBytes
	bResult = m_pPackageSamples->GetTotalFileSizeCount( &dwFileSize, &dwNumberFiles );
	if( FALSE == bResult )
	{
	hrRetVal = E_UNABLE_TO_FINISH_PACKAGE;
	goto Exit_Function;
	}

	// If there are more that 10 MB of files, return failure
	if( dwFileSize > 10 * 1024 * 1024 )
	{
	hrRetVal = E_UNABLE_TO_FINISH_PACKAGE;
	goto Exit_Function;
	}
	*/
    
    MakeBackupForTesting(m_szPackageFilename);

	// Remove progress window
	progress.DestroyWindow();
    
	// If we have made it here, then we can safely return S_Ok
	return S_OK;

Exit_Function:

	// Remove progress window
	progress.DestroyWindow();

	// Close the file handle if it is open.
	if( INVALID_HANDLE_VALUE != hPackage )
	{
		CloseHandle(hPackage);
	}

	// If the package was created, release the memory it used.
    m_pPreSubmissionData.Release();

	// return
	return hrRetVal;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::SendToSarc()
//
// Description      : This function takes the package generated by
//                    CreatePackage and sends it to SARC.
//
//   This function is based on the SMTP delivery code from Env Newvir
//   proj sarccan.
//
// Return type      : TRUE for success
//                    FALSE otherwise
//
////////////////////////////////////////////////////////////////////////////
// 4/19/98 JTAYLOR - Function created / Header added.
// 4/20/98 JTAYLOR - Added more error handling
// 4/21/98 JTAYLOR - Made the mime encoded file be a temporary file.
//                   Added delete call for the temporary file.
// 4/23/98 JTAYLOR - Moved temp file creation to its own function
// 4/30/98 JTAYLOR - Added progress dialog and more error handling
// 4/30/98 JTAYLOR - Do not finish email if the user hits cancel.
// 5/19/98 JTAYLOR - Use the SMTP server and email address from the
//                   ScanDeliverConfig options.
// 5/19/98 JTAYLOR - Fixed an error in the error handling. Goto Exit_Function
//                   inside Exit_Function (infinite loop)  :(
// 7/01/98 JTAYLOR - Added text to the email body.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::SendToSarc( HWND hWnd )
{
    WSADATA             stWSADATA;
    int                 nErr;
    DWORD               dwError;
    SOCKET_ADDRESS_T    stSocketAddress;
    SMTP_CONNECTION_T   stSMTPConnection;
    BOOL                bResult;
    char                szBuf[513];
    DWORD               dwLength;
    DWORD               dwBytesSent;
    int                 nBytesToRead;
    TCHAR               szMailServer[MAX_QUARANTINE_FILENAME_LEN]   = "";
    TCHAR               szSender[MAX_QUARANTINE_FILENAME_LEN]       = "";
    TCHAR               szRecipient[MAX_QUARANTINE_FILENAME_LEN]    = "";
    char                szTempFolder[MAX_QUARANTINE_FILENAME_LEN]   = "";
    TCHAR               szMIMEFileName[MAX_QUARANTINE_FILENAME_LEN] = "";
    HANDLE              hPackage                                    = INVALID_HANDLE_VALUE;
    HRESULT             hrRetVal                                    = E_FAIL;
    CString             szStatusText;
    CString             szEmailText;
    CString             szBoundryString;
    CFile               fInput;
    CFile               fOutput;
    CProgressDlg        progress;
    CString             szHeader;

    CWnd *pWindow = CWnd::FromHandle( hWnd );
    if( NULL == pWindow )
        {
        hrRetVal = E_INVALIDARG;
        goto Exit_Function;
        }

    // Create progress dialog
    progress.Create(pWindow);
    progress.SetRange(0, 100 );
    // Load the status text string
    szStatusText.LoadString(IDS_PROGRESS_PREPARING);
    progress.SetStatus(szStatusText);

    // Load the string for the message body
    szEmailText.LoadString(IDS_EMAIL_BODY);

    // get the temporary Quarantine Directory for unpackaging
    bResult = GetPathFromRegKey( HKEY_LOCAL_MACHINE,
                                 REGKEY_QUARANTINE_TEMP_PATH,
                                 REGVAL_QUARANTINE_TEMP_PATH,
                                 szTempFolder,
                                 MAX_QUARANTINE_FILENAME_LEN);
    if( FALSE == bResult )
        {
        hrRetVal = E_QUARANTINE_DIRECTORY_INVALID;
        goto Exit_Function;
        }

    // Get the email address of the sender
    g_ConfigInfo.GetEmail( szSender, MAX_QUARANTINE_FILENAME_LEN );

    // Get the SMTP server to send to.
    g_ConfigInfo.GetSmtpServer( szMailServer, MAX_QUARANTINE_FILENAME_LEN );

    // Get the SARC email address to send to.
    g_ConfigInfo.GetSarcEmailAddress( szRecipient, MAX_QUARANTINE_FILENAME_LEN );

    // Start up winsock
    nErr = WSAStartup(0x0101,&stWSADATA);

    if (nErr != 0)
        {
        hrRetVal = E_FAIL;
        goto Exit_Function;
        }

    bResult = TRUE;

    // Start the SMTP session

    if (SocketAddressFill(&stSocketAddress,
                          szMailServer,
                          25,
                          &dwError) != SOCKET_STATUS_OK)
        bResult = FALSE;

    // Load the status text string
    szStatusText.LoadString(IDS_PROGRESS_INITIALIZING);
    progress.SetStatus(szStatusText);

    if (bResult == TRUE &&
        SMTPInitiateMailConnection(&stSMTPConnection,
                                   &stSocketAddress,
                                   szSender) != SMTP_STATUS_OK)
        bResult = FALSE;

    if (bResult == TRUE &&
        SMTPConnectionAddRecipient(&stSMTPConnection,
                                   szRecipient) != SMTP_STATUS_OK)
        bResult = FALSE;

    if (bResult == TRUE &&
        SMTPConnectionStartData(&stSMTPConnection) != SMTP_STATUS_OK)
        bResult = FALSE;

    // Send the subject
    if (bResult == TRUE &&
        SMTPConnectionSendData(&stSMTPConnection,
                               "Subject: Scan and Deliver package\r\n"
                               "MIME-Version: 1.0\r\n"
                               "Content-Type: multipart/mixed; boundary=\"") !=
        SMTP_STATUS_OK)
        bResult = FALSE;

    // Generate a unique boundry string for the message
    // seed the random number generator
    srand( (unsigned)time( NULL ) );

    // Make sure that a DWORD stays defined as 32 bits.
    SPRINTF( szBoundryString.GetBuffer( MAX_QUARANTINE_FILENAME_LEN ),
             _T("%.8X%.8X%.8X"),
             MAKELONG(rand(),rand()) & 0xFFFFFFFF,
             MAKELONG(rand(),rand()) & 0xFFFFFFFF,
             MAKELONG(rand(),rand()) & 0xFFFFFFFF );

    // Free the buffer
    szBoundryString.ReleaseBuffer();

    // Append 12 '-' to the head of the boundry string
    szBoundryString = "------------" + szBoundryString;

    szHeader.Empty();
    szHeader += szBoundryString;
    szHeader += "\"\r\n\r\n";
    szHeader += "This is a multi-part message in MIME format.\r\n\r\n--";
    szHeader += szBoundryString;
    szHeader += "\r\nContent-Type: text/plain; charset=us-ascii\r\n";
    szHeader += "Content-Transfer-Encoding: 7bit\r\n\r\n";
    szHeader += szEmailText;
    szHeader += "\r\n\r\n--";
    szHeader += szBoundryString;
    szHeader += "\r\nContent-Transfer-Encoding: base64\r\n";
    szHeader += "Content-Description: Zip compressed file\r\n";
    szHeader += "Content-Disposition: attachment; filename=\"SUBMIT.zip\"\r\n";
    szHeader += "Content-Type: application/zip; name=\"SUBMIT.ZIP\"\r\n";

    // Send the subject
    if (bResult == TRUE &&
        SMTPConnectionSendData(&stSMTPConnection,
                            szHeader.GetBuffer(0) ) !=
        SMTP_STATUS_OK)
        bResult = FALSE;

    szHeader.ReleaseBuffer();

    // Open a file to the package.
    if (fInput.Open(m_szPackageFilename,
                    CFile::modeRead |
                    CFile::shareExclusive,
                    NULL) == 0)
        {
        hrRetVal = E_FILE_CREATE_FAILED;
        goto Exit_Function;
        }

    // Create a unique filename in the Quarantine TEMP directory.
    hrRetVal = CreateNewFileInQuarantineTemp( _T(".zip"), szMIMEFileName, MAX_QUARANTINE_FILENAME_LEN );
    if( FAILED( hrRetVal ) )
        {
        hrRetVal = E_FILE_CREATE_FAILED;
        goto Exit_Function;
        }

    // Open a the mime encoded file.
    if (fOutput.Open(szMIMEFileName,
                     CFile::modeReadWrite  |
                     CFile::shareExclusive |
                     CFile::modeCreate,
                     NULL) == 0)
        {
        hrRetVal = E_FILE_CREATE_FAILED;
        goto Exit_Function;
        }

    // Mime encode the input file.
    MIMEEncodeZip(&fInput, &fOutput);

    // Load the status text string
    szStatusText.LoadString(IDS_PROGRESS_TRANSMITTING);
    progress.SetStatus(szStatusText);

    if (bResult == TRUE)
    {
        TRY
        {
            // Get the attachment size

            dwLength = (DWORD)fOutput.GetLength();

            sprintf(szBuf,"Content-Length: %lu\r\n\r\n",dwLength);

            if (SMTPConnectionSendData(&stSMTPConnection,
                                       szBuf) != SMTP_STATUS_OK)
                bResult = FALSE;

            fOutput.SeekToBegin();

            nBytesToRead = 512;
            dwBytesSent = 0;
            while (dwBytesSent < dwLength)
            {
                if ((dwLength - dwBytesSent) < (DWORD)nBytesToRead)
                    nBytesToRead = (int)(dwLength - dwBytesSent);

                if (fOutput.Read(szBuf,nBytesToRead) != (DWORD)nBytesToRead)
                {
                    bResult = FALSE;
                    break;
                }

                szBuf[nBytesToRead] = 0;

                if (SMTPConnectionSendData(&stSMTPConnection,
                                           szBuf) != SMTP_STATUS_OK)
                {
                    bResult = FALSE;
                    break;
                }

                dwBytesSent += nBytesToRead;

                // Advance progress dialog
                progress.SetPos( dwBytesSent * 100 / dwLength );

                // If the user has hit cancel, then exit.
                if( progress.CheckCancelButton() )
                    {
                    hrRetVal = E_USER_CANCELLED;
                    bResult = FALSE;  // do not finish the email.
                    goto Exit_Function;
                    }
                }
        }
        CATCH(CFileException, e)
        {
            bResult = FALSE;
        }
        END_CATCH

        // terminate the attachment section.
        szHeader.Empty();
        szHeader += "--";
        szHeader += szBoundryString;
        szHeader += "--";

        // Send the subject
        if (bResult == TRUE &&
            SMTPConnectionSendData(&stSMTPConnection,
                                szHeader.GetBuffer(0) ) !=
            SMTP_STATUS_OK)
            bResult = FALSE;

        szHeader.ReleaseBuffer();
        }

    hrRetVal = S_OK;

Exit_Function:

    if (bResult == TRUE &&
        SMTPFinishMailConnection(&stSMTPConnection) != SMTP_STATUS_OK)
        bResult = FALSE;

    WSACleanup();

    // If the files are open, then close them.
    if( _tcslen(fInput.GetFileName()) != 0 )
        {
        fInput.Close();
        }

    if( _tcslen(fOutput.GetFileName()) != 0 )
        {
        fOutput.Close();
        }

    // remove the temporary mime file.
    if( strlen( szMIMEFileName ) > 0 )
        {
        DeleteFile( szMIMEFileName );
        }

    // Destroy the progress dialog
    progress.DestroyWindow();

    if( FALSE == bResult )
        {
        hrRetVal = E_FAIL;
        }

    // If the package was successfully sent, mark the package as sent.
    // This will be used to determine if we should update the status on the
    // files that were selected for submission or not.
    if( SUCCEEDED( hrRetVal ) )
        {
        m_bPackageSent = TRUE;
        }

    return hrRetVal;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::TriggerAutoDial()
//
// Description      : This function attempts to create a socket connection
//                    to trigger AutoDial in IE4.  This will only work once
//                    per process.
//
//   This function is based on the SMTP delivery code from Env Newvir
//   proj sarccan.
//
// Return type      : S_OK      for success
//                    E_FAIL    otherwise
//
////////////////////////////////////////////////////////////////////////////
// 6/10/98 JTAYLOR - Initial revision.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::TriggerAutoDial( )
{
    WSADATA             stWSADATA;
    int                 nErr;
    DWORD               dwError;
    SOCKET_ADDRESS_T    stSocketAddress;
    SMTP_CONNECTION_T   stSMTPConnection;
    BOOL                bResult;
    TCHAR               szMailServer[MAX_QUARANTINE_FILENAME_LEN]   = "";
    TCHAR               szSender[MAX_QUARANTINE_FILENAME_LEN]       = "";
    TCHAR               szRecipient[MAX_QUARANTINE_FILENAME_LEN]    = "";
    HRESULT             hrRetVal                                    = E_FAIL;

    // Get the email address of the sender
    g_ConfigInfo.GetEmail( szSender, MAX_QUARANTINE_FILENAME_LEN );

    // Get the SMTP server to send to.
    g_ConfigInfo.GetSmtpServer( szMailServer, MAX_QUARANTINE_FILENAME_LEN );

    // Get the SARC email address to send to.
    g_ConfigInfo.GetSarcEmailAddress( szRecipient, MAX_QUARANTINE_FILENAME_LEN );

    // Start up winsock
    nErr = WSAStartup(0x0101,&stWSADATA);
    if (nErr != 0)
        {
        hrRetVal = E_FAIL;
        goto Exit_Function;
        }

    bResult = TRUE;

    // Start the SMTP session
    if (SocketAddressFill(&stSocketAddress,
                          szMailServer,
                          25,
                          &dwError) != SOCKET_STATUS_OK)
        bResult = FALSE;

    if (bResult == TRUE &&
        SMTPInitiateMailConnection(&stSMTPConnection,
                                   &stSocketAddress,
                                   szSender) != SMTP_STATUS_OK)
        bResult = FALSE;

    if( TRUE == bResult )
        {
        hrRetVal = S_OK;
        }
    else
        {
        hrRetVal = E_FAIL;
        }

Exit_Function:

    if (bResult == TRUE &&
        SMTPFinishMailConnection(&stSMTPConnection) != SMTP_STATUS_OK)
        bResult = FALSE;

    WSACleanup();

    return hrRetVal;
}

//*************************************************************************
//
// Function:
//  void CScanDeliverDLL::MIMEEncode64TriToQuad()
//
// Parameters:
//  lpbyTri: IN=Array of 3, 8-bit bytes to be encoded
//  lpszQuad: OUT=Array of 4, valid BASE64 characters to be outputted
//
// Description:
//  This function encodes 3 8-bit bytes into 4 valid BASE64 bytes.
//
//   This function is taken directly from Env Newvir proj sarccan.
//
// Returns:
//  Nothing
//
//************************************************************************

void CScanDeliverDLL::MIMEEncode64TriToQuad
(
    LPBYTE              lpbyTri,
    LPSTR               lpszQuad
)
{
    BYTE        byQuad[4];
    int         i;

    byQuad[0] = lpbyTri[0] >> 2;
    byQuad[1] = ((lpbyTri[0] & 0x3) << 4) | (lpbyTri[1] >> 4);
    byQuad[2] = ((lpbyTri[1] & 0xf) << 2) | (lpbyTri[2] >> 6);
    byQuad[3] = lpbyTri[2] & 0x3F;

    for (i=0;i<4;i++)
    {
        if (byQuad[i] <= 25)
            lpszQuad[i] = byQuad[i] + 'A';
        else if (byQuad[i] >= 26 && byQuad[i] <= 51)
            lpszQuad[i] = byQuad[i] + 'a' - 26;
        else if (byQuad[i] >= 52 && byQuad[i] <= 61)
            lpszQuad[i] = byQuad[i] + '0' - 52;
        else if (byQuad[i] == 62)
            lpszQuad[i] = '+';
        else if (byQuad[i] == 63)
            lpszQuad[i] = '/';
    }
}


//*************************************************************************
//
// Function:
//  BOOL CScanDeliverDLL::MIMEEncodeZip(void)
//
// Parameters:
//  None
//
// Description:
//  This function encodes the specified input stream as a BASE64 attachment
//  and writes the output to the current write pointer in the lpstOutput
//  stream.  The calling function should make sure to seek to the proper
//  location in the output stream before invoking this function.
//
//   This function is taken directly from Env Newvir proj sarccan.
//
// Returns:
//  TRUE        On successful encoding
//  FALSE       If an error occurs during encoding
//
//*************************************************************************

BOOL CScanDeliverDLL::MIMEEncodeZip(CFile *pInput, CFile *pOutput)
{
#define BASE64_LINE_LEN 72

    char                szLine[BASE64_LINE_LEN+1];
    BYTE                byTri[3];
    BYTE                byChar;
    int                 nCount, nStringIndex;
    DWORD               dwZipLen;
    DWORD               dwCount;
    BYTE                abyBuf[512];
    int                 nBufIndex;

    TRY
    {
        // Seek to start of input file

        pInput->SeekToBegin();

        // Seek to start of output file

        pOutput->SetLength(0);
        pOutput->SeekToBegin();

        nCount = nStringIndex = 0;
        byTri[0] = byTri[1] = byTri[2] = 0;

        nBufIndex = sizeof(abyBuf);
        dwZipLen = (DWORD)pInput->GetLength();
        for (dwCount=0;dwCount<dwZipLen;dwCount++)
        {
            if (nBufIndex == sizeof(abyBuf))
            {
                // Refill the buffer

                if (dwZipLen - dwCount < sizeof(abyBuf))
                {
                    if (pInput->Read(abyBuf,dwZipLen-dwCount) != dwZipLen - dwCount)
                        return(FALSE);
                }
                else
                {
                    if (pInput->Read(abyBuf,sizeof(abyBuf)) != sizeof(abyBuf))
                        return(FALSE);
                }

                nBufIndex = 0;
            }

            byChar = abyBuf[nBufIndex++];
            //if (pInput->Read(&byChar,1) != 1)
            //  return(FALSE);

            // create sets of 3 characters

            byTri[nCount++] = byChar/* ^ 0xCD*/;

            if (nCount == 3)
            {
                // encode the 3 into 4 valid BASE64 characters and add them
                // to our output line

                MIMEEncode64TriToQuad(byTri,szLine+nStringIndex);

                nCount = 0;
                nStringIndex += 4;
                byTri[0] = byTri[1] = byTri[2] = 0;

                if (nStringIndex == BASE64_LINE_LEN)
                {
                    // Once we have reached the proper line length, output the
                    // BASE64 line to the output file.

                    szLine[nStringIndex++] = '\r';
                    szLine[nStringIndex++] = '\n';
                    pOutput->Write(szLine,nStringIndex);

                    // time to start on the next line

                    nCount = nStringIndex = 0;

                    // Update the progress

/*                    if (m_lpfnProgressCB != NULL &&
                        m_lpfnProgressCB(m_lpvProgressCookie,
                                         eSARCSubmitStageMIME,
                                         dwCount * 100 / dwZipLen,
                                         NULL) == FALSE)
                        return(FALSE);
*/
                }
            }
        }

        // see if we have a partially full byTri (< 3 bytes).  add it to the
        // current line.

        if (nCount != 0)
        {
            MIMEEncode64TriToQuad(byTri,szLine+nStringIndex);
            if (nCount == 1)
                nStringIndex += 2;
            else if (nCount == 2)
                nStringIndex += 3;
        }

        // if there is a non-empty last line, write it out

        if (nStringIndex != 0)
        {
            while (nStringIndex % 4 != 0)
                szLine[nStringIndex++] = '=';
            szLine[nStringIndex++] = '\r';
            szLine[nStringIndex++] = '\n';
            pOutput->Write(szLine,nStringIndex);
        }
    }
    CATCH(CFileException, e)
    {
        return(FALSE);
    }
    END_CATCH

    return(TRUE);
}


////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::CreateNewFileInQuarantineTemp
//
// Description      : This function takes an extension and creates a new
//                    file in the Quarantine temp path.  The new file
//                    is then closed and the filename is returned.
//
// Arguments        : lpszFilename    -- This filename will be used to retrieve the
//                                       extension of the new file.
//                    lpszNewFilename -- [out]This is the name of the new filename
//                    uBufferSize     -- This is the size in characters of the output
//                                       buffer.
// Return type      : S_OK for success.
//
////////////////////////////////////////////////////////////////////////////
// 4/19/98 JTAYLOR - Function created / Header added.
// 4/20/98 JTAYLOR - Added more error handling
// 4/21/98 JTAYLOR - Made the mime encoded file be a temporary file.
//                   Added delete call for the temporary file.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::CreateNewFileInQuarantineTemp( LPSTR lpszFilename, LPSTR lpszNewFilename, UINT uBufferSize )
{
    BOOL    bResult                                               = FALSE;
    char    szTempFolder[MAX_QUARANTINE_FILENAME_LEN]             = "";
    char    szTempFile[MAX_QUARANTINE_FILENAME_LEN]               = "";
    char    szTempExtension[MAX_QUARANTINE_EXTENSION_LENGTH]      = "";
    HANDLE  hPackage                                              = INVALID_HANDLE_VALUE;


    // get the temporary Quarantine Directory for unpackaging
    bResult = GetPathFromRegKey( HKEY_LOCAL_MACHINE,
                                 REGKEY_QUARANTINE_TEMP_PATH,
                                 REGVAL_QUARANTINE_TEMP_PATH,
                                 szTempFolder,
                                 MAX_QUARANTINE_FILENAME_LEN);
    if( FALSE == bResult )
        {
        return E_QUARANTINE_DIRECTORY_INVALID;
        }

    // get the extension of the file to strip
    ReturnExtension( lpszFilename, szTempExtension, MAX_QUARANTINE_EXTENSION_LENGTH );

    // Create a unique filename in the Quarantine TEMP directory.
    hPackage = CreateUniqueFile( szTempFolder, szTempExtension, szTempFile);
    if( INVALID_HANDLE_VALUE == hPackage )
        {
        return E_FILE_CREATE_FAILED;
        }
    else
        {
        // Make sure the file will fit in the input buffer.
        if( strlen( szTempFile ) > uBufferSize )
            {
            CloseHandle(hPackage);
            FileZeroAndDelete( szTempFile, 1 );
            return E_UNABLE_TO_MANIPULATE_STRING;
            }

        // Copy the filename to the argument
        strcpy( lpszNewFilename, szTempFile );

        // Close the file since the caller expects it to be closed.
        if( FALSE == CloseHandle(hPackage) )
            {
            return E_FILE_CLOSE_FAILED;
            }
        else
            {
            hPackage = INVALID_HANDLE_VALUE;
            }
        }

    return S_OK;

}


////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::CheckBackendStatus
//
// Description      : This function contacts the SARC socket server
//                    and determines the status of the SMTP server.
//
// Return type      : S_BACKEND_SEND_PACKAGE for send package.
//                    E_UNABLE_TO_GET_BACKEND_STATUS if error or undefined return
//                    E_BACKEND_UNABLE_TO_CONNECT_TO_INTERNET if unable to get to sarcscan or www.symantec.com
//                    S_BACKEND_BUSY if the server is busy
//                    S_BACKEND_FALSE_POSITIVE if the package is all false positives
//                    S_BACKEND_ALL_CLEAN if the package contains only files that do not have viruses.
////////////////////////////////////////////////////////////////////////////
// 4/24/98 JTAYLOR - Function created / Header added.
// 5/06/98 JTAYLOR - Added real socket server address.  Added correct
//                   priority.
// 5/20/98 JTAYLOR - Added support for backend all clean.
// 5/20/98 JTAYLOR - Added support for E_UNABLE_TO_CONNECT_TO_INTERNET
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::CheckBackendStatus( )
{
    int nResult = 0;
    HRESULT hrRetVal = E_FAIL;
    CString szSocketServerAddress;
    int nPriority = CUSTOMER_PRIORITY;
    
    // Allocate the new class
    SocketClient *pSocket = SocketClientFactory::CreateInstance();
    if( NULL == pSocket )
    {
        return E_OUTOFMEMORY;
    }

    // Copy the submission data from the stream to a buffer we
    // can pass into the network calls
    StahlSoft::CSmartDataPtr<BYTE> pSubmissionData;
    if(m_pPreSubmissionData)
    {
        ULONGLONG qdwSize = NULL;
        if(m_pPreSubmissionData->GetSize(qdwSize) && qdwSize)
        {
            pSubmissionData = new BYTE[qdwSize];
            ZeroMemory(pSubmissionData, qdwSize);
            m_pPreSubmissionData->SetPosition(0);
            m_pPreSubmissionData->Read(pSubmissionData, qdwSize);
        }
    }
    
    if(!pSubmissionData)
        return E_UNABLE_TO_GET_BACKEND_STATUS;

    // Get the socket server address
    g_ConfigInfo.GetSarcSocketServer( szSocketServerAddress.GetBuffer( MAX_QUARANTINE_FILENAME_LEN ),
                                      MAX_QUARANTINE_FILENAME_LEN );
    szSocketServerAddress.ReleaseBuffer( );

    
    // Set the priority to corporate if this is true
    if( g_ConfigInfo.IsCorporateMode( ) )
        {
        nPriority = CORPORATE_PRIORITY;
        }
    
    // Check the status of the back end.
    nResult = pSocket->GetBackEndStatus( szSocketServerAddress.GetBuffer( MAX_QUARANTINE_FILENAME_LEN ),
                                         nPriority,
                                         pSubmissionData );
    szSocketServerAddress.ReleaseBuffer( );

    switch ( nResult )
        {
        case BACK_END_SEND_PACKAGE:
            hrRetVal = S_BACKEND_SEND_PACKAGE;
            break;
        case BACK_END_DATA_ERROR:
            hrRetVal = E_UNABLE_TO_GET_BACKEND_STATUS;
            break;
        case BACK_END_CONNECTION_ERROR:
            {
            // We were unable to connect to the sarc socket server, try
            // to connect to www.symantec.com to verify internet connection.
            szSocketServerAddress.LoadString( IDS_SYMANTEC_MAIN_WEB_SERVER );
            nResult = pSocket->GetBackEndStatus( szSocketServerAddress.GetBuffer( MAX_QUARANTINE_FILENAME_LEN ),
                                                 nPriority,
                                                 pSubmissionData );
            szSocketServerAddress.ReleaseBuffer( );

            // If we were unable to connect to www.symantec.com, return internet connection error.
            if( BACK_END_CONNECTION_ERROR == nResult )
                {
                hrRetVal = E_BACKEND_UNABLE_TO_CONNECT_TO_INTERNET;
                }
            else
                {
                hrRetVal = E_UNABLE_TO_GET_BACKEND_STATUS;
                }
            break;
            }
        case BACK_END_BUSY_TRY_LATER:
            hrRetVal = S_BACKEND_BUSY;
            break;
        case BACK_END_FP_USE_LU:
            hrRetVal = S_BACKEND_FALSE_POSITIVE;
            break;
        case BACK_END_ALL_CLEAN:
            hrRetVal = S_BACKEND_ALL_CLEAN;
        default:
            hrRetVal = E_UNABLE_TO_GET_BACKEND_STATUS;
        }

    if( NULL != pSocket )
        {
        pSocket->Release();
        pSocket = NULL;
        }
    
    return hrRetVal;
}



////////////////////////////////////////////////////////////////////////////
// Function name    : IsFileOfTheDayModeValid
//
// Description      : This function checks to see if File of the Day mode is
//                    valid for the current file list.  This function should
//                    be called by a client before converting to file of the
//                    day mode.  We will not convert to FOD mode if this
//                    function does not return "FOTD_IS_VALID".
//
// Return type      : A DWORD of the possible values:
//                      -- FOTD_IS_VALID
//                      -- FOTD_UNKOWN_ERROR
//                      -- FOTD_FILE_IS_COMPRESSED
//                      -- FOTD_MORE_THAN_ONE_REJECTED_FILE
//                      -- FOTD_INVALID_TIME_LAPSE
//
////////////////////////////////////////////////////////////////////////////
// 4/23/98 SEDWARD - Function created / Header added.
// 4/26/98 JTAYLOR - Added support for corporate and returning the correct
//                   answer after the rejected file has been converted to
//                   an accepted file during file of the day mode.
// 5/19/98 SEDWARD - Function now returns a DWORD instead of BOOL to give the
//                   caller more information as to why we're not in "File
//                   Of The Day" mode
////////////////////////////////////////////////////////////////////////////
DWORD   CScanDeliverDLL::IsFileOfTheDayModeValid(void)
{
    auto    DWORD       dwResult = FOTD_IS_VALID;

    // are we dealing with only one rejected file?
    if ((0 < GetNumberAcceptedFiles())  ||  (1 != GetNumberRejectedFiles()))
        {
        dwResult = FOTD_MORE_THAN_ONE_REJECTED_FILE;
        goto  Exit_Function;
        }

    // is the file compressed?  get the file type...
    auto    IQuarantineItem*        pItemPtr;
    auto    HRESULT                 hResult;
    auto    DWORD                   dwFileType;

    pItemPtr = m_RejectedFileInterfacePointers[0];
    if (NULL == pItemPtr)
        {
        dwResult = FOTD_UNKOWN_ERROR;
        goto  Exit_Function;
        }

    hResult = pItemPtr->GetFileType(&dwFileType);
    if (FAILED(hResult))
        {
        dwResult = FOTD_UNKOWN_ERROR;
        goto  Exit_Function;
        }
    
    // File came from WarChild or OEH which means it doesn't have a N30 Type but
    // it is still suspicious. We want these so they aren't FOD's.
    //
    if ( dwFileType & QFILE_TYPE_WARCHILD || dwFileType & QFILE_TYPE_OEH )
    {
        dwResult = FOTD_IS_VALID;
        goto Exit_Function;
    }

    // test for compressed files here
    if (dwFileType & QFILE_TYPE_COMPRESSED)
        {
        dwResult = FOTD_FILE_IS_COMPRESSED;
        goto  Exit_Function;
        }

    // if the previous tests succeed and we're running in corporate mode, always
    // return a success value (the default)
    if (TRUE == g_ConfigInfo.IsCorporateMode())
        {
        goto  Exit_Function;
        }

    // has it been at least one day since the last submission?
    if (FALSE == CheckFileOfTheDayTimeLapse())
        {
        dwResult = FOTD_INVALID_TIME_LAPSE;
        goto  Exit_Function;
        }


Exit_Function:

    return (dwResult);

}  // end of "CScanDeliverDLL::IsFileOfTheDayModeValid"

////////////////////////////////////////////////////////////////////////////
// Function name    : IsFileOfTheDayMode
//
// Description      : This function checks to see if the File of the Day mode
//                    is currently active.  This will be TRUE only after a
//                    successfull call to ConvertToFileOfTheDay.
//
// Return type      : BOOL
//
////////////////////////////////////////////////////////////////////////////
// 4/26/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL    CScanDeliverDLL::IsFileOfTheDayMode(void)
{
    return m_bInFileOfTheDayMode;
}  // end of "CScanDeliverDLL::IsFileOfTheDayMode"

////////////////////////////////////////////////////////////////////////////
// Function name    : ConvertToFileOfTheDayMode
//
// Description      : This function converts the submission process to file
//                    of the day mode.  This moves the one rejected file into
//                    the accepted file list.
//
// Return type      : BOOL
//
////////////////////////////////////////////////////////////////////////////
// 4/26/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL    CScanDeliverDLL::ConvertToFileOfTheDayMode(void)
{
    BOOL        bResult = TRUE;
    CString cszGeneratedFilename;
    HRESULT hr = E_FAIL;

    // If we are already in file of the day mode, return TRUE
    if( TRUE == m_bInFileOfTheDayMode )
    {
        return TRUE;
    }

    // If we cannot go to FOD mode return FALSE
    if (FOTD_IS_VALID != IsFileOfTheDayModeValid())
    {
        return FALSE;
    }

    CString cszTempFolder;

    // get the temporary Quarantine Directory for unpackaging
    bResult = GetPathFromRegKey( HKEY_LOCAL_MACHINE,
                                 REGKEY_QUARANTINE_TEMP_PATH,
                                 REGVAL_QUARANTINE_TEMP_PATH,
                                 cszTempFolder.GetBuffer(MAX_PATH),
                                 MAX_QUARANTINE_FILENAME_LEN);
    cszTempFolder.ReleaseBuffer();
    if( FALSE == bResult )
    {
        return FALSE;
    }

    IQuarantineItem *pQuarantineItem = m_RejectedFileInterfacePointers[0];
    hr = pQuarantineItem->UnpackageMainFile((LPSTR)(LPCSTR)cszTempFolder,
                                         NULL,
                                         cszGeneratedFilename.GetBuffer(MAX_QUARANTINE_FILENAME_LEN),
                                         MAX_QUARANTINE_FILENAME_LEN,
                                         FALSE,
                                         TRUE);
    cszGeneratedFilename.ReleaseBuffer();
    if( FAILED( hr ) )
    {
        return FALSE;
    }

    // If the file is accepted, then add it to the accepted list
    // Also store the unpackaged filename for use later.
    int nIndex = m_AcceptedFileInterfacePointers.Add(pQuarantineItem);
    m_AcceptedFileNames.SetAtGrow( nIndex, cszGeneratedFilename );
    m_nAcceptedFiles++;

    // Add to files to remove later
    m_lstTemporaryFiles.push_back(cszGeneratedFilename);

    // Set the FOD mode bool to TRUE.
    m_bInFileOfTheDayMode = TRUE;

    return (TRUE);

}  // end of "CScanDeliverDLL::ConvertToFileOfTheDayMode"


////////////////////////////////////////////////////////////////////////////
// Function name    : CheckFileOfTheDayTimeLapse
//
// Description      : This function checks the registry to see if we qualify
//                    to submit another "file of the day".  It basically checks
//                    if a day has lapsed since the last successful submission.
//                    (NOTE: this means "day" boundaries, not 24-hours)
//
// Return type      : BOOL -- TRUE if it's okay to submit a new file of the day,
//                            FALSE if not
//
////////////////////////////////////////////////////////////////////////////
// 4/23/98 SEDWARD - Function created / Header added.
// 5/14/98 JTAYLOR - Updated function to not use KEY_ALL_ACCESS
////////////////////////////////////////////////////////////////////////////
BOOL    CScanDeliverDLL::CheckFileOfTheDayTimeLapse(void)
{
    auto    BOOL            bResult = FALSE;
    auto    CTime           currentTime;
    auto    CTime           registryTime;
    auto    DWORD           dwRegValueType;
    auto    DWORD           dwSize;
    auto    HKEY            hKey = NULL;
    auto    int             nCurrentDay;
    auto    int             nRegistryDay;
    auto    long            lResult;
    auto    time_t          stRegistryTime = 0;


    // open the registry key; if it doesn't exist, create one with the current
    // date and time, and exit
    lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE                       // handle of open key
                                        , REGKEY_QUARANTINE_PATH    // address of name of subkey to open
                                        , 0                         // reserved
                                        , (KEY_READ | KEY_WRITE)    // security access mask
                                        , &hKey);                   // address of handle of open key
    if (ERROR_SUCCESS != lResult)
        {
        hKey = NULL;
        CreateTimeLapseRegistryData(hKey);
        goto  Exit_Function;
        }

    // get the timestamp of the last successful file of the day submission; if it
    // does not exist, create the value with the current date and time, and exit
    dwSize = sizeof(stRegistryTime);
    lResult = RegQueryValueEx(hKey                      // handle of key to query
                            , FILE_OF_THE_DAY_VALUE     // address of name of value to query
                            , 0                         // reserved (must be zero)
                            , &dwRegValueType           // address of buffer for value type
                            , (LPBYTE)&stRegistryTime   // address of data buffer
                            , &dwSize);                 // address of data buffer size


    if ((ERROR_SUCCESS != lResult)  ||  (REG_BINARY != dwRegValueType))
        {
        CreateTimeLapseRegistryData(hKey);
        goto  Exit_Function;
        }

    // initialize a CTime object with what we fetched from the registry
    registryTime = CTime(stRegistryTime);

    // get the current day
    currentTime = CTime::GetCurrentTime();

    // if the current day is at least one day since the last submission, we want
    // to return TRUE; else, return FALSE
    nRegistryDay = registryTime.GetDayOfWeek();
    nCurrentDay = currentTime.GetDayOfWeek();
    if (nCurrentDay - nRegistryDay)
        {
        bResult = TRUE;
        }
    else if (nCurrentDay == nRegistryDay)
        {
        // see if we're on the same day, but over a week
        auto    CTimeSpan       timeLapse = currentTime - registryTime;
        if (timeLapse.GetDays() > 1)
            {
            bResult = TRUE;
            }
        }


Exit_Function:

    if (NULL != hKey)
        {
        RegCloseKey(hKey);
        }

    return (bResult);

}  // end of "CScanDeliverDLL::CheckFileOfTheDayTimeLapse"




////////////////////////////////////////////////////////////////////////////
// Function name    : CreateTimeLapseRegistryData
//
// Description      : This function is used to create a "file of the day"
//                    registry value, in case it's missing.  The value is
//                    initialized with the current system timestamp.
//
// Return type      : BOOL -- TRUE means all went well, FALSE means not
//
////////////////////////////////////////////////////////////////////////////
// 4/23/98 SEDWARD - Function created / Header added.
// 5/14/98 JTAYLOR - Updated function to not use KEY_ALL_ACCESS
////////////////////////////////////////////////////////////////////////////
BOOL    CScanDeliverDLL::CreateTimeLapseRegistryData(HKEY  hRegKey)
{
    auto    BOOL            bResult = FALSE;
    auto    CTime           currentTime;
    auto    DWORD           dwDisposition;
    auto    HKEY            hTempRegKey = NULL;
    auto    long            lResult;
    auto    time_t          timeStruct;



    // if the caller wants the key created, do so
    if (NULL == hRegKey)
        {
        lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE                     // handle of an open key
                                            , REGKEY_QUARANTINE_PATH    // address of subkey name
                                            , 0                         // reserved
                                            , NULL                      // address of class string
                                            , 0                         // special options flag
                                            , (KEY_READ | KEY_WRITE)    // desired security access
                                            , NULL                      // address of key security structure
                                            , &hTempRegKey              // address of buffer for opened handle
                                            , &dwDisposition);          // address of disposition value buffer
        if (ERROR_SUCCESS != lResult)
            {
            goto  Exit_Function;
            }

        // save the new handle in the input argument (note: this does not affect
        // the caller's copy)
        hRegKey = hTempRegKey;
        }

    // get the current date/time and write it to the file of the day registry value
    currentTime = CTime::GetCurrentTime();
    timeStruct = currentTime.GetTime();
    lResult = RegSetValueEx(hRegKey                             // handle of key to set value for
                                    , FILE_OF_THE_DAY_VALUE     // address of value to set
                                    , 0                         // reserved
                                    , REG_BINARY                // flag for value type
                                    , (CONST BYTE*)&timeStruct  // address of value data
                                    , sizeof(timeStruct));      // size of value data
    if (ERROR_SUCCESS == lResult)
        {
        bResult = TRUE;
        }



Exit_Function:

    if (NULL != hTempRegKey)
        {
        RegCloseKey(hTempRegKey);
        }

    return (bResult);

}  // end of "CScanDeliverDLL::CreateTimeLapseRegistryData"



////////////////////////////////////////////////////////////////////////////
// Function name    : UpdateTimeLapseRegistryData
//
// Description      : This function is used to update the "file of the day"
//                    registry value with the current system timestamp.
//
// Return type      : BOOL -- TRUE means all went well, FALSE means not
//
////////////////////////////////////////////////////////////////////////////
// 4/23/98 SEDWARD - Function created / Header added.
// 5/14/98 JTAYLOR - Updated function to not use KEY_ALL_ACCESS
////////////////////////////////////////////////////////////////////////////
BOOL    CScanDeliverDLL::UpdateTimeLapseRegistryData(void)
{
    auto    BOOL            bResult = FALSE;
    auto    CTime           currentTime;
    auto    HKEY            hKey = NULL;
    auto    long            lResult;
    auto    time_t          timeStruct;


    // open the registry key; if it doesn't exist, create one with the current
    // date and time, and exit
    lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE                       // handle of open key
                                        , REGKEY_QUARANTINE_PATH    // address of name of subkey to open
                                        , 0                         // reserved
                                        , (KEY_READ | KEY_WRITE)    // security access mask
                                        , &hKey);                   // address of handle of open key
    if (ERROR_SUCCESS != lResult)
        {
        hKey = NULL;
        CreateTimeLapseRegistryData(hKey);
        goto  Exit_Function;
        }

    // get the current date/time and write it to the file of the day registry value
    currentTime = CTime::GetCurrentTime();
    timeStruct = currentTime.GetTime();
    lResult = RegSetValueEx(hKey                                // handle of key to set value for
                                    , FILE_OF_THE_DAY_VALUE     // address of value to set
                                    , 0                         // reserved
                                    , REG_BINARY                // flag for value type
                                    , (CONST BYTE*)&timeStruct  // address of value data
                                    , sizeof(timeStruct));      // size of value data
    if (ERROR_SUCCESS == lResult)
        {
        bResult = TRUE;
        }


Exit_Function:

    if (NULL != hKey)
        {
        RegCloseKey(hKey);
        }

    return (bResult);

}  // end of "CScanDeliverDLL::UpdateTimeLapseRegistryData"

bool Is64BitOS(VOID)
{
    bool bRet = false;

    // Try to get a pointer to IsWow64Process() to determine if 64-bit Windows
    HINSTANCE hLibrary = LoadLibrary ( _T("KERNEL32") );

    if ( hLibrary == NULL )
    {
        return false;
    }

    static TCHAR szWow64Process[] = _T("IsWow64Process");

    typedef BOOL (WINAPI *fnIsWow64) (HANDLE,PBOOL);
    fnIsWow64 pIsWow64  = (fnIsWow64) GetProcAddress ( hLibrary, szWow64Process );
    BOOL bIsWow64 = FALSE;
    
    if ( pIsWow64 == NULL )
    {
        // Could not find the function so this is not a 64-bit OS
        bRet = false;
    }
    else if( pIsWow64(GetCurrentProcess(), &bIsWow64) != TRUE )
    {
        // Call to IsWow64Process() function call failed, assume 32-bit
        bRet = false;
    }
    else if( bIsWow64 != TRUE )
    {
        // 32-bit process
        bRet = false;
    }
    else
    {
        // 64-bit process
        bRet = true;
    }
        
    if(hLibrary != NULL)
    {
        FreeLibrary(hLibrary);
        hLibrary = NULL;
    }

    return bRet;
        
}

BOOL CScanDeliverDLL::AddGeneralInformationToSubmission(ISNDSubmission* pSubmission)
{
    HRESULT hrRetVal;
    BOOL bRet;

    HRESULT hr;
    TCHAR   szInfoBuf[MAX_INI_BUF_SIZE]                           = "";

    INT i;
    SYSTEMTIME tDefinitionsDate;

    CNAVVersion navVer;

    // Fill out the info on the submission
    g_ConfigInfo.GetFirstName( szInfoBuf, MAX_INI_BUF_SIZE );
    hr = pSubmission->SetInfoField( ISNDSubmission::FirstName, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    g_ConfigInfo.GetLastName( szInfoBuf, MAX_INI_BUF_SIZE );
    hr = pSubmission->SetInfoField( ISNDSubmission::LastName, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    g_ConfigInfo.GetCompany( szInfoBuf, MAX_INI_BUF_SIZE );
    hr = pSubmission->SetInfoField( ISNDSubmission::Company, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    g_ConfigInfo.GetAddress1( szInfoBuf, MAX_INI_BUF_SIZE );
    hr = pSubmission->SetInfoField( ISNDSubmission::AddressLine1, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    g_ConfigInfo.GetAddress2( szInfoBuf, MAX_INI_BUF_SIZE );
    hr = pSubmission->SetInfoField( ISNDSubmission::AddressLine2, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    g_ConfigInfo.GetCity( szInfoBuf, MAX_INI_BUF_SIZE );
    hr = pSubmission->SetInfoField( ISNDSubmission::City, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    g_ConfigInfo.GetState( szInfoBuf, MAX_INI_BUF_SIZE );
    hr = pSubmission->SetInfoField( ISNDSubmission::StateProvince, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    g_ConfigInfo.GetPhone( szInfoBuf, MAX_INI_BUF_SIZE );
    hr = pSubmission->SetInfoField( ISNDSubmission::Phone, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    g_ConfigInfo.GetFax( szInfoBuf, MAX_INI_BUF_SIZE );
    hr = pSubmission->SetInfoField( ISNDSubmission::Fax, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    g_ConfigInfo.GetEmail( szInfoBuf, MAX_INI_BUF_SIZE );
    hr = pSubmission->SetInfoField( ISNDSubmission::Email, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }
    DWORD dwCountryCode;
    g_ConfigInfo.GetUserCountryCode( &dwCountryCode );
    sprintf( szInfoBuf, "%d", dwCountryCode );
    hr = pSubmission->SetInfoField( ISNDSubmission::Country, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    g_ConfigInfo.GetInfectionCountryCode( &dwCountryCode );
    sprintf( szInfoBuf, "%d", dwCountryCode );
    hr = pSubmission->SetInfoField( ISNDSubmission::InfectionCountry, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }


    // WARNING : Cheese ahead
    //
    // WarChild Cheese!!!!!!!
    //
    // We are going to pre-pend a tag to signify that this file is coming from WarChild
    // so that SARC can handle it correctly.
    //
    g_ConfigInfo.GetAddlSymptoms( szInfoBuf, MAX_INI_BUF_SIZE );

    for( i = 0; i < m_nAcceptedFiles; i++ )
    {
        IQuarantineItem *pQItem = m_AcceptedFileInterfacePointers[i];

        DWORD dwFileType;

        pQItem -> GetFileType ( &dwFileType );

        if ( dwFileType & QFILE_TYPE_OEH )
        {
            TCHAR szOEHChildBuf [MAX_INI_BUF_SIZE];
            _tcscpy ( szOEHChildBuf, OEH_TAG );
            _tcscat ( szOEHChildBuf, szInfoBuf );
            _tcscpy ( szInfoBuf, szOEHChildBuf);
            break;
        }
    }

    hr = pSubmission->SetInfoField( ISNDSubmission::Symptoms, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    // Get/Set the pre defined symptoms
    g_ConfigInfo.GetSymptoms( szInfoBuf, MAX_INI_BUF_SIZE );
    hr = pSubmission->SetInfoField( ISNDSubmission::PreDefSymptoms, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    // Get the zip code
    g_ConfigInfo.GetZipCode( szInfoBuf, MAX_INI_BUF_SIZE );
    hr = pSubmission->SetInfoField( ISNDSubmission::ZipPostalCode, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    // Get the ATLAS ID number
    g_ConfigInfo.GetAtlasNumber( szInfoBuf, MAX_INI_BUF_SIZE );
    hr = pSubmission->SetInfoField( ISNDSubmission::AtlasID, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    // Get the corporate support number
    g_ConfigInfo.GetCorporateSupportNumber( szInfoBuf, MAX_INI_BUF_SIZE );
    hr = pSubmission->SetInfoField( ISNDSubmission::SupportNum, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    // Load the install language from the registry.
    bRet = GetPathFromRegKey( HKEY_LOCAL_MACHINE,
        REGKEY_NAV_MAIN,
        REGVAL_NAV_LANGUAGE,
        szInfoBuf,
        MAX_INI_BUF_SIZE);
    if( FALSE == bRet )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    // Set the language in the package
    hr = pSubmission->SetInfoField( ISNDSubmission::Language, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    // get the date of definitions
    hrRetVal = GetCurrentDefinitionsDate( &tDefinitionsDate );
    if( FAILED(hr) )
    {
        goto Exit_Function;
    }

    // set the date of definitions
    hr = pSubmission->SetInfoFieldDateTime( ISNDSubmission::DefinitionsDate,
        tDefinitionsDate.wYear,
        tDefinitionsDate.wMonth,
        tDefinitionsDate.wDay,
        tDefinitionsDate.wHour,
        tDefinitionsDate.wMinute,
        tDefinitionsDate.wSecond);
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    // Set the platforms for which the user would like defs back.
    DWORD dwPlatforms;
    g_ConfigInfo.GetOperatingSystem( &dwPlatforms );
    // Convert this value to a string
    sprintf( szInfoBuf, "%d", dwPlatforms );
    hr = pSubmission->SetInfoField( ISNDSubmission::ProductOS, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    // Add the version
    g_ConfigInfo.SetVersion( navVer.GetPublicRevision() );
    g_ConfigInfo.GetVersion( szInfoBuf, MAX_INI_BUF_SIZE );
    hr = pSubmission->SetInfoField( ISNDSubmission::Version, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }

    // Add the OSBits
    bool bIs64bit = Is64BitOS();
    if( bIs64bit )
        _tcscpy(szInfoBuf, _T("64"));
    else
        _tcscpy(szInfoBuf, _T("32"));

    g_ConfigInfo.SetOSBits( szInfoBuf );

    hr = pSubmission->SetInfoField( ISNDSubmission::OSBits, szInfoBuf );
    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_SET_INFO_FIELD;
        goto Exit_Function;
    }
        
    return TRUE;

Exit_Function:
    return FALSE;

}

BOOL CScanDeliverDLL::AddMainFileToSample(ISNDSample *pSample, IQuarantineItem *pItem, INT iIndex)
{
    BOOL bResult;
    HRESULT hrRetVal, hr;

    BOOL    bStripFiles                                           = FALSE;
    CContentStripper *pStripper                                   = NULL;
    CContentStripper::INIT_STATUS  stContentInitStatus;
    CContentStripper::STRIP_STATUS stContentStripStatus;
    TCHAR   szStrippedFileName[MAX_QUARANTINE_FILENAME_LEN]       = "";
    CString cszOriginalFileName;

    char    szNavFolder[MAX_QUARANTINE_FILENAME_LEN]              = "";
    
    // Get the install path for NAV
    bResult = GetMainNavDir( szNavFolder );
    // Make sure the get path function suceeded.
    if( FALSE == bResult )
    {
        hrRetVal = E_QUARANTINE_DIRECTORY_INVALID;
        goto Exit_Function;
    }
    
    QItemEZ::GetFullANSIFilePath(pItem, cszOriginalFileName);

    // Find out if the user elected to strip the content from files.
    bStripFiles = g_ConfigInfo.GetStripFileContent( );

    // If we want to strip the files, then create the strip file object
    if( TRUE == bStripFiles )
    {
        // Allocate the new class
        pStripper = CContentStripperFactory::CreateInstance();
        if( NULL == pStripper )
        {
            CCTRACEE("CScanDeliverDLL::CreatePackage() - Unable to create the content stripper");
        }
        else
        {
            // Initialize the content stripper
            stContentInitStatus = pStripper->StartUp(szNavFolder);
            if( INIT_NO_ERROR != stContentInitStatus )
            {
                CCTRACEE("CScanDeliverDLL::CreatePackage() - Unable to initialize the content stripper");
            }
        }
    }

    // Check to see if file stripping was enabled
    if( TRUE == bStripFiles )
    {
        hrRetVal = CreateNewFileInQuarantineTemp( (LPSTR)(LPCSTR)cszOriginalFileName,
                                                  szStrippedFileName,
                                                  MAX_QUARANTINE_FILENAME_LEN );
        if( FAILED( hrRetVal ) )
        {
            hrRetVal = E_FILE_CREATE_FAILED;
            goto Exit_Function;
        }

        // Strip the file.
        CString pBuff1 = m_AcceptedFileNames[iIndex];
        CString pBuff2 = szStrippedFileName;

        FILETIME ftCreation;
        FILETIME ftAccess;
        FILETIME ftLastWrite;
        HANDLE   hTempHandle;

        if( pStripper != NULL && INIT_NO_ERROR == stContentInitStatus )
        {
            stContentStripStatus = pStripper->Strip( pBuff1,
                pBuff2 );
            if( CContentStripper::STRIP_NO_ERROR != stContentStripStatus )
            {
                CCTRACEE("CScanDeliverDLL::CreatePackage() - Unable to strip the file contents");
            }


            m_lstTemporaryFiles.push_back(pBuff2);
        }

        // Preserve the file times for submission
        // Open the source file and read in its date/times
        hTempHandle = CreateFile( pBuff1,                              // filename
            GENERIC_READ,                        // access
            0,                                   // don't share
            NULL,                                // don't allow handle inheritance
            OPEN_EXISTING,                       // only open if it exists
            FILE_FLAG_SEQUENTIAL_SCAN,           // attributes and flags
            NULL );                              // no template handle

        if (INVALID_HANDLE_VALUE == hTempHandle)
        {
            hrRetVal = E_FILE_CREATE_FAILED;
            goto  Exit_Function;
        }

        bResult = GetFileTime( hTempHandle, &ftCreation, &ftAccess, &ftLastWrite );
        if( FALSE == bResult )
        {
            hrRetVal = E_FAIL;
            goto Exit_Function;
        }

        // close the file
        if ( INVALID_HANDLE_VALUE != hTempHandle )
        {
            CloseHandle( hTempHandle );
        }


        // Open the Destination file and write its date/times
        hTempHandle = CreateFile( pBuff2,                              // filename
            GENERIC_WRITE,                       // access
            0,                                   // don't share
            NULL,                                // don't allow handle inheritance
            OPEN_EXISTING,                       // only open if it exists
            FILE_FLAG_SEQUENTIAL_SCAN,           // attributes and flags
            NULL );                              // no template handle

        if (INVALID_HANDLE_VALUE == hTempHandle)
        {
            hrRetVal = E_FILE_CREATE_FAILED;
            goto  Exit_Function;
        }

        bResult = SetFileTime( hTempHandle, &ftCreation, &ftAccess, &ftLastWrite );
        if( FALSE == bResult )
        {
            hrRetVal = E_FAIL;
            goto Exit_Function;
        }

        // close the file
        if ( INVALID_HANDLE_VALUE != hTempHandle )
        {
            CloseHandle( hTempHandle );
        }

        // Put the sample in the list of files to collect.
        hr = pSample->AddSampleFile( pBuff2, cszOriginalFileName);
    }
    else
    {
        // Put the sample in the list of files to collect.
        hr = pSample->AddSampleFile(m_AcceptedFileNames[iIndex], cszOriginalFileName);
    }

    if( FAILED(hr) )
    {
        hrRetVal = E_UNABLE_TO_ADD_FILE_TO_PACKAGE;
        goto Exit_Function;
    }

    // If the init succeeded then call shut down
    if( INIT_NO_ERROR == stContentInitStatus )
        pStripper->ShutDown();

    if( pStripper != NULL )
    {
        pStripper->Release();
        pStripper = NULL;
    }

    return TRUE;

    Exit_Function:

    // If the init succeeded then call shut down
    if( INIT_NO_ERROR == stContentInitStatus )
    pStripper->ShutDown();

    if( pStripper != NULL )
    {
        pStripper->Release();
        pStripper = NULL;
    }

    return FALSE;
}


BOOL CScanDeliverDLL::AddSamplesToSubmission(ISNDSubmission *pSubmission)
{
    SYMRESULT sr;

    INT i;
    HRESULT hrRetVal;
    CString cszSampleZip;

    // Add all the files to the package.
    // Loop through each accepted item and add it to the package
    for( i = 0; i < m_nAcceptedFiles; i++ )
    {
        IQuarantineItem *pQItem = m_AcceptedFileInterfacePointers[i];

        ISNDSamplePtr pSample;

        sr = SNDSampleDLL_Loader::CreateObject (GETMODULEMGR(), pSample); 
        if(SYM_FAILED(sr))
            goto Exit_Function;
        
        if(!AddInformationToSample(pSample, pQItem))
            goto Exit_Function;

        if(!AddMainFileToSample(pSample, pQItem, i))
            goto Exit_Function;

        if(!AddDataToSample(pSample, pQItem))
            goto Exit_Function;
        
        if(FAILED(pSubmission->SubmitSample(pSample)))
            goto Exit_Function;

        /*
        // Advance progress dialog
        progress.SetPos( i );

        if( progress.CheckCancelButton() )
        {
        hrRetVal = E_USER_CANCELLED;
        goto Exit_Function;
        }
        */
    }

    return TRUE;

Exit_Function:
    return FALSE;
}

BOOL CScanDeliverDLL::AddInformationToSample(ISNDSample *pSample, IQuarantineItem *pItem)
{
    CString cszTempString;
    HRESULT hr;

    // Get the source machine name
    if(SUCCEEDED(QItemEZ::GetOriginalMachineName(pItem, cszTempString)))
        pSample->SetInfoField(ISNDSample::MachineName, cszTempString);
    
    // Get the virus name
    if(SUCCEEDED(QItemEZ::GetVirusName(pItem, cszTempString)))
        pSample->SetInfoField(ISNDSample::VirusName, cszTempString);
    else
        pSample->SetInfoField(ISNDSample::VirusName, _T("No Virus Name available"));

    // Get the quarantine item GUID
    if(SUCCEEDED(QItemEZ::GetString(pItem, QSERVER_ITEM_INFO_UUID, cszTempString)))
        pSample->SetInfoField(ISNDSample::SampleGUID, cszTempString);
    else
        pSample->SetInfoField(ISNDSample::SampleGUID, _T("No GUID available"));

    // get the date of definitions
    SYSTEMTIME tDefinitionsDate;
    hr = GetCurrentDefinitionsDate( &tDefinitionsDate );
    if( SUCCEEDED(hr) )
    {
        // set the date of definitions
        hr = pSample->SetInfoFieldDateTime( ISNDSample::DefDate,
            tDefinitionsDate.wYear,
            tDefinitionsDate.wMonth,
            tDefinitionsDate.wDay,
            tDefinitionsDate.wHour,
            tDefinitionsDate.wMinute,
            tDefinitionsDate.wSecond);
    }

    // Get the VID
    DWORD dwTemp = NULL;
    if(SUCCEEDED(pItem->GetVirusID(&dwTemp)))
    {
        cszTempString.Format(_T("%u"), dwTemp);
    }
    else
    {
        cszTempString = _T("No VID available");
    }
    pSample->SetInfoField(ISNDSample::VirusID, cszTempString);
    
    // Get the Type
    if(SUCCEEDED(pItem->GetVirusID(&dwTemp)))
    {
        cszTempString.Format(_T("%u"), dwTemp);
    }
    else
    {
        cszTempString = _T("No Type available");
    }
    pSample->SetInfoField(ISNDSample::SampleType, cszTempString);

    // Get the Categories
    if( FAILED(QItemEZ::GetString(pItem, QSERVER_ITEM_INFO_CATEGORIES, cszTempString)) )
    {
        // Assume this was quarantined with a previous version of NAV since
        // it does not have the category field
        cszTempString = _T("Unknown");
    }
    // If it's empty set default categories string
    else if(!cszTempString.GetLength())
        cszTempString = _T("No categories available");
    
    pSample->SetInfoField(ISNDSample::VirusThreatCat, cszTempString);

    // Get the Anomaly ID
    if( FAILED(QItemEZ::GetString(pItem, QSERVER_ITEM_INFO_ANOMALY_ID, cszTempString)) )
    {
        // Assume this was quarantined with a previous version of NAV since
        // it does not have the anomaly field
        cszTempString = _T("Unknown");
    }
    // If it's empty set default Anomaly ID string
    else if(!cszTempString.GetLength())
        cszTempString = _T("No Anomaly ID available");

    pSample->SetInfoField(ISNDSample::AnomalyID, cszTempString);
    
    // Get the Anomaly Categories
    if( FAILED(QItemEZ::GetString(pItem, QSERVER_ITEM_INFO_ANOMALY_CATEGORIES, cszTempString)) )
    {
        // Assume this was quarantined with a previous version of NAV since
        // it does not have the anomaly categories field
        cszTempString = _T("Unknown");
    }
    // If it's empty set default categories string
    else if(!cszTempString.GetLength())
        cszTempString = _T("No Anomaly categories available");

    pSample->SetInfoField(ISNDSample::AnomalyCategory, cszTempString);

    // Get the MD5 hash
    if( FAILED(QItemEZ::GetString(pItem, QSERVER_ITEM_INFO_MD5, cszTempString)) )
    {
        // Assume this was quarantined with a previous version of NAV since
        // it does not have the anomaly categories field
        cszTempString = _T("Unknown");
    }
    // If it's empty set default MD5 hash string
    else if(!cszTempString.GetLength())
        cszTempString = _T("No MD5 hash available");

    pSample->SetInfoField(ISNDSample::MD5, cszTempString);

    // Get the MD5 hash
    if( FAILED(QItemEZ::GetString(pItem, QSERVER_ITEM_INFO_MD5, cszTempString)) )
    {
        // Assume this was quarantined with a previous version of NAV since
        // it does not have the anomaly categories field
        cszTempString = _T("Unknown");
    }
    // If it's empty set default MD5 hash string
    else if(!cszTempString.GetLength())
        cszTempString = _T("No MD5 hash available");

    pSample->SetInfoField(ISNDSample::MD5, cszTempString);

    return TRUE;
}

BOOL CScanDeliverDLL::AddDataToSample(ISNDSample *pSample, IQuarantineItem *pItem)
{
    HRESULT hr, hrReturn;
    INT iIndex;
    BOOL bResult;
    DWORD dwRemediationCount = NULL;
    CString cszQuarantineTempDir;    

    // get the temporary Quarantine Directory for unpackaging
    bResult = GetPathFromRegKey( HKEY_LOCAL_MACHINE,
        REGKEY_QUARANTINE_TEMP_PATH,
        REGVAL_QUARANTINE_TEMP_PATH,
        cszQuarantineTempDir.GetBuffer(MAX_QUARANTINE_FILENAME_LEN),
        MAX_QUARANTINE_FILENAME_LEN);
    cszQuarantineTempDir.ReleaseBuffer();

    if(!bResult || cszQuarantineTempDir.IsEmpty())
    {
        hrReturn = E_FAIL; // fatal
        goto Exit_Function;
    }

    if(g_ConfigInfo.GetSendAddlInfo())
    {
        CString cszSnapshotTempFileName;
        CString cszSnapshotTempLocation;
        DWORD dwSnapshotTempFileSize = MAX_PATH;

        // Add SystemSnapshot data
        hr = pItem->CopySnapshotToFolder((LPSTR)(LPCSTR)cszQuarantineTempDir, cszSnapshotTempFileName.GetBuffer(dwSnapshotTempFileSize), &dwSnapshotTempFileSize);
        cszSnapshotTempFileName.ReleaseBuffer();
        if(SUCCEEDED(hr) && !cszSnapshotTempFileName.IsEmpty())
        {
            PathCombine(cszSnapshotTempLocation.GetBuffer(MAX_PATH), cszQuarantineTempDir, cszSnapshotTempFileName);
            cszSnapshotTempLocation.ReleaseBuffer();
            m_lstTemporaryFiles.push_back(cszSnapshotTempLocation);

            pSample->AddSnapShot(cszSnapshotTempLocation);
        }
    }

    hr = pItem->GetRemediationCount(&dwRemediationCount);
    if(FAILED(hr))
    {
        hrReturn = E_FAIL; // fatal
        goto Exit_Function;
    }

    if(!dwRemediationCount)
    {
        hrReturn = S_OK; // non-fatal
        goto Exit_Function;
    }
    
    for(iIndex = 1; iIndex<= dwRemediationCount; iIndex++)
    {
        DWORD dwDataFileNameSize = MAX_QUARANTINE_FILENAME_LEN;
        DWORD dwUndoFileNameSize = MAX_QUARANTINE_FILENAME_LEN;
        DWORD dwTempFileNameSize = NULL;
        CString cszDataFileName, cszUndoFileName, cszTempFileName;
        
        // Get the remediation data from the quarantine item
        hr = pItem->GetRemediationData( (LPSTR)(LPCSTR)cszQuarantineTempDir, 
                                        iIndex, 
                                        cszDataFileName.GetBuffer(dwDataFileNameSize), &dwDataFileNameSize,
                                        cszUndoFileName.GetBuffer(dwUndoFileNameSize), &dwUndoFileNameSize);
        cszDataFileName.ReleaseBuffer();
        cszUndoFileName.ReleaseBuffer();
        if(FAILED(hr) || cszDataFileName.IsEmpty())
        {
            hrReturn = E_FAIL; // fatal
            goto Exit_Function;
        }
        
        // make a full path to the item
        dwTempFileNameSize = cszQuarantineTempDir.GetLength() + cszDataFileName.GetLength() + 2;
        PathCombine(cszTempFileName.GetBuffer(dwTempFileNameSize), cszQuarantineTempDir, cszDataFileName);
        cszTempFileName.ReleaseBuffer();
        cszDataFileName = cszTempFileName;
        m_lstTemporaryFiles.push_back(cszDataFileName);
        
        if(!cszUndoFileName.IsEmpty())
        {
            dwTempFileNameSize = cszQuarantineTempDir.GetLength() + cszUndoFileName.GetLength() + 2;
            PathCombine(cszTempFileName.GetBuffer(dwTempFileNameSize), cszQuarantineTempDir, cszUndoFileName);
            cszTempFileName.ReleaseBuffer();
            cszUndoFileName = cszTempFileName;
            m_lstTemporaryFiles.push_back(cszUndoFileName);
        }

        // add the data to the sample
        hr = pSample->AddRemedAction(cszDataFileName, iIndex);
        if(!cszUndoFileName.IsEmpty())
        {
            hr = pSample->AddRemedUndo(cszUndoFileName, iIndex);
        }

    }


    return TRUE;

Exit_Function:
    
    return SUCCEEDED(hrReturn) ? TRUE : FALSE;
}

void CScanDeliverDLL::CleanupTemporaryFiles()
{
    CSTRING_LIST::iterator itTempFiles;
    
    // Delete the stripped files and the memory for the filenames
    for( itTempFiles = m_lstTemporaryFiles.begin(); itTempFiles != m_lstTemporaryFiles.end(); itTempFiles++ )
    {
        FileZeroAndDelete((LPTSTR)(LPCTSTR)(*itTempFiles), 1 );
    }

    m_lstTemporaryFiles.clear();

    return;
}

#pragma message("TODO: Remove before shipping!!")
//
// <TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST> 
// to be removed before shipping
HRESULT EncryptPackage(LPCTSTR szFileName);

void MakeBackupForTesting(LPCTSTR szSubmission)
{
    CRegKey rkNavProg;
    LONG lResult = rkNavProg.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton Antivirus"), KEY_READ);
    if(ERROR_SUCCESS == lResult)
    {
        CString cszBackupDir;
        DWORD dwSize = MAX_PATH;
        lResult = rkNavProg.QueryStringValue(_T("SNDBackupDir"), cszBackupDir.GetBuffer(dwSize + 1), &dwSize);
        cszBackupDir.ReleaseBuffer();
        if(ERROR_SUCCESS == lResult && !cszBackupDir.IsEmpty())
        {
            CString cszFileName;
            SYSTEMTIME st;
            GetSystemTime(&st);
            cszFileName.Format(_T("%02d-%02d-%04d-%08X.zip"), st.wMonth, st.wDay, st.wYear, GetTickCount());

            CString cszTargetPath;
            dwSize = cszFileName.GetLength() + cszBackupDir.GetLength() + 2;
            PathCombine(cszTargetPath.GetBuffer(dwSize), cszBackupDir, cszFileName);
            cszTargetPath.ReleaseBuffer();

            if(CopyFile(szSubmission, cszTargetPath, FALSE))
            {
                EncryptPackage(cszTargetPath);
            }

        }
    }
    return;
}

#define SDPACK_ZIP_ENCRYPT_BYTE 0xCD

HRESULT EncryptPackage(LPCTSTR szFileName)
{
    HANDLE              hFile;
    DWORD               dwOffset;
    DWORD               dwSize;
    DWORD               dwBytesToEncrypt;
    DWORD               dwTemp;
    BYTE                abyBuf[4096];

    // Open the file for reading/writing

    hFile = CreateFile(szFileName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return E_FAIL;

    // Get the size

    dwSize = GetFileSize(hFile,NULL);
    if (dwSize == 0xFFFFFFFF)
    {
        CloseHandle(hFile);
        return E_FAIL;
    }

    // Encrypt the file

    dwBytesToEncrypt = sizeof(abyBuf);
    dwOffset = 0;
    while (dwOffset < dwSize)
    {
        if (dwSize - dwOffset < dwBytesToEncrypt)
            dwBytesToEncrypt = dwSize - dwOffset;

        // Read a chunk

        dwTemp = SetFilePointer(hFile,
            dwOffset,
            NULL,
            FILE_BEGIN);

        if (dwTemp != dwOffset)
        {
            CloseHandle(hFile);
            return E_FAIL;
        }

        if (ReadFile(hFile,
            abyBuf,
            dwBytesToEncrypt,
            &dwTemp,
            NULL) == FALSE ||
            dwTemp != dwBytesToEncrypt)
        {
            CloseHandle(hFile);
            return E_FAIL;
        }

        // Encrypt chunk

        for (dwTemp=0;dwTemp<dwBytesToEncrypt;dwTemp++)
            abyBuf[dwTemp] ^= SDPACK_ZIP_ENCRYPT_BYTE;

        // Write the chunk

        dwTemp = SetFilePointer(hFile,
            dwOffset,
            NULL,
            FILE_BEGIN);

        if (dwTemp != dwOffset)
        {
            CloseHandle(hFile);
            return E_FAIL;
        }

        if (WriteFile(hFile,
            abyBuf,
            dwBytesToEncrypt,
            &dwTemp,
            NULL) == FALSE ||
            dwTemp != dwBytesToEncrypt)
        {
            CloseHandle(hFile);
            return E_FAIL;
        }

        // Update offset

        dwOffset += dwBytesToEncrypt;
    }

    CloseHandle(hFile);

    return S_OK;
}

//
// to be removed before shipping
// </TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST>
//
