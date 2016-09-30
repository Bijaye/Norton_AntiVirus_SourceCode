// Copyright 1998 Symantec Corporation.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/RwDelete.h_v   1.0   09 Mar 1998 23:46:12   DALLEE  $
//
// Description:
//      Declaration of CRepWizDelete.
//      Delete all non-repaired items wizard page.
//
// Contains:
//      CRepWizDelete
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/RwDelete.h_v  $
// 
//    Rev 1.0   09 Mar 1998 23:46:12   DALLEE
// Initial revision.
//*************************************************************************

#ifndef __RWDELETE_H
#define __RWDELETE_H

#include "picture.h"
#include "RwPage.h"
#include "problist.h"


/////////////////////////////////////////////////////////////////////////////
// CRepWizDelete dialog

class CRepWizDelete : public CRepWizPage
{
    DECLARE_DYNCREATE(CRepWizDelete)

// Construction
public:
    CRepWizDelete();
    ~CRepWizDelete();

// Attributes
public:
    enum
    {
        DELETE_ITEMS = 0,
        DONT_DELETE_ITEMS
    };

    virtual MOVETO_STATE GetMoveToState();

// Dialog Data
public:
    //{{AFX_DATA(CRepWizDelete)
    enum { IDD = IDD_WIZPAGE3 };
    CProblemListCtrl    m_ListCtrl;
    CPicture    m_Picture;
    int     m_iDeleteItems;
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CRepWizDelete)
    public:
    virtual BOOL OnSetActive();
    virtual LRESULT OnWizardNext();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CRepWizDelete)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    BOOL    m_bDidDelete;
};


#endif // !__RWDELETE_H

