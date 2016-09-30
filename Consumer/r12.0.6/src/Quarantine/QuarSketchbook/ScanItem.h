// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
#pragma once

#include "ccScanInterface.h"

class CScanItem :
	public IScanItems,
	public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
	CScanItem(void);
	virtual ~CScanItem(void);

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY( IID_ScanItems, IScanItems )
	SYM_INTERFACE_MAP_END()

	// IScanItems methods
    virtual int GetScanItemCount() throw();
    virtual LPCSTR GetScanItemPath(int iIndex) throw();

	void SetPath(CString strPath);

protected:
	CStringA m_strPath;

private:
	    // Disallowed
        CScanItem(const CScanItem &) throw();
        CScanItem& operator=(const CScanItem&) throw();
};
