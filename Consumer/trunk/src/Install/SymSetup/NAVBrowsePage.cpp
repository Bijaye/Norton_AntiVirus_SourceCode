////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "NAVBrowsePage.h"

BOOL CNAVBrowsePage::CheckDiskSpace(CString &sSysDrive, CString &sTargetDrive, __int64 *nSysNeeded, __int64 *nTargetNeeded, __int64 *nSysAvailable, __int64 *nTargetAvailable, BOOL &bSysTargetSame)
{
	BOOL bRet = CDefaultBrowsePage::CheckDiskSpace(sSysDrive, sTargetDrive, nSysNeeded, nTargetNeeded, nSysAvailable, nTargetAvailable, bSysTargetSame);

	// For WinXP, we need an extra 25MB on the system drive
	InstallToolBox::COSVerInfo OSVI;
	
	if(TRUE == OSVI.CheckMinOS(0, InstallToolBox::COSVerInfo::ITB_OSI_WINXP, 0, 0))
	{
		*nSysNeeded += 25000000L;
	}

	return bRet;
}

CNAVBrowsePage::CNAVBrowsePage() : CDefaultBrowsePage()
{
}

CNAVBrowsePage::~CNAVBrowsePage()
{
}