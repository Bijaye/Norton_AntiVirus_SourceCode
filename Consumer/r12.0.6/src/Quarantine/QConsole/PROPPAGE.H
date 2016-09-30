//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// PropPage.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/proppage.h_v  $
// 
//    Rev 1.0   06 Mar 1998 11:25:24   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROPPAGE_H__61B899C4_AEFA_11D1_A53D_0000C06F46D0__INCLUDED_)
#define AFX_PROPPAGE_H__61B899C4_AEFA_11D1_A53D_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "iquaran.h"
#include "qconsoledoc.h"


/////////////////////////////////////////////////////////////////////////////
// CItemPage dialog

class CItemPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CItemPage)

// Construction
public:
    CItemPage();
    CItemPage( int iID );
    ~CItemPage();

// Dialog Data
    //{{AFX_DATA(CItemPage)
	//}}AFX_DATA
    void SetItemPointer( IQuarantineItem* p ) { m_pItem = p; }
    void SetDocument( CQconsoleDoc* pDoc ) { m_pDoc = pDoc; }

// Overrides
	// ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CItemPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
    //{{AFX_MSG(CItemPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
    // Pointer to object we are displaying.
    IQuarantineItem*    m_pItem;
    CQconsoleDoc*       m_pDoc;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERPROP_H__61B899C4_AEFA_11D1_A53D_0000C06F46D0__INCLUDED_)
