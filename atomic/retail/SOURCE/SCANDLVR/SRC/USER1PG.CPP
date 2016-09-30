/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/User1Pg.cpv   1.10   30 Jun 1998 15:42:26   sedward  $
/////////////////////////////////////////////////////////////////////////////
//
// UserPg1.cpp: implementation of the CUserInfo1PropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/User1Pg.cpv  $
// 
//    Rev 1.10   30 Jun 1998 15:42:26   sedward
// Enhanced email address validation (defect #118024).
//
//    Rev 1.9   14 Jun 1998 18:27:54   SEDWARD
// Fixed minor message box bug in 'OnWizardNext'.
//
//    Rev 1.8   03 Jun 1998 14:30:00   jtaylor
// Removed the #include resource.h
//
//    Rev 1.7   26 May 1998 18:28:12   SEDWARD
// Put 'm_' in front of the bitmap member variables.
//
//    Rev 1.6   26 May 1998 13:03:34   SEDWARD
// Email address validation.
//
//    Rev 1.5   25 May 1998 13:51:16   SEDWARD
// Set bitmap IDs and caption in constructor, call WizMessageBox instead of
// AfxMessageBox.
//
//    Rev 1.4   19 May 1998 19:44:14   SEDWARD
// Changed 'RequiredFieldsFilled' so the user's phone number is no longer
// considered a required field.
//
//    Rev 1.3   27 Apr 1998 19:50:16   SEDWARD
// Removed needless 'GetBuffer' calls on CString objects.
//
//    Rev 1.1   27 Apr 1998 02:08:02   SEDWARD
// Added RequiredFieldsFilled().
//
//    Rev 1.0   20 Apr 1998 16:26:00   SEDWARD
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScanDlvr.h"
#include "ScanWiz.h"
#include "User1Pg.h"
#include "ScanCfg.h"
#include "Global.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserInfo1PropertyPage property page

IMPLEMENT_DYNCREATE(CUserInfo1PropertyPage, CWizardPropertyPage)

CUserInfo1PropertyPage::CUserInfo1PropertyPage()
    : CWizardPropertyPage(CUserInfo1PropertyPage::IDD, IDS_CAPTION_USER_INFORMATION)
{
    //{{AFX_DATA_INIT(CUserInfo1PropertyPage)
    m_szFax = _T("");
    m_szPhone = _T("");
    m_szEmail = _T("");
    m_szFirstName = _T("");
    m_szLastName = _T("");
    m_szCompany = _T("");
    //}}AFX_DATA_INIT

    m_bitmap16 = IDB_SDBMP_INFO_16;
    m_bitmap256 = IDB_SDBMP_INFO_256;
}

CUserInfo1PropertyPage::~CUserInfo1PropertyPage()
{
}

void CUserInfo1PropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CWizardPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CUserInfo1PropertyPage)
    DDX_Text(pDX, IDC_EDIT_FAX, m_szFax);
    DDX_Text(pDX, IDC_EDIT_PHONE, m_szPhone);
    DDX_Text(pDX, IDC_EDIT_EMAIL, m_szEmail);
    DDX_Text(pDX, IDC_EDIT_FIRSTNAME, m_szFirstName);
    DDX_Text(pDX, IDC_EDIT_LASTNAME, m_szLastName);
    DDX_Text(pDX, IDC_EDIT_COMPANY, m_szCompany);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserInfo1PropertyPage, CWizardPropertyPage)
    //{{AFX_MSG_MAP(CUserInfo1PropertyPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserInfo1PropertyPage message handlers



// ==== OnInitDialog ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL CUserInfo1PropertyPage::OnInitDialog(void)
{
    CWizardPropertyPage::OnInitDialog();

    // TODO: Add extra initialization here


    // limit the length in the edit controls...
    auto    CEdit*      pEditCtrl;

    // first name
    pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_FIRSTNAME);
    if (pEditCtrl)
        {
        pEditCtrl->SetLimitText(MAX_NAME_LEN);
        }

    // last name
    pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_LASTNAME);
    if (pEditCtrl)
        {
        pEditCtrl->SetLimitText(MAX_NAME_LEN);
        }

    // company
    pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_COMPANY);
    if (pEditCtrl)
        {
        pEditCtrl->SetLimitText(MAX_COMPANY_LEN);
        }

    // fax
    pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_FAX);
    if (pEditCtrl)
        {
        pEditCtrl->SetLimitText(MAX_FAX_LEN);
        }

    // phone
    pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_PHONE);
    if (pEditCtrl)
        {
        pEditCtrl->SetLimitText(MAX_PHONE_LEN);
        }

    // email
    pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_EMAIL);
    if (pEditCtrl)
        {
        pEditCtrl->SetLimitText(MAX_EMAIL_LEN);
        }


    // initialize this dialog's member variables
    g_ConfigInfo.GetFirstName(m_szFirstName.GetBuffer(MAX_NAME_LEN), MAX_NAME_LEN);
    m_szFirstName.ReleaseBuffer();
    g_ConfigInfo.GetLastName(m_szLastName.GetBuffer(MAX_NAME_LEN), MAX_NAME_LEN);
    m_szLastName.ReleaseBuffer();
    g_ConfigInfo.GetCompany(m_szCompany.GetBuffer(MAX_COMPANY_LEN), MAX_COMPANY_LEN);
    m_szCompany.ReleaseBuffer();
    g_ConfigInfo.GetFax(m_szFax.GetBuffer(MAX_FAX_LEN), MAX_FAX_LEN);
    m_szFax.ReleaseBuffer();
    g_ConfigInfo.GetPhone(m_szPhone.GetBuffer(MAX_PHONE_LEN), MAX_PHONE_LEN);
    m_szPhone.ReleaseBuffer();
    g_ConfigInfo.GetEmail(m_szEmail.GetBuffer(MAX_EMAIL_LEN), MAX_EMAIL_LEN);
    m_szEmail.ReleaseBuffer();

    // update field contents with the member variables
    this->UpdateData(FALSE);


    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CUserInfo1PropertyPage::OnInitDialog"



// ==== UpdateConfigData ==================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL CUserInfo1PropertyPage::UpdateConfigData(void)
{
    auto    DWORD       dwResult = 0;

    // update the member variables with current field contents
    this->UpdateData(TRUE);

    // update the global configuration data object
    g_ConfigInfo.SetFirstName(m_szFirstName);
    g_ConfigInfo.SetLastName(m_szLastName);
    g_ConfigInfo.SetCompany(m_szCompany);
    g_ConfigInfo.SetFax(m_szFax);
    g_ConfigInfo.SetPhone(m_szPhone);
    g_ConfigInfo.SetEmail(m_szEmail);

    // write the current configuration data to disk
    g_ConfigInfo.WriteConfigFile(&dwResult);

    return (TRUE);

}  // end of "CUserInfo1PropertyPage::UpdateConfigData"



// ==== OnWizardNext ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

LRESULT CUserInfo1PropertyPage::OnWizardNext(void)
{
    // TODO: Add your specialized code here and/or call the base class

    UpdateConfigData();

    auto    BOOL        bGoToNextPage = TRUE;
    auto    UINT        nResID = 0;

    if (FALSE == RequiredFieldsFilled())
        {
        bGoToNextPage = FALSE;
        nResID = IDS_REQUIRED_FIELDS;
        }

    if  ((FALSE != bGoToNextPage)
    &&  (FALSE == IsEmailAddressValid()))
        {
        bGoToNextPage = FALSE;
        nResID = IDS_INVALID_EMAIL_ADDRESS;
        }

    if (FALSE == bGoToNextPage)
        {
        auto    CScanDeliverWizard*         pWizSheet;
        auto    int                         nIndex;

        // display a message to the user
        WizMessageBox(nResID);

        // get a pointer to the parent window (the property sheet)
        pWizSheet = (CScanDeliverWizard*)this->GetParent();
        if (NULL != pWizSheet)
            {
            // don't allow the user to go to the next page
            nIndex = pWizSheet->GetIndexFromResID(IDD_USER_INFO1);
            if (nIndex > 0)
                {
                --nIndex;
                pWizSheet->SetActivePage(nIndex);
                }
            }
        }

    return CWizardPropertyPage::OnWizardNext();

}  // end of "CUserInfo1PropertyPage::OnWizardNext"



// ==== OnWizardBack ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

LRESULT CUserInfo1PropertyPage::OnWizardBack(void)
{
    // TODO: Add your specialized code here and/or call the base class

    UpdateConfigData();
    return CWizardPropertyPage::OnWizardBack();

}  // end of "CUserInfo1PropertyPage::OnWizardBack"



// ==== RequiredFieldsFilled ==============================================
//
//  This function checks to see if any of the required fields are empty.
//
//  Input:  nothing
//  Output: TRUE if all required fields are non-empty, FALSE if not
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL    CUserInfo1PropertyPage::RequiredFieldsFilled(void)
{
    // if any of the required fields are empty, return FALSE
    if  ((m_szFirstName.IsEmpty())
    ||  (m_szLastName.IsEmpty())
    ||  (m_szEmail.IsEmpty()))
        {
        return (FALSE);
        }
    else
        {
        return (TRUE);
        }

}  // end of "CUserInfo1PropertyPage::RequiredFieldsFilled"



// ==== IsEmailAddressValid ===============================================
//
//  This function performs a simple check on the email address to make sure
//  it is valid.  It checks that there are non-space characters, followed by
//  an '@' sign, followed by more non-space characters, followed by a period,
//  followed by more non-space characters (whew!).
//
//  Input:  nothing
//  Output: a value of TRUE if the email address is valid, FALSE if not
//
// ========================================================================
//  Function created: 5/98, SEDWARD
// ========================================================================

BOOL    CUserInfo1PropertyPage::IsEmailAddressValid(void)
{
    // if the string is empty, return FALSE
    if (m_szEmail.IsEmpty())
        {
        return (FALSE);
        }

    // make sure that there are no spaces in the strings on either side of the
    // "at" sign
    auto    BOOL        bValidString = TRUE;
    auto    LPTSTR      strPtr = NULL;

    // get a pointer to the character data
    strPtr = m_szEmail.GetBuffer(MAX_EMAIL_LEN);

    // look for any spaces before the "at sign" character
    while (NULL != *strPtr)
        {
        if (AT_SIGN_CHAR == *strPtr)
            {
            break;
            }

        if (SPACE_CHAR == *strPtr)
            {
            bValidString = FALSE;
            return (FALSE);
            }

        if (NULL != *strPtr)
            {
            strPtr = MyCharNext(strPtr);
            }
        }

    // if we're not pointing to an "at sign" character, the string is invalid
    if (AT_SIGN_CHAR != *strPtr)
        {
        bValidString = FALSE;
        return (FALSE);
        }

    // skip over the "at sign" character
    if (NULL != *strPtr)
        {
        strPtr = MyCharNext(strPtr);
        }

    // make sure there are no spaces between the "at" sign and the period
    while (NULL != *strPtr)
        {
        if (SPACE_CHAR == *strPtr)
            {
            bValidString = FALSE;
            return (FALSE);
            }
        else if (PERIOD_CHAR == *strPtr)
            {
            break;
            }

        if (NULL != *strPtr)
            {
            strPtr = MyCharNext(strPtr);
            }
        }

    // skip over the period
    if (NULL != *strPtr)
        {
        strPtr = MyCharNext(strPtr);
        }

    // finally, make sure we have at least one character (ie, non-space) after
    // the period
    if  ((NULL == *strPtr)
    ||  (SPACE_CHAR == *strPtr)
    ||  (PERIOD_CHAR == *strPtr))
        {
        bValidString = FALSE;
        }

    return  (bValidString);

}  // end of "CUserInfo1PropertyPage::IsEmailAddressValid"



