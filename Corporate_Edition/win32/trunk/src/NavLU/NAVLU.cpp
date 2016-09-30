// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// NAVLU.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "savidefutilsloader.h"
#include "navlu.h"
#include "navluint.h"
#include "navluutils.h"
#include "DarwinResCommon.h"


#include "defutilsinterface.h"
#include "strings.h"
#include "clientreg.h"
#include "slic.h"
#include "licensehelper.h"
#include "vdbversion.h"
#include "vpstrutils.h"
#include "SymSaferRegistry.h"
#include "ccLibDllLink.h"

#include "ccTrace.h"
#include "ccSymDebugOutput.h"

#include "ccEveryoneDacl.h"
#include "ccosinfo.h"


ccSym::CDebugOutput g_DebugOutput(_T("NavLu"));
IMPLEMENT_CCTRACE(g_DebugOutput);

DARWIN_DECLARE_RESOURCE_LOADER(_T("NavLuRes.dll"), _T("NavLu"))

//
//  Note!
//
//      If this DLL is dynamically linked against the MFC
//      DLLs, any functions exported from this DLL which
//      call into MFC must have the AFX_MANAGE_STATE macro
//      added at the very beginning of the function.
//
//      For example:
//
//      extern "C" BOOL PASCAL EXPORT ExportedFunction()
//      {
//          AFX_MANAGE_STATE(AfxGetStaticModuleState());
//          // normal function body here
//      }
//
//      It is very important that this macro appear in each
//      function, prior to any calls into MFC.  This means that
//      it must appear as the first statement within the
//      function, even before any object variable declarations
//      as their constructors may generate calls into the MFC
//      DLL.
//
//      Please see MFC Technical Notes 33 and 58 for additional
//      details.
//

/////////////////////////////////////////////////////////////////////////////
// CNAVLUApp

BEGIN_MESSAGE_MAP(CNAVLUApp, CWinApp)
    //{{AFX_MSG_MAP(CNAVLUApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

SYM_OBJECT_MAP_BEGIN()                           
    SYM_OBJECT_ENTRY( IID_DefUtilsCallbackSink, CLuPatchProgress) 
	SYM_OBJECT_ENTRY( IID_SymFactory2, CLuPatchProgress)
SYM_OBJECT_MAP_END()    

/* Add better logging to NavLu after Almagest ships.
class CNavLuDefUtilsLogger : public IDefUtilsLogCallback
{
public:
	CNavLuDefUtilsLogger()
	{
		m_file = fopen ("c:\\defutils.txt", "at");
	}
	~CNavLuDefUtilsLogger()
	{
		fclose (m_file);
	}
    virtual void Log (const char *szLogEntry)
    {
		if (m_file)
		{
			fprintf (m_file, "%s", szLogEntry);
			fflush (m_file);
		}
    }
	FILE *m_file;
};
*/
/////////////////////////////////////////////////////////////////////////////
// CNAVLUApp construction

CNAVLUApp::CNAVLUApp()
{
    // add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNAVLUApp object

CNAVLUApp theApp;


//*************************************************************************
// DoLiveUpdate()
//
// NAVLUAPI WORD WINAPI DoLiveUpdate (
//      const char* szzPackageTypeFilters, Double NULL terminated list of filter strings.
//                                            These are PackageType's listed in the
//                                            CommandLines section of NAV's registry which
//                                            should NOT be requested.
//      DWORD dwFlags,                       Flags NAVLU_xxx from NAVLU.H
//      LICENSECALLBACKFN fnCheckLicense     Ptr to function for calling back to check for a license.
//
// Command lines requested should be in the registry under [NAV main key]\LiveUpdate\CmdLines.
//
// Returns:
//      DWORD    LOWORD is the LiveUpdate result code, see LiveUpdate.h
//               HIWORD is one of the NAVLURET_ values from NavLu.h
//*************************************************************************

NAVLUAPI DWORD WINAPI DoLiveUpdate (const char       *szzPackageTypeFilters,
                                    DWORD             dwFlags,
                                    LICENSECALLBACKFN fnCheckLicense)
{
    AFX_MANAGE_STATE( AfxGetStaticModuleState() );

    WORD lu_stat    = OAM_SUCCESS;
    WORD navlu_stat = NAVLURET_SUCCESS;
    bool abort      = false;


	// There are cases in which background remediation will happen
	// and either Intall or a user-initiated LU session will attempt 
	// to run at the same time. We display the same exact message
	// to the user that LU would have if we would have actually tried
	// to run LU.
	DWORD dwLastError = NULL;
	SECURITY_DESCRIPTOR sd;
	HANDLE hRunning = NULL;
	bool fValidSecurityDescriptor = false;
	bool fCreatedEveroneDacl = true;
	bool fSuccessfullySetSecurityDescriptorDacl = false;

	if(SUCCEEDED(g_ResLoader.Initialize()))
		AfxSetResourceHandle(g_ResLoader.GetResourceInstance());
	else
		return FALSE;

	// We have to create a security descriptor because we will get an
	// "ERROR_ACCESS_DENIED" when trying to create this named mutex from
	// one process when another already created it.
	fValidSecurityDescriptor = (NULL != InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION));
	if (fValidSecurityDescriptor)
	{

		SAV::CEveryoneDACL nullDacl;
		PACL pDacl= NULL;
		ccLib::COSInfo osInfo;
		if( osInfo.IsWinNT() )
		{
			if (nullDacl.Create() == TRUE)
			{
				pDacl= nullDacl;
			}
		}

		fSuccessfullySetSecurityDescriptorDacl = (NULL != SetSecurityDescriptorDacl(&sd, TRUE, pDacl, FALSE));
		if (fSuccessfullySetSecurityDescriptorDacl)
		{
			SECURITY_ATTRIBUTES sa = { sizeof sa, &sd, FALSE };
		
			// Check to see if anyone else is currently in here.
			hRunning = CreateMutex (&sa, FALSE, "Global\\SAVLU_RUNNING");
			dwLastError = GetLastError(); 
		}
	}

	// We first check for it already existing, since the documentation for CreateMutex was confusing
	// regarding the return values. In particular, we get either a NULL handle or a non-zero handle 
	// when the mutex already exists, but in both cases the extended error info will state "ERROR_ALREADY_EXISTS".
	// However, now that we are using a non-null DACL, we should be getting ERROR_ACCESS_DENIED when
	// the mutex already exists.  If we wanted to open it a second time, we'd need to use OpenMutex,
	// but we don't want to open it, we just want the correct failure.
	if ((ERROR_ALREADY_EXISTS == dwLastError) || (ERROR_ACCESS_DENIED == dwLastError))
	{
		if (NULL != hRunning)
			CloseHandle(hRunning);

		return ((DWORD)(NAVLURET_ALREADY_RUNNING << 16) | (DWORD)OAM_ERROR);
	}

	if (NULL == hRunning)
	{	
		return ((DWORD)(NAVLURET_ERROR << 16) | (DWORD)OAM_ERROR);
	}

    CNavLu  navLu ( AfxGetInstanceHandle(), szzPackageTypeFilters, dwFlags & NAVLU_SILENT? true : false );

    // Check content license and add to the product types to filter,
    // if license is out-of-date or invalid.  Caller specifies whether
    // or not we do this check.

    if( fnCheckLicense != NULL )
    {
        if( navLu.CheckContentLicense( dwFlags & NAVLU_SILENT ? true : false, abort, fnCheckLicense ) == false )
        {
            navlu_stat = NAVLURET_INVALID_LIC;
        }
    }

    if( abort )
        lu_stat = OAM_USERCANCEL;
    else
    {
        // Important Note: LiveUpdate is always performed, even if the license is invalid.
        // The idea is to always get software updates but not def updates.
        lu_stat = navLu.DoLiveUpdate();
    }

	CloseHandle (hRunning);

    return( (DWORD)(navlu_stat << 16) | (DWORD)lu_stat );
}

//****************************************************************************
// Static variable ***********************************************************
//****************************************************************************

static CNavLu *s_pNavLu = NULL;

//****************************************************************************
// Static functions **********************************************************
//****************************************************************************

static BOOL WINAPI NavFinishOAM(LPTSTR lpszBuffer, WORD wSizeOfBuffer, WORD wStatus, LPTSTR lpszTitle, LPDWORD lpdwUserData)
{
    BOOL bResult = FALSE;

    if (s_pNavLu)
        bResult = s_pNavLu->FinishOAM (lpszBuffer, wSizeOfBuffer, wStatus, lpszTitle, lpdwUserData) == true ? TRUE : FALSE;

    return bResult;
}

//****************************************************************************
// CNavLu function implementations *******************************************
//****************************************************************************

//////////////////////////////////////////////////////////////////////////
// Description:
//   Constructor.
//
// Parameters:
//   hInstance               [in] Parent window for LiveUpdate.
//   szzPackageTypeFilters [in] Double null terminated list of ProductType's to
//                                to filter. Pass NULL pointer to allow all types.
//   bReboot                 [in] true if the /SETUP command line switch
//                                was used.
//   bSetup                  [in] true if the /SETUP command line switch
//                                was used.
//
// Returns
//   The OAM_* result code from LiveUpdate.
//////////////////////////////////////////////////////////////////////////
CNavLu::CNavLu( HINSTANCE hInstance,
                LPCTSTR   szzPackageTypeFilters,
                bool      bSilent) :
    m_hInstance                    (hInstance),
    m_wLuFinishStatus              (0),
    m_dwDefAnnuityStatus           (DefAnnuityDownloadAllowed),
    m_bWantMicroDefs               (false),
    m_bWantDefUpdates              (true),
    m_bSetSeqNumToZero             (false),
    m_bSilent                      (bSilent),
    m_bRunLuAgain                  (false),
    m_nNumLuRuns                   (0)
{
    m_szHubDeltaDir   [0] = _T('\0');
    m_szDirectDeltaDir[0] = _T('\0');

    // Build the list of package type filters.
    LPCTSTR it = szzPackageTypeFilters;
    while( it != NULL && _tcslen( it ) > 0 )
    {
        // Save the filter (uppercased for easy comparison).
        AddPackageTypeFiltered( it );

        // Move to the next filter.
        it += (_tcslen(it) + 1);
    }
}

HINSTANCE CNavLu::GetInstance (void) const
{
    return m_hInstance;
}

#pragma optimize( "", off )
//////////////////////////////////////////////////////////////////////////
// Description:
//   Actually initiate a Shared Tech LiveUpdate session. This routine
//   does the setup work and then calls the private CallLiveUpdate function.
//
// Parameters:
//
// Returns
//   The OAM_* result code from LiveUpdate.
//////////////////////////////////////////////////////////////////////////
WORD CNavLu::DoLiveUpdate () throw ()
{
    do
    {
//		CNavLuDefUtilsLogger objDuLogger;
        CString              strCmdLine;
        bool                 bSuccess = false;

        // Get an instance of the IDefUtils interface.
        if (SYM_FAILED (m_objDefUtilsLdr.CreateObject (m_ptrDefUtils)))
            return OAM_ERROR;

        m_bRunLuAgain = false;

        try
        {
            // Check to see if we want Virus Defs.  This will dictate whether or not
            // we do special MicroDef processing and the text that we display to the
            // user in success and error dialog boxes.
            m_bWantDefUpdates = !IsPackageTypeFiltered( szLU_CmdLine_ProdTypeVDef ); // i.e. not filtered means we want it

            // Remember the pointer so that NavFinishOAM can reference this class instance.
            s_pNavLu = this;

            // If anything does not work correctly, return OAM_ERROR result;
            m_wLuFinishStatus = OAM_ERROR;

//			if (m_ptrDefUtils->AddLogCallback (&objDuLogger))
//				throw (WORD)OAM_ERROR;

            // Look at command line to determine whether we're requesting microdefs
            m_bWantMicroDefs = WantMicroDefs();

            if (m_bWantMicroDefs && m_bWantDefUpdates == true)
            {
                bool  bBinResult  = true;
                bool  bBinHubIsOk = true;
                TCHAR szHubVersion[VERSION_BUFFER_SIZE] = {0};

                if (m_ptrDefUtils)
                {
                    // PreMicroDef25Update sets up the Hawking environment for a 
                    // MicroDefs 2.5 update.
                    bSuccess = m_ptrDefUtils->PreMicroDef25Update (m_szHubDeltaDir,    sizeof (m_szHubDeltaDir),
                                                                m_szDirectDeltaDir, sizeof (m_szDirectDeltaDir),
                                                                szHubVersion,       sizeof (szHubVersion), 
                                                                &bBinHubIsOk, DU_LIVEUPDATE);

                    if (bSuccess)
                    {
                        // Note that even if we cannot edit the LU version field,
                        // we use the default and continue.
                        UpdateHubVersion(szHubVersion);
                    }
                    else
                    {
                        // Reset our member variables back to empty on error. This ensures
                        // that any changes caused by the PreMicroDef25Update() call gets
                        // erased on error.
                        m_szHubDeltaDir[0]    = '\0';
                        m_szDirectDeltaDir[0] = '\0';

                        DURESULT duResult = m_ptrDefUtils->GetLastResult();
                        // We couldn't set up the Hawking temp directory structure before LU. Basically, this means
                        // that we will not integrate any defs that we download.
                    }
                }

                // If the hub directory is "broken" (files are missing) then set its
                // sequence number to zero to force LiveUpdate to get it.
                m_bSetSeqNumToZero = (bBinHubIsOk == false);
            }

            BuildCmdLine (&strCmdLine, m_bSilent);

            CallLiveUpdate (strCmdLine);
        }
        catch (WORD wResult)
        {
            m_wLuFinishStatus = wResult;
        }

        // Since we are looping, delete any old IDefUtils object reference
        // before creating a new one.
        m_ptrDefUtils.Release();

        s_pNavLu = NULL;

        // The number of times we have run LU has increased by one.
        ++m_nNumLuRuns;
    }
    while (m_bRunLuAgain);

    return m_wLuFinishStatus;
}
#pragma optimize( "", on )

//////////////////////////////////////////////////////////////////////////
// Description:
//   Dynamically load the LiveUpdate DLL, update the sequence number,
//   and call DoOnramp (LiveUpdate entry point).
//
// Parameters:
//   strCmdLine  [in] The LiveUpdate command line.
//////////////////////////////////////////////////////////////////////////
void CNavLu::CallLiveUpdate (const CString &strCmdLine)
{
    SOnrampAutoMode autoMode =
    {
        {NULL, NULL,         NULL, NULL}, // WelcomeOAM ... not needed anymore.
        {NULL, NavFinishOAM, NULL, NULL}
    };

    // If anything does not work correctly, return OAM_ERROR result;
    m_wLuFinishStatus = OAM_ERROR;

    TCHAR szFile[_MAX_PATH+1];
    NavLuGetLuPathName (szFile, sizeof(szFile));
    HINSTANCE hLuInstance = LoadLibrary (szFile);

    if (hLuInstance)
    {
                            // --------------------------------
                            // SYMMAL32.DLL was loaded. Find
                            // the entry functions DoOnramp()
                            // and SetSquenceNumber()
                            // --------------------------------
        LPFDOONRAMP pfnDoOnramp =
            (LPFDOONRAMP) (GetProcAddress (hLuInstance, "DoOnramp"));

        LPFSETSEQUENCENUMBEREX pfnSetSequenceNumber =
            (LPFSETSEQUENCENUMBEREX) (GetProcAddress (hLuInstance, "SetSequenceNumberEx"));

        if (pfnSetSequenceNumber)
        {
            // Just attempt to set both the MicroDefs and XDB package sequence
            // numbers. On SAV Client machines, setting the microdefs sequence
            // number will be successful. On SAV Server machines, setting the
            // XDB sequence number will be successful.
            SetSequenceNumberMicrodefs (pfnSetSequenceNumber, m_bSetSeqNumToZero);
            SetSequenceNumberXdb       (pfnSetSequenceNumber);
        }

        if (pfnDoOnramp)
        {
            m_wLuFinishStatus =
                (*pfnDoOnramp)(m_hInstance, NULL, (LPTSTR)(LPCTSTR)strCmdLine, &autoMode);
        }
        FreeLibrary (hLuInstance);
    }
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
bool CNavLu::FinishOAM(LPTSTR lpszBuffer, WORD wSizeOfBuffer, WORD wStatus, LPTSTR lpszTitle, LPDWORD lpdwUserData)
{
    bool      ret                 = true;
    bool      bMicroDefUpdate     = false;
    bool      bMicroDefsDelivered = false;

    m_wLuFinishStatus = wStatus;

	if(m_bWantMicroDefs && m_bWantDefUpdates)
    {
        bool                     bProcessPatches = false;
        IDefUtilsCallbackSinkPtr ptrPatchProgress;

        // Call PostMicroDefUpdate so that the GetNumVirusEntries call will
        // be from the new definitions directory. Also, this ensures that
        // we cleanup before LiveUpdate potentially reboots the system.

        if (wStatus == OAM_SUCCESS)
        {
            bMicroDefsDelivered = (DoesDirHaveFiles (m_szHubDeltaDir) ||
                                   DoesDirHaveFiles (m_szDirectDeltaDir));

            if (bMicroDefsDelivered)
            {
                if (!m_bSilent)
                {
                    // Create the progress dialog. We ignore failures to allocate
                    // because NULL works for the remainder of this code. We just
                    // will not be displaying progress info during our MicroDefs
                    // processing ... oh, well.
                    CLuPatchProgress *pPatchProgress = NULL;
                    try
                    {
                        pPatchProgress = new CLuPatchProgress (m_bSilent);
                    }
                    SYMINTCATCHMEMORYEXECPTION()

                    if (pPatchProgress)
                    {
                        ptrPatchProgress = pPatchProgress;
                        pPatchProgress->Create();
                    }
                }

                bProcessPatches = true;
            }
        }

        TCHAR szPatchDll[MAX_PATH+1];
        bMicroDefUpdate = GetPatchDllFullPath (szPatchDll, sizeof (szPatchDll));

        if (bMicroDefUpdate)
            bMicroDefUpdate = m_ptrDefUtils->PostMicroDef25Update(szPatchDll, ptrPatchProgress, bProcessPatches);

        // Cause the progress dialog to disappear if it is visible.
        ptrPatchProgress.Release();

        // Do some special processing if the MicroDefs 2.5 fail to apply correctly.
        if (bMicroDefUpdate == false)
        {
            // If we are running silently, we can automatically run LU again when this
            // is our first time trying. There is no point trying more than once. If the
            // first MicroDefs download fails, the second one should download the full
            // error correction package (which really should never fail to install unless
            // there is no disk space).
            if (m_bSilent)
            {
                m_bRunLuAgain = m_nNumLuRuns == 0;
            }
            // Else, we are not silent. Let's tell the user that the update failed and
            // (if this is the first run) ask them it they want to run again. If this
            // is the second LU run, just tell them that the error is still present.
            else
            {
                CString strTitle;
                CString strMsg;

                strTitle.LoadString (IDS_MICRODEFS_FAILED_TITLE);
                strMsg  .LoadString (m_nNumLuRuns > 0? IDS_MICRODEFS_FAILED_MSG : IDS_MICRODEFS_FAILED_MSG_RUN_AGAIN);

                int nResponse = MessageBox (NULL, strMsg, strTitle,
                                            (m_nNumLuRuns > 0? MB_OK : MB_YESNO) | MB_ICONERROR);

                // Rememorize if the user wants to run again.
                m_bRunLuAgain = nResponse == IDYES;
            }
        }
    }

    return ret;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Fetch date and version information for the newest definitions sitting
//   on this machine.
//
// Parameters:
//   pdwDefYear      [out] Definition's internal year    stamp
//   pdwDefMonth     [out] Definition's internal month   stamp
//   pdwDefYear      [out] Definition's internal day     stamp
//   pdwDefVersion   [out] Definition's internal version stamp
//
// Returns
//   true  - function was successful.
//   false - function failed.
//////////////////////////////////////////////////////////////////////////
bool CNavLu::GetVirusDefDate (LPDWORD pdwDefYear    /*= NULL*/,
                              LPDWORD pdwDefMonth   /*= NULL*/,
                              LPDWORD pdwDefDay     /*= NULL*/,
                              LPDWORD pdwDefVersion /*= NULL*/) const
{
    WORD            wYear;
    WORD            wMonth;
    WORD            wDay;
    DWORD           dwVersion;
    CDefUtilsLoaderHelper objDefUtilsLdr;
    IDefUtils4Ptr   ptrDefUtils;

    // Get an instance of the IDefUtils interface.
    if (SYM_FAILED (objDefUtilsLdr.CreateObject (ptrDefUtils)))
        return false;

    if (ptrDefUtils->GetNewestDefsDate (&wYear, &wMonth, &wDay, &dwVersion) == false)
    {
        const char *szResult = ptrDefUtils->DefUtilsGetLastResultString ();
        
        // No need to display an error message; any type of error is treated as having a Defs Date of zero.
        return false;
    }

    if (pdwDefYear)
        *pdwDefYear = wYear;

    if (pdwDefMonth)
        *pdwDefMonth = wMonth;

    if (pdwDefDay)
        *pdwDefDay = wDay;

    if (pdwDefVersion)
        *pdwDefVersion = dwVersion;

    return true;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Fetch date and version information for the hub on this machine.
//
// Parameters:
//   pdwDefYear      [out] Hub's internal year    stamp
//   pdwDefMonth     [out] Hub's internal month   stamp
//   pdwDefYear      [out] Hub's internal day     stamp
//   pdwDefVersion   [out] Hub's internal version stamp
//
// Returns
//   true  - function was successful.
//   false - function failed.
//////////////////////////////////////////////////////////////////////////
bool CNavLu::GetHubDate (LPDWORD pdwDefYear    /*= NULL*/,
                         LPDWORD pdwDefMonth   /*= NULL*/,
                         LPDWORD pdwDefDay     /*= NULL*/,
                         LPDWORD pdwDefVersion /*= NULL*/) const
{
    WORD            wYear;
    WORD            wMonth;
    WORD            wDay;
    DWORD           dwVersion;
    CDefUtilsLoaderHelper objDefUtilsLdr;
    IDefUtils4Ptr   ptrDefUtils;

    // Get an instance of the IDefUtils interface.
    if (SYM_FAILED (objDefUtilsLdr.CreateObject (ptrDefUtils)))
        return false;

    if (ptrDefUtils->GetBinHubDefsDate (&wYear, &wMonth, &wDay, &dwVersion) == false)
    {
        const char *szResult = ptrDefUtils->DefUtilsGetLastResultString ();
        // No need to display an error message; any type of error is treated as having a Hub Date of zero.
        return false;
    }

    if (pdwDefYear)
        *pdwDefYear = wYear;

    if (pdwDefMonth)
        *pdwDefMonth = wMonth;

    if (pdwDefDay)
        *pdwDefDay = wDay;

    if (pdwDefVersion)
        *pdwDefVersion = dwVersion;

    return true;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Find the latest XDB that is in the SAV installation directory and
//   translate the XDB name into year/month/day/version data.
//
// Parameters:
//   pdwDefYear      [out] XDB's internal year
//   pdwDefMonth     [out] XDB's internal month
//   pdwDefYear      [out] XDB's internal day
//   pdwDefVersion   [out] XDB's internal version
//
// Returns
//   true  - function was successful.
//   false - function failed or did not find any XDB files.
//////////////////////////////////////////////////////////////////////////
bool CNavLu::GetXdbDate (LPDWORD pdwDefYear, 
                         LPDWORD pdwDefMonth,
                         LPDWORD pdwDefDay,
                         LPDWORD pdwDefVersion) const
{
    SAVASSERT (pdwDefYear);
    SAVASSERT (pdwDefMonth);
    SAVASSERT (pdwDefDay);
    SAVASSERT (pdwDefVersion);

    TCHAR  szXdbDir [MAX_PATH+1];
    TCHAR  szXdbFile[MAX_PATH+1];
    HKEY   hKey    = NULL;
    LONG   lResult = 0;

    szXdbFile[0] = _T('\0');

    // Reset our [out] parameters
    *pdwDefYear    = 0;
    *pdwDefMonth   = 0;
    *pdwDefDay     = 0;
    *pdwDefVersion = 0;

    if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, szReg_Key_Main, 0, KEY_QUERY_VALUE, &hKey))
        return false;

    DWORD dwType     = 0;
    DWORD dwByteSize = sizeof (szXdbDir);

    // Get the name of the SAV home directory ... where all the XDBs are located.
    lResult = SymSaferRegQueryValueEx (hKey, szReg_Val_HomeDir, NULL, &dwType, 
                               reinterpret_cast<unsigned char*>(szXdbDir), &dwByteSize);

    RegCloseKey(hKey);

    if (ERROR_SUCCESS != lResult)
        return false;

    vpstrnappendfile (szXdbDir, _T("*.XDB"), sizeof (szXdbDir));

    HANDLE           hFind = NULL;
    WIN32_FIND_DATA  stFind;

    hFind = FindFirstFile (szXdbDir, &stFind);

    // No XDB files in the SAV directory. Tell the caller.
    if (hFind == INVALID_HANDLE_VALUE)
        return false;

    // Loop through all of the XDBs looking for the largest versioned XDB.
    unsigned long ulMaxXdbVer = 0;
    do
    {
        unsigned long ulCurXdbVer = VDBVersion (stFind.cFileName);
        ulMaxXdbVer = max (ulMaxXdbVer, ulCurXdbVer);
    }
    while (FindNextFile (hFind, &stFind));

    FindClose (hFind);

    if (ulMaxXdbVer == 0)
        return false;

    *pdwDefYear    = ((ulMaxXdbVer >> 18) + 1998);
    *pdwDefMonth   = ((ulMaxXdbVer >> 14) & 0x0f);
    *pdwDefDay     = ((ulMaxXdbVer >>  9) & 0x1f);
    *pdwDefVersion = (ulMaxXdbVer         & 0x1FF);

    return true;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Set the LiveUpdate sequence number based on the current Hawking virus
//   definition version information.
//
// Parameters:
//   pfnSetSequenceNumber   [in] Pointer to LiveUpdate's SetSequenceNumber
//                               function.
//////////////////////////////////////////////////////////////////////////
void CNavLu::SetSequenceNumberMicrodefs (LPFSETSEQUENCENUMBEREX pfnSetSequenceNumber, bool bSetToZero) const
{
    DWORD     dwCmdLineNum = 1;
    CString   strMatchProductName (_T(szReg_Val_LU_ProductName_MDefsBase));
    CString   strMatchHubDefsType (_T(szLU_CmdLine_ProdTypeHubDefs));
    CString   strProductName;
    CString   strProductType;
    CString   strProductVersion;
    CString   strProductLanguage;
    DWORD     dwDefYear;
    DWORD     dwDefMonth;
    DWORD     dwDefDay;
    DWORD     dwDefVersion;
    DWORD     dwDefDate;
    DWORD     dwHubYear;
    DWORD     dwHubMonth;
    DWORD     dwHubDay;
    DWORD     dwHubVersion;
    DWORD     dwHubDate;
    bool      bSuccess;

    // If the caller wants the sequence number to be zero, do it.
    if (bSetToZero)
    {
        dwDefDate = 0;
        dwHubDate = 0;
    }
    else
    {
        // Fetch the internal virus definition version.
        bSuccess = GetVirusDefDate (&dwDefYear, &dwDefMonth, &dwDefDay, &dwDefVersion);

        // YYYMMDDRRR format
        dwDefDate  = (dwDefYear - 2000) * 10000000;
        dwDefDate += dwDefMonth         * 100000;
        dwDefDate += dwDefDay           * 1000;
        dwDefDate += (dwDefVersion % 1000);          // chop the version down to 3 digits.

        // Since no internal version found, set the Lu Sequence Key to zero.
        if (!bSuccess)
            dwDefDate = 0;

        // Fetch the internal hub version.
        bSuccess = GetHubDate (&dwHubYear, &dwHubMonth, &dwHubDay, &dwHubVersion);

        // YYYMMDDRRR format
        dwHubDate  = (dwHubYear - 2000) * 10000000;
        dwHubDate += dwHubMonth         * 100000;
        dwHubDate += dwHubDay           * 1000;
        dwHubDate += (dwHubVersion % 1000);          // chop the version down to 3 digits.

        // Since no internal version found, set the Lu Sequence Key to zero.
        if (!bSuccess)
            dwHubDate = 0;
    }

    // Find a command line that has a ProductName of "Avenge MicroDefs25".
    while (GetCmdLineN (dwCmdLineNum, &strProductName, &strProductType,
                        &strProductLanguage, &strProductVersion, NULL))
    {
        ++dwCmdLineNum;
        if( _tcsnicmp( strProductName, strMatchProductName, strMatchProductName.GetLength() ) == 0 )
        {
            if( strProductType.CompareNoCase( strMatchHubDefsType ) == 0 )
            {
                // Set the LU sequence number of the Hub Definitions
                // based on our the defs in the BinHub directory.
                (*pfnSetSequenceNumber)((LPTSTR)(LPCTSTR)strProductName,
                                        (LPTSTR)(LPCTSTR)strProductType,
                                        (LPTSTR)(LPCTSTR)strProductVersion,
                                        (LPTSTR)(LPCTSTR)strProductLanguage,
                                        dwHubDate);
            }
            else
            {
                // Set the LU sequence number of the Virus Definitions
                // based on our newest on-disk defs.
                (*pfnSetSequenceNumber)((LPTSTR)(LPCTSTR)strProductName,
                                        (LPTSTR)(LPCTSTR)strProductType,
                                        (LPTSTR)(LPCTSTR)strProductVersion,
                                        (LPTSTR)(LPCTSTR)strProductLanguage,
                                        dwDefDate);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Set LiveUpdate's XDB package sequence number based on the current XDB
//   registry information. If registry specifies an XDB file that does not
//   exist, the XDB's PVL is set to 0.
//
// Parameters:
//   pfnSetSequenceNumber   [in] Pointer to LiveUpdate's SetSequenceNumber
//                               function.
//////////////////////////////////////////////////////////////////////////
void CNavLu::SetSequenceNumberXdb (LPFSETSEQUENCENUMBEREX pfnSetSequenceNumber) const
{
    DWORD     dwCmdLineNum = 1;
    CString   strMatchName(_T(szReg_Val_LU_ProductName_VDefsServer));
    CString   strMatchType(_T(szReg_Val_LU_CmdLine_ProdTypeVDef));
    CString   strProductName;
    CString   strProductType;
    CString   strProductVersion;
    CString   strProductLanguage;
    DWORD     dwDefYear;
    DWORD     dwDefMonth;
    DWORD     dwDefDay;
    DWORD     dwDefVersion;
    DWORD     dwDefDate = 0;
    bool      bSuccess;

    // Get the currently active XDB version info. "false" is returned when
    // the registry XDB info refers to a non-existant XDB file.
    bSuccess = GetXdbDate (&dwDefYear, &dwDefMonth, &dwDefDay, &dwDefVersion);

    if (bSuccess)
    {
        // YYYMMDDRRR format
        dwDefDate  = (dwDefYear - 2000) * 10000000;
        dwDefDate += dwDefMonth         * 100000;
        dwDefDate += dwDefDay           * 1000;
        dwDefDate += (dwDefVersion % 1000);          // chop the version down to 3 digits.
    }

    // Find a command line that has a ProductName of "Symantec AntiVirus 
    // Corporate Server" and a ProductType of "VirusDef".
    while (GetCmdLineN (dwCmdLineNum, &strProductName, &strProductType,
                        &strProductLanguage, &strProductVersion, NULL))
    {
        ++dwCmdLineNum;
        if (_tcsicmp (strProductName, strMatchName) == 0 &&
            _tcsicmp (strProductType, strMatchType) == 0)
        {
            // Set the XDB package's Lu Sequence Key based on the retrieved
            // date info.
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
//
// Returns:
//   The count of LiveUpdate commands that are placed into pstrCmdLine.
//////////////////////////////////////////////////////////////////////////

DWORD CNavLu::BuildCmdLine (CString *pstrCmdLine, bool bSilent) const
{
    LPCTSTR     szKey           = g_szCmdLinesKey;
    LPCTSTR     szValue         = g_szOtherSwitchesValue;
    DWORD       dwCmdLinesAdded = 0;
    TCHAR       szValueBuf[1024];
    DWORD       dwValueBufLen   = sizeof (szValueBuf);
    HKEY        hBaseKey;
    DWORD       dwValueType;
    bool        bSkipped;
    TCHAR       szKeyBuf[MAX_PATH];

    // We always have the auto-mode switch.
    *pstrCmdLine = "-a ";

    // If we are a Scheduled LiveUpdate, operate core LU in silent mode.
    if (bSilent)
        *pstrCmdLine += "-s ";

    //  Get the OtherSwitches value from the CmdLines key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, szKey, 0, KEY_READ, &hBaseKey))
    {
        if (ERROR_SUCCESS == SymSaferRegQueryValueEx (hBaseKey, szValue, 0, &dwValueType, (LPBYTE)szValueBuf, &dwValueBufLen))
        {
            if (dwValueType == REG_SZ && dwValueBufLen)
                *pstrCmdLine += szValueBuf;

            // If there was something in the OtherSwitches value, add a space.
            if (pstrCmdLine->GetLength() > 0)
                *pstrCmdLine += _T(" ");
        }
        RegCloseKey(hBaseKey);
    }

    // Loop through all of the CmdLines\CmdLine# keys.

    DWORD dwCmdLineCnt = 1;
    
    // Build the command line key name.
    vpsnprintf (szKeyBuf, sizeof (szKeyBuf), g_szCmdLineNKey, dwCmdLineCnt);

    while( AppendCmdLine( szKeyBuf, pstrCmdLine, &bSkipped ) == true )
    {
        if ( bSkipped == false )
        {
            *pstrCmdLine += _T(" ");
            ++dwCmdLinesAdded;
        }

        ++dwCmdLineCnt;
        
        // Build the command line key name.
        vpsnprintf (szKeyBuf, sizeof (szKeyBuf), g_szCmdLineNKey, dwCmdLineCnt);
    }

    // Loop through all of the Managed Products\CmdLines\CmdLine# keys.

    if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, g_szMngProdsKey, 0, KEY_READ, &hBaseKey ) == ERROR_SUCCESS )
    {
        TCHAR key_name[128];
        DWORD key_name_size = sizeof(key_name) / sizeof(key_name[0]) + 1;
        DWORD index         = 0;

        while( RegEnumKeyEx( hBaseKey, index, key_name, &key_name_size,
                             NULL, NULL, NULL, NULL ) == ERROR_SUCCESS )
        {
            dwCmdLineCnt = 1; // reset this

            // Build the command line key name.
            vpsnprintf (szKeyBuf, sizeof (szKeyBuf), g_szMngProdCmdLineNKey, key_name, dwCmdLineCnt);

            while( AppendCmdLine( szKeyBuf, pstrCmdLine, &bSkipped ) == true )
            {
                if( bSkipped == false )
                {
                    *pstrCmdLine += _T(" ");
                    ++dwCmdLinesAdded;
                }

                ++dwCmdLineCnt;

                // Build the command line key name.
                vpsnprintf (szKeyBuf, sizeof (szKeyBuf), g_szMngProdCmdLineNKey, key_name, dwCmdLineCnt);
            }
            index++;
            key_name_size = sizeof(key_name) / sizeof(key_name[0]) + 1;
        }
        RegCloseKey( hBaseKey );
    }

    return dwCmdLinesAdded;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//   Add the specified command line to pstrCmdLine. This reads
//   the command line from the registry.
//
// Parameters:
//   szKeyBuf            [in]  Name of the key that holds the Command line
//   pstrCmdLine         [i/o] Command line string to append onto.
//
// Returns
//   TRUE  - function was successful.
//   FALSE - function failed. This is generally because a "CmdLine<dwCmdLineNum>"
//           value does not exist in the registry.
//////////////////////////////////////////////////////////////////////////
bool CNavLu::AppendCmdLine (LPCTSTR szKeyBuf, CString *pstrCmdLine, bool* pbSkipped) const
{
    typedef struct
    {
        LPCTSTR szValueName;
        LPCTSTR szLuSwitch;
        bool    bQuote;         // TRUE when fetched value should be quoted. Ex. -t"VirusDefs"
    } LOOP_INFO, *PLOOP_INFO;

    LOOP_INFO   pLoopInfo[] = { { g_szOtherSwitchesValue,   _T(""),   false },
                                { g_szProductNameValue,     _T("-p"), true },
//                              { g_szProductTypeValue,     _T("-t"), true }, // don't append the type to the cmd line anymore
                                { g_szProductLanguageValue, _T("-l"), true },
                                { g_szProductVersionValue,  _T("-v"), true } };

    const DWORD      dwNumLoopInfoElems = sizeof (pLoopInfo) / sizeof (pLoopInfo[0]);

    auto  TCHAR      szValueBuf[1024];
    auto  DWORD      dwValueBuf    = 0;
    auto  DWORD      dwValueBufLen = 0;
    auto  DWORD      dwValueType   = 0;
    auto  bool       bRtn          = true;
    auto  HKEY       hBaseKey;

    *pbSkipped = false;

    // Open the key.
    if (ERROR_SUCCESS != RegOpenKeyEx (HKEY_LOCAL_MACHINE, szKeyBuf, 0, KEY_READ, &hBaseKey))
    {
        // Return FALSE when the CmdLine%d key does not exist.
        bRtn = false;
        goto CleanupAndExit;
    }
    else
    {
        // If this command line relates to virus definitions, check to see if the sub-
        // scription has expired. If so, skip this command line.

        dwValueBufLen = sizeof (szValueBuf);

        if (ERROR_SUCCESS == SymSaferRegQueryValueEx (hBaseKey, g_szGroupValue, 0, NULL, (LPBYTE)szValueBuf, &dwValueBufLen))
        {
            if ( _ttoi( szValueBuf ) > 0 )
            {
                if ( ( m_dwDefAnnuityStatus == DefAnnuityUpdatePatches ) || ( m_dwDefAnnuityStatus == DefAnnuityDownloadNotAllowed ) || ( m_dwDefAnnuityStatus == DefAnnuityCancelUpdate ) )
                    goto SkipAndExit;
            }
        }

        // If this command line is marked as not being part of the LU command line,
        // skip it. The second CmdLine# entry for the MicroDefs hub update will
        // have the "InLuCmdLine=0" setting to ensure that only one PVL is listed
        // with LU when we have two CmdLine# entries for the hub updates. We need
        // two in order to correctly set the sequence numbers for both the HubDefs
        // and the CurDefs PVL.

        dwValueBufLen = sizeof (dwValueBuf);
        dwValueBuf    = 0;

        // Default behavor: Include CmdLine# in LU CmdLine (same as "InLuCmdLine" (DWORD) != 0)
        if (ERROR_SUCCESS == SymSaferRegQueryValueEx (hBaseKey, szReg_Val_LU_CmdLine_InLuCmdLine, 0,
                                              NULL, (LPBYTE)dwValueBuf, &dwValueBufLen))
        {
            if (dwValueBuf == 0)
                goto SkipAndExit;
        }

        // Pull the ProductType from the registry.
        dwValueBufLen = sizeof (szValueBuf);
        if (ERROR_SUCCESS != SymSaferRegQueryValueEx (hBaseKey, g_szProductTypeValue, 0, &dwValueType,
                                              (LPBYTE)szValueBuf, &dwValueBufLen) ||
            dwValueType != REG_SZ)
        {
            goto SkipAndExit;
        }
        CString strProdTypeRegValue (szValueBuf);

        // Before we change the command line, filter product types that the caller
        // wants us to exclude.
        if ( m_PackageTypeFilters.GetCount() > 0 )
        {
             if (IsPackageTypeFiltered( strProdTypeRegValue ) == true )
                goto SkipAndExit;
        }

        // Do some tamper proofing.  We know the product name for each MicroDefs and VirusDef cmd lines.
        // Make sure that their ProductTypes are properly matched up.

        dwValueBufLen = sizeof (szValueBuf);

        if( ERROR_SUCCESS == SymSaferRegQueryValueEx (hBaseKey, g_szProductNameValue, 0, &dwValueType,
                                             (LPBYTE)szValueBuf, &dwValueBufLen) &&
            dwValueType == REG_SZ )
        {
            // If this ProductName begins with "Avenge MicroDefs25", ensure that it also has
            // an associated ProductType of either "CurDefs" or "HubDefs".
            if (_tcsnicmp (szValueBuf,
                           szReg_Val_LU_ProductName_MDefsBase,
                           _tcslen (szReg_Val_LU_ProductName_MDefsBase)) == 0)
            {
                if (strProdTypeRegValue.CompareNoCase (szLU_CmdLine_ProdTypeHubDefs) != 0 &&
                    strProdTypeRegValue.CompareNoCase (szLU_CmdLine_ProdTypeCurDefs) != 0)
                    goto SkipAndExit;
            }
            // Else, if this ProductName is "Symantec AntiVirus Corporate Server" the
            // ProductType must be "VirusDef", otherwise we skip this PVL.
            else if (_tcsicmp (szValueBuf, szReg_Val_LU_ProductName_VDefsServer) == 0 &&
                     strProdTypeRegValue.CompareNoCase (szLU_CmdLine_ProdTypeVDef) != 0)
            {
                goto SkipAndExit;
            }
        }

        // Parse command line.
        *pstrCmdLine += "[";

        bool bLastLoop = false;
        for (LOOP_INFO *pCurLoopInfo = pLoopInfo; !bLastLoop;)
        {
            dwValueBufLen  = sizeof (szValueBuf);
            bool bArgAdded = false;

            // Fetch the value and add the value to the command line.
            if (ERROR_SUCCESS == SymSaferRegQueryValueEx (hBaseKey, pCurLoopInfo->szValueName, 0, &dwValueType, (LPBYTE)szValueBuf, &dwValueBufLen))
            {
                if (dwValueType == REG_SZ && dwValueBufLen)
                {
                    *pstrCmdLine += pCurLoopInfo->szLuSwitch;

                    if (pCurLoopInfo->bQuote)
                        *pstrCmdLine += "\"";

                    *pstrCmdLine += szValueBuf;

                    if (pCurLoopInfo->bQuote)
                        *pstrCmdLine += "\"";

                    bArgAdded = true;
                }
            }

            ++pCurLoopInfo;
            bLastLoop = ((pCurLoopInfo - pLoopInfo) >= dwNumLoopInfoElems);

            if (!bLastLoop && bArgAdded)
                *pstrCmdLine += " ";
        }
    }

    *pstrCmdLine += "]";

    bRtn = true; // Everything processed okay.

    goto CleanupAndExit;

SkipAndExit:
    *pbSkipped = true;
    bRtn       = true;

CleanupAndExit:
    RegCloseKey(hBaseKey);

    return bRtn; // Everything processed okay.
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
bool CNavLu::GetCmdLineN (DWORD   dwCmdLineNum,
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

    LOOP_INFO   pLoopInfo[] = { { g_szOtherSwitchesValue,   pstrOtherSwitches },
                                { g_szProductNameValue,     pstrProductName },
                                { g_szProductTypeValue,     pstrProductType },
                                { g_szProductLanguageValue, pstrProductLanguage },
                                { g_szProductVersionValue,  pstrProductVersion } };

    const DWORD      dwNumLoopInfoElems = sizeof (pLoopInfo) / sizeof (pLoopInfo[0]);

    auto  TCHAR      szKeyBuf[MAX_PATH];
    auto  TCHAR      szValueBuf[1024];
    auto  DWORD      dwValueBufLen;
    auto  HKEY       hBaseKey;
    auto  DWORD      dwValueType;

    // Put the command line number into the key name.
    vpsnprintf (szKeyBuf, sizeof (szKeyBuf), g_szCmdLineNKey, dwCmdLineNum);

    // Open the key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, szKeyBuf, 0, KEY_READ, &hBaseKey))
    {
        for (LOOP_INFO *pCurLoopInfo = pLoopInfo;
             pCurLoopInfo - pLoopInfo < dwNumLoopInfoElems;
             ++pCurLoopInfo)
        {
            // Skip all values the caller does not care about.
            if (NULL == pCurLoopInfo->pstrOutput)
                continue;

            // Clear the CString.
            pCurLoopInfo->pstrOutput->Empty();

            dwValueBufLen = sizeof (szValueBuf);

            // Fetch the value and add the value to the command line.
            if (ERROR_SUCCESS == SymSaferRegQueryValueEx (hBaseKey, pCurLoopInfo->szValueName, 0, &dwValueType, (LPBYTE)szValueBuf, &dwValueBufLen))
            {
                if (dwValueType == REG_SZ && dwValueBufLen)
                    *pCurLoopInfo->pstrOutput = szValueBuf;
            }
        }
    }
    else
    {
        // Return FALSE when the CmdLine%d key does not exist.
        return false;
    }

    RegCloseKey(hBaseKey);

    return true; // Everything processed okay.
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
bool CNavLu::SetCmdLineN (DWORD          dwCmdLineNum,
                          const CString *pstrProductName,
                          const CString *pstrProductType,
                          const CString *pstrProductLanguage,
                          const CString *pstrProductVersion,
                          const CString *pstrOtherSwitches) const
{
    typedef struct
    {
        LPCTSTR        szValueName;
        const CString *pstrOutput;
    } LOOP_INFO, *PLOOP_INFO;

    LOOP_INFO   pLoopInfo[] = { { g_szOtherSwitchesValue,   pstrOtherSwitches   },
                                { g_szProductNameValue,     pstrProductName     },
                                { g_szProductTypeValue,     pstrProductType     },
                                { g_szProductLanguageValue, pstrProductLanguage },
                                { g_szProductVersionValue,  pstrProductVersion  } };

    const DWORD      dwNumLoopInfoElems = sizeof (pLoopInfo) / sizeof (pLoopInfo[0]);

    auto  TCHAR      szKeyBuf[MAX_PATH];
    auto  HKEY       hBaseKey;
    auto  bool       bResult = false;


    // Put the command line number into the key name.
    vpsnprintf (szKeyBuf, sizeof (szKeyBuf), g_szCmdLineNKey, dwCmdLineNum);

    // Open the key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, szKeyBuf, 0, KEY_WRITE, &hBaseKey))
    {
        for (LOOP_INFO *pCurLoopInfo = pLoopInfo;
             pCurLoopInfo - pLoopInfo < dwNumLoopInfoElems;
             ++pCurLoopInfo)
        {
            // Skip all values the caller does not care about.
            if (NULL == pCurLoopInfo->pstrOutput)
                continue;

            // Set the new LiveUpdate ProductType value
            if (ERROR_SUCCESS == RegSetValueEx (hBaseKey,
                                                pCurLoopInfo->szValueName,
                                                0,
                                                REG_SZ,
                                                (const unsigned char *)pCurLoopInfo->pstrOutput->GetString(),
                                                pCurLoopInfo->pstrOutput->GetLength() + 1 ))
            {
                bResult = true;
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
bool CNavLu::UpdateHubVersion(const CString &strNewVersion) const
{
    DWORD     dwCmdLineNum = 1;
    CString   strMatchName (_T(szReg_Val_LU_ProductName_MDefsBase));
    CString   strMatchVersion (_T(szLU_CmdLine_ProdVersionCurDefs));
    CString   strProductName;
    CString   strProductType;
    CString   strProductVersion;
    CString   strProductLanguage;

    size_t nNumFound = 0;
    while (nNumFound < 2 &&
           GetCmdLineN (dwCmdLineNum, &strProductName, &strProductType,
                        &strProductLanguage, &strProductVersion, NULL))
    {
        // Find a command line that begins with a ProductName of "Avenge MicroDefs25".
        // There should be three matching this description. We want to update the two
        // that do not have a "MicroDefsB.CurDefs" version.
        if (_tcsnicmp (strProductName, strMatchName, strMatchName.GetLength()) == 0 &&
            strProductVersion.CompareNoCase (strMatchVersion) != 0)
        {
            SetCmdLineN (dwCmdLineNum, NULL, NULL, NULL, &strNewVersion, NULL);
            ++nNumFound;
        }
        ++dwCmdLineNum;
    }

    return nNumFound? true : false;
}

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
bool CNavLu::DoesDirHaveFiles ( LPCTSTR szDir ) const
{
    auto bool               bReturnVal = false;
    auto WIN32_FIND_DATA    stFindData;
    auto TCHAR              szAllFiles[MAX_PATH];

    // Check for files in the temp directory for binary patch files.
    memset (&stFindData, 0, sizeof(stFindData));

    vpstrncpy        (szAllFiles, szDir,     sizeof (szAllFiles));
    vpstrnappendfile (szAllFiles, _T("*.*"), sizeof (szAllFiles));

    // Go through twice: 1st or binary patches, 2nd for text patches.
    HANDLE hFind = FindFirstFile (szAllFiles, &stFindData);

    if ( hFind != INVALID_HANDLE_VALUE )
    {
        while (  FindNextFile(hFind, &stFindData) )
        {
            if ( stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                continue;

            if ( stFindData.dwFileAttributes != 0 )
            {
                bReturnVal = true;
                break;
            }
        }
        FindClose (hFind );
    }

    return bReturnVal;
}


//////////////////////////////////////////////////////////////////////////
// Description:
//   Determine if we need to do microdef processing
//
//////////////////////////////////////////////////////////////////////////
bool CNavLu::WantMicroDefs (void)
{
    DWORD     dwCmdLineNum = 1;
    CString   strProductName;
    CString   strProductType;
    CString   strProductVersion;
    CString   strProductLanguage;

    // Find a command line that has a ProductVersion with "MicroDefs".
    while (GetCmdLineN (dwCmdLineNum, &strProductName, &strProductType,
                        &strProductLanguage, &strProductVersion, NULL))
    {
        ++dwCmdLineNum;
        if (-1 != strProductVersion.Find(PRODUCT_VERSION_MICRODEFS))
        {
            return true;
        }
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
//
// Description:
//   See if a certain type of package has been filtered.
//
// Parameters:
//   LPCTSTR type     - Package type to check for.
//
// Returns:
//   bool
//    true == type is filtered and will not be downloaded
//    false == type is not filtered and will be downloaded.
//
//////////////////////////////////////////////////////////////////////////
bool CNavLu::IsPackageTypeFiltered( LPCTSTR type ) const
{
    CString compare( type );
    compare.MakeUpper();
    return( m_PackageTypeFilters.Find( compare ) != NULL );
}

//////////////////////////////////////////////////////////////////////////
//
// Description:
//   Add a package type to the list that should be filtered.
//
// Parameters:
//   LPCTSTR type     - Package type to filter.
//
// Returns:
//   void
//
//////////////////////////////////////////////////////////////////////////
void CNavLu::AddPackageTypeFiltered( LPCTSTR type )
{
    if( IsPackageTypeFiltered( type ) == false )
    {
        CString add( type );
        add.MakeUpper();
        m_PackageTypeFilters.AddTail( add );
    }
}

//////////////////////////////////////////////////////////////////////////
//
// Description:
//   Make sure that we posess a valid content license.
//
// Parameters:
//   bool bSilentMode             - true to do the licensing check in silent mode.
//   bool& bAbort                 - [out] true if the user aborted the LU session because
//                                  of the license status
//   LICENSECALLBACKFN fnLicCheck - callback to do the license check
//
// Returns: (CGOLDSCHMIDT)
//   bool: true if contetn can be downloaded.
//
//////////////////////////////////////////////////////////////////////////
bool CNavLu::CheckContentLicense( bool bSilentMode, bool& bAbort, LICENSECALLBACKFN fnLicCheck )
{
    // Call the caller's license check callback.

    BOOL bAbortInt = FALSE;

    bool allow_content_download = fnLicCheck( bSilentMode == true ? TRUE : FALSE, &bAbortInt ) ? true : false;

    bAbort = bAbortInt ? true : false;

    // Do we have a license to download content?

    if( allow_content_download == false )
    {
        // No license == no content.
        // Place the content filters in our filter list.

        AddPackageTypeFiltered (szLU_CmdLine_ProdTypeVDef);
        AddPackageTypeFiltered (szLU_CmdLine_ProdTypeHubDefs);
        AddPackageTypeFiltered (szLU_CmdLine_ProdTypeCurDefs);
        AddPackageTypeFiltered (szLU_CmdLine_ProdTypeSCFContent);
    }

    // Return whether the license is valid or not.

    return( allow_content_download );
}

//////////////////////////////////////////////////////////////////////////
//
// Description:
//   Construct the full path to <SAV-Dir>/patch25d.dll.
//
// Returns: (CGOLDSCHMIDT)
//   bool: true if success; false on failure.
//
//////////////////////////////////////////////////////////////////////////
bool CNavLu::GetPatchDllFullPath (TCHAR *szPatchDll, size_t nNumPatchDllBytes)
{
    HKEY   hKey    = NULL;
    long   lResult = 0;

    if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, szReg_Key_Main, 0, KEY_QUERY_VALUE, &hKey))
        return false;

    DWORD dwType     = 0;
    DWORD dwByteSize = (DWORD)nNumPatchDllBytes;

    // Get the name of the SAV home directory
    lResult = SymSaferRegQueryValueEx (hKey, szReg_Val_HomeDir, NULL, &dwType, 
                               reinterpret_cast<unsigned char*>(szPatchDll), &dwByteSize);

    RegCloseKey(hKey);

    if (ERROR_SUCCESS != lResult)
    {
        szPatchDll[0] = '\0';
        return false;
    }

    vpstrnappendfile (szPatchDll, _T("patch25d.dll"), nNumPatchDllBytes);

    return true;
}
