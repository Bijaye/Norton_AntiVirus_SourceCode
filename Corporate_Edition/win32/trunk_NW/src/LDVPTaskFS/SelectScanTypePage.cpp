// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//  File: SelectScanTypePage.cpp
//  Purpose: CSelectScanTypePage Implementation file. Implements the 
//			Selection of a scan type feature, which launches the
//			appropriate scan type page in custom, schedule and startup scans.
//
//	Revisions: 
//	Srikanth Vudathala	- Class created	- 10\15\2004
//----------------------------------------------------------------

#include "stdafx.h"
#include "LDVPTaskFS.h"
#include "SelectScanTypePage.h"
#include "Wizard.H"

#include "ScanOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constants
#define SCAN_IMAGE_SIZE  16
#define SCAN_IMAGE_MASK  RGB(0, 128, 128)

/////////////////////////////////////////////////////////////////////////////
// CSelectScanTypePage property page

IMPLEMENT_DYNCREATE(CSelectScanTypePage, CWizardPage)


BEGIN_EVENTSINK_MAP(CSelectScanTypePage, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(CSelectScanTypePage)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CSelectScanTypePage::CSelectScanTypePage() : CWizardPage(CSelectScanTypePage::IDD)
{
	//{{AFX_DATA_INIT(CSelectScanTypePage)
		//NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_psp.dwFlags |= PSP_HASHELP;
	//Set the Help Context ID
	SetHelpID(IDD);
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CSelectScanTypePage::~CSelectScanTypePage()
{
}

//----------------------------------------------------------------
// DoDataExchange
//----------------------------------------------------------------
void CSelectScanTypePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectScanTypePage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectScanTypePage, CPropertyPage)
	//{{AFX_MSG_MAP(CSelectScanTypePage)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDC_QSCAN_RADIO, OnQScanRButton)
	ON_BN_CLICKED(IDC_FSCAN_RADIO, OnFScanRButton)
	ON_BN_CLICKED(IDC_CSCAN_RADIO, OnCScanRButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------
// OnPaint
//----------------------------------------------------------------
BOOL CSelectScanTypePage::OnSetActive() 
{
	CClientPropSheet *ptrWizParent = (CClientPropSheet*)GetParent();

	CWizardPage::OnSetActive();
	
	if( InWizardMode() )
	{
		CString sTitle;
		sTitle.LoadString(IDS_SCAN);
		ptrWizParent->m_strTitle = sTitle;
		ptrWizParent->HideBack();
		ptrWizParent->SetWizardButtons( PSWIZB_NEXT );
	}

	return TRUE;
}

//----------------------------------------------------------------
// OnWizardNext
//----------------------------------------------------------------
LRESULT CSelectScanTypePage::OnWizardNext() 
{
	CWizard *ptrWizParent = (CWizard*)GetParent();

	//Fire my event telling the container that we are recording a task
	ptrWizParent->m_ptrCtrl->BeginRecordingSession();

	return CPropertyPage::OnWizardNext();
}

//----------------------------------------------------------------
// SizePage
//----------------------------------------------------------------
void CSelectScanTypePage::SizePage(int cx, int cy)
{
	//First, call tha base class version of this virtual method
	CWizardPage::SizePage(cx, cy);

	CRect		staticRect,	dlgRect;

	//Figure out the current client rect
	GetClientRect(&dlgRect);

	//Size all the static description text
	CWnd *ptrWnd;
	ptrWnd = GetDlgItem(IDC_QSCAN_STATIC);
	if(ptrWnd)
	{
		ptrWnd->GetWindowRect(&staticRect);
		ScreenToClient(&staticRect);
		ptrWnd->MoveWindow( staticRect.left, staticRect.top, dlgRect.right - staticRect.left - 12, staticRect.bottom - staticRect.top );
	}

	ptrWnd = GetDlgItem(IDC_FSCAN_STATIC);
	if(ptrWnd)
	{
		ptrWnd->GetWindowRect(&staticRect);
		ScreenToClient(&staticRect);
		ptrWnd->MoveWindow( staticRect.left, staticRect.top, dlgRect.right - staticRect.left - 12, staticRect.bottom - staticRect.top );
	}

	ptrWnd = GetDlgItem(IDC_CSCAN_STATIC);
	if(ptrWnd)
	{
		ptrWnd->GetWindowRect(&staticRect);
		ScreenToClient(&staticRect);
		ptrWnd->MoveWindow( staticRect.left, staticRect.top, dlgRect.right - staticRect.left - 12, staticRect.bottom - staticRect.top );
	}
}

//----------------------------------------------------------------
// OnPaint
//----------------------------------------------------------------
void CSelectScanTypePage::OnPaint() 
{
	CPaintDC dc(this); 
	
	if( InWizardMode() )
		PaintTitle(&dc);

	// Draw the scan images.
	// We draw these images from the image list (in DrawScanImage())
	//	   because this is the only way to draw transparent bitmaps.
	// (I couldn't get the picture control to draw 16x16 images.)
	// We create invisible frames in the dialog editor because the alternative is
	//	   hard-coding image positions here.
	DrawScanImage(&dc, IDC_QSCAN_IMAGE_FRAME, evQuickScanImage);
	DrawScanImage(&dc, IDC_CSCAN_IMAGE_FRAME, evCustomScanImage);
	DrawScanImage(&dc, IDC_FSCAN_IMAGE_FRAME, evFullScanImage);
}


//----------------------------------------------------------------
// DrawScanImage() draws a bitmap in the location of a specified frame.
// pDC      -- the context for drawing the bitmap
// nFrameId -- the frame to draw over
// eImage   -- the scan image to draw
//----------------------------------------------------------------
void CSelectScanTypePage::DrawScanImage(CDC *pDC, UINT nFrameId, EScanImage eImage)
{
	// Get the location of the frame.
	CWnd  *pwndImage = GetDlgItem(nFrameId);
	CPoint ptFrameUL(0, 0);

	ASSERT(pwndImage != NULL);
	pwndImage->ClientToScreen(&ptFrameUL);
	ScreenToClient(&ptFrameUL);
	// Draw the scan image.
	VERIFY(m_ilScanImages.Draw(pDC, eImage, ptFrameUL, ILD_NORMAL));
}


//----------------------------------------------------------------
// OnInitDialog
//----------------------------------------------------------------
BOOL CSelectScanTypePage::OnInitDialog() 
{
	CClientPropSheet *ptrWizParent = (CClientPropSheet*)GetParent();
	
	CWizardPage::OnInitDialog();

	//Set the Context-sensitive ID
	SetHelpID(IDD_SELECT_SCAN_TYPE);

	SetCheck(IDC_QSCAN_RADIO);

	//Set the descriptions for all the scan types
	CString szQuickScanDescription, szFullScanDescription, szCustomScanDescription;

	szQuickScanDescription.LoadString(IDS_QUICKSCAN_DESCRIPTION_SHORT);
	((CWnd*)GetDlgItem(IDC_QSCAN_STATIC))->SetWindowText(szQuickScanDescription);

	szFullScanDescription.LoadString(IDS_FULLSCAN_DESCRIPTION_SHORT);
	((CWnd*)GetDlgItem(IDC_FSCAN_STATIC))->SetWindowText(szFullScanDescription);

	szCustomScanDescription.LoadString(IDS_SCAN_SELECTED_DESCRIPTION);
	((CWnd*)GetDlgItem(IDC_CSCAN_STATIC))->SetWindowText(szCustomScanDescription);

	// Create the scan image list.
	VERIFY(m_ilScanImages.Create(IDB_SCAN_IMAGES, SCAN_IMAGE_SIZE, 1, SCAN_IMAGE_MASK));

	return TRUE;
}

//----------------------------------------------------------------
// OnOK
//----------------------------------------------------------------
void CSelectScanTypePage::OnOK() 
{
	CPropertyPage::OnOK();
}

//----------------------------------------------------------------
// OnLButtonDown
//----------------------------------------------------------------
void CSelectScanTypePage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnLButtonDown(nFlags, point);

	SetFocus();
}

//----------------------------------------------------------------
// OnRButtonDown
//----------------------------------------------------------------
void CSelectScanTypePage::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnRButtonDown(nFlags, point);

	SetFocus();
}

//----------------------------------------------------------------
// SetCheck
//	PURPOSE		Gets the information about the particular radio button
//				clicked and sets the m_dwSelectedScanType appropriately.
//	ARGUMENTS	
//				UINT idRBtn		- The id of the particular scan type selected.
//	RETURNS
//				none
//----------------------------------------------------------------
void CSelectScanTypePage::SetCheck(UINT idToBtn)
{
	CWizard *ptrWizParent = (CWizard*)GetParent();

	((CButton*)GetDlgItem(idToBtn))->SetCheck(1);

	switch(idToBtn)
	{
	case IDC_QSCAN_RADIO:
		ptrWizParent->m_dwSelectedScanType = SCAN_TYPE_QUICK;
		break;

	case IDC_FSCAN_RADIO:
		ptrWizParent->m_dwSelectedScanType = SCAN_TYPE_FULL;
		break;

	default:
        SAVASSERT( FALSE ); // add handling for new scan types.
        // intentionally fall through to handle corrupt settings

	case IDC_CSCAN_RADIO:
		ptrWizParent->m_dwSelectedScanType = SCAN_TYPE_CUSTOM;
		break;
	}
}

//----------------------------------------------------------------
// OnQScanRButton	Invoked, when Quick Scan radio button is clicked.
//----------------------------------------------------------------
void CSelectScanTypePage::OnQScanRButton()
{
	//Sets the m_dwSelectedScanType as quick scan
	SetCheck(IDC_QSCAN_RADIO);
	((CButton*)GetDlgItem(IDC_FSCAN_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CSCAN_RADIO))->SetCheck(0);
}

//----------------------------------------------------------------
// OnFScanRButton	Invoked, when Full Scan radio button is clicked.
//----------------------------------------------------------------
void CSelectScanTypePage::OnFScanRButton()
{
	//Sets the m_dwSelectedScanType as full scan
	SetCheck(IDC_FSCAN_RADIO);
	((CButton*)GetDlgItem(IDC_QSCAN_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CSCAN_RADIO))->SetCheck(0);
}

//----------------------------------------------------------------
// OnCScanRButton	Invoked, when Custom Scan radio button is clicked.
//----------------------------------------------------------------
void CSelectScanTypePage::OnCScanRButton()
{
	//Sets the m_dwSelectedScanType as custom scan
	SetCheck(IDC_CSCAN_RADIO);
	((CButton*)GetDlgItem(IDC_QSCAN_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_FSCAN_RADIO))->SetCheck(0);
}
