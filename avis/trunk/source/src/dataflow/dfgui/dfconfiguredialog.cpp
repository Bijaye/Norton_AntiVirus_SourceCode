// DFConfigureDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DFGui.h"
#include "DFConfigureDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DFConfigureDialog dialog


DFConfigureDialog::DFConfigureDialog(CWnd* pParent /*=NULL*/)
	: CDialog(DFConfigureDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(DFConfigureDialog)
	m_strMaxNumber = _T("");
	m_strSubmissionInterval = _T("");
	m_strUNCPath = _T("");
	m_strArrivalInterval = _T("");
	m_strDefBaseDir = _T("");
	m_strBuildDefFilename = _T("");
	m_strDatabaseRescanInterval = _T("");
	m_strStatusUpdateInterval = _T("");
	m_strDefImportInterval = _T("");
	m_strDefImporterDir = _T("");
	m_strUndeferrerInterval = _T("");
	m_strAttributeInterval = _T("");
	//}}AFX_DATA_INIT
}


void DFConfigureDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DFConfigureDialog)
	DDX_Text(pDX, IDC_MAX_SAMPLES, m_strMaxNumber);
	DDV_MaxChars(pDX, m_strMaxNumber, 4);
	DDX_Text(pDX, IDC_SUBMISSION_INTERVAL, m_strSubmissionInterval);
	DDV_MaxChars(pDX, m_strSubmissionInterval, 2);
	DDX_Text(pDX, IDC_UNC, m_strUNCPath);
	DDV_MaxChars(pDX, m_strUNCPath, 1024);
	DDX_Text(pDX, IDC_ARRIVAL_INTERVAL, m_strArrivalInterval);
	DDV_MaxChars(pDX, m_strArrivalInterval, 2);
	DDX_Text(pDX, IDC_DEF_BASE_DIR, m_strDefBaseDir);
	DDV_MaxChars(pDX, m_strDefBaseDir, 1024);
	DDX_Text(pDX, IDC_BUILD_DEF_FILENAME, m_strBuildDefFilename);
	DDV_MaxChars(pDX, m_strBuildDefFilename, 1024);
	DDX_Text(pDX, IDC_DATABASE_RESCAN_INTERVAL, m_strDatabaseRescanInterval);
	DDV_MaxChars(pDX, m_strDatabaseRescanInterval, 2);
	DDX_Text(pDX, IDC_STATUS_UPDATE_INTERVAL, m_strStatusUpdateInterval);
	DDV_MaxChars(pDX, m_strStatusUpdateInterval, 2);
	DDX_Text(pDX, IDC_DEF_IMPORT_INTERVAL, m_strDefImportInterval);
	DDX_Text(pDX, IDC_DEF_IMPORTER_DIR, m_strDefImporterDir);
	DDX_Text(pDX, IDC_UNDEFERRER_INTERVAL, m_strUndeferrerInterval);
	DDV_MaxChars(pDX, m_strUndeferrerInterval, 2);
	DDX_Text(pDX, IDC_ATTRIBUTE_INTERVAL, m_strAttributeInterval);
	DDV_MaxChars(pDX, m_strAttributeInterval, 2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DFConfigureDialog, CDialog)
	//{{AFX_MSG_MAP(DFConfigureDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DFConfigureDialog message handlers
