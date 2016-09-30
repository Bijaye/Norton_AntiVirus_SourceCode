////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// SuiteOwnerHelper.h

// Must include PEP headers

namespace SuiteOwnerHelper
{

inline bool GetOwnerCfgWizDat(CString& csCfgwizDat)
{
	bool bRet = false;
    CNAVInfo navInfo;
    LPCTSTR szNavDir = navInfo.GetNAVDir();

	if(szNavDir != NULL && _tcslen(szNavDir) > 0)
	{
		CRegKey key;
		CString csKey;

		csKey.Format(_T("Software\\Symantec\\CCPD\\BTEndPoints\\%s"), SZ_IS_BUSINESS_TIER_ENDPOINT);

		if(ERROR_SUCCESS == key.Open(HKEY_LOCAL_MACHINE, csKey, KEY_READ))
		{
			CString csFileName;
			DWORD dwFileSize = MAX_PATH;

			if(ERROR_SUCCESS == key.QueryStringValue(_T("CfgwizDat"), csFileName.GetBufferSetLength(dwFileSize), &dwFileSize))
			{
				csCfgwizDat.Format(_T("%s\\%s"), szNavDir, csFileName);

				CCTRACEI(_T("SuiteOwnerHelper - CfgwizDat: %s"), csCfgwizDat);

				bRet = true;
			}
		}
	}

	return bRet;
}

inline bool GetOwnerCfgWizCmdLine(CString& csApp, CString& csParam)
{	
	bool bRet = false;
    CNAVInfo navInfo;
    LPCTSTR szNavDir = navInfo.GetNAVDir();

    if(szNavDir != NULL && _tcslen(szNavDir) > 0)
	{		
		csApp.Format(_T("%s\\cfgwiz.exe"), navInfo.GetOnlinePlatformClientDir());
		csParam.Format(_T("/GUID %s /MODE CfgWiz /CMDLINE \"NO REBOOT\""), SZ_IS_BUSINESS_TIER_ENDPOINT); 

		CCTRACEI(_T("SuiteOwnerHelper - CfgWiz CmdLine: %s %s"), csApp, csParam);

		bRet = true;
	}

	return bRet;
}

}  // END namespace SuiteOwnerHelper
