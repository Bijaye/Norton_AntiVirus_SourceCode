// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/rwfincln.h_v   1.2   07 Jun 1998 15:13:46   tcashin  $
//
// Description:
//      Declaration of CRepWizFinishClean. Repair Wizard final panel if all
//      infected were "cleaned" - Repaired, (Quarantined?), Deleted.
//
// Contains:
//      CRepWizPage6
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/rwfincln.h_v  $
// 
//    Rev 1.2   07 Jun 1998 15:13:46   tcashin
// Added a string to change the text based on whether of not files
// were added to quarantine (#114308).
// 
//    Rev 1.1   12 May 1998 14:06:06   tcashin
// Added the number of files quarantined and deleted.
// 
//    Rev 1.0   09 Mar 1998 23:46:16   DALLEE
// Initial revision.
//*************************************************************************

#ifndef __RWFINCLN_H
#define __RWFINCLN_H

#include "picture.h"
#include "RWPage.h"

/////////////////////////////////////////////////////////////////////////////
// CRepWizFinishClean dialog

class CRepWizFinishClean : public CRepWizPage
{
    DECLARE_DYNCREATE(CRepWizFinishClean)

// Construction
public:
    CRepWizFinishClean();
    ~CRepWizFinishClean();

// Attributes
public:
    virtual MOVETO_STATE GetMoveToState();

// Dialog Data
public:
    //{{AFX_DATA(CRepWizFinishClean)
    enum { IDD = IDD_WIZPAGE6 };
    CPicture    m_Picture;
    CString     m_sInfected;
    CString     m_sScanned;
    CString     m_sCleaned;
    CString     m_sQuarantined;
    CString     m_sDeleted;
    CString     m_sFinishText;
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CRepWizFinishClean)
    public:
    virtual BOOL OnSetActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CRepWizFinishClean)
    virtual BOOL OnInitDialog();
    afx_msg void OnMoreInfoButton();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};


#endif // !__RWFINCLN_H

