// Copyright 1998 Symantec Corporation.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/RwRepair.h_v   1.0   09 Mar 1998 23:46:12   DALLEE  $
//
// Description:
//      Declaration of CRepWizRepair.
//      Repair all wizard page.
//
// Contains:
//      CRepWizRepair
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/RwRepair.h_v  $
// 
//    Rev 1.0   09 Mar 1998 23:46:12   DALLEE
// Initial revision.
//*************************************************************************

#ifndef __RWREPAIR_H
#define __RWREPAIR_H

#include "picture.h"
#include "RwPage.h"
#include "problist.h"

/////////////////////////////////////////////////////////////////////////////
// CRepWizRepair dialog

class CRepWizRepair : public CRepWizPage
{
    DECLARE_DYNCREATE(CRepWizRepair)

// Construction
public:
    CRepWizRepair();
    ~CRepWizRepair();

// Attributes
public:
    virtual MOVETO_STATE GetMoveToState();

// Dialog Data
public:
    //{{AFX_DATA(CRepWizRepair)
    enum { IDD = IDD_WIZPAGE2 };
    CProblemListCtrl    m_ListCtrl;
    CPicture    m_Picture;
    //}}AFX_DATA

// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CRepWizRepair)
    public:
    virtual BOOL OnSetActive();
    virtual LRESULT OnWizardNext();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CRepWizRepair)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    BOOL    m_bDidRepair;
};


#endif // !__RWREPAIR_H

