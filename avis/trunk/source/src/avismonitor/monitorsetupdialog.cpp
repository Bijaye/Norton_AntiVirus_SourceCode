// MonitorSetupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "avismonitor.h"
#include "MonitorSetupDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMonitorSetupDialog dialog


CMonitorSetupDialog::CMonitorSetupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMonitorSetupDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMonitorSetupDialog)
	m_autoRestart = FALSE;
	m_StatusCheckInterval = 0;
	m_AutoTerminateErrorCount = 0;
	m_startOnMonitorStartup = FALSE;
	//}}AFX_DATA_INIT
}


void CMonitorSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonitorSetupDialog)
	DDX_Check(pDX, IDC_CHECK_AUTO_RESTART, m_autoRestart);
	DDX_Text(pDX, IDC_EDIT_STATUS_CHECK_INTERVAL, m_StatusCheckInterval);
	DDV_MinMaxUInt(pDX, m_StatusCheckInterval, 1, 30);
	DDX_Text(pDX, IDC_EDITAUTO_RESTART_ERROR_COUNT, m_AutoTerminateErrorCount);
	DDV_MinMaxUInt(pDX, m_AutoTerminateErrorCount, 1, 32000);
	DDX_Check(pDX, IDC_CHECK_START_ON_MONITOR_STARTUP, m_startOnMonitorStartup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMonitorSetupDialog, CDialog)
	//{{AFX_MSG_MAP(CMonitorSetupDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonitorSetupDialog message handlers
