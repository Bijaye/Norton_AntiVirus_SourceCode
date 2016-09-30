////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __NAVPATCHINST_H__
#define __NAVPATCHINST_H__

#include "stdafx.h"
#include "PatchInstImpl.h"
#include "InterfaceHelper.h"

extern const LPCTSTR g_cszProductName;
extern const LPCTSTR g_cszProductID;
extern const InstallToolBox::ITB_VERSION g_citbVersion;

//////////////////////////////////////////////////////////////////
// CNAVPatchInst:
//
//	Defines NAVs implementation of the PatchInst interface
//
class CNAVPatchInst : public CPatchInstImpl<&g_cszProductName, &g_cszProductID, &g_citbVersion>
{
public:
	bool GetFullProductName(LPSTR pszProductName, DWORD& dwSize);
	bool PostInstall(LPCSTR pcszPatchInstID, const InstallToolBox::LP_ITB_VERSION lpitbVersion);

protected:
	void UnRegisterNSW_NAVPreC();
};

#endif //__NAVPATCHINST_H__