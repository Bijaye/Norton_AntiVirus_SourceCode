// Copyright 1998 Symantec Corporation.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/RwStill.h_v   1.0   09 Mar 1998 23:46:14   DALLEE  $
//
// Description:
//      Declaration of CRepWizStillInfected. This page displays the
//      items which are still infected after the user has passed the
//      "action" panels - Repair, (Quarantine?), Delete.
//
// Contains:
//      CRepWizStillInfected
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/RwStill.h_v  $
// 
//    Rev 1.0   09 Mar 1998 23:46:14   DALLEE
// Initial revision.
//*************************************************************************

#ifndef __RWSTILL_H
#define __RWSTILL_H

#include "picture.h"
#include "RwPage.h"
#include "problist.h"

/////////////////////////////////////////////////////////////////////////////
// CRepWizStillInfected dialog

class CRepWizStillInfected : public CRepWizPage
{
    DECLARE_DYNCREATE(CRepWizStillInfected)

// Construction
public:
    CRepWizStillInfected();
    ~CRepWizStillInfected();

// Attributes
public:
    virtual MOVETO_STATE GetMoveToState();

// Dialog Data
public:
    //{{AFX_DATA(CRepWizStillInfected)
    enum { IDD = IDD_WIZPAGE4 };
    CProblemListCtrl    m_ListCtrl;
    CPicture    m_Picture;
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CRepWizStillInfected)
    public:
    virtual BOOL OnSetActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CRepWizStillInfected)
    virtual BOOL OnInitDialog();
    afx_msg void OnAdviceButton();
    afx_msg void OnDblclkListview(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


#endif // !__RWSTILL_H

