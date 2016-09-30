// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//  File: SelectScanTypePage.h
//  Purpose: CSelectScanTypePage definition file. Implements the 
//			Selection of a scan type feature, which launches the
//			appropriate scan type page in custom, schedule and startup scans.
//
//	Revisions: 
//	Srikanth Vudathala	- Class created	- 10\15\2004
//----------------------------------------------------------------
//{{AFX_INCLUDES()
//}}AFX_INCLUDES

#if !defined (SELECT_SCAN_TYPE_INCLUDED)
#define SELECT_SCAN_TYPE_INCLUDED

#include "WizardPage.h"
#include "Resource.h"

//----------------------------------------------------------------
//
// CSelectScanTypePage class
//
//----------------------------------------------------------------
class CSelectScanTypePage : public CWizardPage
{
	DECLARE_DYNCREATE(CSelectScanTypePage)

//Construction
public:
	CSelectScanTypePage();
	~CSelectScanTypePage();

	void SizePage(int cx, int cy);

//Dialog Data
	//{{AFX_DATA(CSelectScanTypePage)
	enum { IDD = IDD_SELECT_SCAN_TYPE };
	//}}AFX_DATA

//Operations
	void SetCheck(UINT idRBtn);

//Overrides
	//ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSelectScanTypePage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

//Implementation
protected:
	CImageList m_ilScanImages;

	enum EScanImage
		// A scan's enum values corresponds to its index
		//	   in the image list baed on IDB_SCAN_IMAGES.
	{
		evCustomScanImage = 0,
		evQuickScanImage,
		evFullScanImage
	};

	void DrawScanImage(CDC *pDC, UINT nImageId, EScanImage eImage);

	// Generated message map functions
	//{{AFX_MSG(CSelectScanTypePage)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnQScanRButton();
	afx_msg void OnFScanRButton();
	afx_msg void OnCScanRButton();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
