////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// hammertimeDlg.h : header file
//

#if !defined(AFX_HAMMERTIMEDLG_H__8A814301_0BC9_4DC7_BB64_03DAECC41B9E__INCLUDED_)
#define AFX_HAMMERTIMEDLG_H__8A814301_0BC9_4DC7_BB64_03DAECC41B9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Results.h"
#include "UISink.h"

/////////////////////////////////////////////////////////////////////////////
// CHammertimeDlg dialog

class CHammertimeDlg : public CDialog
{
// Construction
private:
	int m_iMode;
	static unsigned __stdcall ThreadDisplayUI( LPVOID pParam );
	friend UINT ThreadProgress(LPVOID pParam);
	char* m_strIniFile;
	void CheckCommandLine();
	bool bCommandLineMode;


public:
	CHammertimeDlg(CWnd* pParent = NULL);	// standard constructor
	~CHammertimeDlg();


// Dialog Data
	//{{AFX_DATA(CHammertimeDlg)
	enum { IDD = IDD_HAMMERTIME_DIALOG };
	int		m_iRepCount;
	int		m_iQuarCount;
	int		m_iDelCount;
	int		m_iNonDelCount;
	int		m_iCompRepCount;
	int		m_iCompQuarCount;
	int		m_iCompDeleteCount;
	int		m_iCompNonDeleteCount;
	int		m_iCompressionDepth;
	int		m_iNumUI;
	CString	m_szDriveLetter;
	int		m_iDriveNumber;
	int		m_iInocInfectedBR;
	int		m_iInocInfectedMBR;
	int		m_iInocNonInfectedBR;
	int		m_iInocNonInfectedMBR;
	BOOL	m_bInocNotifyOnly;
	int		m_iSeconds;
	int		m_iProgressTime;
	BOOL	m_bShowProgress;
	BOOL	m_bSetEmailInfo;
	int		m_iHandleTime;
	int		m_iMessages;
	int		m_iMBRRep;
	int		m_iMBRUnRep;
	int		m_iBRRep;
	int		m_iBRUnRep;
	BOOL	m_bRepWizbeforeSMTP;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHammertimeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	void SetUI(int setting);
	void SaveSettings();
	void LoadSettings();

	// Generated message map functions
	//{{AFX_MSG(CHammertimeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	afx_msg void OnUidisplaySmtpemailsendingui();
	afx_msg void OnUidisplayMemoryinfection();
	afx_msg void OnUidisplayInnoculationui();
	afx_msg void OnUidisplayCommonuiscanning();
	afx_msg void OnFileQuit();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    int m_iNonViralThreatCount;
    int m_iNonViralThreatNotDeletableCount;
    afx_msg void OnEnChangeEdit21();
    BOOL m_bThreatCatEnabled;
};

struct ThreadParams
{
	int iProgressTime;
	ICommonUI* pCommonUI;
	CResults* pResults;
	CUISink* pUISink;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HAMMERTIMEDLG_H__8A814301_0BC9_4DC7_BB64_03DAECC41B9E__INCLUDED_)

