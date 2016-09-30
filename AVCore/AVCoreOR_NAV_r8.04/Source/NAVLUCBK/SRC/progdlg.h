// Copyright 1996-1998 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLUCBK/VCS/progdlg.h_v   1.0   08 Jan 1999 19:29:40   CEATON  $
//
// Description: Header file for patching progress dialog.
//
// Contains:    CPatchProgressDlg class definition
//
// See Also:
//****************************************************************************
// $Log:   S:/NAVLUCBK/VCS/progdlg.h_v  $
// 
//    Rev 1.0   08 Jan 1999 19:29:40   CEATON
// Initial revision.
// 
//    Rev 1.0   20 May 1998 16:46:04   tcashin
// Initial revision.
// 
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPatchProgressDlg dialog

#ifndef __PROGDLG_H__
#define __PROGDLG_H__

#include "resource.h"

class CPatchProgressDlg : public CDialog
{
// Construction / Destruction
public:
    CPatchProgressDlg(UINT nCaptionID = 0);   // standard constructor
    ~CPatchProgressDlg();

    BOOL Create(CWnd *pParent=NULL);

    // Checking for Cancel button
    BOOL CheckCancelButton();
    // Progress Dialog manipulation
    void SetRange(int nLower,int nUpper);
    int  SetStep(int nStep);
    int  SetPos(int nPos);
    int  OffsetPos(int nPos);
    int  StepIt();
        
// Dialog Data
    //{{AFX_DATA(CPatchProgressDlg)
    enum { IDD = CG_IDD_PROGRESS };
    CProgressCtrl	m_Progress;
    //}}AFX_DATA

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CPatchProgressDlg)
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
    //{{AFX_MSG(CPatchProgressDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // __PROGDLG_H__
