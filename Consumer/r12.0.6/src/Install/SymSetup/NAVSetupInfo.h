#pragma once

#include "SetupInfoBase.h"

#include "NAVLaunchConditions.h"

#include "NAVBrowsePage.h"

class CSetupInfo : public CSetupInfoBase
{
public:
	CSetupInfo();
	virtual ~CSetupInfo();
	
	virtual HRESULT InitInfo();
	virtual HRESULT CheckLaunchConditions();
	virtual HRESULT LaunchWizard();
	virtual HRESULT LaunchFinishWizard();
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
	
protected:

	// Functions that need some work
	virtual BOOL SetupInstallWizard();			// should add pages for install wizard
	virtual BOOL SetupModifyWizard();			// should add pages for modify wizard
	virtual BOOL SetupUninstallWizard();		// should add pages for uninstall

private:
	TCHAR m_szFolder[MAX_PATH*2];
	ATL::CString m_szSKUPath;
	HMODULE m_hUIRES;

	CNAVLaunchConditions m_NAVLaunchConditions;

	// Keeps track of whether or not SymProtect was running when the install started.
	bool m_bSPRunning;

	// dialog pages we need in our wizard
	CDefaultModifyPage pageModify;
	CDefaultVerifyRemovePage pageVerifyRemove;
	CDefaultWelcomePage pageWelcome;
	CDefaultActivationPage pageActivate;
	CNAVBrowsePage pageBrowse;
	CDefaultFinishPage pageFinish;
	CDefaultScanPage pageScan;
};
