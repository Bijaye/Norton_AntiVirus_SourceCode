//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// ItemPropSheet.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/itempropsheet.h_v  $
// 
//    Rev 1.2   20 May 1998 17:13:40   DBuches
// 1st pass at help.
// 
//    Rev 1.1   06 Mar 1998 11:31:08   DBuches
// Added DisplayProperties.
// 
//    Rev 1.0   27 Feb 1998 15:10:24   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMPROPSHEET_H__3E560265_A6FB_11D1_A521_0000C06F46D0__INCLUDED_)
#define AFX_ITEMPROPSHEET_H__3E560265_A6FB_11D1_A521_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "iquaran.h"
#include "GenProp.h"
#include "viruspage.h"
#include "sepage.h"
#include "RemediationPage.h"
#include "RiskMatrixPage.h"


/////////////////////////////////////////////////////////////////////////////
// CItemPropSheet

class CItemPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CItemPropSheet)

// Construction
public:
	CItemPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CItemPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CItemPropSheet();

// Attributes
public:
    CGenProp    m_GenPage;
    CVirusPage  m_VirusPage;
	CRiskMatrixPage m_RiskMatrixPage;
    CSEPage     m_SEPage;
    CRemediationPage m_RemediationPage;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemPropSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	int DisplayProperties(IQuarantineItem* pItem, CImageList* pImageList, int iImage);

	static BOOL ActivateRiskMatrixPage(void* pvCookie);
	
// Generated message map functions
protected:
	//{{AFX_MSG(CItemPropSheet)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITEMPROPSHEET_H__3E560265_A6FB_11D1_A521_0000C06F46D0__INCLUDED_)
