////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StartNAVcc.h"
#include "OSInfo.h"
#include "ccInstLoader.h"
#include "ccSymCommonClientInfo.h"
#include "globalmutex.h"

using namespace NAVToolbox;

LPCTSTR g_pcszMutexList[] = { _T("SYM_DEF_ALERT_MUTEX"), NULL };

bool CStartNAVcc::StartCC(bool bSetRunKey /*= false*/)
{
	// First check if NAV is already running by looking for
	// our single instance mutexes
	ccLib::CMutex cTheMutex;
  
    // Now walk the list of mutexes we want to check to ensure that ccApp is running
    // with the NAV plugins
	for( int i=0; g_pcszMutexList[i]; i++ )
	{
		// This is intentionally not global since we run ccApp per
		// user instance
		if( !cTheMutex.Open(SYNCHRONIZE, FALSE, g_pcszMutexList[i], FALSE) )
		{
			// Need to starup CC
            return doCCStart(bSetRunKey);
		}
		else
			cTheMutex.Destroy();
	}

    // We found all of our happy little mutexes so let homie know
    // that all is good
    CCTRACEI(_T("CStartNAVcc::StartCC() - NAV is already running in ccApp, returning true"));
	return true;
}

bool CStartNAVcc::doCCStart(bool bSetRunKey)
{
    CCTRACEI(_T("CStartNAVcc::doCCStart() - We did not find the required mutex(es) running, we will attempt to start and restore ccApp"));
    bool bRet = false;

    // Validate that the ccApp run key exists and is valid since the SvcCfg object requires this
    // key to start up CCApp
    ValidateRunKey();

    // Start CommonClient and configure it to auto:
	// scope this loader so it go out of scope last
	cc::ccInst_IServicesConfig SvcCfgLdr;

	cc::IServicesConfigPtr spSvcCfg;

	if(SYM_SUCCEEDED(SvcCfgLdr.CreateObject(spSvcCfg.m_p)) && spSvcCfg != NULL)
	{
		if(spSvcCfg->Create())
		{
            if( bSetRunKey )
            {
                // Call the ccConfig's class to set all CC components to Auto starting
                if(!spSvcCfg->SetServicesStartup(cc::IServicesConfig::SC_AUTO))
				{
					CCTRACEE(_T("spSvcCfg->SetServicesStartup Failed."));
                }
            }

			bRet = spSvcCfg->Start();
		}
		else
		{
			CCTRACEE(_T("CStartNAVcc::doCCStart() - spSvcCfg->Create Failed."));
		}
	}

    return bRet;
}

bool CStartNAVcc::ValidateRunKey()
{
    CRegKey regKey;
    ccLib::CString strCCAppKeyValue;
    if( ERROR_SUCCESS == regKey.Open(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run")) )
    {
        ULONG ulBufSize = MAX_PATH;
        if( ERROR_SUCCESS == regKey.QueryStringValue(_T("ccApp"), strCCAppKeyValue.GetBuffer(MAX_PATH), &ulBufSize) )
        {
            strCCAppKeyValue.ReleaseBuffer();
            CCTRACEI(_T("CStartNAVcc::ValidateRunKey() - Retrieved ccApp run key value: %s"), (LPCTSTR)strCCAppKeyValue);
        }
        else
        {
            strCCAppKeyValue.ReleaseBuffer();
            CCTRACEE(_T("CStartNAVcc::ValidateRunKey() - Failed to get ccApp run key value"));
        }
    }
    else
    {
        CCTRACEE(_T("CStartNAVcc::ValidateRunKey() - Failed to open run key"));
        return false;
    }

    // Now compare the real path to ccApp with the run key value and update it if necessary
    CString strRealCCAppValue;
    DWORD dwSize = MAX_PATH;

    // Fetch path to cc.
    if( ccSym::CInstalledApps::GetCCDirectory( strRealCCAppValue ) )
	{
        strRealCCAppValue += _T("\\ccApp.exe");

        //
        // Make sure they are the same
        //

        // Compare with the raw path
        if( 0 != strRealCCAppValue.CompareNoCase(strCCAppKeyValue) )
        {
            // Add quotes to the real path and compare again
            strRealCCAppValue = _T("\"") + strRealCCAppValue;
            strRealCCAppValue += _T("\"");

            if( 0 != strRealCCAppValue.CompareNoCase(strCCAppKeyValue) )
            {
                CCTRACEW(_T("CStartNAVcc::ValidateRunKey() - The real ccApp path does not match that which be-eth in the registry-eth. Setting it to the real value. Real = %s, Registry = %s"), (LPCTSTR)strRealCCAppValue, (LPCTSTR)strCCAppKeyValue);
                
                if( ERROR_SUCCESS != regKey.SetStringValue(_T("ccApp"), strRealCCAppValue) )
                {
                    CCTRACEE(_T("CStartNAVcc::ValidateRunKey() - Failed to set the ccApp run key to %s"), (LPCTSTR)strRealCCAppValue);
                    return false;
                }
            }
            else
                CCTRACEI(_T("CStartNAVcc::ValidateRunKey() - The registry already has the correct ccApp registry key"));
        }
        else
            CCTRACEI(_T("CStartNAVcc::ValidateRunKey() - The registry already has the correct ccApp registry key"));
	}
    else
    {
        CCTRACEE(_T("CStartNAVcc::ValidateRunKey() - ccInfo.GetCCDir() == false\n"));
        return false;
    }

    return true;
}