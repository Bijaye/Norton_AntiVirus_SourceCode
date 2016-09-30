////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// PostCfgWizTasks.h: interface for the PostCfgWizTasks class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPOSTCFGWIZTASKS_H__9FB8CCD4_C851_43F1_9307_DF17FE824B23__INCLUDED_)
#define AFX_CPOSTCFGWIZTASKS_H__9FB8CCD4_C851_43F1_9307_DF17FE824B23__INCLUDED_

#include "NAVSettingsCache.h"

const TCHAR g_cszNAVCfgWizRegKey[] = _T("Software\\Symantec\\Norton AntiVirus\\CfgWiz");

class CPostCfgWizTasks  
{
public:
	CPostCfgWizTasks();
	virtual ~CPostCfgWizTasks();
	void	CreateSystemRestorePoint();
	HRESULT	ConfigureAVComponentDefaults();
	HRESULT RunTasks();
protected:
	HRESULT ConfigureAPDefaults(CNAVOptSettingsCache* pDefaults);
	HRESULT ConfigureEmailDefaults(CNAVOptSettingsCache* pDefaults);
	HRESULT ConfigureIMDefaults(CNAVOptSettingsCache* pDefaults);
	HRESULT ConfigureThreatCatDefaults(CNAVOptSettingsCache* pDefaults);
};

#endif // !defined(AFX_CPOSTCFGWIZTASKS_H__9FB8CCD4_C851_43F1_9307_DF17FE824B23__INCLUDED_)
