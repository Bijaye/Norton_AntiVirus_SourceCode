// ModuleDialog.cpp : implementation file
//

#include "stdafx.h"
#include "avismonitor.h"
#include "ModuleDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModuleDialog dialog


CModuleDialog::CModuleDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CModuleDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModuleDialog)
	m_ModuleFilePath = _T("");
	m_ModuleName = _T("");
	m_ModuleArguments = _T("");
	m_AutoRestart = FALSE;
	m_AutoStopTime = 0;
	m_RestartTime = 0;
	m_SendMail = TRUE;
	//}}AFX_DATA_INIT
}


void CModuleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModuleDialog)
	DDX_Control(pDX, IDC_EDIT_MODULE_ARGUMENTS, m_ModuleArgumentsCtrl);
	DDX_Control(pDX, IDC_EDIT_MODULE_NAME, m_ModuleNameCtrl);
	DDX_Text(pDX, IDC_EDIT_MODULE_FILE_PATH, m_ModuleFilePath);
	DDX_Text(pDX, IDC_EDIT_MODULE_NAME, m_ModuleName);
	DDX_Text(pDX, IDC_EDIT_MODULE_ARGUMENTS, m_ModuleArguments);
	DDX_Check(pDX, IDC_CHECK_RESTART_STOPPED_MODULE, m_AutoRestart);
	DDX_Text(pDX, IDC_EDIT_AUTO_STOP_TIME, m_AutoStopTime);
	DDV_MinMaxUInt(pDX, m_AutoStopTime, 0, 40320);
	DDX_Text(pDX, IDC_EDIT_RESTART_TIME, m_RestartTime);
	DDV_MinMaxUInt(pDX, m_RestartTime, 0, 1440);
	DDX_Check(pDX, IDC_CHECK_SEND_MAIL, m_SendMail);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModuleDialog, CDialog)
	//{{AFX_MSG_MAP(CModuleDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModuleDialog message handlers

void CModuleDialog::OnOK() 
{
    // Remove leading and trailing spaces from string values
    m_ModuleArguments.TrimLeft();
    m_ModuleArguments.TrimRight();
    m_ModuleFilePath.TrimLeft();
    m_ModuleFilePath.TrimRight();
    m_ModuleName.TrimLeft();
    m_ModuleName.TrimRight();
	CDialog::OnOK();
}
