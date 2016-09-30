/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


/*--------------------------------------------------------------------
   CustomerInfo.cpp : implementation file

   Property Page Tab Title: "Avis Customer Info"

   Written by: Jim Hill                 
--------------------------------------------------------------------*/

#include "stdafx.h"
#include "resource.h"
#include "AvisConsole.h"
#include "CustomerInfo.h"
#include "InputRange.h"
#include "mmc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



// Persistant data
// Define
typedef struct tagAVISCCUSTOMERINFO
{
	CString		m_szCompanyName;
	CString		m_szContactEmail;
	CString		m_szContactName;
	CString		m_szContactPhone;
	CString		m_szCustomerAccount;

} AVIS_CUSTOMER_INFO;
AVIS_CUSTOMER_INFO  AvisCustomerInfo;    //={0};
static DWORD dwAvisCustomerInfoDefaultsSet=FALSE;



/////////////////////////////////////////////////////////////////////////////
// CCustomerInfo property page

IMPLEMENT_DYNCREATE(CCustomerInfo, CPropertyPage)

CCustomerInfo::CCustomerInfo() : CPropertyPage(CCustomerInfo::IDD)
{
	//{{AFX_DATA_INIT(CCustomerInfo)
	m_szCompanyName = _T("");
	m_szContactEmail = _T("");
	m_szContactName = _T("");
	m_szContactPhone = _T("");
	m_szCustomerAccount = _T("");
	//}}AFX_DATA_INIT

    // INITIALIZE
    m_pSnapin       =NULL;
    m_dwRef         = 1;
    m_pSConfigData  = NULL;
    m_lpHelpIdArray = g_CustomerInfoHelpIdArray;  // LOAD HELP MAP


    fWidePrintString("CCustomerInfo Constructor called  ObjectID= %d", m_iExtDataObjectID);

}

CCustomerInfo::~CCustomerInfo()
{
    fWidePrintString("CCustomerInfo Destructor called  ObjectID= %d", m_iExtDataObjectID);
}


// DEFINES FOR VARIABLE LENGTHS IN INPUTRANGE.H
void CCustomerInfo::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomerInfo)
	DDX_Text(pDX, IDC_EDIT_COMPANY_NAME, m_szCompanyName);
	DDV_MaxChars(pDX, m_szCompanyName, 255);
	DDX_Text(pDX, IDC_EDIT_CONTACT_EMAIL, m_szContactEmail);
	DDV_MaxChars(pDX, m_szContactEmail, 255);
	DDX_Text(pDX, IDC_EDIT_CONTACT_NAME, m_szContactName);
	DDV_MaxChars(pDX, m_szContactName, 255);
	DDX_Text(pDX, IDC_EDIT_CONTACT_PHONE, m_szContactPhone);
	DDV_MaxChars(pDX, m_szContactPhone, 255);
	DDX_Text(pDX, IDC_EDIT_CUST_ACCT, m_szCustomerAccount);
	DDV_MaxChars(pDX, m_szCustomerAccount, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomerInfo, CPropertyPage)
	//{{AFX_MSG_MAP(CCustomerInfo)
	ON_EN_CHANGE(IDC_EDIT_COMPANY_NAME, OnChangeEditCompanyName)
	ON_EN_CHANGE(IDC_EDIT_CONTACT_NAME, OnChangeEditContactName)
	ON_EN_CHANGE(IDC_EDIT_CONTACT_PHONE, OnChangeEditContactPhone)
	ON_EN_CHANGE(IDC_EDIT_CONTACT_EMAIL, OnChangeEditContactEmail)
	ON_EN_CHANGE(IDC_EDIT_CUST_ACCT, OnChangeEditCustAcct)
	ON_WM_HELPINFO()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomerInfo message handlers



/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CCustomerInfo::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertyPage::PostNcDestroy();

    //fWidePrintString("CCustomerInfo::PostNcDestroy()  ObjectID= %d", m_iExtDataObjectID);

    Release();     
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CCustomerInfo::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// TODO: Add extra initialization here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    m_dwRef++; 

    // SET THE DEFAULT LANG FONT
    mySetDefaultFont( GetSafeHwnd() );

    // INSERT DCOM CODE TO GET LIVE SETTINGS FROM THE SERVER
    if(m_pSConfigData!=NULL) 
    {
        // DID WE READ VALID DATA FROM THE SERVER?
        if( m_pSConfigData->ContainsValidData() )
        {
            m_szCompanyName     = m_pSConfigData->m_szCompanyName;
            m_szContactEmail    = m_pSConfigData->m_szContactEmail;
            m_szContactName     = m_pSConfigData->m_szContactName;
            m_szContactPhone    = m_pSConfigData->m_szContactPhone;
            m_szCustomerAccount = m_pSConfigData->m_szCustomerAccount;

            // UPDATE THE GLOBALS TO CHECK FOR CHANGES ON OK
            AvisCustomerInfo.m_szCompanyName     = m_szCompanyName;
            AvisCustomerInfo.m_szContactEmail    = m_szContactEmail ;
            AvisCustomerInfo.m_szContactName     = m_szContactName;
            AvisCustomerInfo.m_szContactPhone    = m_szContactPhone;
            AvisCustomerInfo.m_szCustomerAccount = m_szCustomerAccount;

        }
    }

    UpdateData( FALSE );         // FORCE IT TO REDISPLAY THE DATA

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CCustomerInfo::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    BOOL   bRet  = TRUE;

//     // VALIDATE SMTP ADDRESS
//     if( !ValidateSMTPaddress() )
//     {                              
//         CString  s;
//         s.LoadString( IDS_ERROR_SMTP_ADDRESS_FMT );  // Format
//         ShowErrorMessage( s, IDS_ERROR_SMTP_ADDRESS );
// 
//         // POINT TO THE FIELD
//     	SetModified(TRUE);
// 		GetDlgItem( IDC_EDIT_CONTACT_EMAIL )->SetWindowText( m_szContactEmail );
// 		GetDlgItem( IDC_EDIT_CONTACT_EMAIL )->SetFocus();
//         ((CEdit*) GetDlgItem( IDC_EDIT_CONTACT_EMAIL ) )->SetSel( 0, -1 );
//         return(FALSE);
//     }

    // SAVE CHANGES?
    if( bRet )
    {
        // Make sure data is good.
        UpdateData(TRUE);

        SaveChangedData(); 

        // 7/8/99
        // WRITE CHANGES TO THE REMOTE REGISTRY
        if( m_pSConfigData )
        {
            if( m_pSConfigData->IsDirty() )   // 1/2/00 jhill
            {
                HRESULT hr = m_pSConfigData->WriteConfigDataToServer();
            }
        
            // DISABLE APPLY BUTTON IF WE SUCCEEDED
          	SetModified(FALSE);
        }

    }
    else
    	SetModified(TRUE);

    //fPrintString("CCustomerInfo::OnApply called");

	return CPropertyPage::OnApply();  // CPropertyPage
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
// void CCustomerInfo::OnOK() 
// {
// 	// TODO: Add your specialized code here and/or call the base class
//     AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
// 
//     // Make sure data is good.
//     //UpdateData(TRUE);
//     //
//     //SaveChangedData(); 
// 
// 	CPropertyPage::OnOK();
// }


/*----------------------------------------------------------------------------
   ValidateSMTPaddress()
   "a@b.com"

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CCustomerInfo::ValidateSMTPaddress() 
{
    CString s    = m_szContactEmail;
    int     iPos = 0;

    // DON'T FORCE THEM TO ENTER IT
    if( m_szContactEmail.IsEmpty() )
        return(FALSE);   //   return(TRUE);

    {
        s.TrimLeft();
        s.TrimRight();
        if(s.GetLength() < 7)
            return(FALSE);
    
        // MUST HAVE '@' SIGN
        iPos = s.Find('@');
        if( iPos < 1 )     // -1 if not exist, if 0 then there's no name component
            return(FALSE);
    
        // MUST HAVE AT LEAST ONE '.'
        iPos = s.Find('.');
        if( iPos < 1 )     // -1 if not exist, if 0 then we can't start with a '.'
            return(FALSE);

        // MUST HAVE .com,.org, .gov, etc
        //s.Find( _T("") )

    }
    return(TRUE);
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CCustomerInfo::SaveChangedData() 
{
	DWORD dwDataChanged=0;

    // AvisCustomerInfo.m_szCompanyName     = m_szCompanyName;

    // Save the persistant data that has been changed
    if(m_szCompanyName.Compare((LPCTSTR) AvisCustomerInfo.m_szCompanyName) != 0)
    {
        // fWidePrintString("DATA SAVED CCustomerInfo::m_szCompanyName  Old=%s  New=%s",
        //                  (LPCTSTR) AvisCustomerInfo.m_szCompanyName,(LPCTSTR) m_szCompanyName);
        AvisCustomerInfo.m_szCompanyName = m_szCompanyName;
        dwDataChanged = TRUE;
        // SAVE IT TO CCustomerInfo
        m_pSConfigData->m_szCompanyName = m_szCompanyName;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_COMPANY_NAME );
    }

    if(m_szContactName.Compare((LPCTSTR) AvisCustomerInfo.m_szContactName) != 0)
    {
        // fWidePrintString("DATA SAVED CCustomerInfo::m_szContactName  Old=%s  New=%s",
        //                  (LPCTSTR) AvisCustomerInfo.m_szContactName, (LPCTSTR) m_szContactName );
        AvisCustomerInfo.m_szContactName = m_szContactName;
        dwDataChanged = TRUE;
        // SAVE IT TO CCustomerInfo
        m_pSConfigData->m_szContactName = m_szContactName;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_CONTACT_NAME );
    }

    if(m_szContactPhone.Compare((LPCTSTR) AvisCustomerInfo.m_szContactPhone) != 0)
    {
        // fWidePrintString("DATA SAVED CCustomerInfo::m_szContactPhone  Old=%s  New=%s",
        //                  (LPCTSTR) AvisCustomerInfo.m_szContactPhone,(LPCTSTR) m_szContactPhone );
        AvisCustomerInfo.m_szContactPhone = m_szContactPhone;
        dwDataChanged = TRUE;
        // SAVE IT TO CCustomerInfo
        m_pSConfigData->m_szContactPhone = m_szContactPhone;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_CONTACT_PHONE );
    }

    if(m_szContactEmail.Compare((LPCTSTR) AvisCustomerInfo.m_szContactEmail) != 0)
    {
        // fWidePrintString("DATA SAVED CCustomerInfo::m_szContactEmail  Old=%s  New=%s",
        //                  (LPCTSTR) AvisCustomerInfo.m_szContactEmail,(LPCTSTR) m_szContactEmail );
        AvisCustomerInfo.m_szContactEmail = m_szContactEmail;
        dwDataChanged = TRUE;
        // SAVE IT TO CCustomerInfo
        m_pSConfigData->m_szContactEmail = m_szContactEmail;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_CONTACT_EMAIL );
    }

    if(m_szCustomerAccount.Compare((LPCTSTR) AvisCustomerInfo.m_szCustomerAccount) != 0)
    {
        // fWidePrintString("DATA SAVED CCustomerInfo::m_szCustomerAccount  Old=%s  New=%s",
        //                  (LPCTSTR) AvisCustomerInfo.m_szCustomerAccount,(LPCTSTR) m_szCustomerAccount );
        AvisCustomerInfo.m_szCustomerAccount = m_szCustomerAccount;
        dwDataChanged = TRUE;
        // SAVE IT TO CCustomerInfo
        m_pSConfigData->m_szCustomerAccount = m_szCustomerAccount;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_CUSTOMER_ACCOUNT );
    }

    // NEED TO WRITE IT?
    if(dwDataChanged)  
        m_pSConfigData->SetDirty(TRUE);

    return(TRUE);
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CCustomerInfo::OnKillActive() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    BOOL   bRet  = TRUE;

    // Now, get the data
    UpdateData(TRUE);

    // VALIDATE SMTP ADDRESS
    if( !ValidateSMTPaddress() )
    {                              
        CString  s;
        s.LoadString( IDS_ERROR_SMTP_ADDRESS_FMT );  // Format
        ShowErrorMessage( s, IDS_ERROR_SMTP_ADDRESS );

        // POINT TO THE FIELD
    	SetModified(TRUE);
		GetDlgItem( IDC_EDIT_CONTACT_EMAIL )->SetWindowText( m_szContactEmail );
		GetDlgItem( IDC_EDIT_CONTACT_EMAIL )->SetFocus();
        ((CEdit*) GetDlgItem( IDC_EDIT_CONTACT_EMAIL ) )->SetSel( 0, -1 );
        return(FALSE);
    }

	// validate Company name
	if (m_szCompanyName.IsEmpty())
	{
        CString  s;

		s.LoadString(IDS_CUSTOMER_FIELD_CO_NAME);
        ShowErrorMessage( s, IDS_ERROR_CUSTOMER_FIELD);

		SetModified(TRUE);
		GetDlgItem(IDC_EDIT_COMPANY_NAME)->SetWindowText( m_szCompanyName );
		GetDlgItem(IDC_EDIT_COMPANY_NAME)->SetFocus();
        return(FALSE);
	}

	// validate account number
	if (m_szCustomerAccount.IsEmpty())
	{
        CString  s;
		s.LoadString(IDS_CUSTOMER_FIELD_ACCT_NUM);
        ShowErrorMessage( s, IDS_ERROR_CUSTOMER_FIELD);

		SetModified(TRUE);
		GetDlgItem(IDC_EDIT_CUST_ACCT)->SetWindowText( m_szCustomerAccount );
		GetDlgItem(IDC_EDIT_CUST_ACCT)->SetFocus();
        return(FALSE);
	}

	// validate contact name
	if (m_szContactName.IsEmpty())
	{
        CString  s;
		s.LoadString(IDS_CUSTOMER_FIELD_CONTACT);
        ShowErrorMessage( s, IDS_ERROR_CUSTOMER_FIELD);

		SetModified(TRUE);
		GetDlgItem(IDC_EDIT_CONTACT_NAME)->SetWindowText( m_szContactName);
		GetDlgItem(IDC_EDIT_CONTACT_NAME)->SetFocus();
        return(FALSE);
	}

	// validate phone
	if (m_szContactPhone.IsEmpty())
	{
        CString  s;
		s.LoadString(IDS_CUSTOMER_FIELD_PHONE);
        ShowErrorMessage( s, IDS_ERROR_CUSTOMER_FIELD);

		SetModified(TRUE);
		GetDlgItem(IDC_EDIT_CONTACT_PHONE)->SetWindowText( m_szContactPhone);
		GetDlgItem(IDC_EDIT_CONTACT_PHONE)->SetFocus();
        return(FALSE);
	}
    //fPrintString("CCustomerInfo::OnKillActive called");
	return CPropertyPage::OnKillActive();    
}



/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CCustomerInfo::OnChangeEditCompanyName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CCustomerInfo::OnChangeEditContactName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CCustomerInfo::OnChangeEditContactPhone() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CCustomerInfo::OnChangeEditContactEmail() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
}


void CCustomerInfo::OnChangeEditCustAcct() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
}




// F1 KEY PRESSED OR WHAT-IS ? CLICKED ON A FIELD
BOOL CCustomerInfo::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    DoHelpPropSheetF1( IDD, m_lpHelpIdArray, pHelpInfo );
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}



// PROP SHEET HELP BUTTON
BOOL CCustomerInfo::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	NMHDR* pNMHDR = (NMHDR*)lParam;
    DoHelpPropSheetHelpButton( IDD, m_lpHelpIdArray, pNMHDR );
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}


static DWORD nResID = 0;
static CWnd* pContextWnd = NULL;
static DWORD hTargetWnd = 0;

void CCustomerInfo::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    pContextWnd = pWnd;
    DoHelpDisplayContextMenuWhatsThis( IDD, &nResID, m_lpHelpIdArray, m_hWnd, pWnd, point, 0, &hTargetWnd );
    return;
}

#if 0
void CCustomerInfo::OnWhatsThisMenuitem() 
{
	// TODO: Add your command handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HWND hWnd = (HWND) hTargetWnd;
    hTargetWnd= 0;
    int nResourceID = nResID;
    nResID = 0;                  // CLEAR
    if( nResourceID == 0)
        nResourceID = IDD;
    DoHelpPropSheetContextHelp( nResourceID, m_lpHelpIdArray, pContextWnd, hWnd );
    return;
}

#endif

