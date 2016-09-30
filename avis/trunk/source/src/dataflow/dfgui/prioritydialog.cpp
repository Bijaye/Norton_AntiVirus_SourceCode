// PriorityDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DFGui.h"
#include "PriorityDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPriorityDialog dialog


CPriorityDialog::CPriorityDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPriorityDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPriorityDialog)
	m_trackingNumber = _T("");
	m_priority = _T("");
	//}}AFX_DATA_INIT
}


void CPriorityDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPriorityDialog)
	DDX_Text(pDX, IDC_TRACKING_NUMBER, m_trackingNumber);
	DDV_MaxChars(pDX, m_trackingNumber, 10000);
	DDX_Text(pDX, IDC_PRIORITY, m_priority);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPriorityDialog, CDialog)
	//{{AFX_MSG_MAP(CPriorityDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPriorityDialog message handlers
