/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/WrapUpPg.cpv   1.14   28 Jul 1998 13:37:28   jtaylor  $
/////////////////////////////////////////////////////////////////////////////
//
// WrapUpPg.cpp: implementation of the CWrapUpPropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/WrapUpPg.cpv  $
// 
//    Rev 1.14   28 Jul 1998 13:37:28   jtaylor
// Added a final panel for all false positives.
// 
//    Rev 1.13   09 Jul 1998 00:29:50   jtaylor
// Added corporate wrapup text.
//
//    Rev 1.12   05 Jun 1998 18:20:52   SEDWARD
// Enabled final wizard panel text for SARC backend failure.
//
//    Rev 1.11   27 May 1998 21:36:50   SEDWARD
// Added support for more SARC errors.
//
//    Rev 1.10   26 May 1998 18:26:54   SEDWARD
// Put 'm_' in front of the bitmap member variables.
//
//    Rev 1.9   25 May 1998 13:50:14   SEDWARD
// Set bitmap IDs and caption in constructor.
//
//    Rev 1.8   19 May 1998 16:04:24   SEDWARD
// Added some more WRAPUP types.
//
//    Rev 1.7   18 May 1998 18:36:46   SEDWARD
// OnSetActive() now disables the "Cancel" button.
//
//    Rev 1.6   27 Apr 1998 20:24:18   SEDWARD
// Added support for new messages: virus defs not paid, file of the day
// cancelled.
//
//    Rev 1.4   27 Apr 1998 02:08:26   SEDWARD
// Added OnInitDialog().
//
//    Rev 1.3   26 Apr 1998 17:30:16   SEDWARD
// Added member variables for the static text controls, added
// SetTextContent().
//
//    Rev 1.2   24 Apr 1998 18:23:26   SEDWARD
// Added logfile headers.
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "scandlvr.h"
#include "ScanWiz.h"
#include "WizPage.h"
#include "WrapUpPg.h"
#include "scancfg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWrapUpPropertyPage property page

IMPLEMENT_DYNCREATE(CWrapUpPropertyPage, CWizardPropertyPage)

CWrapUpPropertyPage::CWrapUpPropertyPage()
    : CWizardPropertyPage(CWrapUpPropertyPage::IDD, IDS_CAPTION_WRAP_UP)
{
    //{{AFX_DATA_INIT(CWrapUpPropertyPage)
    m_szText1.LoadString(IDS_WRAP_UP_FINISH1);

    // Load the appropriate second string.
    if (FALSE == g_ConfigInfo.IsCorporateMode())
        {
        m_szText2.LoadString(IDS_WRAP_UP_FINISH2);
        }
    else
        {
        m_szText2.LoadString(IDS_WRAP_UP_FINISH2_CORP);
        }

	//}}AFX_DATA_INIT

    m_bitmap16 = IDB_SDBMP_WRAPUP_16;
    m_bitmap256 = IDB_SDBMP_WRAPUP_256;
}

CWrapUpPropertyPage::~CWrapUpPropertyPage()
{
}

void CWrapUpPropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CWizardPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CWrapUpPropertyPage)
	DDX_Text(pDX, IDC_WRAP_UP_TEXT1, m_szText1);
	DDX_Text(pDX, IDC_WRAP_UP_TEXT2, m_szText2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWrapUpPropertyPage, CWizardPropertyPage)
    //{{AFX_MSG_MAP(CWrapUpPropertyPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWrapUpPropertyPage message handlers



// ==== OnInitDialog ======================================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL CWrapUpPropertyPage::OnInitDialog(void)
{
	CWizardPropertyPage::OnInitDialog();

	// TODO: Add extra initialization here

    UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CWrapUpPropertyPage::OnInitDialog"



// ==== OnSetActive =======================================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL CWrapUpPropertyPage::OnSetActive(void)
{
    // TODO: Add your specialized code here and/or call the base class

    auto    CScanDeliverWizard*         pWizSheet;
    auto    DWORD                       dwFlags = PSWIZB_FINISH;

    // get a pointer to the parent window (the property sheet) and set the
    // wizard button flags
    pWizSheet = (CScanDeliverWizard*)this->GetParent();
    if (NULL != pWizSheet)
        {
        pWizSheet->SetWizardButtons(dwFlags);

        // disable the "Cancel" button
        auto    CButton*        pCancelButton;
        pCancelButton = (CButton*)pWizSheet->GetDlgItem(ID_WIZCANCEL);
        if (pCancelButton)
            {
            pCancelButton->EnableWindow(FALSE);
            }
        }

    // NOTE: the MS docs say that the "override of this member function should
    // call the default version before any other processing is done"; however,
    // the framework generates the parent call on the return statement...
    return CWizardPropertyPage::OnSetActive();

}  // end of "CWrapUpPropertyPage::OnSetActive"




// ==== SetTextContent ====================================================
//
//  This function is used to initialize the text fields for the wrap-up
//  property page.  It receives an ID value, and switches on that value
//  to extract target strings from the string table.
//
//  Input:
//      nWrapUpType -- a value specifying which group of strings to pull
//                     from the string table
//
//  Output:
//      A value of TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL    CWrapUpPropertyPage::SetTextContent(DWORD  nWrapUpType)
{
    auto    BOOL        bResult = TRUE;
    auto    UINT        resID1;
    auto    UINT        resID2;

    // derive the appropriate resource IDs
    switch (nWrapUpType)
        {
        case   WRAPUP_TYPE_USER_CANCELLED:
            resID1 = IDS_WRAPUP_USER_CANCEL1;
            resID2 = IDS_WRAPUP_USER_CANCEL2;
            break;

        case   WRAPUP_TYPE_NO_FILES_ACCEPTED:
            resID1 = IDS_WRAPUP_NO_ACCEPTED_FILES1;
            resID2 = IDS_WRAPUP_NO_ACCEPTED_FILES2;
            break;

        case   WRAPUP_TYPE_FILE_OF_THE_DAY_CANCEL:
            resID1 = IDS_FILE_OF_THE_DAY_CANCEL1;
            resID2 = IDS_FILE_OF_THE_DAY_CANCEL2;
            break;

        case   WRAPUP_TYPE_VIRUS_SUBSCRIPTION_NOT_PAID:
            resID1 = IDS_VIRUS_DEFS_NOT_PAID1;
            resID2 = IDS_VIRUS_DEFS_NOT_PAID2;
            break;

        case   WRAPUP_TYPE_SINGLE_REJECTED_FILE:
            resID1 = IDS_WRAPUP_SINGLE_REJECTED_FILE1;
            resID2 = IDS_WRAPUP_SINGLE_REJECTED_FILE2;
            break;

        case   WRAPUP_TYPE_INVALID_TIME_LAPSE:
            resID1 = IDS_WRAPUP_INVALID_TIME_LAPSE1;
            resID2 = IDS_WRAPUP_INVALID_TIME_LAPSE2;
            break;

        case   WRAPUP_TYPE_SINGLE_COMPRESSED_FILE:
            resID1 = IDS_WRAPUP_SINGLE_COMPRESSED_FILE1;
            resID2 = IDS_WRAPUP_SINGLE_COMPRESSED_FILE2;
            break;

        case   WRAPUP_TYPE_SARC_ERROR_CREATE_PACKAGE:
            resID1 = IDS_WRAPUP_SARC_ERROR_CREATE_PACKAGE1;
            resID2 = IDS_WRAPUP_SARC_ERROR_CREATE_PACKAGE2;
            break;

        case   WRAPUP_TYPE_SARC_ERROR_CHECK_BACKEND_STATUS:
        case   WRAPUP_TYPE_SARC_ERROR_SEND_TO_SARC:
            resID1 = IDS_WRAPUP_SARC_ERROR_SEND_TO_SARC1;
            resID2 = IDS_WRAPUP_SARC_ERROR_SEND_TO_SARC2;
            break;

        case   WRAPUP_TYPE_ALL_FALSE_POSITIVES:
            resID1 = IDS_WRAPUP_SARC_ERROR_FALSE_POSITIVES1;
            resID2 = IDS_WRAPUP_SARC_ERROR_FALSE_POSITIVES2;
            break;

        default:
            resID1 = IDS_WRAP_UP_FINISH1;
            resID2 = IDS_WRAP_UP_FINISH2;
            break;
        }

    // load the strings into the corresponding member variables
    if  ((0 == m_szText1.LoadString(resID1))
    ||  (0 == m_szText2.LoadString(resID2)))
        {
        bResult = FALSE;
        }

    return (bResult);

}  // end of "CWrapUpPropertyPage::SetTextContent"
