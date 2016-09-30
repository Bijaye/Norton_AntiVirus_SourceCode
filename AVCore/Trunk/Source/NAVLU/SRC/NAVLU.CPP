// Copyright 1996-1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/navlu.cpv   1.38   28 Aug 1998 16:17:46   JBRENNA  $
//
// Description:
//   Integrates LiveUpdate into NAV. This file was moved from the ONRAMP
//   project to here.
//
// Contains:
//
// See Also:
//****************************************************************************
// $Log:   S:/NAVLU/VCS/navlu.cpv  $
// 
//    Rev 1.38   28 Aug 1998 16:17:46   JBRENNA
// Port changes from QAKN branch to trunk:
// 1. Rev 1.37.1.5   20 Jul 1998 15:13:26   tcashin
//    Added code to see if MicroDef updates were delivered.
// 2. Rev 1.37.1.4   15 Jul 1998 16:09:48   TCASHIN
//    Make sure we're using the right MicroDef product name for DEC/Alpha.
// 3. Rev 1.37.1.3   01 Jul 1998 14:53:16   tcashin
//    Make sure we delete the patch progress dialog class. It had been done in
//    the cdefutils destructor ... wrong!
// 4. Rev 1.37.1.2   29 Jun 1998 14:27:30   tcashin
//    Process the /setup switch. If used, don't do the reboot dialogs or text.
// 5. Rev 1.37.1.1   26 Jun 1998 15:47:40   tcashin
//    Removed paragraph in WelcomeOAM about old defs.
// 
//    Rev 1.37   15 Jun 1998 14:08:00   tcashin
// Disable optimizations. The optimized code was always creating the patch
// progress dialog box.
// 
//    Rev 1.36   15 Jun 1998 10:59:42   tcashin
// Make sure the patching progress dialog only get created when DoOnramp()
// completes successfully.
// 
//    Rev 1.35   13 Jun 1998 16:32:10   tcashin
// Tweaked the welcome and finish panel text.
// 
//    Rev 1.34   08 Jun 1998 15:12:22   tcashin
// If the MicroDef update fails, put the right error message in the
// finish panel. Get rid of the message box that had been displaying the
// error since it was in the wrong place anyway.
// 
//    Rev 1.33   07 Jun 1998 17:56:34   tcashin
// Need to make sure m_SetSeqNumToZero is set to false if the Hubs are ok.
// (#117114).
// 
//    Rev 1.32   27 May 1998 15:05:30   TCASHIN
// Misc. fixes: made MicroDef-related structs be private data member in the
// navlu class, force sequence number to zero if defutils says the hubs are bad,
// set seq. numbers for binary and text hubs.
// 
//    Rev 1.31   20 May 1998 16:12:18   tcashin
// Added a progress callback that's used during PostMicroDefUpdate() when
// we're not is silent mode.
// 
//    Rev 1.30   27 Apr 1998 23:42:50   tcashin
// BuildCmdLine() needs to count from the 1st reg key. A count value
// had been initialized to 2 to ignore the first on during testing.
// 
//    Rev 1.28   27 Apr 1998 14:35:02   RStanev
// Added def annuity.
//
//    Rev 1.27   22 Apr 1998 13:30:26   tcashin
// Save/restore the ESI register around PreMicroDefUpdate on Intel only.
//
//    Rev 1.26   20 Apr 1998 23:18:48   tcashin
// Select the MicroDef packages on the server based on product name
// and version.
//
// Also added a temporary fix around the call to PreMicroDefUpdate().
// Seems that someone is trashing esi and not restoring it. This
// needs to be looked at.
//
//    Rev 1.25   16 Apr 1998 10:55:48   tcashin
// If PostMicroDefUpdate() returns an error, check the Onramp return
// value before complaining to the user ... he might have cancelled
// or had some other problem.
//
//    Rev 1.24   09 Apr 1998 17:10:12   mdunn
// Added #define INITGUID and a couple of #include's so that the Windows 98
// Task Scheduler GUIDs are set up properly.
//
//    Rev 1.23   26 Mar 1998 15:36:10   tcashin
// Oops ... missed a change re: microdefs fix below.
//
//    Rev 1.22   26 Mar 1998 15:25:18   tcashin
// Fixed a problem updating command line reg key for Microdefs.
//
//    Rev 1.21   23 Mar 1998 17:06:52   tcashin
// Minor change when building LU command line to make sure
// we're only asking for MicroDefs.
//
//    Rev 1.20   16 Mar 1998 17:23:50   tcashin
// Added support for MicroDefs.
//
//    Rev 1.19   09 Mar 1998 20:47:34   tcashin
// Back out the changes for MicroDefs until we get
// AVCOMP build issues resolved.
//
//    Rev 1.17   11 Sep 1997 13:23:04   MKEATIN
// Ported changes from branch G.
//
//    Rev 1.15.1.2   25 Aug 1997 15:32:58   JBRENNA
// Update RegOpens to use KEY_READ rather than KEY_ALL_ACCESS.
//
//    Rev 1.15.1.1   14 Aug 1997 14:19:58   KSACKIN
// Fixed a problem with now passing enough parameters to the VLRegisterVirusDB()
// call.  We now pass a NULL as the last parameter for the NavExInf file name.
//
//    Rev 1.15.1.0   12 Aug 1997 22:57:40   DLEWIS
// Branch base for version QAKG
//
//    Rev 1.15   01 Aug 1997 08:46:36   JBRENNA
// Remove the unneeded MessageBox warning the user that the LiveUpdate buffer
// was too small in WelcomeOAM.
//
//    Rev 1.14   18 Jul 1997 15:31:50   JBRENNA
// Rework the OAM_SUCCESS finish panel. Display the number of viruses and, if
// there was an increase, the number of additional viruses.
//
//    Rev 1.13   01 Jul 1997 17:00:56   JBRENNA
// Now have the Shared Tech LiveUpdate parented off of our bogus main window.
//
//    Rev 1.12   18 Jun 1997 20:30:34   JBRENNA
// Add a call to NAVSetDefsPath before calling VLRegisterVirusDB. The
// NAVSetDefsPath notifies our AVAPI1 callbacks the definition file location.
//
//    Rev 1.11   18 Jun 1997 20:17:48   JBRENNA
// In FinishOAM, handle a OAM_SUCCESSREBOOT in the same way as OAM_SUCCESS.
//
//    Rev 1.10   18 Jun 1997 14:02:34   JBRENNA
// 1. In FinishOAM, call PostDefUpdate all of the time. This ensures that
//    the Post processing occurs. Potentially, we could have got a package
//    that requires rebooting. If the user reboots, the definitions downloaded
//    will now be integrated before the reboot occurs.
// 2. Add gstNAVCallBacks to our VLScanInit call.
//
//    Rev 1.9   12 Jun 1997 09:45:24   JBRENNA
// Change the 3rd parameter to PreDefUpdate from LiveUpdate to
// CDefUtils::LiveUpdate. The DefUtils API changed.
//
//    Rev 1.8   06 Jun 1997 13:51:52   JBRENNA
// Use a YYYYMMDDHH format for the LiveUpdate sequence number. Where YYYY is
// the year, MM is the month, DD is the day, and HH is the hour.
//
//    Rev 1.7   04 Jun 1997 16:19:58   JBRENNA
// Use VLNumClaimedVirusEntries instead of VLNumVirusEntries. The "Claimed"
// version is the number of viruses we claim to detect. This matches the
// "#### Viruses Detect" string in the virus list dialog.
//
//    Rev 1.6   03 Jun 1997 13:21:44   JBRENNA
// Move some strings to the STRINGS.H file. This enables string sharing
// by having only 1 pointer to a string that is used multiple times in the
// application.
//
//    Rev 1.5   02 Jun 1997 18:07:44   JBRENNA
// Do a lot of clean up:
// 1. Added function comments,
// 2. Removed #ifdefs for HAWKINGs,
// 3. Moved GetLuGetPathName to NavLuUtils.cpp:NavLuGetPathName.
// 4. Moved MyFileExists to     NavLuUtils.cpp:NavLuFileExists.
// 5. Moved MyNameAppendFile to NavLuUtils.cpp:NavLuNameAppendFile.
//
//    Rev 1.4   01 Jun 1997 18:50:02   JBRENNA
// 1. Move class constructor to this file. Previously it was inlined.
// 2. Add support for telling the user how many viruses they are protected from.
// 3. Use the Hawking library (DefUtils.LIB) for managing shared definitions.
// 4. Change the LiveUpdate sequence number to be based on the VirScan1.DAT's
//    version number rather than its date.
// 5. HACK: do not display szParagraph3 in WelcomeOAM when the definitions are
//    older than 1 month. If we include szParagraph3, we overright memory
//    because we would copy 516 characters into a 512 character buffer.
// 6. HACK: Pop a MessageBox to note the problem noted in the previous item.
//    This ensures we do not forget about the problem.
//
//    Rev 1.3   23 May 1997 21:27:48   JBRENNA
//
//    Rev 1.2   16 May 1997 11:02:26   JBRENNA
// 1. Remove dependency on SymKrnl library.
// 2. Fetch virus definition's date from CATALOG.DAT instead of VIRSCAN.DAT.
// 3. Dynamically build the LiveUpdate command line based on registry values.
//
//    Rev 1.1   10 May 1997 02:03:20   JBRENNA
// Use math rather than string manipulation in order to get VirScan's date
// stamp into the sequence number.
//
//    Rev 1.0   10 May 1997 01:21:06   JBRENNA
// Initial revision.
//
//    Rev 1.12   18 Oct 1996 19:26:58   MKEATIN
// Defined hDelayKey
//
//    Rev 1.11   18 Oct 1996 19:22:14   MKEATIN
// Ported Gryphon code for handling files in use.
//
//    Rev 1.10   30 Sep 1996 16:49:40   MKEATIN
// Changed the LiveUpdate reg value to LiveUpdate1
//
//    Rev 1.9   30 Sep 1996 16:21:40   MKEATIN
// Changed title for the OAM_NOTHINGNEW condition on the Finish Panel.
//
//    Rev 1.8   04 Sep 1996 14:47:36   JBRENNA
// Add run-time check to determine if we should use "NAVNT" or "NAV95" for
// the "HKLM\SOFTWARE\Symantec\InstalledApps" value in IsOldThanOneMonth
// and SetSequenceDate.
//
//    Rev 1.7   30 Jul 1996 11:26:28   MKEATIN
// Ported recent changes from LUIGI
//
//    Rev 1.6   16 Jul 1996 12:27:46   MKEATIN
// Changed NAV95 to NAVNT
//
//    Rev 1.5   16 Jul 1996 12:24:32   MKEATIN
// SetSequenceNumber to zero if VIRSCAN.DAT is not found
//
//    Rev 1.4   12 Jul 1996 10:00:08   MKEATIN
// Call SetSequenceEx() instead of SetSequence().  Also, added RegCloseKey()
// for each RegOpenKey().
//
//    Rev 1.3   17 Jun 1996 18:29:08   MKEATIN
//
//
//    Rev 1.2   17 Jun 1996 18:26:06   MKEATIN
// The onramp command line now comes from the Language area
//
//    Rev 1.1   17 Jun 1996 16:23:02   MKEATIN
// Added bReboot parameter to CallOnramp().  Reboot decisions should be made
// prior to calling CallOnramp().
//
//    Rev 1.0   17 Jun 1996 15:18:48   MKEATIN
// Initial revision.
//****************************************************************************

#include "StdAfx.h"
#include <stdlib.h>
#include "platform.h"                   // Platform independence

#define INITGUID
#include <objbase.h>
#include <objidl.h>

#include "file.h"
#include "xapi.h"
#include <onramp.h>
#include "resource.h"
#include "virscan.h"
#include "avapi.h"
#include "defutils.h"
#include "NavLu.h"
#include "NavLuUtils.h"
#include "MyUtils.h"
#include "strings.h"
#include "navcb.h"

#include "DefAnnuity.h"
#include "navluprog.h"

typedef int (WINAPI *LPFUNC_DOONRAMP)(HINSTANCE,HWND,LPTSTR,LPSOnrampAutoMode);

//****************************************************************************
// NAV OnRamp Constants ******************************************************
//****************************************************************************

const int IDB_LU_WELCOME_BITMAP = 327;
const int IDB_LU_FINISH_BITMAP  = 329;

//****************************************************************************
// Static variable ***********************************************************
//****************************************************************************

static CNavLu *s_pNavLu = NULL;


//****************************************************************************
// Local Defines   ***********************************************************
//****************************************************************************

#define VERSION_BUFFER_SIZE     16


//****************************************************************************
// Interesting Pragmas  ******************************************************
//****************************************************************************

#pragma optimize( "", off )                 // Turned back on at end of file


//****************************************************************************
// Static functions **********************************************************
//****************************************************************************

static BOOL WINAPI NavWelcomeOAM(LPTSTR lpszBuffer, WORD wSizeOfBuffer, WORD wStatus, LPTSTR lpszTitle, LPDWORD lpdwUserData)
{
    BOOL bResult = FALSE;

    if (s_pNavLu)
        bResult = s_pNavLu->WelcomeOAM (lpszBuffer, wSizeOfBuffer, wStatus, lpszTitle, lpdwUserData);

    return bResult;
}

static BOOL WINAPI NavFinishOAM(LPTSTR lpszBuffer, WORD wSizeOfBuffer, WORD wStatus, LPTSTR lpszTitle, LPDWORD lpdwUserData)
{
    BOOL bResult = FALSE;

    if (s_pNavLu)
        bResult = s_pNavLu->FinishOAM (lpszBuffer, wSizeOfBuffer, wStatus, lpszTitle, lpdwUserData);

    return bResult;
}

//****************************************************************************
// CNavLu function implementations *******************************************
//****************************************************************************

CNavLu::CNavLu (HINSTANCE hInstance, BOOL bReboot, BOOL bSetup /* Both = FALSE */) :
    m_bReboot                      (bReboot),
    m_hInstance                    (hInstance),
    m_wLuFinishStatus              (0),
    m_uStartNumViruses             (0),
    m_pDefUtils                    (NULL),
    m_bDefUtilsPostDefUpdateCalled (FALSE),
    m_pPAProgress                  (NULL)
{
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Actually initiate a Shared Tech LiveUpdate session. This routine
//   does the setup work and then calls the private CallLiveUpdate function.
//
// Parameters:
//   hParent             [in] Parent window for LiveUpdate.
//   bSilent             [in] T/F: Run LiveUpdate in silent mode?
//   bFilterVirusDefType [in] FALSE if everything should be downloaded,
//                       otherwise ProductType "VirusDef" will be
//                       filtered and not downloaded.
//   bSetup              [in] TRUE if the /SETUP command line switch
//                       was used.
//
// Returns
//   The OAM_* result code from LiveUpdate.
//////////////////////////////////////////////////////////////////////////
WORD CNavLu::DoLiveUpdate (HWND hParent, BOOL bSilent, BOOL bFilterVirusDefType, BOOL bSetup )
{
    CString         strCmdLine;
    BOOL            bSuccess;
    TCHAR           szBinaryPatchDir[_MAX_PATH] = {0};
    TCHAR           szBinaryPatchVersion[VERSION_BUFFER_SIZE] = {0};
    TCHAR           szTextPatchDir[_MAX_PATH] = {0};
    TCHAR           szTextPatchVersion[VERSION_BUFFER_SIZE] = {0};

    memset(&m_MicroDefInfo, 0, sizeof(m_MicroDefInfo));
    memset(&m_MicroDefUpdate, 0, sizeof(m_MicroDefUpdate));

    // Remember the pointer so that NavWelcomeOAM and NavFinishOAM
    // can reference this class instance.
    s_pNavLu = this;

    // If anything does not work correctly, return OAM_ERROR result;
    m_wLuFinishStatus = OAM_ERROR;

    // Memorize the number of viruses for later comparision.
    m_uStartNumViruses = GetNumVirusEntries();

    // Allocate a new CDefUtils class
    m_pDefUtils = new CDefUtils;

    // Remember if we're called from NAV's Setup
    m_bSetup = bSetup;

    // &?  Temporary hack Part 1
    //
    //     There is a problem with PreMicroDefUpdate or one of the new DLLs
    //     that it calls,  Specifically, the esi register, the pointer to the
    //     application object, is not restored.
    //
    //     The very temporary hack is to push and pop esi around the call
    //     to PreMicroDefUpdate

#ifndef _ALPHA_
    _asm push esi
#endif
    // End Temporary hack Part 1
    
    m_MicroDefInfo.lpszBinaryPatchDir  = szBinaryPatchDir;
    m_MicroDefInfo.lpszBinaryVersion   = szBinaryPatchVersion;
    m_MicroDefInfo.wBinaryPatchDirSize = sizeof(szBinaryPatchDir);
    m_MicroDefInfo.wBinaryVersionSize  = VERSION_BUFFER_SIZE;

    m_MicroDefInfo.lpszTextPatchDir    = szTextPatchDir;
    m_MicroDefInfo.lpszTextVersion     = szTextPatchVersion;
    m_MicroDefInfo.wTextPatchDirSize   = sizeof(szBinaryPatchDir);
    m_MicroDefInfo.wTextVersionSize    = VERSION_BUFFER_SIZE;

    // If we're not running in silent mode, the set up the progress dialog. It
    // will be called when patches are applied in PostMicroDefUpdate().

    if (!bSilent)
    {
        m_pPAProgress = new CLuPatchProgress;
        m_MicroDefInfo.pCProgress = m_pPAProgress;
    }

    // PreMicroDefUpdate sets up the Hawking environment
    bSuccess = m_pDefUtils->PreMicroDefUpdate (&m_MicroDefInfo, CDefUtils::LiveUpdate);

    // If either hub directory is "broken" (files are missing) then set it's
    // sequence number to zero to force LiveUpdate to get it.
    if (!m_MicroDefInfo.bBinaryHubIsOk || !m_MicroDefInfo.bTextHubIsOk)
        m_SetSeqNumToZero = TRUE;
    else
        m_SetSeqNumToZero = FALSE;
    
    // &?  Temporary hack Part 2
    //
    //     Restore the esi register.

#ifndef _ALPHA_
    _asm pop esi
#endif

    // End Temporary hack Part 2

    m_bDefUtilsPostDefUpdateCalled = FALSE;

    // If we failed to build the command line, tell the user.
    if (bSuccess)
    {

        // Note that even if we cannot edit the LU version
        // field, we use the default and continue.
        EditMicroDefVersion(m_MicroDefInfo.lpszBinaryVersion);
        EditMicroDefVersion(m_MicroDefInfo.lpszTextVersion);

        // Attempt to build the LiveUpdate command line.
        bSuccess = BuildCmdLine (&strCmdLine, bSilent, bFilterVirusDefType );

        // If we failed to build the command line, tell the user.
        if (!bSuccess)
        {
            CString strErrorTitle;
            CString strError;

            strErrorTitle.LoadString (IDS_ERR_NO_CMDLINES_TITLE);
            strError.LoadString      (IDS_ERR_NO_CMDLINES);

            MessageBox (NULL, strError, strErrorTitle, MB_OK);
        }
        else
        {
            CallLiveUpdate(hParent, strCmdLine);
        }
    }

    // Potentially the PostMicroDefUpdate was called from the FinishOAM
    // callback. But remember that FinishOAM is not called when running
    // LiveUpdate in silent mode.
    if (!m_bDefUtilsPostDefUpdateCalled)
    {
        if (m_wLuFinishStatus == OAM_SUCCESS)
        {
            if (m_pPAProgress)
                m_pPAProgress->Create(); 

            m_MicroDefUpdate.bProcessPatches = TRUE;
        }
        else
            m_MicroDefUpdate.bProcessPatches = FALSE;
  
        bSuccess = m_pDefUtils->PostMicroDefUpdate(&m_MicroDefUpdate);
        
        if (m_pPAProgress)
            m_pPAProgress->DestroyWindow();
      }

    if (m_pPAProgress)
        delete m_pPAProgress;

    delete m_pDefUtils;
    m_pDefUtils = NULL;

    s_pNavLu           = NULL;
    m_uStartNumViruses = 0;

    return m_wLuFinishStatus;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Dynamically load the LiveUpdate DLL, update the sequence number,
//   and call DoOnramp (LiveUpdate entry point).
//
// Parameters:
//   hParent     [in] Handle to the parent window.
//   strCmdLine  [in] The LiveUpdate command line.
//////////////////////////////////////////////////////////////////////////
void CNavLu::CallLiveUpdate (HWND hParent, const CString &strCmdLine)
{
    TCHAR szFile[_MAX_PATH+1];

    SOnrampAutoMode autoMode =
    {
        {IDB_LU_WELCOME_BITMAP, NavWelcomeOAM, NULL, NULL},
        {IDB_LU_FINISH_BITMAP,  NavFinishOAM,  NULL, NULL}
    };

    LoadString(m_hInstance, IDS_CAPTION,       autoMode.fWelcome.fCaption, sizeof(autoMode.fWelcome.fCaption));
    LoadString(m_hInstance, IDS_CAPTION,       autoMode.fFinish.fCaption,  sizeof(autoMode.fFinish.fCaption));
    LoadString(m_hInstance, m_DefAnnuityStatus != DefAnnuityUpdatePatches ? IDS_WELCOME_TITLE : IDS_WELCOME_TITLE_PATCHES, autoMode.fWelcome.fTitle,   sizeof(autoMode.fWelcome.fTitle));

    // If anything does not work correctly, return OAM_ERROR result;
    m_wLuFinishStatus = OAM_ERROR;

    NavLuGetLuPathName (szFile, sizeof(szFile));
    HINSTANCE hLuInstance = LoadLibrary (szFile);

    if (hLuInstance)
    {
                            // --------------------------------
                            // SYMMAL32.DLL was loaded. Find
                            // the entry functions DoOnramp()
                            // and SetSquenceNumber()
                            // --------------------------------
        LPFUNC_DOONRAMP pfnDoOnramp =
            (LPFUNC_DOONRAMP) (GetProcAddress (hLuInstance, "DoOnramp"));

        LPFUNC_SETSEQUENCENUMBER pfnSetSequenceNumber =
            (LPFUNC_SETSEQUENCENUMBER) (GetProcAddress (hLuInstance, "SetSequenceNumberEx"));

        if (pfnSetSequenceNumber)
            SetSequenceNumber(pfnSetSequenceNumber, m_SetSeqNumToZero);

        if (pfnDoOnramp)
        {
            m_wLuFinishStatus =
                (*pfnDoOnramp)(m_hInstance, hParent, (LPTSTR)(LPCTSTR)strCmdLine, &autoMode);
        }
        FreeLibrary (hLuInstance);
    }
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Setup the strings for the Welcome LiveUpdate panel. Note that this
//   is not called by LiveUpdate when we are running is silent mode.
//
// Parameters:
//   lpszBuffer     [out] New string for the text body.
//   wSizeOfBuffer  [in]  Maximum number of characters in lpszBuffer.
//   wStatus        [???] ?????
//   lpszTitle      [out] New title string.
//   lpdwUserData   [i/o] Misc flags
//
// Returns
//   TRUE always. Not sure what a FALSE return would do.
//////////////////////////////////////////////////////////////////////////
BOOL CNavLu::WelcomeOAM (LPTSTR lpszBuffer, WORD wSizeOfBuffer, WORD wStatus, LPTSTR lpszTitle, LPDWORD lpdwUserData) const
{
    BOOL      ret           = TRUE;
    TCHAR     szParagraph1[512];
    TCHAR     szParagraph3[512];

    if ( m_DefAnnuityStatus != DefAnnuityUpdatePatches )
    {
        // Load paragraphs 1 and 3
        LoadString(m_hInstance, IDS_WELCOME_TEXT1, szParagraph1, sizeof(szParagraph1));
        LoadString(m_hInstance, IDS_WELCOME_TEXT3, szParagraph3, sizeof(szParagraph3));

        wsprintf (lpszBuffer, "%s\n\n%s", szParagraph1, szParagraph3);

        // Make sure we do not overrun the buffer.
        ASSERT (_tcslen (lpszBuffer) < wSizeOfBuffer);
    }
    else
    {
        LoadString (m_hInstance, IDS_WELCOME_TEXT_PATCHES, lpszBuffer, wSizeOfBuffer);
    }
    return ret;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Setup the strings for the Finish LiveUpdate panel. Note that this
//   is not called by LiveUpdate when we are running is silent mode.
//
// Parameters:
//   lpszBuffer     [out] New string for the text body.
//   wSizeOfBuffer  [in]  Maximum number of characters in lpszBuffer.
//   wStatus        [in]  OAM_* status code from LiveUpdate.
//   lpszTitle      [out] New title string.
//   lpdwUserData   [i/o] Misc flags
//
// Returns
//   TRUE always. Not sure what a FALSE return would do.
//////////////////////////////////////////////////////////////////////////
BOOL CNavLu::FinishOAM(LPTSTR lpszBuffer, WORD wSizeOfBuffer, WORD wStatus, LPTSTR lpszTitle, LPDWORD lpdwUserData)
{
    BOOL      ret            = TRUE;
    UINT      uCurNumViruses;
    int       nVirusesDelta;
    UINT      uTextId;
    TCHAR     szFmt[512];
    TCHAR     szText[512];
    HKEY      hDelayKey;
    BOOL      bMicroDefUpdate = FALSE;
    BOOL      bMicroDefsDelivered = FALSE;

    m_wLuFinishStatus = wStatus;

    memset(&m_MicroDefUpdate, 0, sizeof(m_MicroDefUpdate));

    // Call PostMicroDefUpdate so that the GetNumVirusEntries call will
    // be from the new definitions directory. Also, this ensures that
    // we cleanup before LiveUpdate potentially reboots the system.

    if (wStatus == OAM_SUCCESS)
    {
        if ( bMicroDefsDelivered = CheckVirusDefsDelivered(&m_MicroDefInfo) )
        {
            if (m_pPAProgress)
                m_pPAProgress->Create(); 

            m_MicroDefUpdate.bProcessPatches = TRUE;
        }
        else
            m_MicroDefUpdate.bProcessPatches = FALSE;
    }
    else
        m_MicroDefUpdate.bProcessPatches = FALSE;

     bMicroDefUpdate = m_pDefUtils->PostMicroDefUpdate(&m_MicroDefUpdate);

    if (m_pPAProgress)
        m_pPAProgress->DestroyWindow();

    m_bDefUtilsPostDefUpdateCalled = TRUE;

    switch (wStatus)
    {
        case OAM_SUCCESS:
        case OAM_SUCCESSREBOOT:

            if ((m_DefAnnuityStatus != DefAnnuityUpdatePatches) && bMicroDefsDelivered)
            {
                if (bMicroDefUpdate == TRUE)
                {
                    // Calculate the number of new viruses we scan for.
                    uCurNumViruses = GetNumVirusEntries ();
                    nVirusesDelta  = uCurNumViruses - m_uStartNumViruses;

                    // Load Finish Panel text according to 'wStatus'

                    LoadString (m_hInstance, IDS_FINISH1_TITLE, lpszTitle, OAM_MAXSTRING);

                    LoadString (m_hInstance, IDS_FINISH_SUCCESS1, szText, sizeof(szText));
                    _tcscpy (lpszBuffer, szText);

                    // Ensure the proper string id is used depending on the
                    // changes between the start of LU and here.
                    if (nVirusesDelta > 1)
                        uTextId = IDS_FINISH_MORE_VIRUSES;
                    else if (nVirusesDelta == 1)
                        uTextId = IDS_FINISH_MORE_VIRUS;
                    else
                        uTextId = IDS_FINISH_NUM_VIRUSES;

                    if (uTextId)
                    {
                        // Make sure this is always a positive value.
                        if (nVirusesDelta < 0)
                            nVirusesDelta = -nVirusesDelta;

                        LoadString (m_hInstance, uTextId, szFmt, sizeof(szFmt));
                        wsprintf (szText, szFmt, uCurNumViruses, nVirusesDelta);
                        _tcscat (lpszBuffer, szText);
                    }
                }
                else
                {
                    // MicroDef Update failed. Don't reboot.
                    m_bReboot = FALSE;

                    // Put up the info the the defs were not updated.
                    LoadString (m_hInstance, IDS_FINISH2_TITLE, lpszTitle, OAM_MAXSTRING);
                    LoadString (m_hInstance, IDS_FINISH_ERROR_MICRODEF, szText, sizeof(szText));

                    _tcscpy (lpszBuffer, szText);

                }
            }
            else
            {
                LoadString (m_hInstance, IDS_FINISH1_TITLE_PATCHES, lpszTitle, OAM_MAXSTRING);
                LoadString (m_hInstance, IDS_FINISH_SUCCESS1_PATCHES, lpszBuffer, sizeof(szText)/sizeof(*szText));
            }

            // If m_bReboot is TRUE and we were not called from Setup, offer rebooting and load appropriate text
            if (m_bReboot && !m_bSetup)
            {
                *lpdwUserData = 1;

                LoadString (m_hInstance, m_DefAnnuityStatus != DefAnnuityUpdatePatches ? IDS_FINISH_SUCCESS2 : IDS_FINISH_SUCCESS2_PATCHES, szText, sizeof(szText));
                STRCAT (lpszBuffer, szText);
            }

            break;

        case OAM_USERCANCEL:
            if (m_DefAnnuityStatus != DefAnnuityUpdatePatches)
            {
                LoadString (m_hInstance, IDS_FINISH2_TITLE, lpszTitle, OAM_MAXSTRING);
                LoadString (m_hInstance, IDS_FINISH_USERCANCEL, lpszBuffer, wSizeOfBuffer);
            }
            else
            {
                LoadString (m_hInstance, IDS_FINISH2_TITLE_PATCHES, lpszTitle, OAM_MAXSTRING);
                LoadString (m_hInstance, IDS_FINISH_USERCANCEL_PATCHES, lpszBuffer, wSizeOfBuffer);
            }
            break;

        case OAM_NOTHINGNEW:
            if (m_DefAnnuityStatus != DefAnnuityUpdatePatches)
            {
                LoadString (m_hInstance, IDS_FINISH3_TITLE, lpszTitle, OAM_MAXSTRING);
                LoadString (m_hInstance, IDS_FINISH_NOTHINGNEW, lpszBuffer, wSizeOfBuffer);
            }
            else
            {
                LoadString (m_hInstance, IDS_FINISH3_TITLE_PATCHES, lpszTitle, OAM_MAXSTRING);
                LoadString (m_hInstance, IDS_FINISH_NOTHINGNEW_PATCHES, lpszBuffer, wSizeOfBuffer);
            }
            break;

        case OAM_HOSTBUSY:
            if (m_DefAnnuityStatus != DefAnnuityUpdatePatches)
            {
                LoadString (m_hInstance, IDS_FINISH2_TITLE, lpszTitle, OAM_MAXSTRING);
                LoadString (m_hInstance, IDS_FINISH_HOSTBUSY, lpszBuffer, wSizeOfBuffer);
            }
            else
            {
                LoadString (m_hInstance, IDS_FINISH2_TITLE_PATCHES, lpszTitle, OAM_MAXSTRING);
                LoadString (m_hInstance, IDS_FINISH_HOSTBUSY_PATCHES, lpszBuffer, wSizeOfBuffer);
            }
            break;

        case OAM_ERROR:
        default:
            if (m_DefAnnuityStatus != DefAnnuityUpdatePatches)
            {
                LoadString (m_hInstance, IDS_FINISH2_TITLE, lpszTitle, OAM_MAXSTRING);
                LoadString (m_hInstance, IDS_FINISH_ERROR, lpszBuffer, wSizeOfBuffer);
            }
            else
            {
                LoadString (m_hInstance, IDS_FINISH2_TITLE_PATCHES, lpszTitle, OAM_MAXSTRING);
                LoadString (m_hInstance, IDS_FINISH_ERROR_PATCHES, lpszBuffer, wSizeOfBuffer);
            }
            break;
    }

                                        // This is being added because a OnRamp
                                        // puts a key in the registry to
                                        // determine if we should reboot when
                                        // we are done.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, kOnrampDelayKey, 0,
                                       KEY_READ, &hDelayKey))
    {
                                        // The Key exists.  We need to set the
                                        // reboot flag and close the key.
        RegCloseKey(hDelayKey);
        *lpdwUserData = 1;

                                        // Now load the correct strings for
                                        // forcing the user to reboot
        if (m_DefAnnuityStatus != DefAnnuityUpdatePatches)
        {
            LoadString (m_hInstance, IDS_FINISH4_TITLE,    lpszTitle,  OAM_MAXSTRING);
            LoadString (m_hInstance, IDS_FINISH_FILEINUSE, lpszBuffer, wSizeOfBuffer);
        }
        else
        {
            LoadString (m_hInstance, IDS_FINISH4_TITLE_PATCHES,    lpszTitle,  OAM_MAXSTRING);
            LoadString (m_hInstance, IDS_FINISH_FILEINUSE_PATCHES, lpszBuffer, wSizeOfBuffer);
        }
    }

    // Make sure we do not overrun the buffer.
    ASSERT (_tcslen (lpszBuffer) < wSizeOfBuffer);

    return ret;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Return the number of virus definitions that exist in VIRSCAN1.DAT.
//
// Returns
//   0    - an error occured.
//   > 0  - the number of definition entries.
//////////////////////////////////////////////////////////////////////////
UINT CNavLu::GetNumVirusEntries () const
{
    HVCONTEXT hVirusContext            = VLScanInit (1, &gstNAVCallBacks, 0);
    UINT      uNumViruses              = 0;
    TCHAR     szVirScan1Dat[_MAX_PATH] = "";
    TCHAR     szVirScanInf[_MAX_PATH]  = "";
    LPCSTR    szFmtAppId               = "NAV_LU_%lu";
    char      szAppId[32];
    TCHAR     szDefDir[_MAX_PATH];
    CDefUtils DefUtils;
    CMyUtils  MyUtils;

    if (!hVirusContext)
        return uNumViruses;

    sprintf (szAppId, szFmtAppId, MyUtils.GetRandomNum());
    DefUtils.InitWindowsApp(szAppId);

    // Setup a temporary usage.
    if (DefUtils.UseNewestDefs (szDefDir, sizeof (szDefDir)))
    {
        // Tell the AVAPI1 Callbacks where the definitions are.
        NAVSetDefsPath (szDefDir);

        // Construct full path strings to VirScan1.DAT and VirScan.INF files.
        _tcscpy (szVirScan1Dat, szDefDir);
        NavLuNameAppendFile (szVirScan1Dat, "VirScan1.DAT");

        _tcscpy (szVirScanInf, szDefDir);
        NavLuNameAppendFile (szVirScanInf, "VirScan.INF");

        // Fetch the number of viruses.
        if (VS_OK == VLRegisterVirusDB (hVirusContext, szVirScan1Dat, szVirScanInf, NULL))
        {
            uNumViruses = VLNumClaimedVirusEntries (hVirusContext);
        }
        VLScanClose (hVirusContext);

        // We are done using these definition files. Remove our usage
        // count.
        DefUtils.StopUsingDefs ();
    }

    return uNumViruses;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Fetch date and version information for the newest definitions sitting
//   on this machine.
//
// Parameters:
//   lpdwDefYear      [out] Definition's internal year    stamp
//   lpdwDefMonth     [out] Definition's internal month   stamp
//   lpdwDefYear      [out] Definition's internal day     stamp
//   lpdwDefVersion   [out] Definition's internal version stamp
//
// Returns
//   TRUE  - function was successful.
//   FALSE - function failed.
//////////////////////////////////////////////////////////////////////////
BOOL CNavLu::GetVirusDefDate (LPDWORD lpdwDefYear    /*= NULL*/,
                              LPDWORD lpdwDefMonth   /*= NULL*/,
                              LPDWORD lpdwDefDay     /*= NULL*/,
                              LPDWORD lpdwDefVersion /*= NULL*/) const
{
    CDefUtils   DefUtils;
    WORD        wYear;
    WORD        wMonth;
    WORD        wDay;
    DWORD       dwVersion;
    BOOL        bSuccess;

    bSuccess = DefUtils.GetNewestDefsDate (&wYear, &wMonth, &wDay, &dwVersion);

    if (!bSuccess)
        return FALSE;

    if (lpdwDefYear)
        *lpdwDefYear = wYear;

    if (lpdwDefMonth)
        *lpdwDefMonth = wMonth;

    if (lpdwDefDay)
        *lpdwDefDay = wDay;

    if (lpdwDefVersion)
        *lpdwDefVersion = dwVersion;

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Determine if the newest virus definitions are older than one month.
//
// Returns
//   TRUE  - Definitions are older than 1 month.
//   FALSE - Definitions are not older than 1 month.
//////////////////////////////////////////////////////////////////////////
BOOL CNavLu::IsOlderThanOneMonth (void) const
{
    BOOL              bIsOlder    = FALSE;
    SYSTEMTIME        SystemTime;
    DWORD             dwDefYear;
    DWORD             dwDefMonth;
    DWORD             dwDefDay;

    // Fetch the virus definition date. If that fails, assume the definitions
    // are older than 1 month.
    if (FALSE == GetVirusDefDate (&dwDefYear, &dwDefMonth, &dwDefDay, NULL))
        return bIsOlder;

    // Get the Current System Date
    GetSystemTime (&SystemTime);

    // See if definitions are older than one month
    if (SystemTime.wYear > dwDefYear)               // System time is
    {                                               // a greater year
        if ((SystemTime.wYear - dwDefYear) > 1)     // 2 years older
          bIsOlder = TRUE;
        else if (dwDefMonth < 12)                   // 01/01/97 vs
          bIsOlder = TRUE;                          // 11/30/96
        else if (SystemTime.wMonth > 1)             // 02/01/97 vs
          bIsOlder = TRUE;                          // 12/31/96
        else if (SystemTime.wDay > dwDefDay)        // 01/16/97 vs
          bIsOlder = TRUE;                          // 12/15/96
    }
    else if (SystemTime.wYear == dwDefYear &&
             SystemTime.wMonth > dwDefMonth)
    {
        if ((SystemTime.wMonth - dwDefMonth) > 1)
            bIsOlder = TRUE;                        // 2 months older
        else if (SystemTime.wDay > dwDefDay)        // 04/16/97 vs
            bIsOlder = TRUE;                        // 03/15/96
    }

    return bIsOlder;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Set the LiveUpdate sequence number based on the current virus
//   definition version information.
//
// Parameters:
//   pfnSetSequenceNumber   [in] Pointer to LiveUpdate's SetSequenceNumber
//                               function.
//////////////////////////////////////////////////////////////////////////
void CNavLu::SetSequenceNumber (LPFUNC_SETSEQUENCENUMBER pfnSetSequenceNumber, BOOL bSetToZero) const
{
    DWORD     dwCmdLineNum = 1;
#ifdef _ALPHA_
    CString   strMatchName(_T("Avenge 1.5 MicroDefs DEC/Alpha"));
#else
    CString   strMatchName(_T("Avenge 1.5 MicroDefs"));
#endif
    CString   strProductName;
    CString   strProductType;
    CString   strProductVersion;
    CString   strProductLanguage;
    DWORD     dwDefYear;
    DWORD     dwDefMonth;
    DWORD     dwDefDay;
    DWORD     dwDefVersion;
    DWORD     dwDefDate;
    BOOL      bSuccess;

    // If the caller wants the sequence number to be zero, do it.
    if (bSetToZero)
        dwDefDate = 0;
    else
    {
        // Fetch the internal virus definition version.
        bSuccess = GetVirusDefDate (&dwDefYear, &dwDefMonth, &dwDefDay, &dwDefVersion);

        dwDefDate  = dwDefYear  * 1000000;
        dwDefDate += dwDefMonth * 10000;
        dwDefDate += dwDefDay   * 100;
        dwDefDate += (dwDefVersion % 100);          // chop the version down to 2 digits.

        // Since no internal version found, set the Lu Sequence Key to zero.
        if (!bSuccess)
            dwDefDate = 0;
    }

    // Find a command line that has a ProductName of "Avenge 1.5 MicroDefs".
    while (GetCmdLineN (dwCmdLineNum, &strProductName, &strProductType,
                        &strProductLanguage, &strProductVersion, NULL))
    {
        ++dwCmdLineNum;
        if (strProductName == strMatchName)
        {
            // Set the Lu Sequence Key based on the internal version.
            (*pfnSetSequenceNumber)((LPTSTR)(LPCTSTR)strProductName,
                                    (LPTSTR)(LPCTSTR)strProductType,
                                    (LPTSTR)(LPCTSTR)strProductVersion,
                                    (LPTSTR)(LPCTSTR)strProductLanguage,
                                    dwDefDate);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Build the LiveUpdate command line based on the registry descriptions.
//   Note that "-a" is always the first switch on the command line.
//
// Parameters:
//   pstrCmdLine         [out] string to place the command line into.
//   bSilent             [in]  T/F: Run LiveUpdate in Silent Mode?
//   bFilterVirusDefType [in]  FALSE if all command line switches should be
//                             parsed, otherwise ProductType "VirusDef"
//                             will be filtered.
//
// Returns:
//   TRUE    when one or more LiveUpdate commands are placed into pstrCmdLine.
//   FALSE   when no command lines are placed into pstrCmdLine. This
//           generally occurs when the registry is not setup correctly.
//////////////////////////////////////////////////////////////////////////

BOOL CNavLu::BuildCmdLine (CString *pstrCmdLine, BOOL bSilent, BOOL bFilterVirusDefType) const
{
    LPCTSTR     szKey         = g_szCmdLinesKey;
    LPCTSTR     szValue       = g_szOtherSwitchesValue;
    DWORD       dwCmdLineCnt  = 1;

    auto  TCHAR      szValueBuf[1024];
    auto  DWORD      dwValueBufLen = sizeof (szValueBuf);
    auto  HKEY       hBaseKey;
    auto  DWORD      dwValueType;

    // We always have the auto-mode switch.
    *pstrCmdLine = "-a ";

    // If we are a Scheduled LiveUpdate, operate core LU in silent mode.
    if (bSilent)
        *pstrCmdLine += "-s ";

    //  Get the OtherSwitches value from the CmdLines key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, szKey, 0, KEY_READ, &hBaseKey))
    {
        if (ERROR_SUCCESS == RegQueryValueEx (hBaseKey, szValue, 0, &dwValueType, (LPBYTE)szValueBuf, &dwValueBufLen))
        {
            if (dwValueType == REG_SZ && dwValueBufLen)
                *pstrCmdLine += szValueBuf;

            // If there was something in the OtherSwitches value, add a space.
            if (pstrCmdLine->GetLength() > 0)
                *pstrCmdLine += _T(" ");
        }
    }

    RegCloseKey(hBaseKey);

    // Loop through all of the CmdLines\CmdLine# keys.
    while (AppendCmdLineN (pstrCmdLine, dwCmdLineCnt, bFilterVirusDefType))
    {
        *pstrCmdLine += _T(" ");
        ++dwCmdLineCnt;
    }

    return (dwCmdLineCnt > 1);
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Add the dwCmdLineNum specified command line to pstrCmdLine. This reads
//   the command line from the registry.
//
// Parameters:
//   pstrCmdLine         [i/o] Command line string to append onto.
//   dwCmdLineNum        [in]  Command line number to append.
//   bFilterVirusDefType [in]  FALSE if the command line should be parsed
//                             no matter what, otherwise ProductType
//                             "VirusDef" will be filtered and not parsed
//                             (yet, the function will succeed).
//
// Returns
//   TRUE  - function was successful.
//   FALSE - function failed. This is generally because a "CmdLine<dwCmdLineNum>"
//           value does not exist in the registry.
//////////////////////////////////////////////////////////////////////////
BOOL CNavLu::AppendCmdLineN (CString *pstrCmdLine, DWORD dwCmdLineNum, BOOL bFilterVirusDefType) const
{
    typedef struct
    {
        LPCTSTR szValueName;
        LPCTSTR szLuSwitch;
        BOOL    bQuote;         // TRUE when fetched value should be quoted. Ex. -t"VirusDefs"
    } LOOP_INFO, *PLOOP_INFO;

    LPCTSTR     szKey       = g_szCmdLineNKey;
    LOOP_INFO   pLoopInfo[] = { { g_szOtherSwitchesValue,   _T(""),   FALSE },
                                { g_szProductNameValue,     _T("-p"), TRUE },
                                { g_szProductTypeValue,     _T("-t"), TRUE },
                                { g_szProductLanguageValue, _T("-l"), TRUE },
                                { g_szProductVersionValue,  _T("-v"), TRUE } };

    const DWORD      dwLoopInfoLen = sizeof (pLoopInfo) / sizeof (pLoopInfo[0]);

    auto  TCHAR      szKeyBuf[SYM_MAX_PATH];
    auto  TCHAR      szValueBuf[1024];
    auto  DWORD      dwValueBufLen;
    auto  HKEY       hBaseKey;
    auto  DWORD      dwValueType;
    auto  PLOOP_INFO pCurLoopInfo;
    auto  BOOL       bLastLoop;
    auto  BOOL       bArgAdded;;


    // Put the command line number into the key name.
    _stprintf (szKeyBuf, szKey, dwCmdLineNum);

    // Open the key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, szKeyBuf, 0, KEY_READ, &hBaseKey))
    {
        // Before we change the command line, filter the "VirusDef" product type.  Note,
        // that we don't need to change the "VirusDef" string when translating, since it
        // is internal to NAV and LiveUpdate.
        if (bFilterVirusDefType &&
            ERROR_SUCCESS == RegQueryValueEx (hBaseKey, g_szProductTypeValue, 0, &dwValueType, (LPBYTE)szValueBuf, &dwValueBufLen) &&
            dwValueType == REG_SZ &&
            STRICMP(szValueBuf,_T("VirusDef")) == 0)
        {
            RegCloseKey(hBaseKey);
            return TRUE;
        }

        // Parse command line.
        *pstrCmdLine += "[";

        bLastLoop = FALSE;
        for (pCurLoopInfo = pLoopInfo; !bLastLoop;)
        {
            dwValueBufLen = sizeof (szValueBuf);
            bArgAdded     = FALSE;

            // Fetch the value and add the value to the command line.
            if (ERROR_SUCCESS == RegQueryValueEx (hBaseKey, pCurLoopInfo->szValueName, 0, &dwValueType, (LPBYTE)szValueBuf, &dwValueBufLen))
            {
                if (dwValueType == REG_SZ && dwValueBufLen)
                {
                    *pstrCmdLine += pCurLoopInfo->szLuSwitch;

                    if (pCurLoopInfo->bQuote)
                        *pstrCmdLine += "\"";

                    *pstrCmdLine += szValueBuf;

                    if (pCurLoopInfo->bQuote)
                        *pstrCmdLine += "\"";

                    bArgAdded = TRUE;
                }
            }

            ++pCurLoopInfo;
            bLastLoop = ((pCurLoopInfo - pLoopInfo) >= dwLoopInfoLen);

            if (!bLastLoop && bArgAdded)
                *pstrCmdLine += " ";
        }
    }
    else
    {
        // Return FALSE when the CmdLine%d key does not exist.
        return FALSE;
    }

    RegCloseKey(hBaseKey);

    *pstrCmdLine += "]";

    return TRUE; // Everything processed okay.
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Fetch information from the CmdLine<dwCmdLineNum> registry value.
//
// Parameters:
//   dwCmdLineNum         [in]  Command line number to fetch.
//   pstrProductName      [out] If non-NULL, "ProductName"     value for dwCmdLineNum.
//   pstrProductType      [out] If non-NULL, "ProductType"     value for dwCmdLineNum.
//   pstrProductLanguage  [out] If non-NULL, "ProductLanguage" value for dwCmdLineNum.
//   pstrProductVersion   [out] If non-NULL, "ProductVersion"  value for dwCmdLineNum.
//   pstrOtherSwitches    [out] If non-NULL, "OtherSwitches"   value for dwCmdLineNum.
//
// Returns
//   TRUE  - function was successful.
//   FALSE - function failed. This is generally because a "CmdLine<dwCmdLineNum>"
//           value does not exist in the registry.
//////////////////////////////////////////////////////////////////////////
BOOL CNavLu::GetCmdLineN (DWORD   dwCmdLineNum,
                          CString *pstrProductName,
                          CString *pstrProductType,
                          CString *pstrProductLanguage,
                          CString *pstrProductVersion,
                          CString *pstrOtherSwitches) const
{
    typedef struct
    {
        LPCTSTR szValueName;
        CString *pstrOutput;
    } LOOP_INFO, *PLOOP_INFO;

    LPCTSTR     szKey       = g_szCmdLineNKey;
    LOOP_INFO   pLoopInfo[] = { { g_szOtherSwitchesValue,   pstrOtherSwitches },
                                { g_szProductNameValue,     pstrProductName },
                                { g_szProductTypeValue,     pstrProductType },
                                { g_szProductLanguageValue, pstrProductLanguage },
                                { g_szProductVersionValue,  pstrProductVersion } };

    const DWORD      dwLoopInfoLen = sizeof (pLoopInfo) / sizeof (pLoopInfo[0]);

    auto  TCHAR      szKeyBuf[SYM_MAX_PATH];
    auto  TCHAR      szValueBuf[1024];
    auto  DWORD      dwValueBufLen;
    auto  HKEY       hBaseKey;
    auto  DWORD      dwValueType;
    auto  PLOOP_INFO pCurLoopInfo;

    // Put the command line number into the key name.
    _stprintf (szKeyBuf, szKey, dwCmdLineNum);

    // Open the key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, szKeyBuf, 0, KEY_READ, &hBaseKey))
    {
        for (pCurLoopInfo = pLoopInfo;
             pCurLoopInfo - pLoopInfo < dwLoopInfoLen;
             ++pCurLoopInfo)
        {
            // Skip all values the caller does not care about.
            if (NULL == pCurLoopInfo->pstrOutput)
                continue;

            // Clear the CString.
            pCurLoopInfo->pstrOutput->Empty();

            dwValueBufLen = sizeof (szValueBuf);

            // Fetch the value and add the value to the command line.
            if (ERROR_SUCCESS == RegQueryValueEx (hBaseKey, pCurLoopInfo->szValueName, 0, &dwValueType, (LPBYTE)szValueBuf, &dwValueBufLen))
            {
                if (dwValueType == REG_SZ && dwValueBufLen)
                    *pCurLoopInfo->pstrOutput = szValueBuf;
            }
        }
    }
    else
    {
        // Return FALSE when the CmdLine%d key does not exist.
        return FALSE;
    }

    RegCloseKey(hBaseKey);

    return TRUE; // Everything processed okay.
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Set information from the CmdLine<dwCmdLineNum> registry value.
//
// Parameters:
//   dwCmdLineNum         [in] Command line number to set.
//   pstrProductName      [in] If non-NULL, "ProductName"     value for dwCmdLineNum.
//   pstrProductType      [in] If non-NULL, "ProductType"     value for dwCmdLineNum.
//   pstrProductLanguage  [in] If non-NULL, "ProductLanguage" value for dwCmdLineNum.
//   pstrProductVersion   [in] If non-NULL, "ProductVersion"  value for dwCmdLineNum.
//   pstrOtherSwitches    [in] If non-NULL, "OtherSwitches"   value for dwCmdLineNum.
//
// Returns
//   TRUE  - function was successful.
//   FALSE - function failed. This is generally because a "CmdLine<dwCmdLineNum>"
//           value does not exist in the registry.
//////////////////////////////////////////////////////////////////////////
BOOL CNavLu::SetCmdLineN (DWORD   dwCmdLineNum,
                          CString *pstrProductName,
                          CString *pstrProductType,
                          CString *pstrProductLanguage,
                          CString *pstrProductVersion,
                          CString *pstrOtherSwitches) const
{
    typedef struct
    {
        LPCTSTR szValueName;
        CString *pstrOutput;
    } LOOP_INFO, *PLOOP_INFO;

    LPCTSTR     szKey       = g_szCmdLineNKey;
    LOOP_INFO   pLoopInfo[] = { { g_szOtherSwitchesValue,   pstrOtherSwitches },
                                { g_szProductNameValue,     pstrProductName },
                                { g_szProductTypeValue,     pstrProductType },
                                { g_szProductLanguageValue, pstrProductLanguage },
                                { g_szProductVersionValue,  pstrProductVersion } };

    const DWORD      dwLoopInfoLen = sizeof (pLoopInfo) / sizeof (pLoopInfo[0]);

    auto  TCHAR      szKeyBuf[SYM_MAX_PATH];
    auto  DWORD      dwValueBufLen;
    auto  HKEY       hBaseKey;
    auto  PLOOP_INFO pCurLoopInfo;
    auto  BOOL       bResult = FALSE;
    auto  TCHAR     *pszValueBuf;


    // Put the command line number into the key name.
    _stprintf (szKeyBuf, szKey, dwCmdLineNum);

    // Open the key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, szKeyBuf, 0, KEY_WRITE, &hBaseKey))
    {
        for (pCurLoopInfo = pLoopInfo;
             pCurLoopInfo - pLoopInfo < dwLoopInfoLen;
             ++pCurLoopInfo)
        {
            // Skip all values the caller does not care about.
            if (NULL == pCurLoopInfo->pstrOutput)
                continue;

            dwValueBufLen = pCurLoopInfo->pstrOutput->GetLength();
            pszValueBuf = pCurLoopInfo->pstrOutput->GetBuffer(dwValueBufLen);

            // Set the new LiveUpdate ProductType value
            if (ERROR_SUCCESS == RegSetValueEx (hBaseKey,
                                                pCurLoopInfo->szValueName,
                                                0,
                                                REG_SZ,
                                                (const unsigned char *)pszValueBuf,
                                                dwValueBufLen) )
            {
                bResult = TRUE;
            }
        }

        RegCloseKey(hBaseKey);

    }

    return (bResult);

}

//////////////////////////////////////////////////////////////////////////
//
// Description:
//   Edit the LiveUpdate command line Version.
//
// Parameters:
//   pszVersion [in]  string containing the new MicroDef Version
//
// Returns:
//   TRUE    when the Version is successfully updated.
//   FALSE
//
//////////////////////////////////////////////////////////////////////////

BOOL CNavLu::EditMicroDefVersion(LPTSTR pszVersion) const
{
    DWORD     dwCmdLineNum = 1;
#ifdef _ALPHA_
    CString   strMatchName(_T("Avenge 1.5 MicroDefs DEC/Alpha"));
#else
    CString   strMatchName(_T("Avenge 1.5 MicroDefs"));
#endif
    CString   strProductName;
    CString   strProductType;
    CString   strProductVersion;
    CString   strProductLanguage;
    BOOL      bFound;
    int       iCmpLen;

    CString   m_MicroDefVersion(pszVersion);

    iCmpLen = m_MicroDefVersion.Find('.');

    if (iCmpLen < 0)            
        return (FALSE);

    // Find a command line that has a ProductName of "Avenge 1.5 MicroDefs".
    bFound = FALSE;
    while (!bFound &&
           GetCmdLineN (dwCmdLineNum, &strProductName, &strProductType,
                        &strProductLanguage, &strProductVersion, NULL))
    {
        if (strProductName == strMatchName)
        {
            // If the version key is a match (either MicroDefsT or MicroDefsB)
            // with the pszVersion passed in, then this is the one to edit.
            if (strProductVersion.Left(iCmpLen) == m_MicroDefVersion.Left(iCmpLen))
                bFound = TRUE;
        }

        if (!bFound)
            ++dwCmdLineNum;
    }

    if (bFound)
   {
        // Update the Version field.   
        SetCmdLineN (dwCmdLineNum, NULL, NULL, NULL, &m_MicroDefVersion, NULL);

        return (TRUE);
    }

    return (FALSE);
}

#pragma optimize( "", on )                          // Re-enable optimizeations

//////////////////////////////////////////////////////////////////////////
//
// Description:
//
//   Check if any files are in the temp directories for microdefs
//
// Parameters:
//   lpMicroDefInfo [in]  structure with temp directory name that will contain
//                        microdef update packages.
//
// Returns:
//   TRUE    if microdef update packages have arrived.
//   FALSE
//
//////////////////////////////////////////////////////////////////////////
BOOL CNavLu::CheckVirusDefsDelivered ( LPMICRODEFINFO lpMicroDefInfo )
{
    auto BOOL               bReturnVal = FALSE;
    auto WIN32_FIND_DATA    stFindData; 
    auto TCHAR              szAllFiles[SYM_MAX_PATH];
    auto HANDLE             hFind;
    auto int                i;

    // Check for files in the temp directory for binary patch files.
    memset( &stFindData, 0, sizeof(stFindData) );
    _tcscpy(szAllFiles, lpMicroDefInfo->lpszBinaryPatchDir);
    _tcscat(szAllFiles, "\\*.*");

    // Go through twice: 1st or binary patches, 2nd for text patches.
    for (i=0;i<2;i++)
    {
        hFind = FindFirstFile( szAllFiles, &stFindData );

        if ( hFind == INVALID_HANDLE_VALUE )
        {
            // Just leave ... something is wrong.
            break;
        }
        else
        {           
            while (  FindNextFile(hFind, &stFindData) )
            {
                if ( stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                    continue;

                if ( stFindData.dwFileAttributes != 0 )
                {
                    bReturnVal = TRUE;
                    break;
                }
            }

            FindClose (hFind );

            // Set up to look for text microdefs on the second pass
            memset( &stFindData, 0, sizeof(stFindData) );
            _tcscpy(szAllFiles, lpMicroDefInfo->lpszTextPatchDir);
            _tcscat(szAllFiles, "\\*.*");
        }
    }

    return ( bReturnVal );

}
