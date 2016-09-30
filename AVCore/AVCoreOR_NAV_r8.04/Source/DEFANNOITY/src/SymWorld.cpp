// SymWorld.cpp : implementation file
//

#include "stdafx.h"
#include "defannty.h"
#include "SubWizard.h"
#include "SymWorld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSymWorld dialog


CSymWorld::CSymWorld(CWnd* pParent /*=NULL*/)
	: CDialog(CSymWorld::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSymWorld)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSymWorld::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSymWorld)
	DDX_Control(pDX, IDC_WORLDWIDE_TEXT, m_Worldwide);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSymWorld, CDialog)
	//{{AFX_MSG_MAP(CSymWorld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSymWorld message handlers

BOOL CSymWorld::OnInitDialog() 
{
    CWnd *pText;
    RECT rEditRect;
    int cxTabStop;
    CString sRead, sEdit;
    int iString;

    CDialog::OnInitDialog();

    if ( pText = GetDlgItem(IDC_TITLESTATIC) )
        AdjustHeaderFont ( pText, &m_Font );

    m_Worldwide.GetRect ( &rEditRect );

    cxTabStop = ( ( rEditRect.right - rEditRect.left ) * 2 ) / LOWORD ( GetDialogBaseUnits() );

    m_Worldwide.SetTabStops ( cxTabStop );

    sEdit = _T("");

    for ( iString = IDS_PHONE01; iString <= IDS_PHONE22; iString++ )
    {
        sRead.LoadString ( iString );
        sEdit += sRead;
    }

    m_Worldwide.SetWindowText ( sEdit );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
