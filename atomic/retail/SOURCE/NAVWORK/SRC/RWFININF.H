// Copyright 1998 Symantec Corporation.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/rwfininf.h_v   1.1   12 May 1998 14:06:40   tcashin  $
//
// Description:
//      Declaration of CRepWizFinishInfected. Repair Wizard final panel if infected
//      items still remain.
//
// Contains:
//      CRepWizFinishInfected
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/rwfininf.h_v  $
// 
//    Rev 1.1   12 May 1998 14:06:40   tcashin
// Added the number of files quarantined and deleted.
// 
//    Rev 1.0   09 Mar 1998 23:46:16   DALLEE
// Initial revision.
//*************************************************************************

#ifndef __RWFININF_H
#define __RWFININF_H

#include "picture.h"
#include "RWPage.h"
#include "animation.h"

/////////////////////////////////////////////////////////////////////////////
// CRepWizFinishInfected dialog

class CRepWizFinishInfected : public CRepWizPage
{
    DECLARE_DYNCREATE(CRepWizFinishInfected)

// Construction
public:
    CRepWizFinishInfected();
    ~CRepWizFinishInfected();

// Attributes
public:
    virtual MOVETO_STATE GetMoveToState();

// Dialog Data
public:
    //{{AFX_DATA(CRepWizFinishInfected)
    enum { IDD = IDD_WIZPAGE5 };
    CAnimation  m_Animation;
    CPicture    m_Picture;
    CString     m_sInfected;
    CString     m_sScanned;
    CString     m_sCleaned;
    CString     m_sQuarantined;
    CString     m_sDeleted;
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CRepWizFinishInfected)
    public:
    virtual BOOL OnSetActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CRepWizFinishInfected)
    virtual BOOL OnInitDialog();
    afx_msg void OnMoreInfoButton();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};

#endif // !__RWFININF_H

