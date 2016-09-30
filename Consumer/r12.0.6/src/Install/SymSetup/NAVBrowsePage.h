#pragma once

#include "stdafx.h"

#include "DefaultBrowsePage.h"

class CNAVBrowsePage : public CDefaultBrowsePage
{
public:
	BOOL CheckDiskSpace(CString &sSysDrive, CString &sTargetDrive, __int64 *nSysNeeded, __int64 *nTargetNeeded, __int64 *nSysAvailable, __int64 *nTargetAvailable, BOOL &bSysTargetSame);

	CNAVBrowsePage();
	~CNAVBrowsePage();
};