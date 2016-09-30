// DFStatisticsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DFGui.h"
#include "DFStatisticsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDFStatisticsDialog dialog


CDFStatisticsDialog::CDFStatisticsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDFStatisticsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDFStatisticsDialog)
	m_strAverageProcessingTime = _T("");
	m_strNumberOfSamples = _T("");
	m_strNumberOfSuccessfulSamples = _T("");
	m_strStartTime = _T("");
	m_strDeferredSamples = _T("");
	m_strRescannedSamples = _T("");
	//}}AFX_DATA_INIT
}


void CDFStatisticsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDFStatisticsDialog)
	DDX_Text(pDX, IDC_AVERAGE_TIME, m_strAverageProcessingTime);
	DDX_Text(pDX, IDC_NUMBER_OF_SAMPLES, m_strNumberOfSamples);
	DDX_Text(pDX, IDC_NUMBER_OF_SUCCESS_SAMPLES, m_strNumberOfSuccessfulSamples);
	DDX_Text(pDX, IDC_START_TIME, m_strStartTime);
	DDX_Text(pDX, IDC_NUMBER_OF_DEFERRED_SAMPLES, m_strDeferredSamples);
	DDX_Text(pDX, IDC_NUMBER_OF_RESCANNED_SAMPLES, m_strRescannedSamples);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDFStatisticsDialog, CDialog)
	//{{AFX_MSG_MAP(CDFStatisticsDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFStatisticsDialog message handlers
