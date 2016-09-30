
#ifndef _CFGWIZDAT_H
#define _CFGWIZDAT_H

#include <string>
#include "NAVOpt32.h"

const TCHAR g_cszCfgWizDat[] = _T("CfgWiz.dat");

//
// The different states of OEM conditions
//
enum SWITCHSTATE
{
	CFGWIZDAT_NOTHING = 0,
	CFGWIZDAT_EXISTS,
	CFGWIZDAT_ABSENT
};

class CCfgWizDat
{
public:
	// Constructor
	CCfgWizDat(LPSTR lpCmdLine);

	// Destructor
	virtual ~CCfgWizDat();

	// Read in the values from CfgWiz.dat
	BOOL Load();

	// Check if we meet the factory file condition
	BOOL CheckFactoryFile();

	// Check if we meet the factory regkey condition
	BOOL CheckFactoryRegKey();

	// Check if we meet the reboot count condition
	BOOL CheckRebootCount();

	// Check if CfgWiz is completed
	BOOL IsFinished();
	
	// Check if CfgWiz is running in silent mode
	BOOL IsSilentMode();

	// Check if reboot is needed before running CfgWiz
	BOOL NeedReboot();

	// Check if we need to detect factory reseal
	BOOL CheckFactoryReseal();

	// Check if we should start AP now without launching the CfgWiz UI
	BOOL ShouldStartAP();
	
	// Read in the install type.
	// O - Retail
	// 1 - OEM
	// 2 - CTO
	DWORD InstallType();

	// Do we need to call licensing to renew initial expired state as in CTO layout?
	BOOL DoNotCallForceOverride();

	// Set the DoNotForceOverride property in cfgwiz.dat.
	BOOL SetDoNotForceOverride();
	
	// Check if the no drivers flag is set.
	BOOL CheckNoDrivers();

	// Disable cfgwiz promt. This prompt is displayed if install NAV with /nodrivers:1.
	BOOL DisableCfgwizPrompt();

	// Check to see if cfgwiz prompt can be displayed.
	BOOL ShouldShowCfgwizPrompt();

	// Check to see if CfgWiz should display the welcome page
	BOOL ShouldShowWelcomePage();

	// Set flag to indicate that Cfgwiz has been launched at least once.
	BOOL SetCfgwizRunOnce();	

	// Check to see if Cfgwiz should display ICF disabled check box
	BOOL ShouldShowICFCheckbox();


protected:
	// Is CfgWiz.dat loaded
	BOOL m_bInit;

	// Options library handle to CfgWiz.dat
	HNAVOPTS32 m_hCfgWizDat;

	// Reboot count
	DWORD m_lRebootCount;

	// Factory file state (not used, must exists or absent)
	SWITCHSTATE m_FileState;

	// Factory regkey state (not used, must exists or absent)
	SWITCHSTATE m_RegKeyState;

	// Factory file name
	TCHAR m_szFactoryFile[MAX_PATH];

	// Factory regkey name
	TCHAR m_szFactoryRegKey[MAX_PATH];

	// CfgWiz.dat file name
	TCHAR m_szCfgWizDatPath[MAX_PATH];

	// Finished flag
	BOOL m_bFinished;

	// Silent mode
	BOOL m_bSilent;

	// Need Reboot
	BOOL m_bNeedReboot;

	// CmdLine
    ::std::string m_strCmdLine;

	// Did CfgWiz rebooted?
	BOOL m_bRebooted;

	// Need to Check For Reseal
	DWORD m_lCheckReseal;

	// Should we check the AP Reboot count?
	BOOL m_bStartAP;

	// Start AP Reboot Count
	DWORD m_lAPRebootCount;

	// Install Type
	DWORD m_lType;

	BOOL m_bDoNotCallForceOverride;

	// No driver flag
	BOOL m_bNoDrivers;

	BOOL m_bShowCfgwizPrompt;

	BOOL m_bShowWelcomePage;

	// Show Disable ICF check box on security panel?
	BOOL m_bShowICFCheckBox;
};

#endif
