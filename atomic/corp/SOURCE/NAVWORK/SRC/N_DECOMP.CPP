// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/N_DECOMP.CPv   1.17   22 Jun 1998 14:37:24   DBuches  $
//
// Description: NAV Decomposer integration code
//
// Contains: CNAVEventSink methods
//           NAVDecomposerInit()
//           NAVDecomposerEnd()
//
// See Also: n_decomp.h, n_scan.cpp, navw.cpp
//************************************************************************
// $Log:   S:/NAVWORK/VCS/N_DECOMP.CPv  $
// 
//    Rev 1.17   22 Jun 1998 14:37:24   DBuches
// Correctly handle cases where there is no Temp or Tmp environment variable.
// 
//    Rev 1.16   15 Jun 1998 17:15:18   DBuches
// Added override of OnBadChild().
// 
//    Rev 1.15   03 Jun 1998 13:02:14   DBuches
// Convert component name to ANSI prior to display.
//
//    Rev 1.14   01 Jun 1998 10:17:14   tcashin
// Initialize infected SFN and LFN in ::OnNew().
//
//    Rev 1.13   26 May 1998 18:32:54   DBuches
// Fixed problem with displaying files being scanned.
//
//    Rev 1.12   19 May 1998 17:45:08   DBuches
// Removed extra exclusion check.
//
//    Rev 1.11   13 May 1998 12:43:04   MKEATIN
// Make sure pgDecomposer is NULL if initialization fails.  Also, made the
// code a bit easer to read (*&pgDecomposer) is now pgDecomposer - duh.
//
//
//    Rev 1.10   13 May 1998 12:03:50   MKEATIN
// Error check the return value of SetTempDir().
//
//    Rev 1.9   11 May 1998 09:58:38   DBuches
// Added code to display name of item in compressed file.
//
//    Rev 1.8   04 May 1998 20:07:38   DBuches
// Fixed problems with loading CAB decomposer.  Also added override of OnBusy().
//
//    Rev 1.7   21 Apr 1998 15:30:40   MKEATIN
// Added CAB scanning support.
//
//    Rev 1.6   31 Mar 1998 14:33:14   MKEATIN
// Changed more DEC_ERR's to DECERROR_GENERIC
//
//    Rev 1.5   31 Mar 1998 13:42:32   MKEATIN
// Changed DEC_ERR to DECERROR_GENERIC
//
//    Rev 1.4   30 Mar 1998 17:28:40   MKEATIN
// Made Alpha compile time changes.
//
//    Rev 1.3   25 Mar 1998 14:10:28   MKEATIN
// fixed a typo.
//
//    Rev 1.2   24 Mar 1998 18:13:54   MKEATIN
// If bScanAll is set, don't check the extention list.
//
//    Rev 1.1   20 Mar 1998 15:33:14   MKEATIN
// Added a file header.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "tchar.h"

#include "nonav.h"
#include "navwork.h"
#include "nunavdlg.h"
#include "nunavstb.h"
#include "cmbstr.h"
#include "navoptx.h"

#include "n_decomp.h"
#include "Dec2AMG.h"
#include "Dec2ARJ.h"
#include "Dec2CAB.h"
#include "Dec2GZIP.h"
#include "Dec2HQX.h"
#include "Dec2ID.h"
#include "Dec2LHA.h"
#include "Dec2LZ.h"
#include "Dec2MIME.h"
#include "Dec2SS.h"
#include "Dec2TAR.h"
#include "Dec2UUE.h"
#include "Dec2ZIP.h"

static IDecEngine *pAMGEngine   = NULL;
static IDecEngine *pARJEngine  = NULL;
static IDecEngine *pCABEngine   = NULL;
static IDecEngine *pGZIPEngine = NULL;
static IDecEngine *pHQXEngine = NULL;
static IDecEngine *pHTTPEngine = NULL;
static IDecEngine *pIDEngine   = NULL;
static IDecEngine *pLHAEngine  = NULL;
static IDecEngine *pLZEngine   = NULL;
static IDecEngine *pMIMEEngine   = NULL;
static IDecEngine *pSSEngine = NULL;
static IDecEngine *pTAREngine = NULL;
static IDecEngine *pUUEEngine = NULL;
static IDecEngine *pZIPEngine  = NULL;


static BOOL       bInitialized = FALSE;


extern HINSTANCE hInstance;

//*****************************************************************************
// Global value to hold the extension list removed from the NAVOPTS structure
//*****************************************************************************
extern char gszExtensions[MAX_EXTENSION_LIST_SIZE+1];

// the global decomposer pointer

IDecomposer *pgDecomposer      = NULL;



//************************************************************************
// Local functions


//************************************************************************
// CNAVEventSink::OnNew(IDecObject* pObject, WORD* pwResult,
//                      LPTSTR pszNewDataFile, BOOL* pbContinue);
//
// Purpose:
//  Called by Decomposer just before it uses the engines to identify a data
//  object.
//
// Parameters:
//      pObject         Pointer to object to be identified.
//      pwResult        Pointer to a word indicating the result of the sink
//                      object's processing.
//      pszNewDataFile  Pointer to a character buffer containing the name of
//                      the replacement data file.
//      pbContinue      Pointer to a boolean flag that is TRUE if Decomposer
//                      should attempt to identify the object, or FALSE otherwise.
//
// Returns:
//      DEC_OK          Signifies to decomposer that the error should be
//                      ignored.
//      eERROR          The error will propagate up the call chain, halting
//                      all further processing.
//
//************************************************************************
HRESULT CNAVEventSink::OnNew
(
    IDecObject* pObject,
    WORD* pwResult,
    LPTSTR pszNewDataFile,
    BOOL* pbContinue
)
{
#ifdef _ALPHA_
    // This shouldn't get called on ALPHA

    return DECERR_GENERIC;
#else

    // scanning happens here if the decomposer depth is 1

    DWORD   dwDepth;
    HRESULT hr;
    TCHAR   szFileName[SYM_MAX_PATH+1];
    TCHAR   szAnsiName[SYM_MAX_PATH+1];

    // get the current depth

    hr = pObject->GetDepth(&dwDepth);

    if (FAILED(hr))
    {
        return hr;
    }

    if (dwDepth == 0)
    {
        // this is the actual archive file - don't scan.

        *pwResult       = DECRESULT_UNCHANGED;
        *pszNewDataFile = '\0';
        *pbContinue     = TRUE;

        return DEC_OK;
    }
    else if (dwDepth > 1)
    {
        // We only want to scan 1 level deep.  We should never get her
        // in this implementation.

        return DECERR_GENERIC;
    }

    // now we know we're an extracted file - set up and scan.
    // okay, get the object's file name

    hr = pObject->GetDataFile(szFileName);

    if (FAILED(hr))
    {
        return hr;
    }

    NavOemToAnsi(szFileName, szAnsiName);

    ValidateAndSaveFileInfo(m_lpNav,
                            szFileName,
                            szAnsiName,
                            TRUE);

    // Fill in the LFN and SFN in the NoNav structure
    NameReturnLongName( m_lpNav->Infected.szFullOemPath, m_lpNav->szInfectedLFN, FALSE );

    if ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() )
        GetShortPathName(m_lpNav->Infected.szFullOemPath, m_lpNav->szInfectedSFN, sizeof(m_lpNav->szInfectedSFN));
    else
        NameReturnShortName(m_lpNav->Infected.szFullOemPath, m_lpNav->szInfectedSFN);

    // Update main UI text
    IDecContainerObject* pParentObject;
    if( SUCCEEDED( pObject->GetParent( &pParentObject ) ) )
        {
        // Get item name.
        TCHAR szName[MAX_PATH];
        if( SUCCEEDED( pParentObject->GetDataFile( szName ) ) )
            {
            // Strip off path of parent file.
            NameReturnFile( szName, szName );

            // Format name for display.
            TCHAR szBuffer[MAX_PATH * 2];
            NameConvertFwdSlashes( m_lpNav->Zip.szZipChildFile );
            NameReturnFile( m_lpNav->Zip.szZipChildFile, szAnsiName );
            OemToChar( szAnsiName, szAnsiName );
            wsprintf( szBuffer, String_Ptr(IDS_SCANDLG_SCANING_COMPRESSED_FILE), szAnsiName, szName );

            // Blast to screen
            Static_SetText (GetDlgItem(m_lpNav->hMommyWindow, ID_SC_WORKINGON),
                            szBuffer );
            }
        }

    ScanTheFile(m_lpNav);

    *pwResult       = DECRESULT_UNCHANGED;
    *pszNewDataFile = '\0';
    *pbContinue     = FALSE;

    return DEC_OK;
#endif // _ALPHA_
}

//************************************************************************
// CNAVEventSink::OnChildFound(IDecContainerObject* pObject,
//                             LPCTSTR pszChildName, BOOL* pbContinue);
//
// Purpose:
//  Called by an engine when it first knows the name of one of its contained
//  files.
//
// Parameters:
//      pObject         Pointer to container object.
//      pszChildName    Pointer to child name within object.
//      pbContinue      Set to TRUE to continue or FALSE to halt processing.
//
// Returns:
//      DEC_OK          Signifies to decomposer that the error should be
//                      ignored.
//      eERROR          The error will propagate up the call chain, halting
//                      all further processing.
//
//************************************************************************
HRESULT CNAVEventSink::OnChildFound
(
    IDecContainerObject* pObject,
    LPCTSTR pszChildName,
    BOOL* pbContinue
)
{
#ifdef _ALPHA_
    // This shouldn't get called on ALPHA

    return DECERR_GENERIC;
#else
    TCHAR szFullPath[SYM_MAX_PATH+1];

    if (((STRLEN(pszChildName) + STRLEN(m_lpNav->szTheDirectory)) > SYM_MAX_PATH) ||
        ((STRLEN(pszChildName) + STRLEN(m_lpNav->Zip.lpZipFileName)) > SYM_MAX_PATH))
    {
        // here we just silently fail to process the child

        *pbContinue = FALSE;
        return DEC_OK;
    }

    // next, check extentions lists

    STRCPY(szFullPath, m_lpNav->szTheDirectory);
    NameAppendFile(szFullPath, pszChildName);

    if ( m_lpNav->Opt.bScanAll || FileInExeList(gszExtensions, szFullPath,
                                            m_lpNav->ExtendedOpts.bScanNoExtFile) )
    {
        STRCPY(m_lpNav->Zip.szZipChildFile, pszChildName);
        *pbContinue = TRUE;
    }
    else
    {
        *pbContinue = FALSE;
    }

    return DEC_OK;
#endif // _ALPHA_
}

//************************************************************************
// CNAVEventSink::OnNotIdentified(IDecObject* pObject, WORD* pwResult,
//                                LPTSTR pszNewDataFile);
//
// Purpose:
//  Called by Decomposer when a data object cannot be recognized as a
//  container by any of the engines.
//
// Parameters:
//      pObject         Pointer to the object.
//      pwResult        Pointer to a word indicating the result of the sink
//                      object's processing.
//      pszNewDataFile  Pointer to a character buffer containing the name of
//                      the replacement data file.
//
// Returns:
//      DEC_OK          Signifies to decomposer that the error should be
//                      ignored.
//      eERROR          The error will propagate up the call chain, halting
//                      all further processing.
//
//************************************************************************
HRESULT CNAVEventSink::OnNotIdentified
(
    IDecObject* pObject,
    WORD* pwResult,
    LPTSTR pszNewDataFile
)
{
#ifdef _ALPHA_
    // This shouldn't get called on ALPHA

    return DECERR_GENERIC;
#else
    // We return error here if the decomposer doesn't handle
    // the file.  Since we don't scan recursively this method
    // will only be called for the top level (parent) archive.
    // So, we don't need to check the depth level.
    //
    // This makes it easy for us to know whether the file was
    // "processed" by the decomposer.

    return DECERR_GENERIC;
#endif // _ALPHA_
}


//************************************************************************
// CNAVEventSink::Busy()
//
// Purpose:
//      This function is called when the decomposer needs to check for user
// input
//
// Parameters:
//
// Returns:
//      DEC_OK              - Operations should continue
//
//      DECERR_USER_CANCEL  - User has canceled.
//
//************************************************************************
HRESULT CNAVEventSink::OnBusy()
{
    // Check for cancel
    if( m_lpNav->bDoScan == FALSE )
        {
        return DECERR_USER_CANCEL;
        }

    return DEC_OK;
}

//************************************************************************
// CNAVEventSink::OnBadChild()
//
// Purpose:
//      This function is called when the decomposer encounters a compression
//      error in an archive.  In our case, we will ignore the error and continue.
//
// Returns:
//      DEC_OK              - Operations should continue
//
//************************************************************************
HRESULT CNAVEventSink::OnBadChild(
		IDecContainerObject* pObject, 
		HRESULT hr,
		LPCTSTR pszChildName,
		BOOL* pbContinue)
    {
    //
    // We want to go on our merry way. 
    //
    *pbContinue = TRUE;
    return DEC_OK;
    }


BOOL NAVDecomposerInit()
{
#ifdef _ALPHA_
    return TRUE;
#else
    // only initialize the decomposer once
    if (bInitialized) return TRUE;

    // instantiate our decomposer
    if FAILED(DecNewDecomposer(&pgDecomposer)) goto Cleanup;

    // instantiate our decomposer engines
    if (FAILED(DecNewAMGEngine(&pAMGEngine)))   goto Cleanup;
    if (FAILED(DecNewARJEngine(&pARJEngine)))   goto Cleanup;
	if (FAILED(DecNewCabEngine(&pCABEngine)))   goto Cleanup;
	if (FAILED(DecNewGZIPEngine(&pGZIPEngine)))   goto Cleanup;
	if (FAILED(DecNewHQXEngine(&pHQXEngine)))   goto Cleanup;
    if (FAILED(DecNewIDEngine(&pIDEngine)))     goto Cleanup;
    if (FAILED(DecNewLHAEngine(&pLHAEngine)))   goto Cleanup;
	if (FAILED(DecNewLZEngine(&pLZEngine)))     goto Cleanup;
    if (FAILED(DecNewMimeEngine(&pMIMEEngine))) goto Cleanup;
	if (FAILED(DecNewSSEngine(&pSSEngine)))     goto Cleanup;
	if (FAILED(DecNewTAREngine(&pTAREngine)))   goto Cleanup;
	if (FAILED(DecNewUUEEngine(&pUUEEngine)))   goto Cleanup;
    if (FAILED(DecNewZipEngine(&pZIPEngine)))   goto Cleanup;


    // *ALWAYS* add the ID engine first.  This allows the other
    // engines to benefit from any information the ID engine can
    // gather.  This is also NECESSARY for self-extracting CAB files
    // to be decomposed since the CAB engine relies on information
    // from the ID engine.
    if (FAILED(pgDecomposer->AddEngine(pIDEngine)))   goto Cleanup;
    
    // add our engines to our decomposer
    if (FAILED(pgDecomposer->AddEngine(pAMGEngine)))  goto Cleanup;
    if (FAILED(pgDecomposer->AddEngine(pARJEngine)))  goto Cleanup;
    if (FAILED(pgDecomposer->AddEngine(pCABEngine)))  goto Cleanup;
    if (FAILED(pgDecomposer->AddEngine(pGZIPEngine))) goto Cleanup;
    if (FAILED(pgDecomposer->AddEngine(pHQXEngine)))  goto Cleanup;
    if (FAILED(pgDecomposer->AddEngine(pLHAEngine)))  goto Cleanup;
    if (FAILED(pgDecomposer->AddEngine(pLZEngine)))   goto Cleanup;
    if (FAILED(pgDecomposer->AddEngine(pMIMEEngine))) goto Cleanup;
    if (FAILED(pgDecomposer->AddEngine(pSSEngine)))   goto Cleanup;
    if (FAILED(pgDecomposer->AddEngine(pTAREngine)))  goto Cleanup;
    if (FAILED(pgDecomposer->AddEngine(pUUEEngine)))  goto Cleanup;
    if (FAILED(pgDecomposer->AddEngine(pZIPEngine)))  goto Cleanup;


    //
    // set the decomposer's temp dir.  We need to make sure 
    // that the temporary folder exists.  We don't want to use the current
    // directory, because when NAVW is launched by the Win95 shell, the
    // current directory could be the desktop folder.  This causes
    // AP to catch files created there when the shell opens the files
    // to extract icon information.
    //
    // If there is no temporary environment variable, use the NAV 
    // program folder for temporary files.
    //
    TCHAR szTempBuffer[ MAX_PATH ];    
    if( GetEnvironmentVariable( _T("TEMP"), szTempBuffer, MAX_PATH ) == 0 &&
        GetEnvironmentVariable( _T("TMP"), szTempBuffer, MAX_PATH ) == 0 )
        {
        //
        // Figure out where this module lives
        //
        if( 0 == GetModuleFileName( hInstance, szTempBuffer, MAX_PATH ) ) 
            goto Cleanup;
        
        //
        // Strip off module filename
        //
        NameReturnPath( szTempBuffer, szTempBuffer );
        
        //
        // Tell decomposer where we want temp files going
        if (FAILED(pgDecomposer->SetTempDir(szTempBuffer))) goto Cleanup;
        }
    else
        {
        //
        // Have at least 1 temporary folder on this system.  Let windows handle this
        // for us.
        //
        if (FAILED(pgDecomposer->SetTempDir(NULL))) goto Cleanup;
        }

    bInitialized = TRUE;

    return TRUE;

Cleanup:

    if (pgDecomposer) pgDecomposer->RemoveEngines();

    if (pAMGEngine)   DecDeleteAMGEngine(pAMGEngine);
    if (pARJEngine)   DecDeleteARJEngine(pARJEngine);
    if (pCABEngine)   DecDeleteCabEngine(pCABEngine);
    if (pGZIPEngine)   DecDeleteGZIPEngine(pGZIPEngine);
    if (pHQXEngine)   DecDeleteHQXEngine(pHQXEngine);
    if (pIDEngine)    DecDeleteIDEngine(pIDEngine);
    if (pLHAEngine)   DecDeleteLHAEngine(pLHAEngine);
    if (pLZEngine)    DecDeleteLZEngine(pLZEngine);
    if (pMIMEEngine)  DecDeleteMimeEngine(pMIMEEngine);
    if (pSSEngine)    DecDeleteSSEngine(pSSEngine);
    if (pTAREngine)   DecDeleteTAREngine(pTAREngine);
    if (pUUEEngine)   DecDeleteUUEEngine(pUUEEngine);
    if (pZIPEngine)   DecDeleteZipEngine(pZIPEngine);

    if (pgDecomposer) DecDeleteDecomposer(pgDecomposer);


    pgDecomposer = NULL;

    return FALSE;
#endif // _ALPHA_
}

void NAVDecomposerEnd()
{
#ifdef _ALPHA_
    return;
#else
    if (!bInitialized) return;

    pgDecomposer->RemoveEngines();

    DecDeleteAMGEngine(pAMGEngine);
    DecDeleteARJEngine(pARJEngine);
    DecDeleteCabEngine(pCABEngine);
    DecDeleteGZIPEngine(pGZIPEngine);
    DecDeleteHQXEngine(pHQXEngine);
    DecDeleteIDEngine(pIDEngine);
    DecDeleteLHAEngine(pLHAEngine);
    DecDeleteLZEngine(pLZEngine);
    DecDeleteMimeEngine(pMIMEEngine);
    DecDeleteSSEngine(pSSEngine);
    DecDeleteTAREngine(pTAREngine);
    DecDeleteUUEEngine(pUUEEngine);
    DecDeleteZipEngine(pZIPEngine);

    DecDeleteDecomposer(pgDecomposer);



    bInitialized = FALSE;
#endif // _ALPHA_
}




