// NavLu.cpp: implementation of the CNavLu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "platform.h"
#include "xapi.h"
#include "disk.h"
#include "LiveUpdate.h"
#include "virscan.h"
#include "avapi.h"
#include "defutils.h"
#include "DefAnnuity.h"
#include "NavLu.h"
#include "NavLuUtils.h"
#include "MyUtils.h"
#include "strings.h"
#include "LSUIHelper.h"

// #include "navcb.h"

// NB That's NOTE BENE, or NOTE WELL for those of you that don't know Latin) -- 
// The two LiveUpdate type libraries and the LCOM.h include file have to appear 
// in the exact order they are right now.  Namely, LUCOM.tlb must come first, 
// then PRODUCTREGCOM.tlb and finally, LuCOM.h.  If you change the order that
// these files are included you will get all kinds of weird errors in the 
// productregcom.tli or lucom.tli.  If you start getting these errors, make sure
// you haven't inadvertantly included a file that includes one of these three file.

// Included files needed to determine if LiveUpdate is running in
// silent mode.
#import "LUCOM.TLB"			// LiveUpdate COM Server type library
#include "SettingsNames.h"  // Settings Names

// #include "navluprog.h"
#import "PRODUCTREGCOM.TLB" //no_namespace
#include "navlucbk.h"
#include "LuCOM.h"			// The LiveUpdate COM interface definitions.

#import "LSClient2.tlb" no_namespace	// LiveSubscribe interfaces. 2.0+

// AutoUpdate includes
#include "AutoUpdateDefines.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern "C" 
{
	//&? Move this code into the pre-processing to update version after
	//&? build command line is called.
	//&? Also, extract BuildCommandLine() from CNavLu, or make it a static function.
void __declspec(dllexport) WINAPI RegisterCmdLines()
{

    if(FAILED(::CoInitialize(NULL)))
        return;
    try
    {
        CNavLu::RegisterCommandLines( DefAnnuitySuccess );
    }
    catch(...)
    {}
    ::CoUninitialize();
}

} // extern "C"


//****************************************************************************
// Static variable ***********************************************************
//****************************************************************************

static CNavLu *s_pNavLu = NULL;


//****************************************************************************
// Local Defines   ***********************************************************
//****************************************************************************


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//****************************************************************************
// CNavLu function implementations *******************************************
//****************************************************************************

CNavLu::CNavLu ( HINSTANCE hInstance, BOOL bReboot, BOOL bSetup /* Both = FALSE */ ) :
    m_bReboot                      (bReboot),
    m_hInstance                    (hInstance),
    m_wLuFinishStatus              (0),
    m_uStartNumViruses             (0),
    m_pDefUtils                    (NULL),
    m_bDefUtilsPostDefUpdateCalled (FALSE),
	m_bMicroDefUpdateSuccess	   (FALSE),
    m_pPAProgress                  (NULL),
	m_byCurrentDrive			   (0),
	m_bSetup					   (bSetup)		// Are we called from NAV's setup?
{
	AfxSetResourceHandle( hInstance );
}

CNavLu::~CNavLu()
{

}

// Pulled from DoLiveUpdate() and modified for LUAll purposes
BOOL CNavLu::PreProcess( )
{
    CRegKey         Key;
    CString         strCmdLine;
    BOOL            bSuccess = FALSE;
	BOOL			bK2Exists = TRUE;

	// Parms of DoLiveUpdate() - in case we need them back as parms.
	BOOL		    bSilent = FALSE;
	BOOL			bFilterVirusDefType = FALSE;
	// End parms of DoLiveUpdate()

	memset(&m_MicroDefInfo, 0, sizeof(m_MicroDefInfo));
	memset(&m_MicroDefUpdate, 0, sizeof(m_MicroDefUpdate));

	// Remember the pointer so that NavWelcomeOAM and NavFinishOAM
	// can reference this class instance.
	s_pNavLu = this;

	// If anything does not work correctly, return OAM_ERROR result;
	m_wLuFinishStatus = OAM_ERROR;

    // Look for LiveSubscribe 2.0+ on the system
	try
	{
		HRESULT hrReturn = S_OK;
        
        // A smart pointer to the LiveSubscribe COM object.
		ISubscriptionInfoPtr	ptrSubscriptionInfo;

		// Create an instance of the LiveSubscribe object.
		hrReturn = ptrSubscriptionInfo.CreateInstance( __uuidof(SubscriptionInfo), NULL, (CLSCTX_INPROC|CLSCTX_LOCAL_SERVER ) );
		if(FAILED(hrReturn))
		{
			bK2Exists = FALSE;
			throw hrReturn;
		}

		TCHAR szServID[MAX_PATH] = {0};
		BOOL bFoundServID = FALSE;

        LONG rc = Key.Open(HKEY_LOCAL_MACHINE,cstrNAVDefnnuityKey,KEY_READ);
        if(rc == ERROR_SUCCESS)
        {
			DWORD dwSize = sizeof(szServID);
			if (ERROR_SUCCESS == Key.QueryValue(szServID,cstrServID,&dwSize))
			{
				bFoundServID = TRUE;
			}
            Key.Close();
        }

		// Attach to the virus definitions' subscription.
		if (bFoundServID)
			hrReturn = ptrSubscriptionInfo->AttachTo( (_bstr_t) szServID );
		else
			hrReturn = ptrSubscriptionInfo->AttachTo( cstrNavLSServiceID );

		if ( hrReturn != S_OK )
			throw 0;

		// determine whether or not to display the LiveSubscribe UI
  		// don't always want this if ALU is running, user will see nag screen
  		// several times a day
		CLSUIHelper LSHelp(ptrSubscriptionInfo->GetWarningPeriod(),
							ptrSubscriptionInfo->GetExpired(),
							ptrSubscriptionInfo->GetSubscriptionRemaining(),
							IsLiveUpdateInSilentMode());
  
		BOOL bDisplayLSUI = LSHelp.GetLSUIMode();

		// Set the LiveSubscribe UI mode.  The first variables determines if the
        // subscription wizard should be shown if the sub. is expired. We always
        // want this (NOT ANY MORE, SEE ABOVE). The second is if the progress is shown. The final NULL is
        //  the window handle for LiveUpdate, which we don't have.
		ptrSubscriptionInfo->SetClientUIMode(bDisplayLSUI, bDisplayLSUI, NULL);

		// Now we have attached to our subscription is LiveSubscribe.  We now
		// need to check on our subscription and potentially display the 
		// LiveSubscribe UI.
		if ( ptrSubscriptionInfo->Verify() )
        {
            ATLTRACE ( _T("LS says : subscription OK") );

#ifdef _DEBUG
            MessageBox ( NULL, "LS says : subscription OK", "navlucbk", MB_OK );
#endif // _DEBUG
			m_DefAnnuityStatus = DefAnnuityUpdateAll;
        }
		else
        {
            ATLTRACE ( _T("LS says : subscription BAD") );

#ifdef _DEBUG
            MessageBox ( NULL, "LS says : subscription BAD", "navlucbk", MB_OK );
#endif // _DEBUG

            m_DefAnnuityStatus = DefAnnuityUpdatePatches;
        }
	}

    // Assume any exception means LiveSubscribe 2.0+ isn't present (only if couldn't init SubscriptionInfo)
    catch (...)
    {
        // Call the def annuity module to get the status of the virus definition
        // subscriptions. Make sure that we do this only the first time the loop
        // is executed (we could move this code outside the loop, but
        // unfortunately BogusMainWnd is only available here).
	    // NOTE:  We could also prevent DefAnnuity from showing a UI by setting the second 
	    // parameter to TRUE.  The way it is right now, if the virus subscription is out of 
	    // date, the nag screen will display even if LiveUpdate is in silent mode.
		try
		{
  			if (!bK2Exists)
 			{
  				if ( ( m_DefAnnuityStatus = DefAnnuityPreUpdateCheck (NULL, FALSE, m_abyDABuffer) ) == DefAnnuityCancelUpdate )
  					return( FALSE );
 			}
 			else // error with subscription catalog (could be missing) so only allow product updates
  				m_DefAnnuityStatus = DefAnnuityUpdatePatches;
		}
		catch (...)
		{
		}

    }

    

	// Memorize the number of viruses for later comparision.
	m_uStartNumViruses = GetNumVirusEntries();

    // Preload PATCH32I.DLL so that CDefUtils class can use it. DefUtils doesn't know
    // where they live so we need to load them for it.
    {
        TCHAR szDirectory[ SYM_MAX_PATH ],szFilename[ SYM_MAX_PATH ];
        DWORD Size = sizeof( szFilename );
        LONG rc = Key.Open(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Symantec\\InstalledApps"), KEY_READ);
        

        if (ERROR_SUCCESS == rc)
        {
            if (ERROR_SUCCESS == Key.QueryValue(szDirectory,TEXT("NAV"),&Size ) )
            {
                STRCPY( szFilename,szDirectory );
                STRCAT( szFilename,"\\PATCH32I.DLL" );

                if ( LoadLibrary( szFilename ) == NULL ) 
					return FALSE;
            }
            Key.Close();

        }
    }

	// Allocate a new CDefUtils class
	if ( m_pDefUtils )
		delete m_pDefUtils;
	m_pDefUtils = new CDefUtils;

	// Display the progress dialog as long as LiveUpdate is not run in silent mode.
	// Now we're not even going to display this dialog.
//	if ( !IsLiveUpdateInSilentMode() )
//	{
//		m_pPAProgress = new CLuPatchProgress;
	m_MicroDefInfo.pCProgress = NULL;
//	}

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

	m_MicroDefInfo.lpszBinaryPatchDir  = m_szBinaryPatchDir;
	m_MicroDefInfo.lpszBinaryVersion   = m_szBinaryPatchVersion;
	m_MicroDefInfo.wBinaryPatchDirSize = sizeof(m_szBinaryPatchDir);
	m_MicroDefInfo.wBinaryVersionSize  = VERSION_BUFFER_SIZE;

	m_MicroDefInfo.lpszTextPatchDir    = m_szTextPatchDir;
	m_MicroDefInfo.lpszTextVersion     = m_szTextPatchVersion;
	m_MicroDefInfo.wTextPatchDirSize   = sizeof(m_szBinaryPatchDir);
	m_MicroDefInfo.wTextVersionSize    = VERSION_BUFFER_SIZE;

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

        // Call the core LiveUpdate functionality to get definitions, patches, etc.,
        // based on the status of the subscriptions.  If the def annuity encountered
        // any errors, and was not able to run properly, we will act as if the user
        // is totally legal.
        m_bLuRetry = TRUE;

		// Attempt to build the LiveUpdate command line.
        if (m_DefAnnuityStatus==DefAnnuityUpdatePatches)
		bSuccess = BuildCmdLine (&strCmdLine, bSilent, TRUE);
        else if (m_DefAnnuityStatus!=DefAnnuityCancelUpdate)
		bSuccess = BuildCmdLine (&strCmdLine, bSilent, FALSE);
        else
            m_bLuRetry = FALSE;

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
			TCHAR szFile[ _MAX_PATH+1 ];

			NavLuGetLuPathName (szFile, sizeof(szFile));
			HINSTANCE hLuInstance = LoadLibrary (szFile);

			if (hLuInstance)
			{
				LPFUNC_SETSEQUENCENUMBER pfnSetSequenceNumber =
					(LPFUNC_SETSEQUENCENUMBER) (GetProcAddress (hLuInstance, "SetSequenceNumberEx"));

				if (pfnSetSequenceNumber)
					SetSequenceNumber( pfnSetSequenceNumber, m_SetSeqNumToZero );

				FreeLibrary (hLuInstance);
			}
			else
			{
				ASSERT( FALSE );
			}
            // Major Hackola!  LUAll expects the RegisteredProducts keys to
            // have updated information, but we only set that stuff up in
            // DllRegisterServer().  So we will copy the info from NAV's
            // CmdLineN keys into RegisteredProducts.
            // LUAll seems like it's ignoring the big ol' command line that
           	// we're building, and instead looking only at the
            // RegisteredProducts keys.

            CNavLu::RegisterCommandLines( m_DefAnnuityStatus );
		}
	}
	return bSuccess;
}

// Pulled from DoLiveUpdate() and modified for LUAll purposes
WORD CNavLu::PostProcess( WORD wLuFinishStatus )
{
	TCHAR szFilename[ SYM_MAX_PATH ],szPath[ SYM_MAX_PATH ];
    BOOL      bMicroDefsDelivered = FALSE;
    // If we're not running in silent mode, the set up the progress dialog. It
    // will be called when patches are applied in PostMicroDefUpdate().

    AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_wLuFinishStatus = wLuFinishStatus;

        m_byCurrentDrive = DiskGet();
        DirGet( m_byCurrentDrive, m_szCurrentDirectory );

        GetModuleFileName( m_hInstance,szFilename,sizeof( szFilename ) );

        NameReturnPath( szFilename,szPath );

        DiskSet( szPath[ 0 ] );
        DirSet( szPath );

        // Potentially the PostMicroDefUpdate was called from the FinishOAM
	// callback. But remember that FinishOAM is not called when running
	// LiveUpdate in silent mode.
	if ( !m_bDefUtilsPostDefUpdateCalled )
	{
        if ( m_bMicroDefsDelivered = CheckVirusDefsDelivered(&m_MicroDefInfo) )
//		if (m_wLuFinishStatus == OAM_SUCCESS)
		{
            if (m_pPAProgress)
                m_pPAProgress->Create(); 

			m_MicroDefUpdate.bProcessPatches = TRUE;
		}
		else
			m_MicroDefUpdate.bProcessPatches = FALSE;

		m_bMicroDefUpdateSuccess = m_pDefUtils->PostMicroDefUpdate(&m_MicroDefUpdate);    

                if ( m_MicroDefUpdate.bProcessPatches == TRUE )
                    if (m_pPAProgress)
                        m_pPAProgress->DestroyWindow();

    if (m_pPAProgress)
        delete m_pPAProgress;

	delete m_pDefUtils;
	m_pDefUtils = NULL;

        m_bDefUtilsPostDefUpdateCalled = TRUE;
	}

    // Look for LiveSubscribe 2.0+ on the system
	try
	{
		HRESULT hrReturn = S_OK;
        
        // A smart pointer to the LiveSubscribe COM object.
		ISubscriptionInfoPtr	ptrSubscriptionInfo;

		// Create an instance of the LiveSubscribe object.
		hrReturn = ptrSubscriptionInfo.CreateInstance( __uuidof(SubscriptionInfo),NULL,(CLSCTX_INPROC|CLSCTX_LOCAL_SERVER ) );
		if ( FAILED(hrReturn) )
			throw hrReturn;
    }
    catch (...)
    {
        // If LiveUpdate downloaded something, then let the def annuity module display
        // any reports that it needs to display.  Note, that currently we will force
        // the display even if a reboot needs to be done.
        if (m_bLuRetry && (wLuFinishStatus==OAM_SUCCESS || wLuFinishStatus==OAM_SUCCESSREBOOT || wLuFinishStatus==OAM_NOTHINGNEW))
            DefAnnuityPostUpdateCheck (NULL, FALSE, m_abyDABuffer);
    }

    DiskSet ( m_byCurrentDrive );
    DirSet( m_szCurrentDirectory );

    return m_wLuFinishStatus;
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
    CString   strProductName;
    CString   strProductType;
    CString   strProductVersion;
    CString   strProductLanguage;
    CString   strGroupTogether;
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
                        &strProductLanguage, &strProductVersion, NULL, NULL, NULL, NULL, &strGroupTogether))
    {
        ++dwCmdLineNum;
        if ( atoi( ( LPCSTR ) strGroupTogether ) > 0 )
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

BOOL CNavLu::BuildCmdLine (CString *pstrCmdLine, BOOL bSilent, BOOL bFilterVirusDefType)
{
    LPCTSTR     szKey         = g_szCmdLinesKey;
    LPCTSTR     szValue       = g_szOtherSwitchesValue;
    DWORD       dwCmdLineCnt  = 1;

    auto  TCHAR      szValueBuf[1024];
    auto  DWORD      dwValueBufLen = sizeof (szValueBuf);
    auto  DWORD      dwValueType;

    // We always have the auto-mode switch.
    *pstrCmdLine = "-a ";

    // If we are a Scheduled LiveUpdate, operate core LU in silent mode.
    if (bSilent)
        *pstrCmdLine += "-s ";

    //  Get the OtherSwitches value from the CmdLines key.
    CRegKey BaseKey;
    LONG rc = BaseKey.Open(HKEY_LOCAL_MACHINE,szKey,KEY_READ);

    if (ERROR_SUCCESS == rc)
    {
        if (ERROR_SUCCESS == RegQueryValueEx (BaseKey, szValue, 0, &dwValueType, (LPBYTE)szValueBuf, &dwValueBufLen))
        {
            if (dwValueType == REG_SZ && dwValueBufLen)
                *pstrCmdLine += szValueBuf;

            // If there was something in the OtherSwitches value, add a space.
            if (pstrCmdLine->GetLength() > 0)
                *pstrCmdLine += _T(" ");
        }
    }


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
BOOL CNavLu::AppendCmdLineN (CString *pstrCmdLine, DWORD dwCmdLineNum, BOOL bFilterVirusDefType)
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
    auto  DWORD      dwValueType;
    auto  PLOOP_INFO pCurLoopInfo;
    auto  BOOL       bLastLoop;
    auto  BOOL       bArgAdded;;


    // Put the command line number into the key name.
    _stprintf (szKeyBuf, szKey, dwCmdLineNum);

    // Open the key.
    CRegKey BaseKey;
    if (ERROR_SUCCESS == BaseKey.Open(HKEY_LOCAL_MACHINE, szKeyBuf,KEY_READ))
    {
        // Before we change the command line, filter the "VirusDef" product type.  Note,
        // that we don't need to change the "VirusDef" string when translating, since it
        // is internal to NAV and LiveUpdate.
        if (bFilterVirusDefType &&
            ERROR_SUCCESS == RegQueryValueEx (BaseKey, g_szProductTypeValue, 0, &dwValueType, (LPBYTE)szValueBuf, &dwValueBufLen) &&
            dwValueType == REG_SZ &&
            STRICMP(szValueBuf,_T("VirusDef")) == 0)
        {
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
            if (ERROR_SUCCESS == RegQueryValueEx (BaseKey, pCurLoopInfo->szValueName, 0, &dwValueType, (LPBYTE)szValueBuf, &dwValueBufLen))
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
//   pstrSequenceNumber   [in]  If non-NULL, "SequenceNumber"  value for dwCmdLineNum.
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
                          CString *pstrOtherSwitches,
						  CString *pstrSequenceNumber,
						  CString *pstrProductMoniker,
			  CString *pstrCallbacksHere,
			  CString *pstrGroupTogether )
{
    typedef struct
    {
        LPCTSTR szValueName;
        CString *pstrOutput;
    } LOOP_INFO, *PLOOP_INFO;

    LPCTSTR     szKey       = g_szCmdLineNKey;
    LOOP_INFO   pLoopInfo[] = 
	{ 
		{ g_szSequenceNumberValue,	pstrSequenceNumber },
		{ g_szOtherSwitchesValue,   pstrOtherSwitches },
        { g_szProductNameValue,     pstrProductName },
        { g_szProductTypeValue,     pstrProductType },
        { g_szProductLanguageValue, pstrProductLanguage },
        { g_szProductVersionValue,  pstrProductVersion },
		{ g_szProductMonikerValue,  pstrProductMoniker },
		{ g_szCallbacksHereValue,  pstrCallbacksHere },
		{ g_szGroupValue,  pstrGroupTogether }

	};

    const DWORD      dwLoopInfoLen = sizeof (pLoopInfo) / sizeof (pLoopInfo[0]);

    auto  TCHAR      szKeyBuf[SYM_MAX_PATH];
    auto  TCHAR      szValueBuf[1024];
    auto  DWORD      dwValueBufLen;
    auto  DWORD      dwValueType;
    auto  PLOOP_INFO pCurLoopInfo;

    // Put the command line number into the key name.
    _stprintf (szKeyBuf, szKey, dwCmdLineNum);

    // Open the key.
    CRegKey BaseKey;
    if (ERROR_SUCCESS == BaseKey.Open(HKEY_LOCAL_MACHINE, szKeyBuf, KEY_READ))
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
            if (ERROR_SUCCESS == RegQueryValueEx (BaseKey, pCurLoopInfo->szValueName, 0, &dwValueType, (LPBYTE)szValueBuf, &dwValueBufLen))
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


    return TRUE; // Everything processed okay.
}

BOOL IsNAVProf()
{
	CRegKey Key;
	LONG lResult;
	BOOL bReturn = FALSE;
	
	lResult = Key.Open(HKEY_LOCAL_MACHINE, cstrNAVRegKey,KEY_QUERY_VALUE);
	
	if (lResult == ERROR_SUCCESS)
	{
		TCHAR szNAVInstType[2] = {0};
		DWORD dwSize = sizeof(szNAVInstType);
		
		lResult = Key.QueryValue( szNAVInstType, cstrNAVType,&dwSize);
		
		if (lResult == ERROR_SUCCESS)
		{
			// If it's NAV Professional, this value should be 1
			if (_ttoi(szNAVInstType) == 1)
				bReturn = TRUE;
		}

	}
	
	return bReturn;
}

void GetDescriptiveName( CString strProductName,CString& strProductVersion,CString& strCallbacksHere,CString& strDescriptiveText,CString& strDefaultText )
{
	if ( atoi( ( LPCSTR ) strCallbacksHere ) > 0 )
	{
		if ( IsNAVProf() == TRUE )
			strDescriptiveText.LoadString( IDS_DESC_NAV_PRO );
		else
			strDescriptiveText.LoadString( IDS_DESC_NAV );

		return;
	}
	if ( strProductName.Find( "Engine" ) >= 0 ) 
	{
		strDescriptiveText.LoadString( IDS_DESC_ENGINE );

		return;
	}
	if ( strProductName.Find( "Avenge" ) >= 0 ) 
	{
		if ( strProductVersion.Find( "MicroDefsT" ) >= 0 )
			strDescriptiveText.LoadString( IDS_DESC_MICRO_TEXT );
		else
			if ( strProductVersion.Find( "MicroDefsB" ) >= 0 )
				strDescriptiveText.LoadString( IDS_DESC_MICRO_BINARY );
			else
				strDescriptiveText.LoadString( IDS_DESC_DEFS );

		return;
	}

	if ( strProductName.Find( "Scan" ) >= 0 ) 
	{
		strDescriptiveText.LoadString( IDS_DESC_SCAN_DELIVER );

		return;
	}

	strDescriptiveText = strDefaultText;
}

// This function will return TRUE if the given product is safe for use by AutoUpdate
// in silent mode.
BOOL IsProductAutoUpdateSafe( CString strProductName, CString& strCallbacksHere )
{
	if ( atoi( ( LPCSTR ) strCallbacksHere ) > 0 ) 
	{
		return FALSE;
	}
	if ( strProductName.Find( "Engine" ) >= 0 ) 
	{
		return TRUE;
	}
	if ( strProductName.Find( "Avenge" ) >= 0 ) 
	{
		return TRUE;
	}

	return FALSE;
}

BOOL GetCmdLineMoniker( CString strProductName,CString& strProductVersion,CString& strCallbacksHere,CString& strGroup,_variant_t& vMoniker )
{
if ( atoi( ( LPCSTR ) strCallbacksHere ) > 0 ) {
    vMoniker = _bstr_t( SOFTWARE_GUID );

    return( TRUE );
    }
if ( strProductName.Find( "Engine" ) >= 0 ) {
    vMoniker = _bstr_t( ENGINE_GUID );

    return( TRUE );
    }
if ( atoi( ( LPCSTR ) strGroup ) > 0 ) {
    if ( strProductVersion.Find( "MicroDefsT" ) >= 0 )
        vMoniker = _bstr_t( TEXT_HUB_GUID );
    else
        vMoniker = _bstr_t( BINARY_HUB_GUID );

    return( TRUE );
    }
if ( strProductName.Find( "Scan" ) >= 0 ) {
    vMoniker = _bstr_t( SCAN_DELIVER_GUID );

    return( TRUE );
    }

return( FALSE );
}

// Need to handle re-registration properly.
// Define one of the command lines to be the "default" - this
// will be the one that has the callback registered to it, and
// it will be the one named "Avenge 1.5 MicroDefs".
// As each command line is registered, a property will be saved in
// the registry with the command line to give the moniker associated with it.
// As we retrieve command lines from the registry, if they haven't already been
// registered, they will be registered and their monikers will be saved.
// If they have been registered, the registration will simply be updated
// with the current values.
// This scheme should work well so long as the number of command lines 
// doesn't change.
HRESULT CNavLu::RegisterCommandLines( DWORD DefAnnuityStatus )
{
	PRODUCTREGCOMLib::IluProductRegPtr ptrProductReg;
	LPTSTR lpszCmdLine = NULL;
	HRESULT hr = S_OK;

	try 
	{
		DWORD     dwCmdLineNum = 1;
        	BOOL bRegisteredElements;
		CString strProductName;
		CString strProductVersion;
		CString strProductLanguage;
		CString strProductType;
		CString strProductMoniker;
		CString strCallbacksHere;
		CString strGroup;
		CString strDescriptiveText;
		CString strAutoUpdateSilent;
		CString strAutoUpdatePrompt;
		CString strSequenceNumber;

		// Set the AutoUpdate silent and prompt tags.
		strAutoUpdateSilent.LoadString( IDS_AUTOUPDATE_SILENT );
		strAutoUpdatePrompt.LoadString( IDS_AUTOUPDATE_PROMPT );

		hr = ptrProductReg.CreateInstance( __uuidof( luProductReg ),NULL,(CLSCTX_INPROC|CLSCTX_LOCAL_SERVER ) );

		_variant_t vFullDefsMoniker( _bstr_t( FULL_DEFS_GUID ) );
		_variant_t vOverrideMoniker( _bstr_t( OVERRIDE_GUID ) );

		PRODUCTREGCOMLib::IluGroupPtr ptrGroup;
		hr = ptrGroup.CreateInstance( __uuidof( luGroup ), NULL, (CLSCTX_INPROC|CLSCTX_LOCAL_SERVER ) );

		hr = ptrGroup->RegisterGroup( LUPR_GT_COUPLED, &vFullDefsMoniker );
		hr = ptrGroup->RegisterGroup( LUPR_GT_OVERRIDDEN, &vOverrideMoniker );

        hr = ptrGroup->AddToGroup( vOverrideMoniker.bstrVal, _bstr_t( TEXT( "Avenge1.5 Definitions" ) ), _bstr_t( TEXT( "1.0" ) ), _bstr_t( TEXT( "English" ) ), _bstr_t( "" ) );

		while ( GetCmdLineN( dwCmdLineNum, &strProductName, &strProductType,
							&strProductLanguage, &strProductVersion, 
							NULL, &strSequenceNumber, &strProductMoniker, &strCallbacksHere, &strGroup ) )
		{
			try 
			{
				_bstr_t strMoniker;
				_variant_t vMoniker;

				try 
				{
					vMoniker = ptrProductReg->FindMoniker( _bstr_t( strProductName ),
													 _bstr_t( strProductVersion ),
													 _bstr_t( strProductLanguage ),
													 _bstr_t( strProductType ) );

					bRegisteredElements = ( vMoniker.vt != VT_EMPTY );
				}
				catch ( ... )
				{
							bRegisteredElements = FALSE;
				}

				// strProductMoniker represents what NAV has saved in the registry.
				// vMoniker represents the moniker of something registered 
				// with the given command line (if any).
				// If there is a saved NAV moniker, and it doesn't match
				// the registered moniker for these command line elements, 
				// then delete the possible registration of the NAV saved
				// moniker and make the NAV saved moniker be the one for the
				// matching registered command line elements.
				// All of these conditions were hurting my brain, so 
				// I explicitly spelled out the conditions in the code below -
				// this definitely could be condensed, but it would be harder
				// to understand.
				BOOL bHaveSavedMoniker = !strProductMoniker.IsEmpty();
				BOOL bSavedMatchesRegistered = FALSE;
				BOOL bSavedIsRegistered = FALSE;

                if ( atoi( ( LPCSTR ) strGroup ) > 0 ) 
				{
					if ( ( DefAnnuityStatus == DefAnnuityUpdatePatches ) || ( DefAnnuityStatus == DefAnnuityDownloadNotAllowed ) || ( DefAnnuityStatus == DefAnnuityCancelUpdate ) )
						ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szAbort ), _bstr_t( "YES" ) );

					hr = ptrGroup->AddToGroup( vFullDefsMoniker.bstrVal, _bstr_t( strProductName ), _bstr_t( strProductVersion ), _bstr_t( strProductLanguage ), _bstr_t( "" ) );
				}

				if ( bHaveSavedMoniker && bRegisteredElements )
				{
					bSavedMatchesRegistered = ( _bstr_t( strProductMoniker ) == _bstr_t( vMoniker ) );
				}
				if ( bHaveSavedMoniker )
				{
					_variant_t vValue;
					//&? BEM - 12/11/98 - using try catch here, since 
					//  GetProperty() might return an error if nothing is registered
					//  at all, but it will only return S_FALSE/S_OK if the given moniker 
					//  exists, but does not have the given property.
					try
					{
                        HRESULT hr;

						hr = ptrProductReg->GetProperty( _bstr_t( strProductMoniker ), g_szProduct, &vValue );
						bSavedIsRegistered = ( hr == S_OK );
					}
					catch ( _com_error e )
					{
						bSavedIsRegistered = FALSE;
					}
				}

				if ( bHaveSavedMoniker )
				{
					if ( bSavedIsRegistered )
					{
						if ( bRegisteredElements )
						{
							if ( !bSavedMatchesRegistered )
							{
								// The command line elements are registered and
								// so is the saved moniker, but they don't match - 
								// THIS IS BAD.
								ATLASSERT( FALSE );
							}
							else
							{
								// Saved matches registered - nothing to do.
							strMoniker = strProductMoniker;
							}
						}
						else
						{
							// Saved is registered, but it's elements have changed
							// so update the registration with the new information.
							strMoniker = strProductMoniker;
							ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szProduct ), _bstr_t( strProductName ) );
							ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szVersion ), _bstr_t( strProductVersion ) );
							ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szLanguage ), _bstr_t( strProductLanguage ) );
							ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szType ), _bstr_t( "" ) );
							ptrProductReg->SetProperty( strMoniker, _bstr_t( "SEQ.UPDATE"), _bstr_t( strSequenceNumber ) );
							GetDescriptiveName( strProductName, strProductVersion, strCallbacksHere, 
								                strDescriptiveText, strProductName );
							ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szDescriptiveName ), 
								                        _bstr_t( strDescriptiveText ) );

							// Set the AutoUpdate flag for this command line.
							if ( IsProductAutoUpdateSafe( strProductName, strCallbacksHere ) )
							{
								ptrProductReg->SetProperty( strMoniker, 
															_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
															_bstr_t( AU_REG_USE_AU_FOR_UPDATES_ON ) );
								ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
															_bstr_t( AU_REG_PATCH_SILENT ) );
							}
							else
							{
								ptrProductReg->SetProperty( strMoniker, 
															_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
															_bstr_t( AU_REG_USE_AU_FOR_UPDATES_OFF ) );
								ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
															_bstr_t( AU_REG_PATCH_NOISY ) );
							}
						}
					}
					else
					{
						// Saved but not registered as the saved moniker
						if ( bRegisteredElements )
						{
							// Just use registered moniker and save it instead of
							// what we had as the NAV saved moniker.
							// Update the elements of this moniker.
							strProductMoniker = LPCTSTR( _bstr_t( vMoniker ) );
							strMoniker = strProductMoniker;
							ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szProduct ), _bstr_t( strProductName ) );
							ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szVersion ), _bstr_t( strProductVersion ) );
							ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szLanguage ), _bstr_t( strProductLanguage ) );
							ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szType ), _bstr_t( "" ) );
							ptrProductReg->SetProperty( strMoniker, _bstr_t( "SEQ.UPDATE"), _bstr_t( strSequenceNumber ) );
							GetDescriptiveName( strProductName,strProductVersion,strCallbacksHere,strDescriptiveText,strProductName );
							ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szDescriptiveName ), _bstr_t( strDescriptiveText ) );

							// Set the AutoUpdate flag for this command line.
							if ( IsProductAutoUpdateSafe( strProductName, strCallbacksHere ) )
							{
								ptrProductReg->SetProperty( strMoniker, 
															_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
															_bstr_t( AU_REG_USE_AU_FOR_UPDATES_ON ) );
								ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
															_bstr_t( AU_REG_PATCH_SILENT ) );
							}
							else
							{
								ptrProductReg->SetProperty( strMoniker, 
															_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
															_bstr_t( AU_REG_USE_AU_FOR_UPDATES_OFF ) );
								ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
															_bstr_t( AU_REG_PATCH_NOISY ) );
							}
						}
						else
						{
							// Saved moniker is not registered and neither is there
							// a registration for these command line elements.
							// Register anew and save the new moniker as the
							// saved NAV moniker.
							GetCmdLineMoniker( strProductName,strProductVersion,strCallbacksHere,strGroup,vMoniker );

							hr = ptrProductReg->RegisterProduct( _bstr_t( strProductName ), 
																 _bstr_t( strProductVersion ), 
																 _bstr_t( strProductLanguage ),
																 _bstr_t( "" ),
																 &vMoniker );
							strMoniker = _bstr_t( vMoniker );					
							strProductMoniker = LPCTSTR( strMoniker );
						    GetDescriptiveName( strProductName,strProductVersion,strCallbacksHere,strDescriptiveText,strProductName );
							ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szDescriptiveName ), _bstr_t( strDescriptiveText ) );
							ptrProductReg->SetProperty( strMoniker, _bstr_t( "SEQ.UPDATE"), _bstr_t( strSequenceNumber ) );
							
							// Set the AutoUpdate flag for this command line.
							if ( IsProductAutoUpdateSafe( strProductName, strCallbacksHere ) )
							{
								ptrProductReg->SetProperty( strMoniker, 
															_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
															_bstr_t( AU_REG_USE_AU_FOR_UPDATES_ON ) );
								ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
															_bstr_t( AU_REG_PATCH_SILENT ) );
							}
							else
							{
								ptrProductReg->SetProperty( strMoniker, 
															_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
															_bstr_t( AU_REG_USE_AU_FOR_UPDATES_OFF ) );
								ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
															_bstr_t( AU_REG_PATCH_NOISY ) );
							}
						}
					}
				}
				else 
				{
					// Don't have a saved moniker 
					if ( bRegisteredElements )
					{
						// Just use registered moniker and save it as the 
						// NAV saved moniker.
						// The elements are already registered, just under
						// a different moniker, so switch to that moniker
						// and save it.
						strProductMoniker = LPCTSTR( _bstr_t( vMoniker ) );
					}
					else // No saved, no registered 
					{
						// Register anew and save the new moniker as the saved
						// NAV moniker.
						GetCmdLineMoniker( strProductName,strProductVersion,strCallbacksHere,strGroup,vMoniker );
						hr = ptrProductReg->RegisterProduct( _bstr_t( strProductName ), 
															 _bstr_t( strProductVersion ), 
															 _bstr_t( strProductLanguage ),
															 _bstr_t( "" ),
															 &vMoniker );
						strMoniker = _bstr_t( vMoniker );					
						strProductMoniker = LPCTSTR( strMoniker );
						// Register a descriptive name for final status messages
						GetDescriptiveName( strProductName,strProductVersion,strCallbacksHere,strDescriptiveText,strProductName );
						ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szDescriptiveName ), _bstr_t( strDescriptiveText ) );
						ptrProductReg->SetProperty( strMoniker, _bstr_t( "SEQ.UPDATE"), _bstr_t( strSequenceNumber ) );

						// Set the AutoUpdate flag for this command line.
						if ( IsProductAutoUpdateSafe( strProductName, strCallbacksHere ) )
						{
							ptrProductReg->SetProperty( strMoniker, 
														_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
														_bstr_t( AU_REG_USE_AU_FOR_UPDATES_ON ) );
							ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
														_bstr_t( AU_REG_PATCH_SILENT ) );
						}
						else
						{
							ptrProductReg->SetProperty( strMoniker, 
														_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
														_bstr_t( AU_REG_USE_AU_FOR_UPDATES_OFF ) );
							ptrProductReg->SetProperty( strMoniker, _bstr_t( AU_REG_UPDATE_TYPE ),
														_bstr_t( AU_REG_PATCH_NOISY ) );
						}
					}

				}

				ATLASSERT( !strProductMoniker.IsEmpty() );
				// Save off the moniker for next time
				SetCmdLineN( dwCmdLineNum, NULL, NULL, NULL, NULL, NULL, NULL, &strProductMoniker );

                if ( atoi( ( LPCSTR ) strCallbacksHere ) > 0 )
				{
					strMoniker = _bstr_t( strProductMoniker );
					// Register the callback on the default command line
					hr = ptrProductReg->RegisterCallback( strMoniker, _uuidof( luNavCallBack ), (PRODUCTREGCOMLib::tagLU_CALLBACK_TYPE)(PreSession | PostSession | WelcomeText | FinishText ) );
					ptrProductReg->SetProperty( strMoniker, _bstr_t( g_szOverrideGroup ), vOverrideMoniker.bstrVal );

					// Clear the status text just in case something was left over
					ptrProductReg->DeleteProperty( strMoniker, _bstr_t( g_szStatusText ) );
				}

			}
			catch ( _com_error e )
			{
				ATLTRACE("RegisterCommandLines() - Caught COM exception registering command line\n");
			}
			++dwCmdLineNum;
			strProductMoniker.Empty();
		}

	}
        catch ( _com_error e )
	{
		ATLTRACE( _T("RegisterCommandLines() - caught exception") );
	}

	delete [] lpszCmdLine;
	ptrProductReg = NULL;
	return hr;
}

HRESULT CNavLu::DisableCommandLines( DWORD DefAnnuityStatus )
{
	HRESULT hr;

	hr = S_OK;

	PRODUCTREGCOMLib::IluProductRegPtr ptrProductReg;
	LPTSTR lpszCmdLine = NULL;

	try 
	{
		DWORD     dwCmdLineNum = 1;
		CString strProductName;
		CString strProductVersion;
		CString strProductLanguage;
		CString strProductType;
		CString strProductMoniker;
		CString strGroup;

		hr = ptrProductReg.CreateInstance( __uuidof( luProductReg ), NULL, (CLSCTX_INPROC|CLSCTX_LOCAL_SERVER ) );

		while ( GetCmdLineN( dwCmdLineNum, &strProductName, &strProductType,
							&strProductLanguage, &strProductVersion, 
							NULL, NULL, &strProductMoniker, NULL, &strGroup ) )
		{
            if ( atoi( ( LPCSTR ) strGroup ) > 0 ) 
			{
                if ( ( DefAnnuityStatus == DefAnnuityUpdatePatches ) || ( DefAnnuityStatus == DefAnnuityDownloadNotAllowed ) || ( DefAnnuityStatus == DefAnnuityCancelUpdate ) )
                    ptrProductReg->SetProperty( _bstr_t( strProductMoniker ), _bstr_t( g_szAbort ), _bstr_t( "YES" ) );
            }

			++dwCmdLineNum;
			strProductMoniker.Empty();
		}

	}
	catch ( ... )
	{
		ATLTRACE( _T("RegisterCommandLines() - caught exception") );
		hr = E_FAIL;
	}

	ptrProductReg = NULL;
	return hr;
}

HRESULT CNavLu::UnregisterCommandLines( void )
{
	HRESULT hr;

	PRODUCTREGCOMLib::IluGroupPtr ptrGroup;
	hr = ptrGroup.CreateInstance( __uuidof( luGroup ), NULL, (CLSCTX_INPROC|CLSCTX_LOCAL_SERVER ) );

    // If CreateInstance() failed, LU was uninstalled before NAV, so
    // bail out.
    if ( FAILED(hr) )
    {
        return E_FAIL;
    }

	hr = ptrGroup->DeleteGroup( _bstr_t( FULL_DEFS_GUID ) );
	hr = ptrGroup->DeleteGroup( _bstr_t( OVERRIDE_GUID ) );

	hr = S_OK;

	PRODUCTREGCOMLib::IluProductRegPtr ptrProductReg;
	LPTSTR lpszCmdLine = NULL;

	try 
	{
		DWORD     dwCmdLineNum = 1;
		CString strProductName;
		CString strProductVersion;
		CString strProductLanguage;
		CString strProductType;
		CString strProductMoniker;

		hr = ptrProductReg.CreateInstance( __uuidof( luProductReg ), NULL, (CLSCTX_INPROC|CLSCTX_LOCAL_SERVER ) );
        
        // If CreateInstance() failed, LU was uninstalled before NAV, so
        // bail out.
        if ( FAILED(hr) )
        {
            return E_FAIL;
        }
        
        while ( GetCmdLineN( dwCmdLineNum, &strProductName, &strProductType,
							&strProductLanguage, &strProductVersion, 
							NULL, NULL, &strProductMoniker, NULL, NULL ) )
		{
			ptrProductReg->DeleteProduct( _bstr_t( strProductMoniker ) );

			++dwCmdLineNum;
			strProductMoniker.Empty();
		}

	}
	catch ( ... )
	{
		ATLTRACE( _T("RegisterCommandLines() - caught exception") );
		hr = E_FAIL;
	}

	ptrProductReg = NULL;
	return hr;
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
//   pstrSequenceNumber   [in] If non-NULL, "SequenceNumber"  value for dwCmdLineNum.
//
// Returns
//   TRUE  - function was successful.
//   FALSE - function failed. This is generally because a "CmdLine<dwCmdLineNum>"
//           value does not exist in the registry.
//////////////////////////////////////////////////////////////////////////
// static function
BOOL CNavLu::SetCmdLineN (DWORD   dwCmdLineNum,
                          CString *pstrProductName,
                          CString *pstrProductType,
                          CString *pstrProductLanguage,
                          CString *pstrProductVersion,
                          CString *pstrOtherSwitches,
						  CString *pstrSequenceNumber,
						  CString *pstrProductMoniker )
{
    typedef struct
    {
        LPCTSTR szValueName;
        CString *pstrOutput;
    } LOOP_INFO, *PLOOP_INFO;

    LPCTSTR     szKey       = g_szCmdLineNKey;
    LOOP_INFO   pLoopInfo[] = 
	{ 
		{ g_szSequenceNumberValue,	pstrSequenceNumber },
		{ g_szOtherSwitchesValue,   pstrOtherSwitches },
        { g_szProductNameValue,     pstrProductName },
        { g_szProductTypeValue,     pstrProductType },
        { g_szProductLanguageValue, pstrProductLanguage },
        { g_szProductVersionValue,  pstrProductVersion },
		{ g_szProductMonikerValue,  pstrProductMoniker }
	};

    const DWORD      dwLoopInfoLen = sizeof (pLoopInfo) / sizeof (pLoopInfo[0]);

    auto  TCHAR      szKeyBuf[SYM_MAX_PATH];
    auto  DWORD      dwValueBufLen;
    auto  PLOOP_INFO pCurLoopInfo;
    auto  BOOL       bResult = FALSE;
    auto  TCHAR     *pszValueBuf;


    // Put the command line number into the key name.
    _stprintf (szKeyBuf, szKey, dwCmdLineNum);

    // Open the key.
    CRegKey BaseKey;
    if (ERROR_SUCCESS == BaseKey.Open(HKEY_LOCAL_MACHINE, szKeyBuf, KEY_WRITE))
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
            if (ERROR_SUCCESS == BaseKey.SetValue(pszValueBuf,pCurLoopInfo->szValueName) )
            {
                bResult = TRUE;
            }
        }


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
    CString   strProductName;
    CString   strProductType;
    CString   strProductVersion;
    CString   strProductLanguage;
    CString   strGroupTogether;
    BOOL      bFound;
    int       iCmpLen;

    CString   strMicroDefVersion(pszVersion);

    iCmpLen = strMicroDefVersion.Find('.');

    if ( iCmpLen < 0 )            
        return ( FALSE );

    // Find a command line that has a ProductName of "Avenge 1.5 MicroDefs".
    bFound = FALSE;
    while ( !bFound &&
           GetCmdLineN( dwCmdLineNum, &strProductName, &strProductType,
                        &strProductLanguage, &strProductVersion, NULL, NULL, NULL, NULL, 
						&strGroupTogether ) )
    {
        if ( atoi( ( LPCSTR ) strGroupTogether ) > 0 )
        {
            // If the version key is a match (either MicroDefsT or MicroDefsB)
            // with the pszVersion passed in, then this is the one to edit.
            if ( strProductVersion.Left( iCmpLen ) == strMicroDefVersion.Left( iCmpLen ) )
                bFound = TRUE;
        }

        if ( !bFound )
            ++dwCmdLineNum;
    }

    if ( bFound )
   {
        // Update the Version field.   
        SetCmdLineN ( dwCmdLineNum, NULL, NULL, NULL, &strMicroDefVersion, NULL, NULL, NULL );

        return ( TRUE );
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

//////////////////////////////////////////////////////////////////////////
// Description:
//   Setup the strings for the Finish LiveUpdate panel. Note that this
//   is not called by LiveUpdate when we are running in silent mode.
//
// Parameters:
//   lpszBuffer     [out] New string for the text body.
//   wSizeOfBuffer  [in]  Maximum number of characters in lpszBuffer.
//   lpszTitle      [out] New title string.
//
// Returns
//   TRUE always. Not sure what a FALSE return would do.
//////////////////////////////////////////////////////////////////////////
BOOL CNavLu::GetStatusText(
	CString& sStatusText,
	CString& sTitle
	)
{
    BOOL      ret            = TRUE;
	CString	  sText;
	WORD	  wStatus = m_wLuFinishStatus;

    AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	sStatusText.Empty();
	sTitle.Empty();

    if ( m_bDefUtilsPostDefUpdateCalled == FALSE ) {
    // Call PostMicroDefUpdate so that the GetNumVirusEntries call will
    // be from the new definitions directory. Also, this ensures that
    // we cleanup before LiveUpdate potentially reboots the system.

    if (wStatus == OAM_SUCCESS)
    {
        if ( m_bMicroDefsDelivered = CheckVirusDefsDelivered(&m_MicroDefInfo) )
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

    m_bMicroDefUpdateSuccess = m_pDefUtils->PostMicroDefUpdate(&m_MicroDefUpdate);

    if (m_pPAProgress)
        m_pPAProgress->DestroyWindow();

    if (m_pPAProgress)
        delete m_pPAProgress;

	delete m_pDefUtils;
	m_pDefUtils = NULL;

    m_bDefUtilsPostDefUpdateCalled = TRUE;
    }

    // NOTE:: We've decided to just take the default LU strings, and not
    // load our own.  Code to do this is #ifdef'd out (#ifdef CHUCK).
    // 2/5/99 Mike Dunn.
	// I've decided to chuck all of the chuck code, since it's not being used.
	// 7/21/00

    return ret;
}

// This routine tries to simulate the final LiveUpdate return code by
// looking at the individual status of each command line - the
// code should probably be updated to use the individual status of
// each command line instead.
// The return value is a simulated LiveUpdate return code - it can
// mostly be OAM_SUCCESS, OAM_NOTHINGNEW, or OAM_ERROR, but 
// it seems possible that one of the download status codes could be something
// else (like OAM_HOSTBUSY).
// The purpose of this routine is to create a OAM_XXX code specific to the
// packages NAV cares about - since the overall LiveUpdate return code will
// be talking about the whole session, in which more than just NAV patches
// are being retrieved.
//&? SHOULD CHECK THE RAW LIVEUPDATE CODE IF AVAILABLE TO SEE IF SOMETHING
//&? LIKE USERCANCEL HAPPENED AND RETURN THAT.
WORD CNavLu::GetStatusCodes()
{

	PRODUCTREGCOMLib::IluProductRegPtr ptrProductReg;
	HRESULT hr = S_OK;
	WORD	wLUStatus = OAM_NOTHINGNEW;

        m_wLuPatchStatus = OAM_NOTHINGNEW;

	try 
	{
		DWORD   dwCmdLineNum = 1;
		CString strProductName;
		CString strProductVersion;
		CString strProductLanguage;
		CString strProductType;
		CString strProductMoniker;
		CString strCallbacksHere;
		CString strValue;
		hr = ptrProductReg.CreateInstance( __uuidof( luProductReg ),NULL,(CLSCTX_INPROC|CLSCTX_LOCAL_SERVER ) );

		// Find a command line that has a ProductName of "Avenge 1.5 MicroDefs"
		// and define that to be the "default" command line.
		for ( dwCmdLineNum = 1; 
			  GetCmdLineN( dwCmdLineNum, &strProductName, &strProductType,
						   &strProductLanguage, &strProductVersion, 
						   NULL, NULL, &strProductMoniker, &strCallbacksHere, NULL );
			  dwCmdLineNum++, strProductMoniker.Empty() )
		{
			try 
			{
				if ( !strProductMoniker.IsEmpty() )
				{
					_variant_t vUpdateStatus;
					hr = ptrProductReg->GetProperty( _bstr_t( strProductMoniker ), _bstr_t( g_szUpdateStatus ), &vUpdateStatus );
					if ( hr == S_FALSE || vUpdateStatus.vt == VT_EMPTY )
					{
						// If there was no update status, then
						// the entry has not appeared in the TRI file yet, 
						// so there can be no update.
						continue;
					}
					else 
					{
						CString sUpdateStatus = LPCTSTR( _bstr_t( vUpdateStatus ) );
						if ( sUpdateStatus.CompareNoCase( _T("NO") ) == 0 )
						{
							continue;
						}
#ifdef TESTING
						else
						{
							// Indicate successful download of at least one thing
							// (we'll continue checking below if there was an
							//  error downloading or patching this thing, but
							//  make status success to start, and only change it
							//  if we detect an error)
							if ( wLUStatus == OAM_NOTHINGNEW )
								wLUStatus = OAM_SUCCESS;
						}
#endif
					}
					_variant_t vDownloadStatus;
					hr = ptrProductReg->GetProperty( _bstr_t( strProductMoniker ), _bstr_t( g_szDownloadStatus ), &vDownloadStatus );
					// If there was an update available, then
					// there should also be a download status.
					// Download status is a number representing an OAM_XXXX code.
					if ( hr == S_FALSE || vDownloadStatus.vt == VT_EMPTY ) 
                                            continue;
					strValue = LPCTSTR( _bstr_t( vDownloadStatus ) );
					WORD wDownloadStatus = atoi( ( LPCTSTR ) strValue );
					if ( wDownloadStatus != OAM_SUCCESS )
					{
                                                if ( atoi( ( LPCSTR ) strCallbacksHere ) > 0 )
                                                    m_wLuPatchStatus = wDownloadStatus;

						wLUStatus = wDownloadStatus;
						// If there was an error downloading, there
						// can't be a patch status. Break out, as 
						// we're only interested in simulating
						// a return code, and if there's any error,
						// that is our return code.
						break; 
					}
											 
					_variant_t vPatchStatus;
					hr = ptrProductReg->GetProperty( _bstr_t( strProductMoniker ), _bstr_t( g_szPatchStatus ), &vPatchStatus );
					// If there was a successful download (and there must have to get
					// here), then there has to be a patch status.
					// Patch status is a number representing either OAM_SUCCESS or
					// OAM_ERROR
					if ( hr == S_FALSE || vPatchStatus.vt == VT_EMPTY ) 
                                            continue;
					strValue = LPCTSTR( _bstr_t( vPatchStatus ) );
					WORD wPatchStatus = atoi( ( LPCTSTR ) strValue );
					if ( wPatchStatus != OAM_SUCCESS )
					{
                        if ( atoi( ( LPCSTR ) strCallbacksHere ) > 0 )
                            m_wLuPatchStatus = wPatchStatus;

						wLUStatus = wPatchStatus;
						// If there was an error patching, 
						// break out, as we're only interested in simulating
						// a return code, and if there's any error,
						// that is our return code.
						break;
					}

					wLUStatus = OAM_SUCCESS;
				}
				else
				{
					ASSERT( FALSE );
				}
			}
			catch ( _com_error e )
			{
				ATLTRACE("GetStatusCodes() - Caught COM exception\n");
			}
		}

	}
	catch ( ... )
	{
		ATLTRACE( _T("RegisterCommandLines() - caught exception") );
		hr = E_FAIL;
	}

	ptrProductReg = NULL;
	return wLUStatus;
}

DWORD CNavLu::GetDefAnnuityStatus()
{
    return( m_DefAnnuityStatus );
}

//		This function will return TRUE if LiveUpdate is run in silent mode.  This function 
// can only determine this for LiveUpdate 1.6 and above.  In LiveUpdate 1.6, this function
// will only work if LuAll has already started.
//		This function will return FALSE if there is an error.
BOOL CNavLu::IsLiveUpdateInSilentMode()
{
	BOOL bResult = FALSE;
	HRESULT hr = S_OK;

	try
	{
		// Declare a Smart Pointer to the LiveUpdate engine.
		STLIVEUPDATECOMLib::IstCheckForUpdatesPtr	ptrCheckForUpdates;

		// Initialize the Smart Pointer.
		hr = ptrCheckForUpdates.CreateInstance( __uuidof( stCheckForUpdates ), NULL, 
											   (CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER) );
		if ( FAILED(hr) )
			throw hr;

		// Load the Settings into the Settings object.  (This has probably been done
		// already, but doing it again won't hurt anything.)
		hr = ptrCheckForUpdates->LoadSettings();
		if ( FAILED(hr) )
			throw hr;

		// Get a pointer to the Settings object.
		STLIVEUPDATECOMLib::IstSettingsPtr ptrSettings = ptrCheckForUpdates->GetSettings();
		if ( ptrSettings == NULL )
			throw E_FAIL;	

		// Get the Silent switch out of the settings. 
		_bstr_t bstrSilentMode = ptrSettings->GetProperty( bstrTEMP_SILENT_MODE );

		if ( bstrSilentMode.length() )
		{
            bResult = TRUE;
		}
		else
  		{
  			// this is used for detection with LU 1.62, where the TEMP\SILENT_MODE flag was not set
  			// mutex detection code taken from NIS function DoesSomeoneElseOwnMutex in alerttracker\atrack.cpp
  			HANDLE shMutex = ::CreateMutex(NULL, FALSE, SYM_AUTO_UPDATE_MUTEX);
  			DWORD dwRet = ::WaitForSingleObject(shMutex, 0);
  			switch(dwRet)
  			{
  			case WAIT_TIMEOUT:
  				bResult = TRUE;
  				break;
  			case WAIT_OBJECT_0:
  				::ReleaseMutex(shMutex);
  				break;
  			default://bad
  				break;
  			}
  			CloseHandle(shMutex);			
  		}
		// The Smart Pointers will take care of doing the appropriate Releases and
		// memory frees to release their associated objects.
	}
	catch(...)
	{
        // Something went wrong or LiveUpdate 1.6+ is not installed on this machine.
		ASSERT(FALSE);
		bResult = FALSE;
	}

	return bResult;
} // IsLiveUpdateInSilentMode
