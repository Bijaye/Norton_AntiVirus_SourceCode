////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LaunchConditions.h"

//installtoolbox includes
#include "NAVSetupResource.h"
#include "unmark.h"
#include "navdetection.h"

class CNAVLaunchConditions : public CLaunchConditions
{
public:
	CNAVLaunchConditions() : CLaunchConditions() {}
	CNAVLaunchConditions(CSetupInfoBase* pSetupInfo) : CLaunchConditions(pSetupInfo) {}
	~CNAVLaunchConditions(void) {}	
	
	//
	// use these function to check all the launch conditions that need 
	// to be met before (un)installing Norton AntiVirus
	//
	bool CheckInstallConditions(void);
	bool CheckUninstallConditions(void); 
	
	void SetNAVInfo(CSetupInfo * pSetupInfo) {m_pNAVInfo = pSetupInfo;}

	//
	// individual launch conditions needed before both in/uninstall
	//
	static bool CheckForRunningNAV(DWORD&, DWORD&, DWORD&);
	static bool CheckForVistaMigration(DWORD&, DWORD&, DWORD&);	
	
	//
	// individual launch conditions needed before install
	//
	static bool CheckForNIS(DWORD&, DWORD&, DWORD&);
	static bool CheckForCorpNAV(DWORD&, DWORD&, DWORD&);
	static bool CheckForServicesMarkedForDeletion(DWORD&, DWORD&, DWORD&);
	static bool CheckForFilesMarkedForDeletion(DWORD&, DWORD&, DWORD&);
	static bool CheckForServer(DWORD&, DWORD&, DWORD&);
	static bool CheckOverInstall(DWORD&, DWORD&, DWORD&);

protected:
	static CSetupInfo *m_pNAVInfo;

private:
	//
	// helper functions
	//
	static bool ShutdownWndByClass(LPTSTR, UINT);
	static bool IsServerInstallAllowed(void);
	static bool GetExistingNPFVersion(InstallToolBox::LP_ITB_VERSION itbExistingVer);

	static TCHAR* g_szNavFiles[];
	static TCHAR* g_szServiceNames[];

};
