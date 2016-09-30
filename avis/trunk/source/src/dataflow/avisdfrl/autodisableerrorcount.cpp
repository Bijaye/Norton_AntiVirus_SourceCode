// AutoDisableErrorCount.cpp : implementation file
//

#include "stdafx.h"
#include "AVISDFRL.h"
#include "AutoDisableErrorCount.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoDisableErrorCount dialog

// Class for Auto disable error count dialog. The information is collected from
// the user. The actual processing of the information is done in the
// DFLauncherView class. Initial values for the fields are also set in the
// DFLauncherView class

CAutoDisableErrorCount::CAutoDisableErrorCount(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoDisableErrorCount::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoDisableErrorCount)
	m_ErrorCount = 0;
	//}}AFX_DATA_INIT
}


void CAutoDisableErrorCount::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoDisableErrorCount)
	DDX_Text(pDX, IDC_EDIT_ERROR_COUNT, m_ErrorCount);
	DDV_MinMaxInt(pDX, m_ErrorCount, 0, 10000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoDisableErrorCount, CDialog)
	//{{AFX_MSG_MAP(CAutoDisableErrorCount)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoDisableErrorCount message handlers
