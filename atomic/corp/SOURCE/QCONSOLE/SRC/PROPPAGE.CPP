/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/proppage.cpv   1.0   06 Mar 1998 11:25:24   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// PropPage.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/proppage.cpv  $
// 
//    Rev 1.0   06 Mar 1998 11:25:24   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "PropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CItemPage property page

IMPLEMENT_DYNCREATE(CItemPage, CPropertyPage)

CItemPage::CItemPage()
{
}

CItemPage::CItemPage( int iID ) : CPropertyPage(iID)
{
    m_pDoc = ( CQconsoleDoc* )((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument();
    m_pItem = NULL;
}


CItemPage::~CItemPage()
{
}

void CItemPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CItemPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CItemPage, CPropertyPage)
    //{{AFX_MSG_MAP(CItemPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemPage message handlers
