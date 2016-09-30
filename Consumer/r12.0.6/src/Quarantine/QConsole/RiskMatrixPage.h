//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// RiskMatrix.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RiskMatrixPage_H__E252C0AC_1A79_4d23_90F1_DA5475C51843__INCLUDED_)
#define AFX_RiskMatrixPage_H__E252C0AC_1A79_4d23_90F1_DA5475C51843__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "proppage.h"
#include "imagebutton.h"
#include "PseudoHyperlink.h"


class CRiskMatrixPage :	public CItemPage
{
	DECLARE_DYNCREATE(CRiskMatrixPage)

public:
	CRiskMatrixPage();
	~CRiskMatrixPage();

	// Dialog Data
	//{{AFX_DATA(CRiskMatrixPage)
	enum {IDD = IDD_RISK_MATRIX_PAGE};
	//CString	m_sThreatName;
	CPseudoHyperlink m_linkOverall;
	CPseudoHyperlink m_linkPerformance;
	CPseudoHyperlink m_linkPrivacy;
	CPseudoHyperlink m_linkRemoval;
	CPseudoHyperlink m_linkStealth;
	CImageButton m_PictureOverall;
	CImageButton m_PicturePerformance;
	CImageButton m_PicturePrivacy;
	CImageButton m_PictureRemoval;
	CImageButton m_PictureStealth;
	CImageButton m_PictureDetail;
    CString m_sLevelOverall;
	CString m_sLevelPerformance;
	CString m_sLevelPrivacy;
	CString m_sLevelRemoval;
	CString m_sLevelStealth;
	CString m_sLevelDetail;
	CString	m_sDetailHeader;
	CString	m_sDetailDescription;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRemediationPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRemediationPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	int GetRiskLevelBitmapIndex(DWORD dwLevel);
	bool UpdateRiskCategorySelected(RiskCategory riskCategory);

	static BOOL LinkCallback(void* pData, void* pParam);

private:
	CImageList m_ImageList;

	DWORD m_dwRiskOverall;
	DWORD m_dwRiskPerformance;
	DWORD m_dwRiskPrivacy;
	DWORD m_dwRiskRemoval;
	DWORD m_dwRiskStealth;

	RiskCategory m_RiskCategoryEnum_Overall;
	RiskCategory m_RiskCategoryEnum_Performance;
	RiskCategory m_RiskCategoryEnum_Privacy;
	RiskCategory m_RiskCategoryEnum_Removal;
	RiskCategory m_RiskCategoryEnum_Stealth;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_RiskMatrixPage_H__E252C0AC_1A79_4d23_90F1_DA5475C51843__INCLUDED_)