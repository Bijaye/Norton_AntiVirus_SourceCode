// ActivityLogSetting.cpp : implementation file
//

#include "stdafx.h"
#include "AVISDFRL.h"
#include "ActivityLogSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActivityLogSetting dialog

// Class for Activity Log Settings dialog. The information is collected from
// the user. The actual processing of the information is done in the
// DFLauncherView class. Initial values for the fields are also set in the
// DFLauncherView class

CActivityLogSetting::CActivityLogSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CActivityLogSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CActivityLogSetting)
	m_LogBufferSize = 0;
	m_LogFileName = _T("");
	m_MaxLogFileSize = 0;
	//}}AFX_DATA_INIT
}


void CActivityLogSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CActivityLogSetting)
	DDX_Text(pDX, IDC_EDIT_LOG_BUFFER_SIZE, m_LogBufferSize);
	DDV_MinMaxUInt(pDX, m_LogBufferSize, 0, 32);
	DDX_Text(pDX, IDC_EDIT_LOG_FILE_NAME, m_LogFileName);
	DDX_Text(pDX, IDC_EDIT_MAX_LOG_SIZE, m_MaxLogFileSize);
	DDV_MinMaxUInt(pDX, m_MaxLogFileSize, 0, 10240);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CActivityLogSetting, CDialog)
	//{{AFX_MSG_MAP(CActivityLogSetting)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CActivityLogSetting message handlers
