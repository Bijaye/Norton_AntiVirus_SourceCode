#include "stdafx.h"
#include "Global.h"
#include "ScanDeliverDLL.h"
#include "scandlvr.h"
#include "ScanWiz.h"
#include "SendAddlInfoPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendAddlInfoPropertyPage property page

IMPLEMENT_DYNCREATE(CSendAddlInfoPropertyPage, CWizardPropertyPage)

CSendAddlInfoPropertyPage::CSendAddlInfoPropertyPage()
: CWizardPropertyPage(CSendAddlInfoPropertyPage::IDD, IDS_CAPTION_USER_INFORMATION)
{
    //{{AFX_DATA_INIT(CSendAddlInfoPropertyPage)
    //}}AFX_DATA_INIT

    m_bitmap16 = IDB_SDBMP_INFO_16;
    m_bitmap256 = IDB_SDBMP_INFO_256;

}

CSendAddlInfoPropertyPage::~CSendAddlInfoPropertyPage()
{
}

void CSendAddlInfoPropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CWizardPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSendAddlInfoPropertyPage)
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendAddlInfoPropertyPage, CWizardPropertyPage)
    //{{AFX_MSG_MAP(CSendAddlInfoPropertyPage)
    ON_BN_CLICKED(IDC_BTN_CHOOSE_YES, OnChkSendAddlInfoClicked)
    ON_BN_CLICKED(IDC_BTN_CHOOSE_NO, OnChkSendAddlInfoClicked)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendAddlInfoPropertyPage message handlers



// ==== UpdateConfigData ==================================================
//
//
// ========================================================================
//  Function created: 12/10, APIERCE
// ========================================================================

BOOL CSendAddlInfoPropertyPage::UpdateConfigData(void)
{
    auto    DWORD       dwResult = 0;

    // update the member variables with current field contents
    this->UpdateData(TRUE);

    BOOL bYes = BST_CHECKED == ((CButton*)GetDlgItem(IDC_BTN_CHOOSE_YES))->GetCheck();
    g_ConfigInfo.SetSendAddlInfo(bYes);

    // write the current configuration data to disk
    g_ConfigInfo.WriteConfigFile(&dwResult);

    return (TRUE);

}  // end of "CSendAddlInfoPropertyPage::UpdateConfigData"

// ==== OnWizardBack ======================================================
//
//
// ========================================================================
//  Function created: 12/10, APIERCE
// ========================================================================

LRESULT CSendAddlInfoPropertyPage::OnWizardBack(void)
{
    // TODO: Add your specialized code here and/or call the base class

    UpdateConfigData();

    // get a pointer to the parent window (the property sheet)
    CScanDeliverWizard*         pWizSheet;
    pWizSheet = (CScanDeliverWizard*)this->GetParent();
    if (NULL != pWizSheet)
    {
        pWizSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
    }


    return CWizardPropertyPage::OnWizardBack();

}  // end of "CSendAddlInfoPropertyPage::OnWizardBack"



// ==== OnWizardNext ======================================================
//
//
// ========================================================================
//  Function created: 12/10, APIERCE
// ========================================================================

LRESULT CSendAddlInfoPropertyPage::OnWizardNext(void)
{
    // TODO: Add your specialized code here and/or call the base class

    UpdateConfigData();

    return CWizardPropertyPage::OnWizardNext();

}  // end of "CSendAddlInfoPropertyPage::OnWizardNext"

// ==== OnSetActive ======================================================
//
//
// ========================================================================
//  Function created: 12/10, APIERCE
// ========================================================================

BOOL CSendAddlInfoPropertyPage::OnSetActive(void)
{
    // if user hasn't taken action on the opt-in button
    // then disable the "NEXT" button.
    BOOL bYes = BST_CHECKED == ((CButton*)GetDlgItem(IDC_BTN_CHOOSE_YES))->GetCheck();
    BOOL bNo = BST_CHECKED == ((CButton*)GetDlgItem(IDC_BTN_CHOOSE_NO))->GetCheck();
    
    if(!bYes && !bNo)
    {
        // get a pointer to the parent window (the property sheet)
        CScanDeliverWizard*         pWizSheet;
        pWizSheet = (CScanDeliverWizard*)this->GetParent();
        if (NULL != pWizSheet)
        {
            pWizSheet->SetWizardButtons(PSWIZB_BACK);
           
        }
    }

    return CWizardPropertyPage::OnSetActive();
}  // end of "CSendAddlInfoPropertyPage::OnWizardNext"



// ==== OnInitDialog ======================================================
//
//
// ========================================================================
//  Function created: 12/10, APIERCE
// ========================================================================

BOOL CSendAddlInfoPropertyPage::OnInitDialog(void)
{
    CWizardPropertyPage::OnInitDialog();

    // update field contents with the member variables
    this->UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CSendAddlInfoPropertyPage::OnInitDialog"

// ==== OnCheckCustomServer ===============================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

void CSendAddlInfoPropertyPage::OnChkSendAddlInfoClicked(void)
{
    // check box initializes "indeterminate"
    // once user clicks on it, we want to change it to 2 state check box,
    // and checked.
    BOOL bYes = BST_CHECKED == ((CButton*)GetDlgItem(IDC_BTN_CHOOSE_YES))->GetCheck();
    BOOL bNo = BST_CHECKED == ((CButton*)GetDlgItem(IDC_BTN_CHOOSE_NO))->GetCheck();

    if(bYes || bNo)
    {
        // get a pointer to the parent window (the property sheet)
        CScanDeliverWizard*         pWizSheet;
        pWizSheet = (CScanDeliverWizard*)this->GetParent();
        if (NULL != pWizSheet)
        {
            pWizSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
        }
    }

}  // end of "CUserInfo3PropertyPage::OnCheckCustomServer"

