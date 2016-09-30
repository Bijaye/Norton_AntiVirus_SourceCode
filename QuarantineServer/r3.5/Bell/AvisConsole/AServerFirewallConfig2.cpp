/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


/*--------------------------------------------------------------------
   AServerFirewallConfig2.cpp : implementation file

   Property Page Tab Title: "Firewall"         

   Written by: Jim Hill                 
--------------------------------------------------------------------*/


#include "stdafx.h"	               
#include "resource.h"
#include "AvisConsole.h"
#include "AServerFirewallConfig2.h"
#include "InputRange.h"
#include "mmc.h"
#include "vpstrutils.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// HOW MANY SHEETS DOES THE USER HAVE UP IN THE SCOPE PANE?
//static int g_ScopePropSheetTotalCount = 0;





// Persistant data
// Define
typedef struct tagAVISDATAFIREWALLCONFIG2
{
	TCHAR	szProxyFirewall[WEB_DEFAULT_PROXY_FIREWALL_MAXLEN+10];
	int		iFirewallPort;
	TCHAR	szFirewallUsername[FIREWALL_USERNAME_MAXLEN+10];
	TCHAR	szFirewallPassword[FIREWALL_PASSWORD_MAXLEN+10];
}   AVIS_FIREWALL_DATA_CONFIG2;
// Declare
AVIS_FIREWALL_DATA_CONFIG2  AvisFirewallConfig2={0};
static DWORD dwAvisFirewallConfig2DefaultsSet=FALSE;



/////////////////////////////////////////////////////////////////////////////
// CAServerFirewallConfig2 property page

IMPLEMENT_DYNCREATE(CAServerFirewallConfig2, CPropertyPage)

CAServerFirewallConfig2::CAServerFirewallConfig2() : CPropertyPage(CAServerFirewallConfig2::IDD)
{
	//{{AFX_DATA_INIT(CAServerFirewallConfig2)
	m_szProxyFirewall = _T("");
	m_iFirewallPort = 0;
	m_sFirewallUsername = _T("");
	m_sFirewallPassword = _T("");
	//}}AFX_DATA_INIT

    // INITIALIZE
    m_pSnapin       = NULL;
    m_dwRef         = 1;
    m_lParam        = 0;
    m_pSConfigData  = NULL;
    m_lpHelpIdArray = g_FirewallConfigHelpIdArray;  // LOAD THE HELP MAP
//    m_pAvisServerConfig = NULL;
//    m_pAServerFirewallConfig2= NULL;
//    m_pAvisCustomerInfo = NULL;


// 	// DEFAULT SETTINGS
//     CheckAvis2DataDefaults();
// 

}

CAServerFirewallConfig2::~CAServerFirewallConfig2()
{
  	fWidePrintString("CAServerFirewallConfig2 Destructor called. ObjectID= %d", m_iExtDataObjectID);

}


// DEFINES FOR VARIABLE LENGTHS IN INPUTRANGE.H
void CAServerFirewallConfig2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAServerFirewallConfig2)
	DDX_Control(pDX, IDC_SPIN_FIREWALL_PORT, m_SpinFirewallPort);
	DDX_Text(pDX, IDC_EDIT_PROXY_FIREWALL, m_szProxyFirewall);
	DDV_MaxChars(pDX, m_szProxyFirewall, 259);
	DDX_Text(pDX, IDC_EDIT_FIREWALL_PORT, m_iFirewallPort);
	DDX_Text(pDX, IDC_EDIT_FIREWALL_USERNAME, m_sFirewallUsername);
	DDV_MaxChars(pDX, m_sFirewallUsername, 259);
	DDX_Text(pDX, IDC_EDIT_FIREWALL_PASSWORD, m_sFirewallPassword);
	DDV_MaxChars(pDX, m_sFirewallPassword, 259);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAServerFirewallConfig2, CPropertyPage)
	//{{AFX_MSG_MAP(CAServerFirewallConfig2)
	ON_EN_CHANGE(IDC_EDIT_PROXY_FIREWALL, OnChangeEditProxyFirewall)
	ON_WM_HELPINFO()
	ON_WM_CONTEXTMENU()
	ON_EN_CHANGE(IDC_EDIT_FIREWALL_PORT, OnChangeEditFirewallPort)
	ON_EN_CHANGE(IDC_EDIT_FIREWALL_PASSWORD, OnChangeEditFirewallPassword)
	ON_EN_CHANGE(IDC_EDIT_FIREWALL_USERNAME, OnChangeEditFirewallUsername)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAServerFirewallConfig2 message handlers







/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAServerFirewallConfig2::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::PostNcDestroy();

    Release();     

}


///*----------------------------------------------------------------------------
//
//   Written by: Jim Hill
//----------------------------------------------------------------------------*/
//DWORD CAServerFirewallConfig2::CheckAvis2DataDefaults()
//{
//    if(!dwAvisFirewallConfig2DefaultsSet) {
//        dwAvisFirewallConfig2DefaultsSet=TRUE;
//    }
//    return(0);
//}

//#ifdef _UNICODE
//    #define  strtoul   wcstoul
//#endif


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAServerFirewallConfig2::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// TODO: Add extra initialization here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    CString  szDisplayName;
    m_dwRef++; 
    m_bGatewayAddressModified  = FALSE;
    m_bFirewallAddressModified = FALSE;

    // SET THE DEFAULT LANG FONT
    mySetDefaultFont( GetSafeHwnd() );

    // INSERT DCOM CODE TO GET LIVE SETTINGS FROM THE SERVER
    if(m_pSConfigData!=NULL) 
    {
        // hr = m_pSConfigData->ReadConfigDataFromServer();
        // DID WE READ VALID DATA FROM THE SERVER?
        if( m_pSConfigData->ContainsValidData() )
        {
            m_szProxyFirewall    = m_pSConfigData->m_szProxyFirewall; 
			m_iFirewallPort      = m_pSConfigData->m_iFirewallPort;
            m_sFirewallUsername  = m_pSConfigData->m_sFirewallUsername;
            m_sFirewallPassword  = m_pSConfigData->m_sFirewallPassword;

            // UPDATE THE GLOBALS TO CHECK FOR CHANGES ON OK
            //_tcscpy(AvisFirewallConfig2.szGatewayAddress, m_szGatewayAddress);
            vpstrncpy(AvisFirewallConfig2.szProxyFirewall, m_szProxyFirewall, sizeof(AvisFirewallConfig2.szProxyFirewall));
            AvisFirewallConfig2.iFirewallPort   = m_iFirewallPort;
            vpstrncpy(AvisFirewallConfig2.szFirewallUsername, m_sFirewallUsername, sizeof(AvisFirewallConfig2.szFirewallUsername));
            vpstrncpy(AvisFirewallConfig2.szFirewallPassword, m_sFirewallPassword, sizeof (AvisFirewallConfig2.szFirewallPassword));
        }
    }

  	// Spin control for Firewall Port
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_FIREWALL_PORT))->SetRange32(WEB_MIN_SPIN_FIREWALL_PORT, WEB_MAX_SPIN_FIREWALL_PORT);
    // Set the buddy
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_FIREWALL_PORT))->SetBuddy( GetDlgItem(IDC_EDIT_FIREWALL_PORT));
    // Set initial pos
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_FIREWALL_PORT))->SetPos(m_iFirewallPort);

    UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
                   

/*----------------------------------------------------------------------------
   CAServerFirewallConfig2::SaveChangedData
   Save data in remote registry for the AVIS client on the server
   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAServerFirewallConfig2::SaveChangedData() 
{
	DWORD dwDataChanged=0;

    // Save the persistant data that has been changed
    if(m_szProxyFirewall.Compare((LPCTSTR) AvisFirewallConfig2.szProxyFirewall) != 0)
    {
        fWidePrintString("DATA SAVED CAServerFirewallConfig2::m_szProxyFirewall  Old=%s  New=%s",
                         (LPCTSTR) AvisFirewallConfig2.szProxyFirewall,m_szProxyFirewall);
        vpstrncpy(AvisFirewallConfig2.szProxyFirewall, m_szProxyFirewall, sizeof (AvisFirewallConfig2.szProxyFirewall));
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_szProxyFirewall = m_szProxyFirewall;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_PROXY_FIREWALL );
    }

    //  m_iFirewallPort
    if( AvisFirewallConfig2.iFirewallPort != m_iFirewallPort )
    {
        AvisFirewallConfig2.iFirewallPort = m_iFirewallPort;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_iFirewallPort = m_iFirewallPort;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_FIREWALL_PORT );
    }

    //  m_sFirewallUsername    
    if(m_sFirewallUsername.Compare((LPCTSTR) AvisFirewallConfig2.szFirewallUsername) != 0)
    {
        vpstrncpy(AvisFirewallConfig2.szFirewallUsername, m_sFirewallUsername, sizeof(AvisFirewallConfig2.szFirewallUsername));
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_sFirewallUsername = m_sFirewallUsername;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_FIREWALL_USERNAME );
    }

    //  m_sFirewallPassword   
    if(m_sFirewallPassword.Compare((LPCTSTR) AvisFirewallConfig2.szFirewallPassword) != 0)
    {
        vpstrncpy(AvisFirewallConfig2.szFirewallPassword, m_sFirewallPassword, sizeof(AvisFirewallConfig2.szFirewallPassword));
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_sFirewallPassword = m_sFirewallPassword;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_FIREWALL_PASSWORD );
    }




    // NEED TO WRITE IT?
    if(dwDataChanged)  
        m_pSConfigData->SetDirty(TRUE);

    return;
}


/*----------------------------------------------------------------------------
   OK button pressed
   Save data in remote registry for the AVIS client on the server
   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAServerFirewallConfig2::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 

    // Make sure data is good.
    //UpdateData(TRUE);       7/8/99

    //SaveChangedData(); 

	CPropertyPage::OnOK();
}   // OnOk



/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAServerFirewallConfig2::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 

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

    // FORCE IT TO REDISPLAY THE DATA
    // UpdateData( FALSE );         



	return CPropertyPage::OnApply();
}



/*----------------------------------------------------------------------------

   Written by: Jim Hill
   Code borrowed from QConsole
----------------------------------------------------------------------------*/
BOOL CAServerFirewallConfig2::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 

    BOOL   bRet  = TRUE;
    BOOL   bRet2 = TRUE;
    int    iRet  = 0;
    TCHAR  szAddr[512];
    CString sTitle, sText;
    sTitle.LoadString( IDS_CONFIG_ERROR );
    int iControl = 0;

    //  FIREWALL PORT
    long iFirewallPort = GetDlgItemInt( IDC_EDIT_FIREWALL_PORT, &bRet, FALSE );
    if( !bRet || ( iFirewallPort < WEB_MIN_SPIN_FIREWALL_PORT || iFirewallPort > WEB_MAX_SPIN_FIREWALL_PORT ) )
    {
        // Inform the user 
        sText.Format( IDS_BADFMT_FIREWALL_PORT, WEB_MIN_SPIN_FIREWALL_PORT, WEB_MAX_SPIN_FIREWALL_PORT );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_EDIT_FIREWALL_PORT;
    }


    // RESET THE CONTROL
    if( iControl )
    {
        GetDlgItem( iControl )->SetFocus();
        ((CEdit*) GetDlgItem( iControl) )->SetSel( 0, -1 );
        return FALSE;
    }


    // Now, get the text data
    UpdateData(TRUE);


    // CHECK THE FIREWALL ADDRESS            
    if(m_bFirewallAddressModified)
    {
        m_szProxyFirewall.TrimRight();
        m_szProxyFirewall.TrimLeft();
        vpstrncpy( szAddr, m_szProxyFirewall.GetBuffer(0), sizeof(szAddr));
        m_szProxyFirewall.ReleaseBuffer(-1);
        bRet2 = m_pSConfigData->ValidateIpAddress( szAddr ); 
        if( bRet2 == FALSE )
        {
            sText.Format( IDS_ERROR_IP_VALIDATE_FIREWALL_ADDR, szAddr ); 
            iRet = ShowErrorMessage( (LPCTSTR) sText, IDS_ERROR_CAPTION_IP_VALIDIDATE, MB_ICONERROR | MB_YESNO);
            if( iRet == IDOK )
            {
                m_bFirewallAddressModified = FALSE;
            }
            else
            {
         	    SetModified(TRUE);
	 			GetDlgItem( IDC_EDIT_PROXY_FIREWALL )->SetWindowText( m_szProxyFirewall );
         		GetDlgItem( IDC_EDIT_PROXY_FIREWALL )->SetFocus();
                ((CEdit*) GetDlgItem( IDC_EDIT_PROXY_FIREWALL ) )->SetSel( 0, -1 );
                return(FALSE);
            }
        }
        else
            m_bFirewallAddressModified = FALSE;
    }


    
	return CPropertyPage::OnKillActive();
}







/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAServerFirewallConfig2::OnChangeEditProxyFirewall() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
    m_bFirewallAddressModified = TRUE;

}

/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAServerFirewallConfig2::OnChangeEditFirewallPort() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
}

void CAServerFirewallConfig2::OnChangeEditFirewallPassword() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
	
}

void CAServerFirewallConfig2::OnChangeEditFirewallUsername() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
	
}





#if 0
/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAServerFirewallConfig2::OnChangeEditGatewayAddress() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
    m_bGatewayAddressModified = TRUE;

}
/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAServerFirewallConfig2::OnChangeEditStatusInterval() 
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
void CAServerFirewallConfig2::OnChangeEditGatewayPort() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
}
#endif




// F1 KEY PRESSED OR WHAT-IS ? CLICKED ON A FIELD
/*----------------------------------------------------------------------------
   CAServerFirewallConfig2::OnHelpInfo()
   F1 KEY PRESSED OR WHAT-IS ? CLICKED ON A FIELD
   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAServerFirewallConfig2::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    DoHelpPropSheetF1( IDD_AVIS_WEB_COMM_PAGE, m_lpHelpIdArray, pHelpInfo );  // (AVIS_HELP_ID_STRUCT *) &m_HelpIdArray
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}

// PROP SHEET HELP BUTTON
/*----------------------------------------------------------------------------
   CAServerFirewallConfig2::OnNotify()
   PROP SHEET HELP BUTTON
   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAServerFirewallConfig2::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	NMHDR* pNMHDR = (NMHDR*)lParam;
    DoHelpPropSheetHelpButton(IDD_AVIS_WEB_COMM_PAGE, m_lpHelpIdArray, pNMHDR );  // (AVIS_HELP_ID_STRUCT *)&m_HelpIdArray
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

static DWORD nResID = 0;
static CWnd* pContextWnd = NULL;
static DWORD hTargetWnd = 0;

/*----------------------------------------------------------------------------
   CAServerFirewallConfig2::OnContextMenu()

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAServerFirewallConfig2::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());   
    pContextWnd = pWnd;
    DoHelpDisplayContextMenuWhatsThis( IDD_AVIS_WEB_COMM_PAGE, &nResID, m_lpHelpIdArray, m_hWnd, pWnd, point, 0, &hTargetWnd ); // (AVIS_HELP_ID_STRUCT *)&m_HelpIdArray
    return;
}
#if 0
/*----------------------------------------------------------------------------
   CAServerFirewallConfig2::OnWhatsThisMenuitem()

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAServerFirewallConfig2::OnWhatsThisMenuitem() 
{
	// TODO: Add your command handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
    HWND hWnd = (HWND) hTargetWnd;
    hTargetWnd= 0;
    int nResourceID = nResID;
    nResID = 0;                  // CLEAR
    if( nResourceID == 0)
        nResourceID = IDD_AVIS_WEB_COMM_PAGE;
    DoHelpPropSheetContextHelp( nResourceID, m_lpHelpIdArray, pContextWnd, hWnd );  // (AVIS_HELP_ID_STRUCT *)&m_HelpIdArray
    pContextWnd = NULL;
    return;
}
#endif


