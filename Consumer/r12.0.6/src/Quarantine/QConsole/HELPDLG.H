//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// HLPropPg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/HELPDLG.H_v  $
// 
//    Rev 1.0   20 May 1998 17:15:34   DBuches
// Initial revision.
// 
//    Rev 1.1   29 Dec 1997 16:53:58   DBuches
// Added help support.
// 
//    Rev 1.0   29 Dec 1997 16:33:38   DBuches
// Initial revision.
// 
//    Rev 1.0   20 Aug 1997 18:32:28   DBUCHES
// Initial revision.
//
//    Rev 1.4   22 Jul 1997 13:13:52   DBUCHES
// Added call to NG_SetFontsInDialog()
//
//    Rev 1.3   20 Jun 1997 12:01:08   DBUCHES
// Added Help support.
//
//    Rev 1.2   27 May 1997 20:56:20   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.1   16 May 1997 15:28:14   DBUCHES
// Fixed up pragmas
//
//    Rev 1.0   14 May 1997 15:02:32   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
#ifndef _CHelperDialog_CLASS
#define _CHelperDialog_CLASS

#include "resource.h"


typedef struct tCHILDWINDOW {
    HWND hWnd;
    BOOL bEnabled;
    } CHILDWINDOW;


/////////////////////////////////////////////////////////////////////////////
// CHelperPage dialog

class CHelperPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CHelperPage)

// Construction
public:
    DWORD m_dwHelpButtonHelpID;
    BOOL OnInitDialog();

    //virtual BOOL HelpTopicExists(int ID);
    CHelperPage(UINT nIDTemplate);
    CHelperPage();
    ~CHelperPage();
    void Initialize();


// Dialog Data
    //{{AFX_DATA(CHelperPage)
        // NOTE - ClassWizard will add data members here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CHelperPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CHelperPage)
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()

protected:
    //CWnd*   m_pCurrentContextWnd;
    //DWORD * m_pHelpHowToMap;
    //DWORD * m_pHelpWhatsThisMap;
    //UINT    m_uiMenuIndex;
};


#endif
