////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// CCFRAT.h: interface of FRAT class
//
//////////////////////////////////////////////////////////////////////

//constants
const TCHAR WIN9X_AUDITMODE_KEY[]	= _T("Software\\Microsoft\\Windows\\CurrentVersion");
const TCHAR WINNT_FRAT_KEY[]		= _T("SYSTEM\\Setup");
const TCHAR AUDIT_MODE_VALUE[]		= _T("AuditInProgress");
const TCHAR FACTORY_MODE_VALUE[]	= _T("FactoryPreInstallInProgress");
const TCHAR FACTORY_RESEAL_VALUE[]	= _T("CloneTag");
const TCHAR WINVISTA_OOBE_VALUE[]	= _T("OOBEInProgress");
const TCHAR WINVISTA_SSIP_VALUE[]	= _T("SystemSetupInProgress");

class CFRAT
{
public:
	CFRAT();
	~CFRAT();
	void Initialize();
	bool IsAuditMode();
	bool IsFactoryMode();
	bool IsReseal();
	
private:
	bool m_bIsAuditMode;
	bool m_bIsFactoryMode;
	bool m_bIsReseal;
};