// HeurSetting.cpp : implementation file
//

#include "stdafx.h"
#include "nvlb.h"
#include "HeurSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHeurSetting dialog


CHeurSetting::CHeurSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CHeurSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHeurSetting)
	m_HeurValue = 0;
	//}}AFX_DATA_INIT
}


void CHeurSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHeurSetting)
	DDX_Control(pDX, IDC_HEUR_LEVEL_STATUS, m_HeurStatus);
	DDX_Control(pDX, IDC_HEUR_LEVEL, m_HeurLevel);
	DDX_Slider(pDX, IDC_HEUR_LEVEL, m_HeurValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHeurSetting, CDialog)
	//{{AFX_MSG_MAP(CHeurSetting)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_HEUR_LEVEL, OnCustomdrawHeurLevel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHeurSetting message handlers

BOOL CHeurSetting::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CSliderCtrl* pHeurLevel = (CSliderCtrl*) GetDlgItem(IDC_HEUR_LEVEL);
	

	//Set Range for heuristics 0-3
	pHeurLevel->SetRange(0,3,TRUE);
	
	// Set the tic marks
	pHeurLevel->SetTic(1);
	pHeurLevel->SetTic(2);
	pHeurLevel->SetTic(3);

	// Set initial position
	pHeurLevel->SetPos(0);

	// Display default heuristics level
	pHeurLevel->SetWindowText("Heuristics Off");



	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CHeurSetting::OnCustomdrawHeurLevel(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	switch(m_HeurLevel.GetPos())
	{

		case HEUR_LEVEL_OFF:
			m_HeurStatus.SetWindowText("Heuristics Off");
			break;

		case HEUR_LEVEL_1:
			m_HeurStatus.SetWindowText("Heuristics Level 1");
			break;
		
		case HEUR_LEVEL_2:
			m_HeurStatus.SetWindowText("Heuristics Level 2");
			break;

		case HEUR_LEVEL_3:
			m_HeurStatus.SetWindowText("Heuristics Level 3");
			break;

	}

	*pResult = 0;
}
