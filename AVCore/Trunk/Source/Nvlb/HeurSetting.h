#if !defined(AFX_HEURSETTING_H__C97160ED_172F_4E73_809A_67AAD1B7DC73__INCLUDED_)
#define AFX_HEURSETTING_H__C97160ED_172F_4E73_809A_67AAD1B7DC73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HeurSetting.h : header file
//


#define HEUR_LEVEL_OFF	0
#define HEUR_LEVEL_1	1
#define HEUR_LEVEL_2	2
#define HEUR_LEVEL_3	3



/////////////////////////////////////////////////////////////////////////////
// CHeurSetting dialog

class CHeurSetting : public CDialog
{
// Construction
public:
	CHeurSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHeurSetting)
	enum { IDD = IDD_HEUR_SETTING };
	CStatic	m_HeurStatus;
	CSliderCtrl	m_HeurLevel;
	int		m_HeurValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHeurSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHeurSetting)
	virtual BOOL OnInitDialog();
	afx_msg void OnCustomdrawHeurLevel(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEURSETTING_H__C97160ED_172F_4E73_809A_67AAD1B7DC73__INCLUDED_)
