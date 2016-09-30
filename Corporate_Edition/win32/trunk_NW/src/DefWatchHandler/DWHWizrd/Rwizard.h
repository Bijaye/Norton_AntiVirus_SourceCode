// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/rwizard.h_v   1.1   03 Apr 1998 11:20:26   tcashin  $
//
// Description:
//      Declaration of CRepairWizard. Modal propertysheet/wizard for
//      dealing with infected items found during scanning.
//
// Contains:
//      CRepairWizard
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/rwizard.h_v  $
// 
//    Rev 1.1   03 Apr 1998 11:20:26   tcashin
// Added the Quarantine panel.
// 
//    Rev 1.0   09 Mar 1998 23:46:10   DALLEE
// Initial revision.
//*************************************************************************

#ifndef __RWIZARD_H
#define __RWIZARD_H


// #include "nonav.h"

#include "RwIntro.h"
#include "RwRepair.h"
#include "RwQuar.h"
#include "RwDelete.h"
#include "RwStill.h"
#include "RwFinInf.h"
#include "RwFinCln.h"

/////////////////////////////////////////////////////////////////////////////
// CRepairWizard
//
// Usage is:
//
//      CRepairWizard()
//      SetNoNav()
//
//      DoModal()
//
//      GetManualRepair() -- check if user selected to not use the wizard.

class CRepairWizard : public CPropertySheet
{
    DECLARE_DYNAMIC(CRepairWizard)

// Construction
public:
    CRepairWizard(CWnd* pWndParent = NULL);

// Attributes
public:
    CRepWizIntro            m_pageIntro;
    CRepWizRepair           m_pageRepair;
    CRepWizQuarantine       m_pageQuarantine;
    CRepWizDelete           m_pageDelete;
    CRepWizStillInfected    m_pageStillInfected;
    CRepWizFinishInfected   m_pageFinishInfected;
    CRepWizFinishClean      m_pageFinishClean;

    LPVOID  GetNoNav();
    void    SetNoNav( LPVOID lpNoNav );

    // Manual repair flag.
    BOOL m_bManual;

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CRepairWizard)
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CRepairWizard();

    void SetManualRepair( BOOL bManual = TRUE) { m_bManual = bManual; }
    BOOL GetManualRepair() { return m_bManual; }

private:
    LPVOID     m_lpNoNav;

// Generated message map functions
protected:
    //{{AFX_MSG(CRepairWizard)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif  // !__RWIZARD_H

