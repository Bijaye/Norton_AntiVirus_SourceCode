#include "StdAfx.h"
#include "installwatcher.h"
#include "NPFMonitorApp.h"
#include "shlobj.h"
#include "OSInfo.h"

#include "SymNetDriverAPI.H"
#include "SymNetAPIVer.h"

#define INITIIDS
#include "IWPPrivateSettingsInterface.h"
#include "IWPSettingsInterface.h"
#include "IWPPrivateSettingsLoader.h"
#include "IWPSettingsLoader.h"
#include "SymIDSI.h"
#include "IDSLoader.h"
#include "IDSLoaderLink.h"
#include "ccSymModuleLifetimeMgrHelper.h"

#include "NAVInfo.h"

const TCHAR* _REG_SYMC_INSTALLEDAPPS_KEY			= _T("Software\\Symantec\\InstalledApps");
const TCHAR* _REG_LEGACY_INSTALLEDAPPS_VALUES[]		= {_T("Norton Internet Security"), _T("Internet Security"), NULL};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor, and Inititialization 
//
CInstallWatcher::CInstallWatcher() :
	m_bLegacyFWInstalled(false)
{
	CCTRACEI(_T("CInstallWatcher::CInstallWatcher()\n"));

	return;
}

CInstallWatcher::~CInstallWatcher()
{
	CCTRACEI(_T("CInstallWatcher::~CInstallWatcher()\n"));
}

void CInstallWatcher::PostTerminate()
{
	CCTRACEI(_T("CInstallWatcher::PostTerminate()\n"));
	
	if(m_StopEvent.m_h)
		m_StopEvent.Set();
}

BOOL CInstallWatcher::InitRun()
{
	// update our state
	CheckRegistry();

	
	// initialize COM on this thread because CC Settings needs it and IWP uses CC Settings
	HRESULT hrCoInit = CoInitialize(NULL);
	if(FAILED(hrCoInit))
	{
		return FALSE;
	}

	try
	{
        IWP::IIWPPrivateSettingsPtr spIWPPrivateSettings;
		
		// load IWPPrivateSettings
        if(SYM_SUCCEEDED(IWP_IWPPrivateSettings::CreateObject(GETMODULEMGR(), &spIWPPrivateSettings)) && spIWPPrivateSettings != NULL)
		{
			CCTRACEI(_T("CInstallWatcher::InitRun() - Got spIWPPrivateSettings Object."));
			
			// check if the settings show that a legacy product used to be installed.
			DWORD dwWasInstalled = 0;
			if(SYM_FAILED(spIWPPrivateSettings->GetValue(spIWPPrivateSettings->IWPLegacyFWInstalled, dwWasInstalled)))
			{
				// if we can't determine whether a legacy client was installed or not, we assume is was not for safety.
				CCTRACEE(_T("CInstallWatcher::InitRun() - Could not determine if there was FW installed. Assuming it is not.\n"));
				dwWasInstalled = 0;
			}				
			
			bool bWasInstalled = (dwWasInstalled != 0);
			
			IWP::IIWPSettingsQIPtr spIWPSettings(spIWPPrivateSettings);

			// If the current machine state is out of sync with what our settings tell us then we need to refresh
			// IMPORTANT NOTE: this should only be the case in two scenarios
			//	 1) when a LegacyFW was just uninstalled in the last session and we are starting up after reboot
			//	 2) we just installed and the LegacyFW was already present (in this case we were never active)
			// If this code gets hit in any other case then something went wrong and we need to try our best
			// to recover. (We do NOT want reset the driver states in these exception cases because that could
			// be more harmful to the other client than leaving it alone).
			if(bWasInstalled != m_bLegacyFWInstalled)
			{
				CCTRACEI(_T("CInstallWatcher::InitRun() - A Legacy Firewall state change was detected.\n"));
			    
                // if a legacy product has just been uninstalled we need to unsubscribe from IDS for them
				if(!m_bLegacyFWInstalled)
				{
					IDSUnsubscribe();
				}
				
                // load IWPSettings
				if(spIWPSettings != NULL)
				{
					// update the yielding state
					if(SYM_FAILED(spIWPSettings->SetYield(m_bLegacyFWInstalled ? 1 : 0, "Legacy")))
						CCTRACEW(_T("CInstallWatcher::InitRun() - Failed updated yielding state.\n"));
				}

				// update legacy product flag
				if(SYM_FAILED(spIWPPrivateSettings->SetValue(IWP::IIWPPrivateSettings::IWPLegacyFWInstalled, m_bLegacyFWInstalled ? 1 : 0)))
					CCTRACEW(_T("CInstallWatcher::InitRun() - Failed to Set IWPLegacyFWInstalled."));

				// save the changes
				if(SYM_FAILED(spIWPPrivateSettings->Save()))
					CCTRACEW(_T("CInstallWatcher::InitRun() - Failed to save settings changes."));
			}
			
			// if we're yielding then we need to turn off the NAV FW (we do this everytime we boot up)
			DWORD dwState = 0;
			if(spIWPSettings != NULL && SYM_SUCCEEDED(spIWPSettings->GetValue(IWP::IIWPSettings::IWPState, dwState)))
			{
				CCTRACEI(_T("CInstallWatcher::InitRun() - got IWPState = %d"), dwState);
				if(IWP::IIWPSettings::IWPStateYielding == dwState)
				{
					ShutOffNAVFW();					
				}
			}
		}
	}
	catch(...)
	{
		CCTRACEE(_T("CInstallWatcher::InitRun() - Unknown Exception occurred."));
	}

	if(SUCCEEDED(hrCoInit))
		CoUninitialize();

	return TRUE;
}

int CInstallWatcher::Run()
{
	CCTRACEI(_T("CInstallWatcher::Run()\n"));

	// initialize current state.
	InitRun();

	// we don't need to run the thread if a legacy product is installed already.
	if(m_bLegacyFWInstalled)
	{
		CCTRACEI(_T("CInstallWatcher::Run() -- Legacy Firewall already installed, not listening for changes.\n"));
		return -1;
	}
	
	// create the event used to signal registry changes
	if(m_RegListenEvent.Create(NULL, TRUE, FALSE, NULL) == FALSE)
	{
		CCTRACEE(_T("CInstallWatcher::Run() : m_RegListenEvent.Create() == FALSE"));
		return -1;
	}

	// create the event used to signal us to stop listening
	if(m_StopEvent.Create(NULL, TRUE, FALSE, NULL) == FALSE)
	{
		CCTRACEE(_T("CInstallWatcher::Run() : m_StopEvent.Create() == FALSE"));
		return -1;
	}
	
	HANDLE pEventHandles[2] = {m_RegListenEvent.m_h, m_StopEvent.m_h};
	
	// open the installed apps key for watching
	ATL::CRegKey rkInstalledApps;
	if(ERROR_SUCCESS != rkInstalledApps.Open(HKEY_LOCAL_MACHINE, _REG_SYMC_INSTALLEDAPPS_KEY))
	{
		CCTRACEE(_T("CInstallWatcher::Run() -- Error opening InstalledApps key.  Can't listen for changes.\n"));
		return -1;
	}

	// sign up for events on this key
	if(ERROR_SUCCESS != RegNotifyChangeKeyValue(rkInstalledApps, FALSE, REG_NOTIFY_CHANGE_LAST_SET, m_RegListenEvent.m_h, TRUE))
	{
		CCTRACEE(_T("CInstallWatcher::Run() -- Failed to subscript to registry change notifications.\n"));
		return -1;
	}

    ccLib::CMessageLock msgLock ( TRUE, TRUE );

	for(;;)
	{
		CCTRACEI(_T("CInstallWatcher::Run() - Listening for changes to InstalledApps key. \n"));

        // Force swap of all non-locked memory pages.  
    	SetProcessWorkingSetSize( GetCurrentProcess(), -1, -1 );

		// wait for something to do
        DWORD dwEventIndex = msgLock.Lock ( 2, pEventHandles, FALSE, INFINITE, FALSE );
		
		if(m_RegListenEvent.m_h == pEventHandles[dwEventIndex])
		{
			CCTRACEI(_T("CInstallWatcher::Run() - Change detected in InstalledApps key.  Checking for Legacy Firewalls.\n"));

			// the installed apps key was changed, check for a legacy product's entry
			if(CheckRegistry())
			{
				CCTRACEI(_T("CInstallWatcher::Run() - Legacy Firewall found.  Starting yielding process.\n"));
				StartYieldOnKeyDetected();
				break;
			}

			m_RegListenEvent.Reset();
			// sign up for events on this key
			if(ERROR_SUCCESS != RegNotifyChangeKeyValue(rkInstalledApps, FALSE, REG_NOTIFY_CHANGE_NAME, m_RegListenEvent.m_h, TRUE))
			{
				CCTRACEE(_T("CInstallWatcher::InitInstance() -- Failed to subscript to registry change notifications.\n"));
				break;
			}
		}

		if(m_StopEvent.m_h == pEventHandles[dwEventIndex])
		{
			CCTRACEI(_T("CInstallWatcher::Run() - Stop event detected.  Thread exiting.\n"));
			break;
		}
	}

	return NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Internal Functions
//
BOOL CInstallWatcher::CheckRegistry(LPTSTR lpszInstallPath, DWORD dwSize)
{
	CCTRACEI(_T("CInstallWatcher::CheckRegistry()\n"));

	ATL::CRegKey rkInstalledApps;
	rkInstalledApps.Open(HKEY_LOCAL_MACHINE, _REG_SYMC_INSTALLEDAPPS_KEY);
	for(int i = 0; _REG_LEGACY_INSTALLEDAPPS_VALUES[i] != NULL; i++)
	{
		// if the caller cares where its installed to then use the
		// lpszInstallPath and size, otherwise use our own local buffer
		TCHAR szLocalPath[MAX_PATH] = {0};
		DWORD dwPathSize = lpszInstallPath ? dwSize : MAX_PATH;
		if(rkInstalledApps.QueryStringValue(_REG_LEGACY_INSTALLEDAPPS_VALUES[i], 
											lpszInstallPath ? lpszInstallPath : szLocalPath,
											&dwPathSize) == ERROR_SUCCESS)
		{
			// check the version of NIS thats installed, if its before 8.0 then don't do anything
			TCHAR szNISVER[MAX_PATH] = {0};
			TCHAR szIAMCPL[MAX_PATH] = {0};
			_tcsncpy(szNISVER, szLocalPath, MAX_PATH);
			_tcsncpy(szIAMCPL, szLocalPath, MAX_PATH);
			PathAppend(szNISVER, _T("NISVER.DAT"));
			PathAppend(szIAMCPL, _T("IAMCPL.CPL"));

			TCHAR szNisVersion[MAX_PATH] = {0};
			if(GetFileAttributes(szNISVER) != INVALID_FILE_ATTRIBUTES)			
			{
				GetPrivateProfileString(_T("Versions"), _T("PublicVersion"), _T("0.0.0.0"),
										szNisVersion, MAX_PATH,	szNISVER);
				if(GetVersionFromString(szNisVersion, m_NisVer) && m_NisVer.nMajorHi < 8)
				{
					CCTRACEI(_T("CInstallWatcher::CheckRegistry() - Found Legacy Firewall.\n"));
			
					m_bLegacyFWInstalled = true;

					return TRUE;
				}
			} 
			else if(GetFileAttributes(szIAMCPL) != INVALID_FILE_ATTRIBUTES)	
			{
				// NIS 4.5 and prior did not have NISVER.dat, so look for IAMCPL.CPL
				CCTRACEI(_T("CInstallWatcher::CheckRegistry() - Found Legacy Firewall.\n"));
			
				m_bLegacyFWInstalled = true;

				return TRUE;
			}
		}
	}
	
	CCTRACEI(_T("CInstallWatcher::CheckRegistry() - No Legacy Firewall found.\n"));

	// NOTE: do not set the IWP::IWPLegacyFWInstalled flag to 0 zero here.  The constructor will
	// will do that the reboot after it tells IWP to unyield

	return FALSE;
}

BOOL CInstallWatcher::StartYieldOnKeyDetected()
{
	CCTRACEI(_T("CInstallWatcher::StartYieldOnKeyDetected()\n"));
	
	// initialize COM on this thread because CC Settings needs it and IWP uses CC Settings
	HRESULT hrCoInit = CoInitialize(NULL);
	if(FAILED(hrCoInit))
	{
		return FALSE;
	}

	BOOL bRet = TRUE;

	try
	{
		IWP::IIWPPrivateSettingsPtr spIWPPrivateSettings;

        if(SYM_SUCCEEDED(IWP_IWPPrivateSettings::CreateObject(GETMODULEMGR(), &spIWPPrivateSettings)) &&
           spIWPPrivateSettings != NULL)
		{
			IWP::IIWPSettingsQIPtr spIWPSettings(spIWPPrivateSettings);

 			if(spIWPSettings != NULL)
			{
				// step 1: yield
				if(SYM_FAILED(spIWPSettings->SetYield(1, "Legacy")))
				{
					CCTRACEE(_T("CInstallWatcher::StartYieldOnKeyDetected() - Unable to Reset IDS settings.\n"));
					bRet = FALSE;
				}
			}
			else
			{
				CCTRACEE(_T("CInstallWatcher::StartYieldOnKeyDetected() - Unable load IWPSettings object.\n"));
				bRet = FALSE;
			}
				
			// step 2: set the flag that a legacy product is installed
			if(SYM_FAILED(spIWPPrivateSettings->SetValue(IWP::IIWPPrivateSettings::IWPLegacyFWInstalled, 1)))
			{
				CCTRACEE(_T("CInstallWatcher::StartYieldOnKeyDetected() - Failed to Set IWPLegacyFWInstalled."));
				bRet = FALSE;
			}

			// step 3: reset the SymNETDrv driver settings	
			FACTORYRESETPARAMS frp;
			ZeroMemory(&frp, sizeof(FACTORYRESETPARAMS));
			frp.structsize = sizeof(FACTORYRESETPARAMS);
			frp.MajorVersion = m_NisVer.nMajorHi;
			frp.MinorVersion = m_NisVer.nMajorLo;
			frp.eProductID = eCLIENTID_Nis;
			if(SYM_FAILED(spIWPPrivateSettings->ResetSymNetDrvSettings(&frp)))
			{
				CCTRACEE(_T("CInstallWatcher::StartYieldOnKeyDetected() - Unable to Reset SymNetDrv Settings.\n"));
				bRet = FALSE;
			}

			// step 3: reset the IDS settings
			UINT64 ui64Version = (m_NisVer.nMajorHi << 48) + 
								(m_NisVer.nMajorLo << 32) + 
								(m_NisVer.nMinorHi << 16) + 
								(m_NisVer.nMinorLo);
			if(SYM_FAILED(spIWPPrivateSettings->ResetIDSSettings("NIS" , ui64Version)))
			{
				CCTRACEE(_T("CInstallWatcher::StartYieldOnKeyDetected() - Unable to Reset IDS settings.\n"));
				bRet = FALSE;
			}

			// step 4: remove the lv plugin
			if(SYM_FAILED(spIWPPrivateSettings->InstallLVPlugin(FALSE)))
			{
				CCTRACEE(_T("CInstallWatcher::StartYieldOnKeyDetected() - InstallLVPlugin failed"));
				bRet = FALSE;
			}
			
			// step 5: save
			if(SYM_FAILED(spIWPPrivateSettings->Save()))
			{
				CCTRACEE(_T("CInstallWatcher::StartYieldOnKeyDetected() - Failed to Save Settings."));
				bRet = FALSE;
			}
		}
		else
		{
			CCTRACEE(_T("CInstallWatcher::StartYieldOnKeyDetected() - Unable load IWPPrivateSettings object.\n"));
			bRet = FALSE;
		}
	}
	catch(...)
	{
		CCTRACEE(_T("CInstallWatcher::StartYieldOnKeyDetected() - Unknown exception caught."));
		bRet = FALSE;
	}

	if(SUCCEEDED(hrCoInit))
		CoUninitialize();

	return bRet;
}

BOOL CInstallWatcher::IDSUnsubscribe()
{
	CCTRACEI(_T("CInstallWatcher::IDSUnsubscribe() - Unsubscribing from IDS for the legacy product."));
	
	CIDSLoader IDSLoader;
	
	try
	{
		CSymPtr<SymIDS::IIDSInstallNotification> pIDSInst;
				
		if(SYM_FAILED(IDSLoader.GetObject ( SymIDS::IID_IDSInstallNotification, (ISymBase**) &pIDSInst))
			|| pIDSInst == NULL)
		{
			CCTRACEW(_T("CInstallWatcher::IDSUnsubscribe() - Unable to load IDS object"));
			return FALSE;
		}
		
		if(SYM_FAILED(pIDSInst->Initialize()))
		{
			CCTRACEW(_T("CInstallWatcher::IDSUnsubscribe() - pIDSInst->Initialize() failed."));
			return FALSE;
		}
			
		if(SYM_FAILED(pIDSInst->OnUnInstalled("Internet Security")))
		{
			CCTRACEW(_T("CInstallWatcher::IDSUnsubscribe() - pIDSInst->OnUnInstalled() failed."));
			return FALSE;
		}
	}
	catch(...)
	{
		CCTRACEE(_T("CInstallWatcher::IDSUnsubscribe() - pIDSInst->Initialize() failed."));
	}
	
	return TRUE;
}

BOOL CInstallWatcher::ShutOffNAVFW()
{
	CCTRACEI(_T("CInstallWatcher::ShutOffNAVFW() - Shutting off NAV FW."));
	
	HMODULE hSymNeti = NULL;
	
	try
	{
		// build path to SymNeti
		TCHAR szSymNetiPath[MAX_PATH+1] = {0};
		GetSystemDirectory(szSymNetiPath, MAX_PATH+1);
		PathAppend(szSymNetiPath, _T("SymNeti.dll"));
		
		// load SymNeti
		hSymNeti = LoadLibraryEx(szSymNetiPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

		if( !hSymNeti )
		{
			CCTRACEE(_T("CInstallWatcher::ShutOffNAVFW() - unable to load SymNeti.dll"));
			return FALSE;
		}
		
		// get the proc address
		SNSETPRODUCTSTATE SNSetProductState = (SNSETPRODUCTSTATE)::GetProcAddress(hSymNeti,  reinterpret_cast<LPCTSTR>(eSNSetProductState));		
		SNINIT SNInit = (SNINIT)::GetProcAddress(hSymNeti,  reinterpret_cast<LPCTSTR>(eSNInit));		

        if( SNInit )
        {
            DWORD dwSuccess = 0;
            HRESULT hr = SNInit(VER_SN_VERSION, &dwSuccess);	
            if ( FAILED (hr))
    	        CCTRACEE("CInstallWatcher::ShutOffNAVFW() - SNInit: dwSuccess: %d, HRESULT: 0x%x", dwSuccess, hr);
        }

		if( SNSetProductState )
		{
			HRESULT hr = SNSetProductState(eCLIENTID_Nav_FW, GS_CURRENT_STATE, 0);
            if ( FAILED (hr))
                CCTRACEE(_T("CInstallWatcher::ShutOffNAVFW() - SNSetProductState Failed, 0x%x"), hr);
		}
	}
	catch(...)
	{
		CCTRACEE(_T("CInstallWatcher::ShutOffNAVFW() - Unknown exception occured."));
	}
	
	try
	{
		if(hSymNeti)
			FreeLibrary(hSymNeti);
	}
	catch(...)
	{
		CCTRACEE(_T("CInstallWatcher::ShutOffNAVFW() - Exception occured while trying to free library."));
	}

	return TRUE;
}

BOOL CInstallWatcher::GetVersionFromString(const LPCTSTR szVersion, NPFVERSION &ver)
{
	LPTSTR pszVersion = (LPTSTR)new TCHAR[_tcslen(szVersion)+1];
	
	if(!pszVersion)
		return FALSE;
	
	_tcscpy(pszVersion, szVersion);
	
	// Parse szVer for what we want...
	LPTSTR pszPtr = pszVersion;
	LPTSTR pszEnd = pszPtr + _tcslen(pszPtr);
	int nVers[4] = {0};
	int nSectionCount = 0;

	while(pszPtr < pszEnd)
	{
		if(nSectionCount > 3)
			break;

		LPTSTR pszDot = _tcschr(pszPtr, _T('.'));
		if(!pszDot)
			break;
		else if(pszDot)
			*pszDot = _T('\0');

		nVers[nSectionCount] = _ttoi(pszPtr);

		// Next section
		pszPtr = pszPtr + _tcslen(pszPtr) + 1;
		++nSectionCount;
	}

	// Put the last section in...
	nVers[nSectionCount] = _ttoi(pszPtr);

	// find out if there is a paritial letter
	TCHAR szTemp[15] = {0};
	_stprintf(szTemp, _T("%d"), nVers[nSectionCount]);

	// store the number of parts used
	ver.nPartsUsed = nSectionCount+1;

	// if these are different a character was removed
	if(_tcscmp(szTemp, pszPtr))
		ver.nPartialBuildLetter = *(pszEnd-1);

	// convert to upper case
	if(_T('a') <= ver.nPartialBuildLetter)
		ver.nPartialBuildLetter -= (_T('a') - _T('A'));

	// make sure partial is a letter
	if((_T('A') > ver.nPartialBuildLetter) || (ver.nPartialBuildLetter > _T('Z')))
		ver.nPartialBuildLetter = 0;

	// Save the version info
	ver.nMajorHi = nVers[0];
	ver.nMajorLo = nVers[1];
	ver.nMinorHi = nVers[2];
	ver.nMinorLo = nVers[3];

	delete [] pszVersion;

	return TRUE;
}