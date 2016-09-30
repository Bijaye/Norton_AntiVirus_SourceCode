#ifndef __NAVPATCHINST_H__
#define __NAVPATCHINST_H__

#include "stdafx.h"
#include "PatchInstImpl.h"

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

	// we only need the default implemation for now
};

#endif //__NAVPATCHINST_H__