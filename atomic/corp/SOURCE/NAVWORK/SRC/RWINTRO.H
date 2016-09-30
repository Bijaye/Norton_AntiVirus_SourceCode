// Copyright 1998 Symantec Corporation.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/RwIntro.h_v   1.0   09 Mar 1998 23:46:18   DALLEE  $
//
// Description:
//      Declaration for CRepWizIntro. Introductory Repair Wizard page
//      displays infection information and allow choice of automatic (Wizard)
//      or manual (Problems Found dlg) mode to fix problems.
//
// Contains:
//      CRepWizIntro
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/RwIntro.h_v  $
// 
//    Rev 1.0   09 Mar 1998 23:46:18   DALLEE
// Initial revision.
//*************************************************************************

#ifndef __RWINTRO_H
#define __RWINTRO_H

#include "picture.h"
#include "RWPage.h"

#include <vector>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CRepWizIntro dialog

class CRepWizIntro : public CRepWizPage
{
    DECLARE_DYNCREATE(CRepWizIntro)

// Construction
public:
    CRepWizIntro();
    ~CRepWizIntro();

// Dialog Data
    //{{AFX_DATA(CRepWizIntro)
    enum { IDD = IDD_WIZPAGE1 };
    CListBox    m_VirusListBox;
    CPicture    m_Picture;
    int     m_iRepairMode;
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CRepWizIntro)
    public:
    virtual BOOL OnSetActive();
    virtual LRESULT OnWizardNext();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    BOOL FillVirusList();
    BOOL IsVirusInList( WORD wVirID );
    BOOL AddVirusToList( LPINFECTEDFILE lpItem );

    vector<WORD>    m_vectViruses;

    // Generated message map functions
    //{{AFX_MSG(CRepWizIntro)
    virtual BOOL OnInitDialog();
    afx_msg void OnVirusInfoButton();
    afx_msg void OnDblclkListViruses();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // __RWINTRO_H
