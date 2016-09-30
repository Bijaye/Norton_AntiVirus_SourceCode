// NoCtxtPS.cpp : implementation file
//


#include "stdafx.h"
#include "alertui.h"
#include "NoCtxtPS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNoContextPropertySheet

IMPLEMENT_DYNAMIC(CNoContextPropertySheet, CPropertySheet)

CNoContextPropertySheet::CNoContextPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CNoContextPropertySheet::CNoContextPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CNoContextPropertySheet::~CNoContextPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CNoContextPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CNoContextPropertySheet)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNoContextPropertySheet message handlers


// ==== OnContextMenu =====================================================
//
//  This purpose of this function is to "swallow" the context menu events
//  so that the user cannot get a context menu from the property sheet's
//  "OK", "Cancel" or "Help" buttons.  The reason for this is so that
//  derived property sheets are consistent with the rest of NAV, which has
//  no context menus on these buttons in its property sheets.  Hopefully
//  this will no longer be necessary once the "Options" property sheet is
//  converted from the Norton Control Panel to a true MFC property sheet.
//  When that day comes, rejoice and nuke this class.
//
//  Input:  the default MFC arguments
//  Output: nothing
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

void    CNoContextPropertySheet::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	return;

}  // end of "CNoContextPropertySheet::OnContextMenu"



// ==== OnInitDialog ======================================================
//
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CNoContextPropertySheet::OnInitDialog(void)
{
	// TODO: Add your specialized code here and/or call the base class

    // clear the "help" flag
    m_psh.dwFlags &= ~PSH_HASHELP;
	return CPropertySheet::OnInitDialog();

}  // end of "CNoContextPropertySheet::OnInitDialog"