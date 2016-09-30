//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// RiskMatrixPage.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconres.h"
#include "RiskMatrixPage.h"
#include "ccEraserInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CRiskMatrixPage property page

IMPLEMENT_DYNCREATE(CRiskMatrixPage, CItemPage)

CRiskMatrixPage::CRiskMatrixPage() : CItemPage(CRiskMatrixPage::IDD)
{
    //{{AFX_DATA_INIT(CRiskMatrixPage)
	//m_sThreatName = _T("");
	m_sLevelOverall = _T("");
	m_sLevelPerformance = _T("");
	m_sLevelPrivacy = _T("");
	m_sLevelRemoval = _T("");
	m_sLevelStealth = _T("");
	m_sLevelDetail = _T("");
	m_sDetailHeader = _T("");
	m_sDetailDescription = _T("");
    //}}AFX_DATA_INIT

	m_dwRiskOverall = -1;
	m_dwRiskPerformance = 1;
	m_dwRiskPrivacy = 1;
	m_dwRiskRemoval = 1;
	m_dwRiskStealth = 1;

	m_RiskCategoryEnum_Overall = RiskCategory_Overall;
	m_RiskCategoryEnum_Performance = RiskCategory_Performance;
	m_RiskCategoryEnum_Privacy = RiskCategory_Privacy;
	m_RiskCategoryEnum_Removal = RiskCategory_Removal;
	m_RiskCategoryEnum_Stealth = RiskCategory_Stealth;
}


CRiskMatrixPage::~CRiskMatrixPage()
{
}


void CRiskMatrixPage::DoDataExchange(CDataExchange* pDX)
{
    CItemPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRiskMatrixPage)
//	DDX_Text(pDX, IDC_RISK_MATRIX_THREAT_NAME, m_sThreatName);
	DDX_Control(pDX, IDC_RISK_LINK_OVERALL, m_linkOverall);
	DDX_Control(pDX, IDC_RISK_LINK_PERFORMANCE, m_linkPerformance);
	DDX_Control(pDX, IDC_RISK_LINK_PRIVACY, m_linkPrivacy);
	DDX_Control(pDX, IDC_RISK_LINK_REMOVAL, m_linkRemoval);
	DDX_Control(pDX, IDC_RISK_LINK_STEALTH, m_linkStealth);
	DDX_Control(pDX, IDC_RISK_MATRIX_PICTURE_OVERALL, m_PictureOverall);
	DDX_Control(pDX, IDC_RISK_MATRIX_PICTURE_PERFORMANCE, m_PicturePerformance);
	DDX_Control(pDX, IDC_RISK_MATRIX_PICTURE_PRIVACY, m_PicturePrivacy);
	DDX_Control(pDX, IDC_RISK_MATRIX_PICTURE_REMOVAL, m_PictureRemoval);
	DDX_Control(pDX, IDC_RISK_MATRIX_PICTURE_STEALTH, m_PictureStealth);
	DDX_Control(pDX, IDC_RISK_MATRIX_PICTURE_DETAIL, m_PictureDetail);
	DDX_Text(pDX, IDC_RISK_MATRIX_LEVEL_OVERALL, m_sLevelOverall);
	DDX_Text(pDX, IDC_RISK_MATRIX_LEVEL_PERFORMANCE, m_sLevelPerformance);
	DDX_Text(pDX, IDC_RISK_MATRIX_LEVEL_PRIVACY, m_sLevelPrivacy);
	DDX_Text(pDX, IDC_RISK_MATRIX_LEVEL_REMOVAL, m_sLevelRemoval);
	DDX_Text(pDX, IDC_RISK_MATRIX_LEVEL_STEALTH, m_sLevelStealth);
	DDX_Text(pDX, IDC_RISK_MATRIX_LEVEL_DETAIL, m_sLevelDetail);
	DDX_Text(pDX, IDC_RISK_MATRIX_DETAIL_HEADER, m_sDetailHeader);
	DDX_Text(pDX, IDC_RISK_MATRIX_DETAIL_DESCRIPTION, m_sDetailDescription);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRiskMatrixPage, CItemPage)
    //{{AFX_MSG_MAP(CRiskMatrixPage)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRiskMatrixPage message handlers


BOOL CRiskMatrixPage::OnInitDialog() 
{
	CItemPage::OnInitDialog();

	//m_pDoc->GetThreatName(m_pItem, m_sThreatName);

	m_ImageList.Create(IDB_BITMAP_RISK_LEVEL, 29, 1, RGB(0, 0, 0));

	// Set up buttons
	m_PictureOverall.SetImageList(&m_ImageList);
	m_PicturePerformance.SetImageList(&m_ImageList);
	m_PicturePrivacy.SetImageList(&m_ImageList);
	m_PictureRemoval.SetImageList(&m_ImageList);
	m_PictureStealth.SetImageList(&m_ImageList);
	m_PictureDetail.SetImageList(&m_ImageList);

	// Set up Overall Risk, which is the current detail selection
	DWORD dwRiskLevel = m_pDoc->GetRiskLevelAverage(m_pItem);
	m_dwRiskOverall = dwRiskLevel;

	UpdateRiskCategorySelected(RiskCategory_Overall);
	bool bGotText = m_pDoc->GetRiskLevelText(dwRiskLevel, m_sLevelOverall);
	m_sLevelDetail = m_sLevelOverall;

	CString sLink;
	m_linkOverall.GetWindowText(sLink);

	RiskCategory* pRiskCategory = &m_RiskCategoryEnum_Overall;
	m_linkOverall.SetLink(sLink, LinkCallback, static_cast<void*>(this), static_cast<void*>(pRiskCategory));

	int iIndex = GetRiskLevelBitmapIndex(dwRiskLevel);
	if(iIndex >= 0)
	{
		m_PictureOverall.SetImageIndex(iIndex);
		m_PictureDetail.SetImageIndex(iIndex);
	}

	// Performance risk
	m_linkPerformance.GetWindowText(sLink);
	pRiskCategory = &m_RiskCategoryEnum_Performance;
	m_linkPerformance.SetLink(sLink, LinkCallback, static_cast<void*>(this), static_cast<void*>(pRiskCategory));

	dwRiskLevel = m_pDoc->GetRiskLevelNumber(m_pItem, ccEraser::IAnomaly::Performance);
	m_dwRiskPerformance = dwRiskLevel;
	bGotText = m_pDoc->GetRiskLevelText(dwRiskLevel, m_sLevelPerformance);
	iIndex = GetRiskLevelBitmapIndex(dwRiskLevel);
	if(iIndex >= 0)
		m_PicturePerformance.SetImageIndex(iIndex);

	// Privacy risk
	m_linkPrivacy.GetWindowText(sLink);
	pRiskCategory = &m_RiskCategoryEnum_Privacy;
	m_linkPrivacy.SetLink(sLink, LinkCallback, static_cast<void*>(this), static_cast<void*>(pRiskCategory));

	dwRiskLevel = m_pDoc->GetRiskLevelNumber(m_pItem, ccEraser::IAnomaly::Privacy);
	m_dwRiskPrivacy = dwRiskLevel;
	bGotText = m_pDoc->GetRiskLevelText(dwRiskLevel, m_sLevelPrivacy);
	iIndex = GetRiskLevelBitmapIndex(dwRiskLevel);
	if(iIndex >= 0)
		m_PicturePrivacy.SetImageIndex(iIndex);

	// Removal risk
	m_linkRemoval.GetWindowText(sLink);
	pRiskCategory = &m_RiskCategoryEnum_Removal;
	m_linkRemoval.SetLink(sLink, LinkCallback, static_cast<void*>(this), static_cast<void*>(pRiskCategory));

	dwRiskLevel = m_pDoc->GetRiskLevelNumber(m_pItem, ccEraser::IAnomaly::Removal);
	m_dwRiskRemoval = dwRiskLevel;
	bGotText = m_pDoc->GetRiskLevelText(dwRiskLevel, m_sLevelRemoval);
	iIndex = GetRiskLevelBitmapIndex(dwRiskLevel);
	if(iIndex >= 0)
		m_PictureRemoval.SetImageIndex(iIndex);

	// Stealth risk
	m_linkStealth.GetWindowText(sLink);
	pRiskCategory = &m_RiskCategoryEnum_Stealth;
	m_linkStealth.SetLink(sLink, LinkCallback, static_cast<void*>(this), static_cast<void*>(pRiskCategory));

	dwRiskLevel = m_pDoc->GetRiskLevelNumber(m_pItem, ccEraser::IAnomaly::Stealth);
	m_dwRiskStealth = dwRiskLevel;
	bGotText = m_pDoc->GetRiskLevelText(dwRiskLevel, m_sLevelStealth);
	iIndex = GetRiskLevelBitmapIndex(dwRiskLevel);
	if(iIndex >= 0)
		m_PictureStealth.SetImageIndex(iIndex);

	// Blast data to controls
	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


int CRiskMatrixPage::GetRiskLevelBitmapIndex(DWORD dwLevel)
{
	int iIndex = -1;

	RiskLevel rLevel = m_pDoc->GetRiskLevel(dwLevel);

	switch(rLevel)
	{
		case RiskLevel_Low:
			iIndex = 0;
			break;
		case RiskLevel_Medium:
			iIndex = 1;
			break;
		case RiskLevel_High:
			iIndex = 2;
			break;
	}

	return iIndex;
}


bool CRiskMatrixPage::UpdateRiskCategorySelected(RiskCategory riskCategory)
{
	bool bSuccess = false;
	int iIndex = -1;
	DWORD dwRiskLevel = m_dwRiskOverall;

	switch(riskCategory)
	{
	case RiskCategory_Overall:
		dwRiskLevel = m_dwRiskOverall;
		break;
	case RiskCategory_Performance:
		dwRiskLevel = m_dwRiskPerformance;
		break;
	case RiskCategory_Privacy:
		dwRiskLevel = m_dwRiskPrivacy;
		break;
	case RiskCategory_Removal:
		dwRiskLevel = m_dwRiskRemoval;
		break;
	case RiskCategory_Stealth:
		dwRiskLevel = m_dwRiskStealth;
		break;
	}

	bSuccess = m_pDoc->GetRiskLevelCategoryText(riskCategory, dwRiskLevel, m_sDetailHeader, m_sDetailDescription);

	bSuccess = m_pDoc->GetRiskLevelText(dwRiskLevel, m_sLevelDetail);

	iIndex = GetRiskLevelBitmapIndex(dwRiskLevel);

	if(iIndex >= 0)
		m_PictureDetail.SetImageIndex(iIndex);	

	return bSuccess;
}


BOOL CRiskMatrixPage::LinkCallback(void* pData, void* pParam)
{
	CRiskMatrixPage* pRiskPage = static_cast<CRiskMatrixPage*>(pData);
	RiskCategory* riskCategory = static_cast<RiskCategory*>(pParam);
	pRiskPage->UpdateRiskCategorySelected(*riskCategory);

	pRiskPage->UpdateData(FALSE);
	return TRUE;
}
