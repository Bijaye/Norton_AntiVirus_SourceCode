#if !defined(AFX_AVISACSAMPLECOMMDLG_H__02BFA346_4B46_11D3_8AC3_00203529AC86__INCLUDED_)
#define AFX_AVISACSAMPLECOMMDLG_H__02BFA346_4B46_11D3_8AC3_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AVISACSampleCommDlg.h : header file
//

#include "AVISGatewayManagerSampleComm.h"

/////////////////////////////////////////////////////////////////////////////
// CAVISACSampleCommDlg dialog

class CAVISACSampleCommDlg : public CDialog
{
private:
	BOOL m_boShutdown;
	CAVISGatewayManagerSampleComm* m_pGwMgr;
	CTime m_timeLast;

// Construction
public:
	void ShutDown();
	CAVISACSampleCommDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAVISACSampleCommDlg)
	enum { IDD = IDD_AVISACSAMPLECOMM_DIALOG };
	CButton	m_btnOK;
	CStatic	m_ctlUserMsg;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAVISACSampleCommDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAVISACSampleCommDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AVISACSAMPLECOMMDLG_H__02BFA346_4B46_11D3_8AC3_00203529AC86__INCLUDED_)
