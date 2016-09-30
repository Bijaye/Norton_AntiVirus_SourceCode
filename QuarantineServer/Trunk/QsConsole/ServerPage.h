/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_SERVERPAGE_H__91EA4538_BACC_11D2_8F46_3078302C2030__INCLUDED_)
#define AFX_SERVERPAGE_H__91EA4538_BACC_11D2_8F46_3078302C2030__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerPage.h : header file
//

#include <afxdlgs.h>
#include "QPropPage.h"
#import "qserver.tlb" no_namespace

/////////////////////////////////////////////////////////////////////////////
// CServerPage dialog

class CServerPage : public CQPropPage
{
	DECLARE_DYNCREATE(CServerPage)

// Construction
public:
	CServerPage();
	~CServerPage();

// Dialog Data
	//{{AFX_DATA(CServerPage)
	enum { IDD = IDD_QSERVER_PAGE };
	CSpinButtonCtrl	m_NoSampleSpin;
	CSpinButtonCtrl	m_RefreshSpin;
	CEdit	m_RefreshEdit;
	BOOL	m_bListenIP;
	BOOL	m_bListenSPX;
	int		m_iIPPort;
	CString	m_sFolder;
	UINT	m_uMaxSize;
	int		m_iSPXPort;
	int		m_iRefreshInteveral;
	UINT	m_uWarningSize;
	UINT	m_uMaxNoSamples;
	BOOL	m_bPurge;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CServerPage)
	public:
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL
    virtual void OnOK();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CServerPage)
	afx_msg void OnIpCheck();
	afx_msg void OnIpxCheck();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeQFolder();
	afx_msg void OnChangeQfolderSize();
	afx_msg void OnChangeRefreshEdit();
	afx_msg void OnChangeEditQfolderWarningSize();
	afx_msg void OnChangeIpPort();
	afx_msg void OnChangeSpxPort();
	afx_msg void OnChangeQfolderNoSamples();
	afx_msg void OnCheckPurge();
	afx_msg void OnButtonPurgeLog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    

public:
    LPSTREAM    m_pServerStream;
    long        m_lNotifyHandle;
    LPVOID      m_pSnapin;
    DWORD       dwFreeSpace;

private:
	int IsValidQserverFolder();
    IQserverConfig*  m_pConfig;
    CString     m_sOrigQFolder;
	void SetRefreshInterval(DWORD dwRefreshInteveral);	
	DWORD GetRefreshInterval(void);	
    static DWORD m_dwWhatsThisMap[];
	int m_ifMoveQFolder;
    

};

// 
// Property change notifications
// 
#define PROPERTY_CHANGE_QFOLDER     0x00000001

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERPAGE_H__91EA4538_BACC_11D2_8F46_3078302C2030__INCLUDED_)
