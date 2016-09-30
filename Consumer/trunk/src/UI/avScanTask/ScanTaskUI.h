////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ScanTaskUI.h : header file
//
#pragma once

#include "ScanTaskInterface.h"


// {862CBD39-162B-46a6-B6E3-61E4AB93000E}
SYM_DEFINE_OBJECT_ID(CLSID_ScanTaskUI, 0x862cbd39, 0x162b, 0x46a6, 0xb6, 0xe3, 0x61, 0xe4, 0xab, 0x93, 0x0, 0xe);

class CScanTaskUI:
	public ISymBaseImpl<CSymThreadSafeRefCount>,
	public avScanTask::IScanTaskUI
{
public:
	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(avScanTask::IID_IScanTaskUI, avScanTask::IScanTaskUI)
	SYM_INTERFACE_MAP_END()

public:
	CScanTaskUI(void);
	virtual ~CScanTaskUI(void);

	// IScanTaskUI methods
	virtual HRESULT DisplayUI(HWND hWndParent, cc::IKeyValueCollection* pKeyValueCol) throw();
};
