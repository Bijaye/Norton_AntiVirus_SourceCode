// Copyright 1998 Symantec Corporation.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/rwquar.h_v   1.0   03 Apr 1998 11:17:04   tcashin  $
//
// Description:
//      Declaration of CRepWizQuarantine.
//      Quarantine all non-repaired items wizard page.
//
// Contains:
//      CRepWizQuarantine
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/rwquar.h_v  $
// 
//    Rev 1.0   03 Apr 1998 11:17:04   tcashin
// Initial revision.
// 
//*************************************************************************

#ifndef __RWQUAR_H
#define __RWQUAR_H

#include "picture.h"
#include "RwPage.h"
#include "problist.h"


/////////////////////////////////////////////////////////////////////////////
// CRepWizQuarantine dialog

class CRepWizQuarantine : public CRepWizPage
{
    DECLARE_DYNCREATE(CRepWizQuarantine)

// Construction
public:
    CRepWizQuarantine();
    ~CRepWizQuarantine();

// Attributes
public:
    enum
    {
        QUARANTINE_ITEMS = 0,
        DONT_QUARANTINE_ITEMS
    };

    virtual MOVETO_STATE GetMoveToState();

// Dialog Data
public:
    //{{AFX_DATA(CRepWizQuarantine)
    enum { IDD = IDD_WIZPAGE7 };
    CProblemListCtrl    m_ListCtrl;
    CPicture    m_Picture;
    int     m_iQuarantineItems;
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CRepWizQuarantine)
    public:
    virtual BOOL OnSetActive();
    virtual LRESULT OnWizardNext();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CRepWizQuarantine)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    BOOL    m_bDidQuarantine;
};


#endif // !__RWQUAR_H

