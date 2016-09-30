////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SetupInfoBase.h"

#include "NAVLaunchConditions.h"

#include "NAVBrowsePage.h"

#include "Shobjidl.h"
#include "intshcut.h"

class CSetupInfo : public CSetupInfoBase
{
public:
	CSetupInfo();
	virtual ~CSetupInfo();
	
	virtual HRESULT InitInfo();
	virtual HRESULT CheckLaunchConditions();
	virtual HRESULT LaunchWizard();
	virtual HRESULT LaunchFinishWizard();
	virtual HRESULT SetupInstallMode();
	virtual HRESULT ProcessCommandLines(LPTSTR lpstrCmdLine);
	virtual BOOL PreInstall();
	virtual BOOL PostInstall(BOOL bRollback);
	virtual BOOL PreUninstall();
	virtual BOOL PostUninstall(BOOL bRollback);
    virtual BOOL PostUninstallPrevious(BOOL bRollback);

    virtual BOOL GetNewProductPNU(CString& sNewPNU);
    virtual BOOL GetExistingProductPNU(CString& sExistingPNU);
    virtual BOOL GetNewLicenseType(DWORD& nNewLicType);
    virtual BOOL GetExistingLicenseType(DWORD& nExistingLicType);
    virtual BOOL GetExistingProductVersion(InstallToolBox::LP_ITB_VERSION itbExistingVer);
    virtual BOOL GetNewProductVersion(InstallToolBox::LP_ITB_VERSION itbNewVer);
	virtual void PreOverInstallValidation(InstallToolBox::LP_ITB_VERSION itbExistingVer, InstallToolBox::LP_ITB_VERSION itbNewVer, DWORD dwExistingLicType, DWORD dwNewLicType, bool& bContinue, bool& bRet);

	//called by ICF page
	void setRestoreICFSettings(BOOL bRestore){m_bRestoreICFSettings = bRestore;}
	BOOL getRestoreICFSettings(){return m_bRestoreICFSettings;}
	
protected:

	// Functions that need some work
	virtual BOOL SetupInstallWizard();			// should add pages for install wizard
	virtual BOOL SetupModifyWizard();			// should add pages for modify wizard
	virtual BOOL SetupUninstallWizard();		// should add pages for uninstall
	
	bool RegisterWithWMI();
	void UnRegisterWithWMI();
	bool SetCCServicesStates(BOOL bAutoStart, BOOL bStartNow);
	static void ResetPassword();
	bool FakeNISInstall(bool bAdd);
	bool IWPIsYielding();
	bool ShowUninstallSubWarningPage();
	void CreateShortcuts();
	void CreateShortcut(LPCTSTR szLocation, LPCTSTR szTarget, LPCTSTR szArguments, LPCTSTR szDescription, bool bUseNAVIcon, int nShowCmd, LPCTSTR szWorkingDir, IShellLink *pShellLink, IPersistFile *pPersistFile);
	void DeleteShortcuts();
	void DeleteFile(CString &sFileName);
	void GetShortcutPaths(CString &sNAVProgramMenuFolder, CString &sDesktopDir, CString &sCoBrandedShortcutName);
	void GetShorcutTargetPaths(CString &sInstallDir, CString &sSymSharedDir, CString &sUIDir, CString &sProgramFilesFolder);
	void RemoveSNDUpdaterPatches();
	
private:
	TCHAR m_szFolder[MAX_PATH*2];
	ATL::CString m_szSKUPath;
	HMODULE m_hUIRES;

	CNAVLaunchConditions m_NAVLaunchConditions;

	// Keeps track of whether or not SymProtect was running when the install started.
	bool m_bSPRunning;
	BOOL m_bRestoreICFSettings; //Used by ICF restore page.

	// dialog pages we need in our wizard
	CDefaultModifyPage pageModify;
	CDefaultVerifyRemovePage pageVerifyRemove;
	CDefaultActivationPage pageActivate;
	CDefaultOptionPage pageBrowseOptions;
	CDefaultFinishPage pageFinish;
	CDefaultStartInstallPage pageStartInstall;
	CNAVSubscriptionWarningPage pageSubscriptionWarning;
	CICFRestorePage pageICFRestore;
};
