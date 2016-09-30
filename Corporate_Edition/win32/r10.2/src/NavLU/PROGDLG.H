// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1996 - 2003, 2005 Symantec Corporation. All rights reserved.
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/PROGDLG.H_V   1.0.1.0   22 Jun 1998 19:51:26   RFULLER  $
//
// Description: Header file for patching progress dialog.
//
// Contains:    CPatchProgressDlg class definition
//
// See Also:
//****************************************************************************
// $Log:   S:/NAVLU/VCS/PROGDLG.H_V  $
// 
//    Rev 1.0.1.0   22 Jun 1998 19:51:26   RFULLER
// Branch base for version QAKN
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
