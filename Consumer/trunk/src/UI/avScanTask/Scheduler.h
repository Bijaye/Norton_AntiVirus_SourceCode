////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Scheduler.h : header/implementation file
//
#pragma once

#include "ScanTaskInterface.h"


/////////////////////////////////////////////////////////////////////////////
// CScheduler
class CScheduler: 
	public ISymBaseImpl<CSymThreadSafeRefCount>,
	public avScanTask::IScheduler
{
public:
	CScheduler() {}
	~CScheduler() {}

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(avScanTask::IID_IScheduler, avScanTask::IScheduler)
	SYM_INTERFACE_MAP_END()

	// IScheduler
	virtual HRESULT GetRunAtStartup(bool& bRunAtStartup) throw();
	virtual HRESULT SetRunAtStartup(bool bRunAtStartup) throw();
	virtual HRESULT GetRunning(bool& bRunning) throw();
	virtual HRESULT SetRunning(bool bStart) throw();

protected:
	HRESULT stopScheduler();
	HRESULT startScheduler();

	// bRunning - Wait until it's started or stopped (default = running )
    // dwRetries - Number of 100 millisecond retries (default = 1 sec )
    HRESULT waitForScheduler(bool bRunning=true, DWORD dwRetries=10);
};

