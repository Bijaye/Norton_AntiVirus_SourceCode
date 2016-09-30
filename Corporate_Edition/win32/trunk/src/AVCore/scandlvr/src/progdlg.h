// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/progdlg.h_v   1.0   01 May 1998 22:21:36   jtaylor  $
/////////////////////////////////////////////////////////////////////////////
//
// ProgDlg.h : header file
// CG: This file was added by the Progress Dialog component
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/progdlg.h_v  $
//
//	Rev 1.1	12 Jan 2005 KTALINKI
//	Added ISNDProgress, ISymBaseImpl<CSymThreadSafeRefCount> to the parent class list.
//	Added support for Interfaces ISNDProgress, ISymBaseImpl
//	Added UpdateProgress method
//
//    Rev 1.0   01 May 1998 22:21:36   jtaylor
// Initial revision.
// 
//    Rev 1.0   13 Mar 1998 15:24:24   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

#ifndef __PROGDLG_H__
#define __PROGDLG_H__

class CProgressDlg : public CDialog, public ISNDProgress, public ISymBaseImpl<CSymThreadSafeRefCount>
{
// Construction / Destruction
public:
	SYM_INTERFACE_MAP_BEGIN()		
		SYM_INTERFACE_ENTRY(IID_ISNDProgress, ISNDProgress)
	SYM_INTERFACE_MAP_END()

    CProgressDlg(UINT nCaptionID = 0);   // standard constructor
    ~CProgressDlg();
	
    BOOL Create(CWnd *pParent=NULL);

    // Checking for Cancel button
    BOOL CheckCancelButton();
    // Progress Dialog manipulation
    void SetStatus(LPCTSTR lpszMessage);
    void SetRange(int nLower,int nUpper);
    int  SetStep(int nStep);
    int  SetPos(int nPos);
    int  OffsetPos(int nPos);
    int  StepIt();
	
	HRESULT UpdateProgress(DWORD_PTR dwpProgressCookie, int nPercentageComplete);

// Dialog Data
    //{{AFX_DATA(CProgressDlg)
	enum { IDD = IDD_PROGRESS_DIALOG };
    CProgressCtrl	m_Progress;
	CString	m_szStatusText;
	//}}AFX_DATA

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CProgressDlg)
    public:
    virtual BOOL DestroyWindow();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
	UINT m_nCaptionID;
    int m_nLower;
    int m_nUpper;
    int m_nStep;

    BOOL m_bCancel;
    BOOL m_bParentDisabled;

    void ReEnableParent();

    virtual void OnCancel();
    virtual void OnOK() {};
    void UpdatePercent(int nCurrent);
    void PumpMessages();


    // Generated message map functions
    //{{AFX_MSG(CProgressDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
#endif // __PROGDLG_H__
