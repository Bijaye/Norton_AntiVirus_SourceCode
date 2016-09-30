
#include "stdafx.h"
#include "CfgWizDat.h"
#include "NAVInfo.h"
#include "InstOptsNames.h"
#include "NavOptHelperEx.h"

using namespace std;

CCfgWizDat::CCfgWizDat(LPSTR lpCmdLine) :
	m_lRebootCount(0),
	m_bInit(FALSE),
	m_bSilent(FALSE),
	m_bRebooted(FALSE),
	m_bNeedReboot(FALSE),
	m_FileState(CFGWIZDAT_NOTHING),
	m_RegKeyState(CFGWIZDAT_NOTHING),
	m_hCfgWizDat(NULL),
	m_lCheckReseal(0),
	m_bStartAP(FALSE),
	m_lAPRebootCount(0),
	m_lType(0),
	m_bDoNotCallForceOverride(TRUE),
	m_bNoDrivers(FALSE),
	m_bShowCfgwizPrompt(FALSE),
	m_bShowWelcomePage(TRUE),
	m_bShowICFCheckBox(TRUE)
{
	// Save the CmdLine

	m_strCmdLine = lpCmdLine;
}

CCfgWizDat::~CCfgWizDat()
{
	if (m_hCfgWizDat != NULL)
		NavOpts32_Free(m_hCfgWizDat);
}

BOOL CCfgWizDat::Load()
{
	try
	{
		if (m_bInit != TRUE)
		{
			// Check if user rebooted the machine, if we can't find "NO REBOOT"
			if (_tcsstr(m_strCmdLine.c_str(), _T("NO REBOOT")) == NULL)
				m_bRebooted = TRUE;

			// Create a handle to CfgWiz.dat
			NAVOPTS32_STATUS Status = NavOpts32_Allocate(&m_hCfgWizDat);
			if (Status != NAVOPTS32_OK)
				throw runtime_error("Unable to initialize options library.");
			
			TCHAR szNAVDir[MAX_PATH] = {0};
			DWORD dwSetting;
			CNAVInfo NAVInfo;
			
			wsprintf(m_szCfgWizDatPath, "%s\\%s", NAVInfo.GetNAVDir(), g_cszCfgWizDat);
			
			// Load CfgWiz.dat
			Status = NavOpts32_Load(m_szCfgWizDatPath, m_hCfgWizDat, FALSE);
			if (Status != NAVOPTS32_OK)
				throw runtime_error("Unable to load CfgWiz.dat.");

			// Read in the reboot count
			Status = NavOpts32_GetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_RebootCount, &m_lRebootCount, 10);
			if (Status != NAVOPTS32_OK)
			{
				m_lRebootCount = 0;
			}

			// Read in the reseal detection setting
			Status = NavOpts32_GetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_Reseal, &m_lCheckReseal, 0);
			//we dont throw because this value does not always exist

			// Read in factory file exists/absent
			NavOpts32_GetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_FactoryFileExists, &dwSetting, -1);
			if (dwSetting == -1)
			{
				m_FileState = CFGWIZDAT_NOTHING;
			}
			else
			{
				m_FileState = (dwSetting == 1 ? CFGWIZDAT_EXISTS : CFGWIZDAT_ABSENT);

				// Read in the factory file location
				Status = NavOpts32_GetStringValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_FactoryFile, m_szFactoryFile, 
					sizeof(m_szFactoryFile), NULL);
				
				// If the factory file location is not specified then we will set the factory file exists to zero.
				if (Status != NAVOPTS32_OK)
				{
					Status = NavOpts32_SetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_FactoryFileExists, 0);
				}
			}

			// Read in factory RegKey exists/absent
			NavOpts32_GetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_FactoryRegKeyExists, &dwSetting, -1);
			if (dwSetting == -1)
			{
				m_RegKeyState = CFGWIZDAT_NOTHING;
			}
			else
			{
				m_RegKeyState = (dwSetting == 1 ? CFGWIZDAT_EXISTS : CFGWIZDAT_ABSENT);

				// Read in the factory RegKey location
				Status = NavOpts32_GetStringValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_FactoryRegKey, m_szFactoryRegKey, 
					sizeof(m_szFactoryRegKey), NULL);
				
				// If the factory regkey does not exist then we will set the factory regkey exists value to zero.
				if (Status != NAVOPTS32_OK)
				{
					Status = NavOpts32_SetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_FactoryRegKeyExists, 0);
				}
			}

			// Read silent mode flag
			Status = NavOpts32_GetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_SilentMode, &dwSetting, 0);
			if (Status == NAVOPTS32_OK)
			{
				m_bSilent = (dwSetting == 1 ? TRUE : FALSE);
			}
			else
			{
				m_bSilent = FALSE;
			}

			// Read the ShowWelcomePage flag
			Status = NavOpts32_GetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_ShowWelcomePage, &dwSetting, 1);
			if (Status == NAVOPTS32_OK)
			{
				m_bShowWelcomePage = (dwSetting == 1 ? TRUE : FALSE);
			}
			else
			{
				m_bShowWelcomePage = TRUE;
			}
			
			// Read NeedReboot flag
			Status = NavOpts32_GetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_RebootWarning, &dwSetting, 0);
			if (Status == NAVOPTS32_OK)
			{
				m_bNeedReboot = (dwSetting == 1 ? TRUE : FALSE);
			}
			else
			{
				m_bNeedReboot = FALSE;
			}

			// Read the APRebootCount setting
			Status =  NavOpts32_GetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_APRebootCount, &m_lAPRebootCount, 0);
			if(Status == NAVOPTS32_OK)
			{
				m_bStartAP = TRUE;
			}
			else
			{
				m_bStartAP = FALSE;
			}

			// Read in the type.
			// O - Retail
			// 1 - OEM
			// 2 - CTO
			Status = NavOpts32_GetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_OEM, &m_lType, 0);
			if (Status != NAVOPTS32_OK)
			{
				m_lType = 0;
			}
				
			// Read the DoNotCallForceOverride setting
			Status =  NavOpts32_GetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_DoNotCallForceOverride, &dwSetting, 0);
			if(Status == NAVOPTS32_OK)
			{
				m_bDoNotCallForceOverride = TRUE;
			}
			else
			{
				m_bDoNotCallForceOverride = FALSE;
			}

			// Read No driver flag
			Status = NavOpts32_GetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_NODRIVERS, &dwSetting, 0);
			if (Status == NAVOPTS32_OK)
			{
				m_bNoDrivers = (dwSetting == 1 ? TRUE : FALSE);
			}
			else
			{
				m_bNoDrivers = FALSE;
			}
	
			// Read ShowCfgwiz prompt if /nodrivers
			if(m_bNoDrivers)
			{
				Status = NavOpts32_GetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_ShowCfgwizPrompt, &dwSetting, 0);
				if (Status == NAVOPTS32_OK)
				{
					m_bShowCfgwizPrompt = (dwSetting == 1 ? TRUE : FALSE);
				}
				else if(Status == NAVOPTS32_VALUE_NOT_FOUND)
				{
					m_bShowCfgwizPrompt = TRUE;
				}
			}
			
			Status = NavOpts32_GetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_ShowICFCheckbox, &dwSetting, 0);
			if (Status == NAVOPTS32_OK)
			{
				m_bShowICFCheckBox = (dwSetting == 1 ? TRUE : FALSE);
			}
			else
			{
				m_bShowICFCheckBox = TRUE;
			}


			CCTRACEI(_T("**** m_bNoDrivers=%d, m_bShowCfgwizPrompt=%d"), m_bNoDrivers, m_bShowCfgwizPrompt);

			// Save CfgWiz.dat
			Status = NavOpts32_Save(m_szCfgWizDatPath, m_hCfgWizDat);
			if (Status != NAVOPTS32_OK)
				throw runtime_error("Unable to save CfgWiz.dat");

			m_bInit = TRUE;
		}
	}
	catch(exception& Ex)
	{
		m_bInit = FALSE;
		
		CCTRACEE(Ex.what());
	}

	// Clean up

	if (m_hCfgWizDat != NULL)
	{
		NavOpts32_Free(m_hCfgWizDat);
		m_hCfgWizDat = NULL;
	}

	return m_bInit;
}

BOOL CCfgWizDat::CheckFactoryFile()
{
	_ASSERT(m_bInit == TRUE);
	
	//we want to allow users to be able to run cfgwiz through desktop no matter what
	if(m_bRebooted == TRUE)
	{
		// Check to see if we meet the factory file condition
		if (m_FileState == CFGWIZDAT_NOTHING)
		{
			return TRUE;
		}
		else if (m_FileState == CFGWIZDAT_EXISTS)
		{
			return (GetFileAttributes(m_szFactoryFile) != DWORD(-1));
		}
		else
		{
			return (GetFileAttributes(m_szFactoryFile) == DWORD(-1));
		}	
	}
	else
	{
		return TRUE;
	}
}

BOOL CCfgWizDat::CheckFactoryRegKey()
{
	_ASSERT(m_bInit == TRUE);

	BOOL bReturn = FALSE;

	const struct tagKEYS
	{
		HKEY  hKey;
		TCHAR szKeyName[8];

	} rKeys[] = { HKEY_CURRENT_USER,   _T("HKCU"),
				  HKEY_CLASSES_ROOT,   _T("HKCR"),
			      HKEY_USERS,		   _T("HKUS"),
				  HKEY_CURRENT_CONFIG, _T("HKCC"),
				  HKEY_LOCAL_MACHINE,  _T("HKLM")}; 
	
	//we want to allow users to launch cfgwiz through the desktop no matter what
	if(m_bRebooted == TRUE)
	{
		// Check to see if we meet the factory regkey condition
		if (m_RegKeyState == CFGWIZDAT_NOTHING)
		{
			bReturn = TRUE;
		}
		else
		{
			string strRegKey = m_szFactoryRegKey;
			int iIndex = -1;

			// Try to find what regkey we need to open

			for (int x = 0; x < sizeof(rKeys)/sizeof(tagKEYS); x++)
			{
				if (_tcsnicmp(strRegKey.c_str(), rKeys[x].szKeyName, 4) == 0)
				{
					iIndex = x;
					break;
				}
			}

			// If there is a match, open the regkey

			if (iIndex != -1)
			{
				// Get the subkey name

				strRegKey = strRegKey.erase(0, 5);

				// Open the regkey

				HKEY hKey;

				LONG lResult = RegOpenKeyEx(rKeys[x].hKey, strRegKey.c_str(), 0, KEY_QUERY_VALUE, &hKey);

				if (lResult == ERROR_SUCCESS)
				{
					if (m_RegKeyState == CFGWIZDAT_EXISTS)
					{
						bReturn = TRUE;
					}

					RegCloseKey(hKey);
				}
				else
				{
					if (m_RegKeyState == CFGWIZDAT_ABSENT)
					{
						bReturn = TRUE;
					}
				}
			}
		}
	}
	else
	{
		bReturn = TRUE;
	}

	return bReturn;
}

BOOL CCfgWizDat::CheckRebootCount()
{
	BOOL bReturn = FALSE;

	// Check to see if we meet the reboot count condition
	try
	{
		if (m_lRebootCount > 0 && m_bRebooted == TRUE)
		{
			// Create a handle to CfgWiz.dat

			NAVOPTS32_STATUS Status = NavOpts32_Allocate(&m_hCfgWizDat);
			if (Status != NAVOPTS32_OK)
				throw runtime_error("Unable to initialize options library.");

			// Load CfgWiz.dat

			TCHAR szNAVDir[MAX_PATH] = {0};
			CNAVInfo NAVInfo;
			
			wsprintf(m_szCfgWizDatPath, "%s\\%s", NAVInfo.GetNAVDir(), g_cszCfgWizDat);
			
			Status = NavOpts32_Load(m_szCfgWizDatPath, m_hCfgWizDat, FALSE);
			if (Status != NAVOPTS32_OK)
				throw runtime_error("Unable to load CfgWiz.dat.");

			// decrement the reboot count
			m_lRebootCount--;

			// Set the new reboot count
			Status = NavOpts32_SetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_RebootCount, m_lRebootCount);
			
			if (Status != NAVOPTS32_OK)
				throw runtime_error("Unable to set reboot count.");

			// Save CfgWiz.dat
			Status = NavOpts32_Save(m_szCfgWizDatPath, m_hCfgWizDat);
			if (Status != NAVOPTS32_OK)
				throw runtime_error("Unable to save CfgWiz.dat");

			bReturn = FALSE;
		}
		else
		{
			bReturn = TRUE;
		}
	}
	catch(exception& Ex)
	{
		bReturn = FALSE;

		CCTRACEE(Ex.what());
	}

	// Clean-up

	if (m_hCfgWizDat != NULL)
	{
		NavOpts32_Free(m_hCfgWizDat);
		m_hCfgWizDat = NULL;
	}

	return bReturn;
}

BOOL CCfgWizDat::IsFinished()
{
	try
	{
		// Create a handle to CfgWiz.dat

		NAVOPTS32_STATUS Status = NavOpts32_Allocate(&m_hCfgWizDat);
		if (Status != NAVOPTS32_OK)
			throw runtime_error("Unable to initialize options library.");
		
		// Load CfgWiz.dat

		TCHAR szNAVDir[MAX_PATH] = {0};
		DWORD dwSetting;
		CNAVInfo NAVInfo;
		
		wsprintf(m_szCfgWizDatPath, "%s\\%s", NAVInfo.GetNAVDir(), g_cszCfgWizDat);
		
		Status = NavOpts32_Load(m_szCfgWizDatPath, m_hCfgWizDat, FALSE);
		if (Status != NAVOPTS32_OK)
			throw runtime_error("Unable to load CfgWiz.dat.");

		// Check if users have gone through CfgWiz's UI successfully

		Status = NavOpts32_GetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_Finished, &dwSetting, 0);
		if (Status == NAVOPTS32_OK)
		{
			m_bFinished = (dwSetting == 1 ? TRUE : FALSE);
		}
		else
		{
			m_bFinished = FALSE;
		}
	}
	catch(exception& Ex)
	{
		m_bFinished = FALSE;
		
		CCTRACEE(Ex.what());
	}

	// Clean-up

	if (m_hCfgWizDat != NULL)
	{
		NavOpts32_Free(m_hCfgWizDat);
		m_hCfgWizDat = NULL;
	}

	return m_bFinished;
}

BOOL CCfgWizDat::IsSilentMode()
{
	_ASSERT(m_bInit == TRUE);
	
	return m_bSilent;
}

BOOL CCfgWizDat::NeedReboot()
{
	_ASSERT(m_bInit == TRUE);
	
	return m_bNeedReboot;
}

BOOL CCfgWizDat::CheckFactoryReseal()
{
	_ASSERT(m_bInit == TRUE);

	//we want to allow the user to launch cfgwiz at anytime by clickon the icon
	if(m_bRebooted == TRUE)
	{
		return (m_lCheckReseal == 1 ? TRUE : FALSE);
	}
	else
	{
		return FALSE;
	}
}

BOOL CCfgWizDat::ShouldStartAP()
{
	BOOL bReturn = FALSE;

	try
	{
		//check to see if we are supposed to start ap
		if(m_bStartAP == TRUE)
		{
			if (m_lAPRebootCount > 0 && m_bRebooted == TRUE)
			{
				// Create a handle to CfgWiz.dat

				NAVOPTS32_STATUS Status = NavOpts32_Allocate(&m_hCfgWizDat);
				if (Status != NAVOPTS32_OK)
					throw runtime_error("Unable to initialize options library.");

				// Load CfgWiz.dat

				TCHAR szNAVDir[MAX_PATH] = {0};
				CNAVInfo NAVInfo;
				
				wsprintf(m_szCfgWizDatPath, "%s\\%s", NAVInfo.GetNAVDir(), g_cszCfgWizDat);
				
				Status = NavOpts32_Load(m_szCfgWizDatPath, m_hCfgWizDat, FALSE);
				if (Status != NAVOPTS32_OK)
					throw runtime_error("Unable to load CfgWiz.dat.");

				// decrement the reboot count

				m_lAPRebootCount--;

				// Set the new reboot count

				Status = NavOpts32_SetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_APRebootCount, m_lAPRebootCount);
				if (Status != NAVOPTS32_OK)
					throw runtime_error("Unable to set reboot count.");

				// Save CfgWiz.dat

				Status = NavOpts32_Save(m_szCfgWizDatPath, m_hCfgWizDat);
				if (Status != NAVOPTS32_OK)
					throw runtime_error("Unable to save CfgWiz.dat");
			}
			else
			{
				//we should start AP silently
				bReturn = TRUE;

				//delete the value so we dont keep starting it on every reboot
				NavOpts32_RemoveValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_APRebootCount);
			}
		}
	}
	catch(exception& Ex)
	{
		CCTRACEE(Ex.what());
		bReturn = FALSE;
	}

	// Clean-up

	if (m_hCfgWizDat != NULL)
	{
		NavOpts32_Free(m_hCfgWizDat);
		m_hCfgWizDat = NULL;
	}

	return bReturn;
}

// Get the Install Type.
// 0 - Retail.
// 1 - OEM.
// 2 - CTO.
DWORD CCfgWizDat::InstallType()
{
	_ASSERT(m_bInit == TRUE);

	return m_lType;
}

// renew initial expired state?
BOOL CCfgWizDat::DoNotCallForceOverride()
{
	_ASSERT(m_bInit == TRUE);
	
	return m_bDoNotCallForceOverride;	
}

// Set ForceOverride property in cfgwiz.
BOOL CCfgWizDat::SetDoNotForceOverride()
{
	try
	{
		// Load CfgWiz.dat
		TCHAR szNAVDir[MAX_PATH] = {0};
		CNAVInfo NAVInfo;
		
		// Create a handle to CfgWiz.dat
		wsprintf(m_szCfgWizDatPath, "%s\\%s", NAVInfo.GetNAVDir(), g_cszCfgWizDat);
		NavOpts32_Allocate(&m_hCfgWizDat);
		NavOpts32_Load(m_szCfgWizDatPath, m_hCfgWizDat, FALSE);
		NavOpts32_SetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_DoNotCallForceOverride, 1);
		NavOpts32_Save(m_szCfgWizDatPath, m_hCfgWizDat);
	}
	catch(...)
	{
		// If anything fails... we'll still continue. Worst case scenario, we'll
        // just call the forceoverride function again.
	}

	return TRUE;
}

BOOL CCfgWizDat::CheckNoDrivers()
{
	_ASSERT(m_bInit == TRUE);

	return m_bNoDrivers;
}

BOOL CCfgWizDat::DisableCfgwizPrompt()
{
	_ASSERT(m_bInit == TRUE);

	BOOL bRetVal = FALSE;
	StahlSoft::HRX hrx;

	try
	{
		CNAVOptFileEx DatFile;
		hrx << (DatFile.Init(m_szCfgWizDatPath, FALSE) ? S_OK : E_FAIL);
		hrx << DatFile.SetValue(InstallToolBox::CFGWIZ_ShowCfgwizPrompt, (DWORD) 0);
		hrx << DatFile.Save();
		bRetVal = TRUE;

		CCTRACEE(_T("CCfgWizDat::DisableCfgwizPrompt - successful"));
	}
	catch(_com_error& e)
	{
		CCTRACEE(_T("CCfgWizDat::DisableCfgwizPrompt - Exception thrown while set CFGWIZ:RemoveNoDriversPrompt=1 0x%08X"), e.Error());
	}

	return bRetVal;
}

BOOL CCfgWizDat::ShouldShowCfgwizPrompt()
{
	_ASSERT(m_bInit == TRUE);

	return m_bShowCfgwizPrompt;
}

BOOL CCfgWizDat::ShouldShowWelcomePage()
{
	_ASSERT(m_bInit == TRUE);

	return m_bShowWelcomePage;
}

BOOL CCfgWizDat::SetCfgwizRunOnce()
{
	_ASSERT(m_bInit == TRUE);

	BOOL bRetVal = FALSE;
	StahlSoft::HRX hrx;

	try
	{
		CNAVOptFileEx DatFile;
		hrx << (DatFile.Init(m_szCfgWizDatPath) ? S_OK : E_FAIL);
		hrx << DatFile.SetValue(InstallToolBox::CFGWIZ_RunOnce, (DWORD) 1);
		hrx << (DatFile.Save() ? S_OK : E_FAIL);
		bRetVal = TRUE;

		CCTRACEI(_T("CCfgWizDat::SetCfgwizRunOnce - successful"));
	}
	catch(_com_error& e)
	{
		CCTRACEE(_T("CCfgWizDat::SetCfgwizRunOnce - Exception thrown while set CFGWIZ:SetCfgwizRunOnce=1 0x%08X"), e.Error());
	}

	return bRetVal;
}

BOOL CCfgWizDat::ShouldShowICFCheckbox()
{
	_ASSERT(m_bInit == TRUE);

	return m_bShowICFCheckBox;
}
