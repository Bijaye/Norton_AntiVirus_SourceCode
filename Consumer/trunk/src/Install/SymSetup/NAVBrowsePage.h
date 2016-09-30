////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

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