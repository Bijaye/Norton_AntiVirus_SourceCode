// SuiteOwnerHelper.h

// Must include PEP headers

namespace SuiteOwnerHelper
{
inline bool GetOwnerPath(CString& csPath)
{
	bool bRet = false;

    CRegKey key;
	CString csOwnerSeed;
    CString csKey( DRMREG::SZ_SUITE_OWNER_KEY );
    csKey = csKey  + "\\" + V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT;

	// Find owner GUID
    BasicDRMMiddleMan::CPreferedOwnerEnum<NULL, NULL> poe(csKey, V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
    poe.GetOwnerGUID(0, csOwnerSeed);

	// Find path to owner directory
    CString sKey;
    CRegKey rk;
    sKey.Format(_T("SOFTWARE\\Symantec\\CCPD\\SuiteOwners\\%s"), csOwnerSeed);

    if(ERROR_SUCCESS == rk.Open(HKEY_LOCAL_MACHINE, sKey, KEY_READ))
    {
        DWORD dwSize = MAX_PATH * 2;
        rk.QueryStringValue(_T("ActivationDirectory"), csPath.GetBuffer(dwSize), &dwSize);
        csPath.ReleaseBuffer();

		// Remove "\\" at end of path
		PathRemoveBackslash(csPath.GetBuffer());
		csPath.ReleaseBuffer();

		CCTRACEI(_T("SuiteOwnerHelper - OwnerPath: %s"), csPath);

		bRet = true;
    }

	return bRet;
}

inline bool GetOwnerCfgWizDat(CString& csCfgwizDat)
{
	bool bRet = false;
	CString csOwnerPath;

	if(GetOwnerPath(csOwnerPath))
	{
		CRegKey key;
		CString csKey;

		csKey.Format(_T("Software\\Symantec\\CCPD\\SuiteOwners\\%s"), V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);

		if(ERROR_SUCCESS == key.Open(HKEY_LOCAL_MACHINE, csKey, KEY_READ))
		{
			CString csFileName;
			DWORD dwFileSize = MAX_PATH;

			if(ERROR_SUCCESS == key.QueryStringValue(_T("CfgwizDat"), csFileName.GetBufferSetLength(dwFileSize), &dwFileSize))
			{
				csCfgwizDat.Format(_T("%s\\%s"), csOwnerPath, csFileName);

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

	CString csOwnerPath;

	if(GetOwnerPath(csOwnerPath))
	{		
		csApp.Format(_T("%s\\cfgwiz.exe"), csOwnerPath);

		csParam.Format(_T("/GUID %s /MODE CfgWiz /CMDLINE \"NO REBOOT\""), V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT); 

		CCTRACEI(_T("SuiteOwnerHelper - CfgWiz CmdLine: %s %s"), csApp, csParam);

		bRet = true;
	}

	return bRet;
}
}
