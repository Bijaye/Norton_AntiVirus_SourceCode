// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_CONFIGQPURGE_H__03F30102_F668_11D1_82F8_00A0C9749EEF__INCLUDED_)
#define AFX_CONFIGQPURGE_H__03F30102_F668_11D1_82F8_00A0C9749EEF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ConfigHistory.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfigQPurge dialog
#include "vpcommon.h"
#include "Resource.h"

class CConfigQPurge : public CDialog
{
private:
	BOOL m_bManualChange;

// Construction
public:
	CConfigQPurge(CWnd* pParent = NULL);   // standard constructor
	IGenericConfig	*m_pGenConfig;
	IConfig			*m_pConfig;
	IGenericConfig	*m_pConfig2;	// used by the admin only
	DWORD			m_Flags;
	DWORD			m_QuarantineType;
	DWORD           m_PurgeBySizeDirLimit;

// Dialog Data
	//{{AFX_DATA(CConfigQPurge)
	enum { IDD = IDD_CONFIG_QUARANTINE_PURGE };
	CComboBox	m_ctlType;
	CSpinButtonCtrl	m_ctlSpin;
	CEdit	m_ctlEdit;
	BOOL	m_EnablePurgingFlags;
	BOOL    m_EnablePurgeBySizeFlag;
    CEdit   m_ctlSizeEdit;
    CStatic m_cSizeUnitStatic;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigQPurge)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigQPurge)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKillfocusValue();
	afx_msg void OnHistoryHelp();
	afx_msg void OnCheckEnableQpurge();
	afx_msg void OnCheckEnableQpurgeBySize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGQPURGE_H__03F30102_F668_11D1_82F8_00A0C9749EEF__INCLUDED_)
