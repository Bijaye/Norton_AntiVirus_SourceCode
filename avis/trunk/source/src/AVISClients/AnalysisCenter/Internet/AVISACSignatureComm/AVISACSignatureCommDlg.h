// AVISACSignatureCommDlg.h : header file
//

#if !defined(AFX_AVISACSIGNATURECOMMDLG_H__2BC75337_E169_11D2_8A97_00203529AC86__INCLUDED_)
#define AFX_AVISACSIGNATURECOMMDLG_H__2BC75337_E169_11D2_8A97_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISGatewayManagerSignatureComm.h"

/////////////////////////////////////////////////////////////////////////////
// CAVISACSignatureCommDlg dialog

class CAVISACSignatureCommDlg : public CDialog
{
private:
	BOOL m_boShutdown;
	CAVISGatewayManagerSignatureComm* m_pGwMgr;
	CTime m_timeLast;

// Construction
public:
	void ShutDown();
	CAVISACSignatureCommDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAVISACSignatureCommDlg)
	enum { IDD = IDD_AVISACSIGNATURECOMM_DIALOG };
	CButton	m_btnOK;
	CStatic	m_ctlUserMsg;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAVISACSignatureCommDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAVISACSignatureCommDlg)
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

#endif // !defined(AFX_AVISACSIGNATURECOMMDLG_H__2BC75337_E169_11D2_8A97_00203529AC86__INCLUDED_)
