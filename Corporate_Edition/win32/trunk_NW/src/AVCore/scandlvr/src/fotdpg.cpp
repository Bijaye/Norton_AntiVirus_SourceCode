// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/FOTDPg.cpv   1.8   26 May 1998 18:27:48   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// FOTDPg.cpp: implementation of the CFileOfTheDayPropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/FOTDPg.cpv  $
// Rev 1.9 1/12/05 KTALINKI
// Updated PopulateListCtrl() method to use IQuarantineSession Interfaces instead of IQuarantineItem. 
//    Rev 1.8   26 May 1998 18:27:48   SEDWARD
// Put 'm_' in front of the bitmap member variables.
//
//    Rev 1.7   25 May 1998 13:50:02   SEDWARD
// Set bitmap IDs and caption in constructor.
//
//    Rev 1.6   19 May 1998 16:01:30   SEDWARD
// Set File of the Day radio button to default to 'No'.
//
//    Rev 1.5   27 Apr 1998 20:25:04   SEDWARD
// Added support for specific message in Finish dialog if file of the day is
// cancelled.
//
//    Rev 1.3   27 Apr 1998 02:05:08   SEDWARD
// Fixed index problem in call from GetIndexFromResID().
//
//    Rev 1.2   26 Apr 1998 17:26:58   SEDWARD
// Added GetTemplateID(); if user declines file of the day submission,
// we jump to the "finish" page.
//
//    Rev 1.1   24 Apr 1998 19:39:06   jtaylor
// Fixed an uninitialized variable.
//
//    Rev 1.0   24 Apr 1998 18:10:18   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "scandlvr.h"
#include "ScanWiz.h"
#include "WrapUpPg.h"
#include "FOTDPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileOfTheDayPropertyPage property page

IMPLEMENT_DYNCREATE(CFileOfTheDayPropertyPage, CWizardPropertyPage)

CFileOfTheDayPropertyPage::CFileOfTheDayPropertyPage(CScanDeliverDLL*  pDLL)
    : CWizardPropertyPage(pDLL, CFileOfTheDayPropertyPage::IDD, IDS_CAPTION_FILE_OF_THE_DAY)
{
    //{{AFX_DATA_INIT(CFileOfTheDayPropertyPage)
	//}}AFX_DATA_INIT

    m_bitmap16 = IDB_SDBMP_REJECTEDFILES_16;
    m_bitmap256 = IDB_SDBMP_REJECTEDFILES_256;
}


CFileOfTheDayPropertyPage::CFileOfTheDayPropertyPage()
    : CWizardPropertyPage(CFileOfTheDayPropertyPage::IDD)
{
    m_bitmap16 = IDB_SDBMP_REJECTEDFILES_16;
    m_bitmap256 = IDB_SDBMP_REJECTEDFILES_256;
}

CFileOfTheDayPropertyPage::~CFileOfTheDayPropertyPage()
{
}

void CFileOfTheDayPropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CWizardPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileOfTheDayPropertyPage)
	DDX_Control(pDX, IDC_EDIT_FILE_OF_THE_DAY, m_fileOfTheDayDescription);
	DDX_Control(pDX, IDC_LIST_FILE_OF_THE_DAY, m_fileOfTheDayListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileOfTheDayPropertyPage, CWizardPropertyPage)
	//{{AFX_MSG_MAP(CFileOfTheDayPropertyPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FILE_OF_THE_DAY, OnItemchangedListFileOfTheDay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileOfTheDayPropertyPage message handlers




// ==== GetFileOfTheDayRadioButtonValue ===================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL CFileOfTheDayPropertyPage::GetFileOfTheDayRadioButtonValue(void)
{
    auto    BOOL            bResult = TRUE;
    auto    CButton*        ptrYes;

    // the buttons are mutually exclusive, so we only need to get a pointer to
    // one of the buttons (we choose "yes")
    ptrYes = (CButton*)GetDlgItem(IDC_RADIO_FILE_OF_THE_DAY_YES);
    if (ptrYes)
        {
        bResult = ptrYes->GetCheck();
        }

    return (bResult);

}  // end of "CFileOfTheDayPropertyPage::GetFileOfTheDayRadioButtonValue"



// ==== InitializeColumnHeaders ===========================================
//
//  This function is responsible for setting up the column header(s) in the
//  list control.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

void CFileOfTheDayPropertyPage::InitializeColumnHeaders(void)
{
    auto    CString         tempStr;
    auto    LONG            lWidth;
    auto    RECT            clientRect;

    // since we only are displaying one column, get the width of the client
    // area of the list control so our column is the same size
    m_fileOfTheDayListCtrl.GetClientRect(&clientRect);
    lWidth = clientRect.right - clientRect.left;

    // fetch the column header string
    tempStr.LoadString(IDS_FILENAME);

    // insert the column
    m_fileOfTheDayListCtrl.InsertColumn(0, tempStr,  LVCFMT_LEFT, lWidth);

}  // end of "CFileOfTheDayPropertyPage::::InitializeColumnHeaders"



// ==== InitializeFileOfTheDayRadioButtons ================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

void CFileOfTheDayPropertyPage::InitializeFileOfTheDayRadioButtons(void)
{
    auto    CButton*        ptrNo;
    auto    CButton*        ptrYes;

    // get pointers to the radio buttons
    ptrNo = (CButton*)GetDlgItem(IDC_RADIO_FILE_OF_THE_DAY_NO);
    ptrYes = (CButton*)GetDlgItem(IDC_RADIO_FILE_OF_THE_DAY_YES);

    // default to "no"...
    if (NULL != ptrNo)
        {
        ptrNo->SetCheck(TRUE);
        }

    if (NULL != ptrYes)
        {
        ptrYes->SetCheck(FALSE);
        }

}  // end of "CFileOfTheDayPropertyPage::InitializeFileOfTheDayRadioButtons"



// ==== OnInitDialog ======================================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL CFileOfTheDayPropertyPage::OnInitDialog(void)
{
	CWizardPropertyPage::OnInitDialog();

	// TODO: Add extra initialization here

    InitializeColumnHeaders();
    PopulateListCtrl();
    InitializeFileOfTheDayRadioButtons();

    // set focus and selection to the first item in the list
    m_fileOfTheDayListCtrl.SetItemState(0, (LVIS_FOCUSED | LVIS_SELECTED)
                                        , (LVIS_FOCUSED | LVIS_SELECTED));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CFileOfTheDayPropertyPage::OnInitDialog"



// ==== OnItemchangedListRejectedFiles ====================================
//
//  This function displays any text associated with the item that currently
//  has focus in the list control.
//
//  Input:  the default MFC arguments for this handler
//
//  Output: nothing
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

void CFileOfTheDayPropertyPage::OnItemchangedListFileOfTheDay(NMHDR* pNMHDR, LRESULT* pResult)
{
    auto    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    // TODO: Add your control notification handler code here

    auto    int         nIndex;
    nIndex = m_fileOfTheDayListCtrl.GetNextItem(-1, LVNI_FOCUSED);
    if (nIndex > -1)
        {
        auto    CString*    ptrCString;
        ptrCString = m_pScanDeliverDLL->GetRejectionReason(nIndex);
        m_fileOfTheDayDescription.SetWindowText(*ptrCString);
        }

    *pResult = 0;

}  // end of "CFileOfTheDayPropertyPage::OnItemchangedListRejectedFiles"



// ==== OnWizardNext ======================================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

LRESULT CFileOfTheDayPropertyPage::OnWizardNext(void)
{
	// TODO: Add your specialized code here and/or call the base class

    auto    CScanDeliverWizard*         pWizSheet;

    // get a pointer to the parent window (the property sheet)
    pWizSheet = (CScanDeliverWizard*)this->GetParent();

    // if the user sets the "no" radio button, wrap up the session
    if ((NULL != pWizSheet)  &&  (FALSE == GetFileOfTheDayRadioButtonValue()))
        {
        pWizSheet->GoToWrapUpPage(WRAPUP_TYPE_FILE_OF_THE_DAY_CANCEL);
        }

	return CWizardPropertyPage::OnWizardNext();

}  // end of "CFileOfTheDayPropertyPage::OnWizardNext"



// ==== PopulateListCtrl ==================================================
//
//  This function is responsible for populating the list control.  Since
//  "file of the day" mode only allows the user to submit one file, we
//  fetch the first item in the rejected files list.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 4/98, SEDWARD
//	1/4/05 - KTALINKI	Modified to use IQuarantineSession interface instead
//						of IQuarantineItem for Extended Threats
//			TBD - Displaying Virus Name instead of the file names
// ========================================================================

void CFileOfTheDayPropertyPage::PopulateListCtrl(void)
{
    auto    int                 nIndex = 0;
    auto    char				szBuf[MAX_QUARANTINE_FILENAME_LEN];
	auto    IQuarantineSession*   pQSes = NULL;
	auto	HRESULT				hr = 0;
	DWORD	dwBufSz = MAX_QUARANTINE_FILENAME_LEN;

    if (m_pScanDeliverDLL->GetNumberRejectedFiles() > 0)
        {
        pQSes = m_pScanDeliverDLL->GetRejectedQuarantineSession(0);
        if (NULL != pQSes)
            {
				hr = pQSes->GetDescription(szBuf, &dwBufSz );
				if(SUCCEEDED(hr))
                    m_fileOfTheDayListCtrl.InsertItem(nIndex, A2T(szBuf));
            }
        }

}  // end of "CFileOfTheDayPropertyPage::::PopulateListCtrl"