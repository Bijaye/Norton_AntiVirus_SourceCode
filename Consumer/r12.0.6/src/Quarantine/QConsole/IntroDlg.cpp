//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// IntroDlg.cpp : implementation file of the CIntroDlg class.  Used to present
//	description of types of items in Quarantine (post Eraser integration).
//  User can choose not to see intro dlg again.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconres.h"
#include "IntroDlg.h"
#include "AVRESBranding.h"


// CIntroDlg dialog

IMPLEMENT_DYNAMIC(CIntroDlg, CDialog)
CIntroDlg::CIntroDlg(CWnd* pParent /*=NULL*/)
: CDialog(CIntroDlg::IDD, pParent)
, m_sIntroHeading(_T(""))
, m_sExpandedThreatHeading(_T(""))
, m_sExpandedThreatDescription(_T(""))
, m_sViralHeading(_T(""))
, m_sViralDescription(_T(""))
, m_bIntroCheck(FALSE)
{
	m_pWhiteBkBrush = new CBrush(RGB(255, 255, 255));
}

CIntroDlg::~CIntroDlg()
{
	delete m_pWhiteBkBrush;
}

void CIntroDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INTRODLG_INTRO, m_sIntroHeading);
	DDX_Text(pDX, IDC_INTRODLG_EXPANDED_THREAT_HEADING, m_sExpandedThreatHeading);
	DDX_Text(pDX, IDC_INTRODLG_EXPANDED_THREAT_DESCRIPTION, m_sExpandedThreatDescription);
	DDX_Text(pDX, IDC_INTRODLG_VIRAL_HEADING, m_sViralHeading);
	DDX_Text(pDX, IDC_INTRODLG_VIRAL_DESCRIPTION, m_sViralDescription);
	DDX_Check(pDX, IDC_INTRODLG_CHECK, m_bIntroCheck);
}


BEGIN_MESSAGE_MAP(CIntroDlg, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CIntroDlg message handlers

BOOL CIntroDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CBrandingRes BrandRes;
	m_sAppName = BrandRes.GetString(IDS_QUARANTINE_APP_NAME);

	m_sIntroHeading.Format(IDS_INTRODLG_INTRO, m_sAppName);
	m_sExpandedThreatHeading.LoadString(IDS_INTRODLG_EXPANDED_THREAT_HEADING);
	m_sExpandedThreatDescription.LoadString(IDS_INTRODLG_EXPANDED_THREAT_DESCRIPTION);
	m_sViralHeading.LoadString(IDS_INTRODLG_VIRAL_HEADING);
	m_sViralDescription.LoadString(IDS_INTRODLG_VIRAL_DESCRIPTION);

	CString sCheckboxText;
	sCheckboxText.LoadString(IDS_INTRODLG_CHECK);
	SetDlgItemText(IDC_INTRODLG_CHECK, sCheckboxText);

	CWnd* pWndCtrl = GetDlgItem(IDC_INTRODLG_EXPANDED_THREAT_HEADING);
	SetBoldfaceFont(pWndCtrl->m_hWnd);
	pWndCtrl = GetDlgItem(IDC_INTRODLG_VIRAL_HEADING);
	SetBoldfaceFont(pWndCtrl->m_hWnd);

	if(IsHighContrastMode())
	{
		GetDlgItem(IDC_INTRODLG_GRAPHIC)->ShowWindow(FALSE);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CIntroDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch(nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
	case CTLCOLOR_BTN:
		if(!IsHighContrastMode())
		{
			pDC->SetBkColor(RGB(255, 255, 255));
		}

		pDC->SetBkMode(TRANSPARENT);
		break;
	}

	if(!IsHighContrastMode())
	{
		hbr = (HBRUSH)(m_pWhiteBkBrush->GetSafeHandle());
	}

	return hbr;
}

bool CIntroDlg::GetIntroEnabled()
{
	// If it isn't checked, the intro is still enabled
	return ((FALSE == m_bIntroCheck) ? true : false);
}


bool CIntroDlg::SetBoldfaceFont(HWND hWnd)
{
	HFONT hFont = (HFONT)::SendMessage(::GetParent(hWnd), WM_GETFONT, 0, 0);
	if(NULL == hFont)
		return false;

	LOGFONT LogFont;
	if(0 == GetObject(hFont, sizeof(LOGFONT), &LogFont))
		return false;

	if(0 == GetSystemMetrics(SM_DBCSENABLED))
		LogFont.lfWeight = FW_BOLD;

	HFONT hBoldFont = CreateFontIndirect(&LogFont);
	if(NULL == hBoldFont)
		return false;

	::SendMessage(hWnd, WM_SETFONT, (WPARAM)hBoldFont, MAKELPARAM(TRUE, 0));

	return true;
}


bool CIntroDlg::IsHighContrastMode()
{
	HIGHCONTRAST highContrast;
	ZeroMemory(&highContrast, sizeof(HIGHCONTRAST));
	highContrast.cbSize = sizeof(HIGHCONTRAST);

	SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &highContrast, 0);

	if(highContrast.dwFlags & HCF_HIGHCONTRASTON)
	{
		CCTRACEI(_T("%s - High Contrast mode ON"), __FUNCTION__);
		return true;
	}
	else
	{
		return false;
	}
}
