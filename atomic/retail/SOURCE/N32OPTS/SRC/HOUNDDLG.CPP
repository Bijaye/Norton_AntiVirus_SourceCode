/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/HOUNDDLG.CPv   1.3   02 Jul 1997 13:33:54   DBUCHES  $
/////////////////////////////////////////////////////////////////////////////
//
//
// houndDlg.h : Contains class implementation for CBlooghoundDlg class
//
//
//
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/HOUNDDLG.CPv  $
// 
//    Rev 1.3   02 Jul 1997 13:33:54   DBUCHES
// Now, save correct Bloodhound values
// 
//    Rev 1.2   25 Jun 1997 11:54:42   DBUCHES
// Hooked up more help topics.
// 
//    Rev 1.1   13 Jun 1997 21:50:56   JBRENNA
// Change IDC_BLOODHOUND_MINIMUM to ID*S*_BLOODHOUND_MINIMUM. It is a string
// resource ... not a control.
// 
//    Rev 1.0   13 Jun 1997 13:41:24   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "n32opts.h"
#include "houndDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD CBloodhoundDlg::m_dwWhatsThisMap[] = {
    IDC_ENABLE_CHECK,   IDH_NAVW_WHATHIS_ENABLE_BLOODHOUND_VIRUS_DETECTION_TECHNOLOGY,
    IDC_SLIDER,         IDH_NAVW_WHATHIS_SELECT_DESIRED_SENSITIVITY_LEVEL_CTRL,
    0,0
    };
DWORD CBloodhoundDlg::m_dwHowToMap[] = { 0,0 };;



/////////////////////////////////////////////////////////////////////////////
// CBloodhoundDlg dialog


CBloodhoundDlg::CBloodhoundDlg(CWnd* pParent /*=NULL*/)
	: CHelperDialog(CBloodhoundDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBloodhoundDlg)
	m_bEnabled = FALSE;
    m_iProtectionLevel = 1;
	//}}AFX_DATA_INIT
    m_pHelpHowToMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_dwHelpButtonHelpID = IDH_NAVW_HEURISTIC_SCANNING_OPTIONS_DLG_HLPBTN;

}


void CBloodhoundDlg::DoDataExchange(CDataExchange* pDX)
{
	CHelperDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBloodhoundDlg)
	DDX_Control(pDX, IDC_SLIDER, m_SliderCtrl);
	DDX_Check(pDX, IDC_ENABLE_CHECK, m_bEnabled);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBloodhoundDlg, CHelperDialog)
	//{{AFX_MSG_MAP(CBloodhoundDlg)
	ON_BN_CLICKED(IDC_ENABLE_CHECK, OnEnableCheck)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBloodhoundDlg message handlers

void CBloodhoundDlg::OnEnableCheck() 
{
    // Get check state.
    BOOL bEnabled = IsDlgButtonChecked( IDC_ENABLE_CHECK );

    // Enable windows
    GetDlgItem(IDC_DESC_STATIC)->EnableWindow(bEnabled);
    GetDlgItem(IDC_SLIDER_STATIC)->EnableWindow(bEnabled);
    m_SliderCtrl.EnableWindow(bEnabled);


}

BOOL CBloodhoundDlg::OnInitDialog() 
{
	CHelperDialog::OnInitDialog();
	
    //Set up slider control.
    m_SliderCtrl.SetRange( 0, 2 );
    m_SliderCtrl.SetTic(1);
    m_SliderCtrl.SetPos( m_iProtectionLevel - 1 );
    SetProtectionText();
    
    // Update state of controls.
    OnEnableCheck();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBloodhoundDlg::OnOK() 
{
	// Save off protection level.
    m_iProtectionLevel = m_SliderCtrl.GetPos() + 1;
	
	CHelperDialog::OnOK();
}

void CBloodhoundDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
    
    // Update protection level text control
    if( nSBCode == TB_ENDTRACK ||
        nSBCode == TB_LINEDOWN ||
        nSBCode == TB_THUMBTRACK ||
        nSBCode == TB_LINEUP )	
        {
        SetProtectionText();
        }


	CHelperDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CBloodhoundDlg::SetProtectionText()
{
    // Get correct text string
    CString sText;
    int iPos = m_SliderCtrl.GetPos();
    
    sText.LoadString( IDS_BLOODHOUND_MINIMUM + iPos );
    GetDlgItem(IDC_DESC_STATIC)->SetWindowText(sText);

}
