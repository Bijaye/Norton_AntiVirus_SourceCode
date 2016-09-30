////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#define _WIN32_DCOM
#ifdef WIN32
    #pragma warning ( disable : 4250 )
    #pragma warning( disable : 4290 )
#endif

#include "stdafx.h"
#include "ccString.h"
#include "ccLibStd.h"
using namespace ATL;
class CScanHandler 
{
	bool m_bLogScanStartStop;
	bool m_bShouldAddItems;
	bool m_bEnableEraserScan;

	void InitOptSetting();
public:
	bool InitializeScanMgr();
	bool CallScanMgr(ATL::CAtlString strTemp);
	bool ProcessSwitch();
	bool UnInitializeScanMgr();
	CScanHandler (void);
	virtual ~CScanHandler (void);
};
