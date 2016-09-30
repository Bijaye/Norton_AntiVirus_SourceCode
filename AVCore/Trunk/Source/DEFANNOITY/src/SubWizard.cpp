// SubWizard.cpp : implementation file
//

#include "stdafx.h"
#include "DefAnnty.h"
#include <prsht.h>
#include "SubWizard.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSubWizard

IMPLEMENT_DYNAMIC(CSubWizard, CPropertySheet)

CSubWizard::CSubWizard(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{	CString csTemp;
	m_iRemaining = 60;
	m_result = RESULT_ERROR;
	csTemp.LoadString(nIDCaption);
	SetWizardMode();
	m_psh.dwFlags &= (~PSH_HASHELP);
	panelA.SetParentInfo(this, &csTemp );
	panelBPre.SetParentInfo(this, &csTemp );
	panelB.SetParentInfo(this, &csTemp );
	panelC.SetParentInfo(this, &csTemp );
	panelD.SetParentInfo(this, &csTemp );
	panelE.SetParentInfo(this, &csTemp );
	panelF.SetParentInfo(this, &csTemp );
	panelG.SetParentInfo(this, &csTemp );
	panelH.SetParentInfo(this, &csTemp );
	panelI.SetParentInfo(this, &csTemp );
	panelJ.SetParentInfo(this, &csTemp );
	AddPage(&panelA);
	AddPage(&panelD);
	AddPage(&panelE);
	AddPage(&panelF);
    AddPage(&panelBPre);
	AddPage(&panelB);
	AddPage(&panelC);
	AddPage(&panelG);
	AddPage(&panelH);
	AddPage(&panelI);
	AddPage(&panelJ);
}

CSubWizard::CSubWizard(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{	CString csTemp;
	m_iRemaining = 60;
	m_result = RESULT_ERROR;
	csTemp = pszCaption;
	SetWizardMode();
	m_psh.dwFlags &= (~PSH_HASHELP);
	panelA.SetParentInfo(this, &csTemp );
	panelBPre.SetParentInfo(this, &csTemp );
	panelB.SetParentInfo(this, &csTemp );
	panelC.SetParentInfo(this, &csTemp );
	panelD.SetParentInfo(this, &csTemp );
	panelE.SetParentInfo(this, &csTemp );
	panelF.SetParentInfo(this, &csTemp );
	panelG.SetParentInfo(this, &csTemp );
	panelH.SetParentInfo(this, &csTemp );
	panelI.SetParentInfo(this, &csTemp );
	panelJ.SetParentInfo(this, &csTemp );
	AddPage(&panelA);
	AddPage(&panelD);
	AddPage(&panelE);
	AddPage(&panelF);
    AddPage(&panelBPre);
	AddPage(&panelB);
	AddPage(&panelC);
	AddPage(&panelG);
	AddPage(&panelH);
	AddPage(&panelI);
	AddPage(&panelJ);
}


void AdjustHeaderFont(CWnd *pHeaderWnd, CFont *pHeaderFont)
{
    CString sFontDescription;
    CString sPoint = _T("130");             // Default to 13 pt.
    CString sFace  = _T("Arial");           // Default to Arial.
    CString sStyle = _T("");                // Default to normal text.
    int     nPointSize;
    BOOL    bBoldFont;
    LOGFONT lf = {0};

    // Read the preferred font/point size/style from the string table.

    VERIFY ( sFontDescription.LoadString ( IDS_HEADER_FONT ));

    AfxExtractSubString ( sFace,  sFontDescription, 0, ',' );
    AfxExtractSubString ( sPoint, sFontDescription, 1, ',' );
    AfxExtractSubString ( sStyle, sFontDescription, 2, ',' );

    nPointSize = _ttoi ( sPoint );

    bBoldFont = ( 0 == sStyle.CompareNoCase ( _T("bold") ));

    if (pHeaderWnd)
    {
        CFont *pOldFont = pHeaderWnd->GetFont();
        pOldFont->GetLogFont ( &lf );
        lf.lfHeight = nPointSize;
        lf.lfWeight = bBoldFont ? FW_BOLD : FW_NORMAL;
        lstrcpyn ( lf.lfFaceName, sFace, LF_FACESIZE );

        pHeaderFont->CreatePointFontIndirect ( &lf );
        pHeaderWnd->SetFont ( pHeaderFont );
    }
}

void CSubWizard::SetState(SUB_STATE s)
{
	state = s;
	switch(state)
	{	case SUB_FREEWARN:
			SetActivePage(PANEL_FREEWARN);
			break;
		case SUB_FREEEXP:
			SetActivePage(PANEL_FREEEXP);
			break;
		case SUB_PAIDWARN:
			SetActivePage(PANEL_PAIDWARN);
			break;
		case SUB_PAIDEXP:
			SetActivePage(PANEL_PAIDEXP);
			break;
	}

}

BEGIN_MESSAGE_MAP(CSubWizard, CPropertySheet)
	//{{AFX_MSG_MAP(CSubWizard)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubWizard message handlers

BOOL CSubWizard::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	CenterWindow ( GetDesktopWindow() );

	return bResult;
}
