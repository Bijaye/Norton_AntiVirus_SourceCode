////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// UISink.h: interface for the CUISink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UISINK_H__AE706131_DF45_4CD5_B653_5D6837D9CDD8__INCLUDED_)
#define AFX_UISINK_H__AE706131_DF45_4CD5_B653_5D6837D9CDD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Forward declaration.
#include "results.h"
#include "commonuiinterface.h"

class CUISink : public ICommonUISink, public ISymBaseImpl< CSymThreadSafeRefCount >
{
public:
	CUISink( CResults& results, int iHandleTime);
	virtual ~CUISink();

    SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY( IID_ICommonUISink, ICommonUISink )
	SYM_INTERFACE_MAP_END()

	//
	// ICommonUISink methods
	//
	virtual HRESULT OnCUIReady();
	virtual HRESULT OnCUIAbort();
	virtual HRESULT OnCUIPause();
	virtual HRESULT OnCUIResume();
	virtual HRESULT OnCUIRepairInfection(/* [in] */ IScanInfection* pInfection);
	virtual HRESULT OnCUIQuarantineInfection(/* [in] */ IScanInfection* pInfection);
	virtual HRESULT OnCUIDeleteInfection(/* [in] */ IScanInfection* pInfection);
	virtual HRESULT OnCUIGetInfectionName(/* [in] */ IScanInfection* pInfection,/* [out] */ string& strName);
	virtual HRESULT OnCUIGetInfectionDescription(/* [in] */ IScanInfection* pInfection,/* [out] */ string& strDescription);
    virtual HRESULT OnCUIGetThreatCategoryEnabled(/* [in] */ long lCategory,/* [out] */ BOOL* pbEnabled);
    virtual HRESULT OnCUIExcludeThreat(/* [in] */ IScanInfection* pInfection);
    virtual HRESULT OnCUIGetCurrentFolder(LPSTR szFolder, long nLength);
	virtual HRESULT OnCUIDeletedFileCount(long& nDeletedFileCount);
	virtual HRESULT OnCUIRepairedFileCount(long& nRepairedFileCount);
	virtual HRESULT OnCUIInfectedFileCount(long& nInfectedFileCount);
	virtual HRESULT OnCUIScanFileCount(long& nScanFileCount);
    virtual HRESULT OnCUIReScanManualItems();

	bool m_bAbort;

private:
	// Results object to party on.
	CResults& m_Results;
	int m_iHandleTime;
    long m_lScanCount;
};

#endif // !defined(AFX_UISINK_H__AE706131_DF45_4CD5_B653_5D6837D9CDD8__INCLUDED_)
