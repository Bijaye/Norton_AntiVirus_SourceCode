////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ScanTaskCollection.h : header file
//
#pragma once

#include <vector>
#include "ScanTaskInterface.h"
#include "NAVInfo.h"
#include "ccCriticalSection.h"
#include "ccSymStringImpl.h"

//-------------------------------------------------------------------------
class CScanTaskCollection: 
	public ISymBaseImpl<CSymThreadSafeRefCount>,
	public avScanTask::IScanTaskCollection
{
public:
	CScanTaskCollection(void);
    ~CScanTaskCollection(void);

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(avScanTask::IID_IScanTaskCollection, avScanTask::IScanTaskCollection)
	SYM_INTERFACE_MAP_END()

	// IScanTaskCollection
	virtual HRESULT GetCount(DWORD& dwCount) throw();
	virtual HRESULT GetItem(DWORD dwIndex, avScanTask::IScanTask** ppItem) throw();
	virtual HRESULT GetItem(LPCWSTR szTaskName, avScanTask::IScanTask** ppItem) throw();
	virtual HRESULT CreateItem(avScanTask::IScanTask** ppItem) throw();
	virtual HRESULT DeleteItem(LPCWSTR szTaskName) throw();
	virtual HRESULT DeleteAll(void) throw();
	virtual HRESULT CheckDuplicateTaskName(LPCWSTR szTaskName, bool& bDuplicate) throw();

protected:
	HRESULT findItem(LPCWSTR szTaskName, std::vector<avScanTask::IScanTask*>::iterator& it);
	HRESULT addTasks(LPCWSTR szDir);
	HRESULT emptyTaskList(void);
	HRESULT generateFilePath(CStringW& strFilePath);
	HRESULT deleteFileAgressive(LPCTSTR lpszFilePath);
    
protected:
	// Branding product name
	CStringW m_sProductName;

	// Collection of scan task items
	std::vector<avScanTask::IScanTask*> m_ScanTaskColl;

	// Critical section for m_Items
	ccLib::CCriticalSection m_Crit;

	// Directory helper object 
	CNAVInfo m_NavInfo;
};

