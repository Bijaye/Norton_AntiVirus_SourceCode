// Copyright 1998 Symantec Corporation.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/RwPage.h_v   1.0   09 Mar 1998 23:46:08   DALLEE  $
//
// Description:
//      Base class declaration for all Repair Wizard pages.
//
// Contains:
//      CRepWizPage
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/RwPage.h_v  $
// 
//    Rev 1.0   09 Mar 1998 23:46:08   DALLEE
// Initial revision.
//*************************************************************************

#ifndef __RWPAGE_H
#define __RWPAGE_H

#include "nonav.h"


// Bitmap dimensions

#define WIZARD_PICTURE_WIDTH    111
#define WIZARD_PICTURE_HEIGHT   236

#define SIREN_WIDTH             32
#define SIREN_HEIGHT            32

// return values for GetMoveToState()

enum MOVETO_STATE {
    MOVETO_YES,                         // Page should be displayed.
    MOVETO_SKIP,                        // Page should be skipped.
    MOVETO_NO };                        // Page should not be displayed, previous pages
                                        // should not be displayed either.

/////////////////////////////////////////////////////////////////////////////
// CRepWizPage dialog

class CRepWizPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CRepWizPage)

// Construction
public:
    CRepWizPage();
    CRepWizPage(int iDlgID );
    ~CRepWizPage();

// Attributes
public:
    LRESULT GetNextPage();
    LRESULT GetPreviousPage();

    virtual MOVETO_STATE GetMoveToState();
    LPNONAV GetNoNav();
    void    SetNoNav( LPNONAV lpNoNav );

// Dialog Data
    //{{AFX_DATA(CRepWizPage)
        // NOTE - ClassWizard will add data members here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CRepWizPage)
    public:
    virtual LRESULT OnWizardNext();
    virtual LRESULT OnWizardBack();
    virtual BOOL OnQueryCancel();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    LPNONAV     m_lpNoNav;

protected:
    // Generated message map functions
    //{{AFX_MSG(CRepWizPage)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    DWORD DisplayableColors();
};


#endif // !__RWPAGE_H

