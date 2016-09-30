// NAVDefinitionsAlert.cpp

#include "stdafx.h"
#include "NAVDefinitionsAlert.h"
#include "NavSettingsHelperEx.h"
#include "defutils.h"
#include "ccScanInterface.h"
#include "navtrust.h"
#include "ccSymCommonClientInfo.h"
#include "atltime.h"
#include "NAVDefutilsLoader.h"
#include "ccScanLoader.h"
#include "ISymMceCmdLoader.h"	// detect optional Media center components

SIMON_STDMETHODIMP CDefinitionsAlert::Init()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		m_dwInstallTime = _GetAlertDword(ALERT_InstallTime, 0, FALSE);

		DWORD dwEnabled = 0;
		READ_SETTING_START()
			READ_SETTING_DWORD(DEFALERT_VirusDefsDelay, m_dwFirstAlertDelay, 0)
			READ_SETTING_DWORD(DEFALERT_MaxDefsAge, m_dwMaxDefsAge, MAX_DEFS_AGE)
			READ_SETTING_DWORD(DEFALERT_EnableOldDefs, dwEnabled, 1)
		READ_SETTING_END

		m_bAlertEnabled = (dwEnabled == 0) ? false : true;

		// Check QA flag to see if we needs to disable licensing alerts.
		CRegKey key;
		if(ERROR_SUCCESS == key.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec"), KEY_READ))
		{
			DWORD dwVal = 1;
			if(ERROR_SUCCESS == key.QueryDWORDValue(_T("DADEnabled"), dwVal))
			{
				if(dwVal == 0)
				{
					m_bAlertEnabled = false;
					CCTRACEI(_T("CDefinitionsAlert::Init - Virus Defs alert disabled for testing purpose."));
				}
			}
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CDefinitionsAlert::Refresh(BOOL bRefreshDefsCount)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if (bRefreshDefsCount)
		{
			long nDefsAge = 0;
			GetDefsAge(nDefsAge);	// Refresh Virus defs count.
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CDefinitionsAlert::ShouldShowAlert(BOOL* pbShow, SYMALERT_TYPE* pType, BOOL bCheckTimer, DWORD dwFlag)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << (pbShow ? S_OK : E_POINTER);
		hrx << (pType ? S_OK : E_POINTER);

		*pbShow = FALSE;
		*pType = SYMALERT_TYPE_INVALID;

		if(m_bAlertEnabled == false)
			return S_OK;

		long nDefsAge = 0;
		hrx << GetDefsAge(nDefsAge);

		CCTRACEI(_T("CDefinitionsAlert::ShouldShowAlert - DefsAge=%d"), nDefsAge);

		// Are the defs more than 2 weeks old?
		if(nDefsAge > m_dwMaxDefsAge) 
		{
			BOOL bOnScan = ((dwFlag & SYMALERT_TYPE_OldDefsScan) == SYMALERT_TYPE_OldDefsScan);
			if(bOnScan)
			{
				*pType = SYMALERT_TYPE_OldDefsScan;
			}
			else
			{
				// Check to see if an alert on scan is currently active.
				StahlSoft::CSmartHandle smAlertMutex;
				smAlertMutex = OpenMutex(SYNCHRONIZE, FALSE, SZ_NAV_OLDDEFS_ONSCAN_ALERT_MUTEX);
				if(smAlertMutex != NULL)
				{
					CCTRACEI(_T("CDefinitionsAlert::ShouldShowAlert - Suppress alert on timer because an alert on scan is currently active"));
					return S_OK;
				}

				// Check to see if ALU is running.
				StahlSoft::CSmartHandle shALUMutex;
				shALUMutex = ::OpenMutex(SYNCHRONIZE, FALSE, _T("Symantec.LuComServer.Running"));				
				if(shALUMutex)
				{
					CCTRACEI(_T("CDefinitionsAlert::ShouldShowAlert - Suppress on timer alert because ALU is currently running."));
					return FALSE;
				}

				// Check to see if actwiz or cfgwiz is currently running.
				if(_IsCfgWizRunning() || _IsActWizRunning())
				{
					CCTRACEI(_T("CDefinitionsAlert::ShouldShowAlert - Suppress alert because CfgWiz or ActWiz is currently active"));
					return S_OK;
				}

				// Under Windows MCE surpress alert if user is watching video.
				SymMCE::ISymMceCmdLoader mce;
				if (mce.IsMceVideo())
				{
					CCTRACEI(_T("CDefinitionsAlert::ShouldShowAlert - Suppress alert because User is currently watching video on MCE"));
					return S_OK;
				}

				*pType = SYMALERT_TYPE_OldDefs;
			}

			if(*pType != SYMALERT_TYPE_INVALID)
			{
				if(bCheckTimer)
					*pbShow = IsTimeForAlert(*pType);
				else
					*pbShow = TRUE;
			}
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

BOOL CDefinitionsAlert::IsTimeForAlert(SYMALERT_TYPE Type)
{
	BOOL bShow = FALSE;

	// Check to see if we need to delay this alert.
	time_t curtime = 0;	
	if(Type == SYMALERT_TYPE_OldDefs && m_dwFirstAlertDelay)
	{
		time(&curtime);
		if((abs(curtime - m_dwInstallTime)) < m_dwFirstAlertDelay)
		{
			CCTRACEI(_T("CDefinitionsAlert::IsTimeForAlert - Definitions alert on-timer is delayed for %ds."), m_dwFirstAlertDelay);
			return bShow;
		}
	}
	// Check if the cycle says to show for this alert.	
	ALERT_MAP item;
	_LookupAlertMap(Type, item);

	time(&curtime);
	long nLastDisplay = 0;
	long nCycle = 0;

	nLastDisplay = _GetAlertDword(item.lpszLastDisplay, 0);  
	nCycle = _GetAlertDword(item.lpszCycle, (Type == SYMALERT_TYPE_OldDefs)? _1_DAY : 0); 
	bShow = (abs(curtime - nLastDisplay) >= nCycle);

	return bShow;
}

HRESULT CDefinitionsAlert::GetDefsAge(long& nDefsAge)
{
	DefUtilsLoader DefUtilsLoader;
    cc::ccScan_IScanner ccScanLoader;
	TCHAR szDefAlertDefUtilAppID[] = _T("DefAlert");		// Use our own ID

	STAHLSOFT_HRX_TRY(hr)   
	{
		nDefsAge = 0;

		// Check for the lulock.dat file in the shared defs dir.  If it is present,
		//  the defs are in the process of being updated.  In this  case, we'll assume
		//  defs are up-to-date. We'll check its status again in the next 6 hour.

		// Read in the Defs directory.
		TCHAR szLULock[MAX_PATH] = {0};
		TCHAR szDefsDir[MAX_PATH] = {0};
		DWORD dwBuffLength = sizeof(szDefsDir)/sizeof(TCHAR);
		CRegKey key;	
		hrx << HRESULT_FROM_WIN32(key.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Symantec\\InstalledApps"), KEY_READ));
		hrx << HRESULT_FROM_WIN32(key.QueryStringValue(_T("AVENGEDEFS"), szDefsDir, &dwBuffLength));
		::wsprintf(szLULock, _T("%s\\lulock.dat"), szDefsDir);

		FILE* pFile;
		pFile = _tfopen(szLULock, "r");
		if(pFile)
		{
			// The file exists, meaning that defs are temp locked out
			fclose(pFile);
			CCTRACEI(_T("CDefinitionsAlert::GetDefsAge - Found lulock.dat in the defs directory. Assuming defs are currently updated."));
			return S_OK;
		}

        // If symsetup is running don't do the check because our uninstall might have removed the
        // defs and we don't want to try to use them at that point
        ccLib::CMutex symSetupMutex;

        if( symSetupMutex.Open(SYNCHRONIZE, FALSE, _T("SYMSETUP_MUTEX"), TRUE) )
        {
            CCTRACEI(_T("CDefinitionsAlert::GetDefsAge - Found symsetup mutex, not checking for updated defs."));
            return S_OK;
        }

		// Load the Scanner
		// Only one scanner object is allowed per process!
		IDefUtilsPtr pIDefUtils;
        hrx << (SYM_FAILED(DefUtilsLoader.CreateObject(pIDefUtils.m_p)) ? E_FAIL : S_OK);
		
        hrx << ((false == pIDefUtils->InitWindowsApp(szDefAlertDefUtilAppID)) ? E_FAIL : S_OK);
										
        // Try to use the newest defs.  If this fails, it's probably because
	    // there is a new set of defs on the machine that has not been integrated
	    // yet, but the current process doesn't have rights to the hawkings tree.
	    // In this case, just use the current defs.
        bool bNewer = true;
        WORD wYear = 0, wMonth = 0, wDay = 0;
        DWORD dwRev = 0;
        
	    if( pIDefUtils->UseNewestDefs( &bNewer ) == FALSE )
	    {
		    pIDefUtils->GetCurrentDefs( szDefsDir, MAX_PATH );
        }

        // get the definitions date
        if( !pIDefUtils->GetCurrentDefsDate( &wYear, &wMonth, &wDay, &dwRev ) )
        {
            CCTRACEE(_T("CDefinitionsAlert::GetDefsAge() - IDefUtils::GetCurrentDefsDate() failed."));
            return S_FALSE;
        }

        ATL::CTime timeDefTime( wYear, wMonth, wDay, 0, 0, 0 );
        
        // get the current time
        ATL::CTime timeCurrentTime = CTime::GetCurrentTime();

        // Compute age of defs
		//  Note: We experienced that difftime return the difference of
		//   two timers, not neccessary (timer1-timer2).
		//   Therefore, the check for (timeCurrentTime > timeDefTime)
		//   is neccessary.
        if(timeCurrentTime > timeDefTime)
        {
            // get the difference in time
            ATL::CTimeSpan span = timeCurrentTime - timeDefTime;
        
            // how many days old are the defs?
            nDefsAge = span.GetDays();   
        }


        // We still to Load the Scanner to get detected virus counts
		// Only one scanner object is allowed per process!
    	TCHAR szTempDir[MAX_PATH] = {0};
		GetTempPath(MAX_PATH, szTempDir);
        
        CScanPtr <IScanner> spTheScanner;
		CScanPtr <IScanDefinitionsInfo> spDefInfo;
		
        hrx << (SYM_FAILED(ccScanLoader.CreateObject(&spTheScanner)) ? E_FAIL : S_OK);
        
        // Initialize the scanning engine.
		hrx << ((spTheScanner->Initialize(szDefAlertDefUtilAppID, "", szTempDir, 0, 0) != SCAN_OK)? E_FAIL : S_OK);
        
        // get the definfo object
        spTheScanner->GetDefinitionsInfo(&spDefInfo);
        hrx << ((spDefInfo == NULL) ? E_POINTER : S_OK);

		// Store the current count of detected virus threats
		long count = spDefInfo->GetCountWithAliases();

		CNAVOptSettingsEx NavOpts;
		if (NavOpts.Init())
		{
			hrx << NavOpts.SetValue(VIRUSDEFS_ThreatCount, (DWORD)count);
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("CDefinitionsAlert::GetDefsAgeFail - Exception while accessing scanner object %08X"), hr);
	}

	return hr;
}