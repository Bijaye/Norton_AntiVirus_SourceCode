/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/RejectPg.cpv   1.11   26 May 1998 18:27:54   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// RejectPg.cpp: implementation of the CRejectedFilesPropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/RejectPg.cpv  $
// 
//    Rev 1.11   26 May 1998 18:27:54   SEDWARD
// Put 'm_' in front of the bitmap member variables.
//
//    Rev 1.10   25 May 1998 13:50:06   SEDWARD
// Set bitmap IDs and caption in constructor.
//
//    Rev 1.9   20 May 1998 20:47:22   jtaylor
// Fixed a logic error.
//
//    Rev 1.8   19 May 1998 16:02:12   SEDWARD
// Enhanced 'OnWizardNext' so the 'Finish' dialog has more flexibility.
//
//    Rev 1.7   27 Apr 1998 18:12:12   SEDWARD
// Removed needless calls to CString::GetBuffer for ScanCfg "Set" calls.
//
//    Rev 1.6   27 Apr 1998 02:06:00   SEDWARD
// Added OnWizardNext().
//
//    Rev 1.5   24 Apr 1998 18:17:02   SEDWARD
// Set first item in list control to selected state.
//
//    Rev 1.4   20 Apr 1998 18:04:28   SEDWARD
// Removed duplicate MFC insertion block.
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "ScanDeliverDLL.h"
#include "scandlvr.h"
#include "RejectPg.h"
#include "ScanWiz.h"
#include "QuarAdd.h"
#include "WrapUpPg.h"
#include "qsfields.h"
#include "QItemEz.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRejectedFilesPropertyPage property page

IMPLEMENT_DYNCREATE(CRejectedFilesPropertyPage, CWizardPropertyPage)

CRejectedFilesPropertyPage::CRejectedFilesPropertyPage(CScanDeliverDLL*  pDLL)
    : CWizardPropertyPage(pDLL, CRejectedFilesPropertyPage::IDD, IDS_CAPTION_REJECTED_FILES)
{
    //{{AFX_DATA_INIT(CRejectedFilesPropertyPage)
	//}}AFX_DATA_INIT

    m_bitmap16 = IDB_SDBMP_REJECTEDFILES_16;
    m_bitmap256 = IDB_SDBMP_REJECTEDFILES_256;
}

CRejectedFilesPropertyPage::CRejectedFilesPropertyPage()
    : CWizardPropertyPage(CRejectedFilesPropertyPage::IDD, IDS_CAPTION_REJECTED_FILES)
{
    m_bitmap16 = IDB_SDBMP_REJECTEDFILES_16;
    m_bitmap256 = IDB_SDBMP_REJECTEDFILES_256;
}

CRejectedFilesPropertyPage::~CRejectedFilesPropertyPage()
{
}

void CRejectedFilesPropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CWizardPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRejectedFilesPropertyPage)
	DDX_Control(pDX, IDC_EDIT_REJECTED_FILE_DESC, m_rejectedDescription);
    DDX_Control(pDX, IDC_LIST_REJECTED_FILES, m_rejectedFilesListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRejectedFilesPropertyPage, CWizardPropertyPage)
    //{{AFX_MSG_MAP(CRejectedFilesPropertyPage)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_REJECTED_FILES, OnItemchangedListRejectedFiles)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRejectedFilesPropertyPage message handlers



// ==== OnInitDialog ======================================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL CRejectedFilesPropertyPage::OnInitDialog(void)
{
    CWizardPropertyPage::OnInitDialog();

    // TODO: Add extra initialization here

    InitializeColumnHeaders();
    PopulateListCtrl();

    // set focus and selection to the first item in the list
    m_rejectedFilesListCtrl.SetItemState(0, (LVIS_FOCUSED | LVIS_SELECTED)
                                        , (LVIS_FOCUSED | LVIS_SELECTED));

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CRejectedFilesPropertyPage::OnInitDialog"



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

void CRejectedFilesPropertyPage::InitializeColumnHeaders(void)
{
    auto    CString         tempStr;
    auto    LONG            lWidth;
    auto    RECT            clientRect;

    // since we only are displaying one column, get the width of the client
    // area of the list control so our column is the same size
    m_rejectedFilesListCtrl.GetClientRect(&clientRect);
    lWidth = clientRect.right - clientRect.left;

    // fetch the column header string
    tempStr.LoadString(IDS_COLNAME_THREAT_NAME);

    // insert the column
    m_rejectedFilesListCtrl.InsertColumn(0, tempStr,  LVCFMT_LEFT, lWidth);

}  // end of "CRejectedFilesPropertyPage::InitializeColumnHeaders"



// ==== PopulateListCtrl ==================================================
//
//  This function is responsible for populating the list control.  It loops
//  through the "rejected" list of items in the scan and deliver DLL object
//  to derive the display content.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

void CRejectedFilesPropertyPage::PopulateListCtrl(void)
{
    auto    int                 nIndex;
    auto    int                 numItems;
    auto    IQuarantineItem*    pQItem;

    numItems = m_pScanDeliverDLL->GetNumberRejectedFiles();
    for (nIndex = 0; nIndex < numItems; ++nIndex)
    {
        pQItem = m_pScanDeliverDLL->GetRejectedQuarantineItem(nIndex);
        if (NULL == pQItem)
        {
            continue;
        }
        
        CString cszItemName;
        QItemEZ::GetThreatName(pQItem, cszItemName);
        if(cszItemName.IsEmpty())
        {
            QItemEZ::GetANSIFileNameOnly(pQItem, cszItemName);
        }

        m_rejectedFilesListCtrl.InsertItem(nIndex, cszItemName);
    }

}  // end of "CRejectedFilesPropertyPage::PopulateListCtrl"



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

void CRejectedFilesPropertyPage::OnItemchangedListRejectedFiles(NMHDR* pNMHDR
                                                            , LRESULT* pResult)
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    // TODO: Add your control notification handler code here

    auto    int         nIndex;
    nIndex = m_rejectedFilesListCtrl.GetNextItem(-1, LVNI_FOCUSED);
    if (nIndex > -1)
    {
        m_rejectedDescription.SetWindowText(m_pScanDeliverDLL->GetRejectionReason(nIndex));
    }

    *pResult = 0;

}  // end of "CRejectedFilesPropertyPage::OnItemchangedListRejectedFiles"



// ==== OnWizardNext ======================================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// 5/20/98 JTAYLOR - Fixed problem with any reject files jumping to finish page.
// ========================================================================

LRESULT CRejectedFilesPropertyPage::OnWizardNext(void)
{
	// TODO: Add your specialized code here and/or call the base class

    auto    CScanDeliverWizard*         pWizSheet;
    auto    DWORD                       dwResult;
    auto    DWORD                       dwMessageID;

    // if the user is trying to proceed and all we're working with are rejected
    // files (there are no accepted files), we need to go to the last panel
    if (0 == m_pScanDeliverDLL->GetNumberAcceptedFiles())
        {
        // find out why "file of the day" mode is invalid and set the text for
        // the final wizard page accordingly
        dwResult = m_pScanDeliverDLL->IsFileOfTheDayModeValid();
        switch (dwResult)
            {
            case  FOTD_FILE_IS_COMPRESSED:
                dwMessageID = WRAPUP_TYPE_SINGLE_COMPRESSED_FILE;
                break;

            case  FOTD_INVALID_TIME_LAPSE:
                dwMessageID = WRAPUP_TYPE_INVALID_TIME_LAPSE;
                break;

            default:
                dwMessageID = WRAPUP_TYPE_NO_FILES_ACCEPTED;
                break;
            }

        // get a pointer to the parent window (the property sheet)
        pWizSheet = (CScanDeliverWizard*)this->GetParent();
        if (NULL != pWizSheet)
            {
            // set the "wrap up" text for the final dialog and make it the next page
            pWizSheet->GoToWrapUpPage(dwMessageID);
            }
        }

    return CWizardPropertyPage::OnWizardNext();

}  // end of "CRejectedFilesPropertyPage::OnWizardNext"