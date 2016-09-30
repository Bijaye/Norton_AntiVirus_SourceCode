/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/AcceptPg.cpv   1.7   26 May 1998 18:27:56   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// AcceptPg.cpp: implementation of the CAcceptedFilesPropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/AcceptPg.cpv  $
// 
//    Rev 1.7   26 May 1998 18:27:56   SEDWARD
// Put 'm_' in front of the bitmap member variables.
//
//    Rev 1.6   25 May 1998 13:49:40   SEDWARD
// Set bitmap IDs and caption in constructor.
//
//    Rev 1.5   24 Apr 1998 18:16:42   SEDWARD
// Set first item in list control to selected state.
//
//    Rev 1.4   20 Apr 1998 20:58:12   SEDWARD
// Removed OnKillActive() and OnSetActive().
//
//    Rev 1.3   20 Apr 1998 18:04:24   SEDWARD
// Removed duplicate MFC insertion block.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScanDeliverDLL.h"
#include "scandlvr.h"
#include "AcceptPg.h"
#include "ScanWiz.h"
#include "QuarAdd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAcceptedFilesPropertyPage property page

IMPLEMENT_DYNCREATE(CAcceptedFilesPropertyPage, CWizardPropertyPage)

CAcceptedFilesPropertyPage::CAcceptedFilesPropertyPage(CScanDeliverDLL*  pDLL)
    : CWizardPropertyPage(pDLL, CAcceptedFilesPropertyPage::IDD, IDS_CAPTION_ACCEPTED_FILES)
{
    m_bitmap16 = IDB_SDBMP_ACCEPTEDFILES_16;
    m_bitmap256 = IDB_SDBMP_ACCEPTEDFILES_256;
}

CAcceptedFilesPropertyPage::CAcceptedFilesPropertyPage()
    : CWizardPropertyPage(CAcceptedFilesPropertyPage::IDD, IDS_CAPTION_ACCEPTED_FILES)
{
    //{{AFX_DATA_INIT(CAcceptedFilesPropertyPage)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT

    m_bitmap16 = IDB_SDBMP_ACCEPTEDFILES_16;
    m_bitmap256 = IDB_SDBMP_ACCEPTEDFILES_256;
}

CAcceptedFilesPropertyPage::~CAcceptedFilesPropertyPage()
{
}

void CAcceptedFilesPropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CWizardPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAcceptedFilesPropertyPage)
    DDX_Control(pDX, IDC_LIST_ACCEPTED_FILES, m_acceptedFilesListCtrl);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAcceptedFilesPropertyPage, CWizardPropertyPage)
    //{{AFX_MSG_MAP(CAcceptedFilesPropertyPage)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAcceptedFilesPropertyPage message handlers



// ==== OnInitDialog ======================================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL CAcceptedFilesPropertyPage::OnInitDialog(void)
{
    CWizardPropertyPage::OnInitDialog();

    // TODO: Add extra initialization here

    InitializeColumnHeaders();
    PopulateListCtrl();

    // set focus and selection to the first item in the list
    m_acceptedFilesListCtrl.SetItemState(0, (LVIS_FOCUSED | LVIS_SELECTED)
                                        , (LVIS_FOCUSED | LVIS_SELECTED));

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CAcceptedFilesPropertyPage::OnInitDialog"



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

void CAcceptedFilesPropertyPage::InitializeColumnHeaders(void)
{
    auto    CString         tempStr;
    auto    LONG            lWidth;
    auto    RECT            clientRect;

    // since we only are displaying one column, get the width of the client
    // area of the list control so our column is the same size
    m_acceptedFilesListCtrl.GetClientRect(&clientRect);
    lWidth = clientRect.right - clientRect.left;

    // fetch the column header string
    tempStr.LoadString(IDS_FILENAME);

    // insert the column
    m_acceptedFilesListCtrl.InsertColumn(0, tempStr,  LVCFMT_LEFT, lWidth);

}  // end of "CAcceptedFilesPropertyPage::InitializeColumnHeaders"



// ==== PopulateListCtrl ==================================================
//
//  This function is responsible for populating the list control.  It loops
//  through the "accepted" list of items in the scan and deliver DLL object
//  to derive the display content.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

void CAcceptedFilesPropertyPage::PopulateListCtrl(void)
{
    auto    int                 nIndex;
    auto    int                 numItems;
    auto    IQuarantineItem*    pQItem;
    auto    TCHAR               szBuf[MAX_QUARANTINE_FILENAME_LEN];

    numItems = m_pScanDeliverDLL->GetNumberAcceptedFiles();
    for (nIndex = 0; nIndex < numItems; ++nIndex)
        {
        pQItem = m_pScanDeliverDLL->GetAcceptedQuarantineItem(nIndex);
        if (NULL == pQItem)
            {
            continue;
            }

        pQItem->GetOriginalAnsiFilename(szBuf, MAX_QUARANTINE_FILENAME_LEN);
        m_acceptedFilesListCtrl.InsertItem(nIndex, szBuf);
        }

}  // end of "CAcceptedFilesPropertyPage::PopulateListCtrl"