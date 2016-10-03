/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// InstallDefinitions.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "avisconsole.h"
#include "InstallDefinitions.h"
#include "InputRange.h"
#include "SelectTargetDialog.h"
#include "mmc.h"
#include "qserver.h"
#include "password.h"
#include "vpcommon.h"
//#include "ldvpdefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





// Persistant data
typedef struct tagAVISDATAINSTALLDEFINITIONSCONFIG
{
	BOOL	bCheckBlessedBroadcast;
	BOOL	bCheckUnblessedBroadcast;
	BOOL	bCheckUnblessedNarrowCast;
	BOOL	bCheckUnblessedPointCast;
	// int		iDefDeliveryTimeout;
	int		iDefDeliveryPriority;
	int		iDefDeliveryInterval;

}   AVIS_DATA_INSTALL_DEFINITIONS_CONFIG;

// Declare
AVIS_DATA_INSTALL_DEFINITIONS_CONFIG  AvisInstallDefsConfig={0};




/////////////////////////////////////////////////////////////////////////////
// CInstallDefinitions property page

IMPLEMENT_DYNCREATE(CInstallDefinitions, CPropertyPage)    // CPropertyPage

CInstallDefinitions::CInstallDefinitions() : CPropertyPage(CInstallDefinitions::IDD)  // CPropertyPage(CInstallDefinitions::IDD)  
{
	//{{AFX_DATA_INIT(CInstallDefinitions)
	m_bCheckBlessedBroadcast = FALSE;
	m_bCheckUnblessedBroadcast = FALSE;
	m_bCheckUnblessedNarrowCast = FALSE;
	m_bCheckUnblessedPointCast = FALSE;
	m_iDefDeliveryInterval = 0;
	//}}AFX_DATA_INIT

    // INITIALIZE
    m_pSnapin              = NULL;
    m_dwRef                = 1;
    m_pSConfigData         = NULL;
    m_sDefBlessedTargets   = _T("");
    m_sDefUnblessedTargets = _T("");
	m_sDefSecureUsername   = _T("");
	m_sDefSecurePassword   = _T("");
    m_lpHelpIdArray        = g_InstallDefinitionsHelpIdArray;  // LOAD THE HELP MAP


    // fWidePrintString("CAvisServerDefPolicyConfig Constructor called  ObjectID= %d", m_iExtDataObjectID);

}

CInstallDefinitions::~CInstallDefinitions()
{
    // fWidePrintString("CInstallDefinitions Destructor called  ObjectID= %d", m_iExtDataObjectID);
}

void CInstallDefinitions::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstallDefinitions)
	DDX_Control(pDX, IDC_SPIN_DEFS_DELIVERY_INTERVAL, m_SpinDeliveryInterval);
	DDX_Check(pDX, IDC_CHECK_BLESSED_BROADCAST, m_bCheckBlessedBroadcast);
	DDX_Check(pDX, IDC_CHECK_UNBLESSED_BROADCAST, m_bCheckUnblessedBroadcast);
	DDX_Check(pDX, IDC_CHECK_UNBLESSED_NARROWCAST, m_bCheckUnblessedNarrowCast);
	DDX_Check(pDX, IDC_CHECK_UNBLESSED_POINTCAST, m_bCheckUnblessedPointCast);
	DDX_Text(pDX, IDC_EDIT_DEF_DELIVERY_INTERVAL, m_iDefDeliveryInterval);
	DDX_Text(pDX, IDC_USERNAME_EDIT, m_sDefSecureUsername);
	DDV_MaxChars(pDX, m_sDefSecureUsername, SYM_MAX_USERNAME_LEN);
	DDX_Text(pDX, IDC_PASSWORD_EDIT, m_sDefSecurePassword);
	DDV_MaxChars(pDX, m_sDefSecurePassword, SYM_MAX_PASSWORD_LEN);
	DDX_Text(pDX, IDC_CONFIRM_EDIT, m_sDefSecureConfirm);
	DDV_MaxChars(pDX, m_sDefSecureConfirm, SYM_MAX_PASSWORD_LEN);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInstallDefinitions, CPropertyPage)
	//{{AFX_MSG_MAP(CInstallDefinitions)
	ON_BN_CLICKED(IDC_CHECK_BLESSED_BROADCAST, OnCheckBlessedBroadcast)
	ON_BN_CLICKED(IDC_CHECK_UNBLESSED_BROADCAST, OnCheckUnblessedBroadcast)
	ON_BN_CLICKED(IDC_CHECK_UNBLESSED_NARROWCAST, OnCheckUnblessedNarrowcast)
	ON_BN_CLICKED(IDC_CHECK_UNBLESSED_POINTCAST, OnCheckUnblessedPointcast)
	ON_EN_CHANGE(IDC_EDIT_DEF_DELIVERY_INTERVAL, OnChangeEditDefDeliveryInterval)
	ON_EN_CHANGE(IDC_USERNAME_EDIT, OnChangeEditDefUsername)
	ON_EN_CHANGE(IDC_PASSWORD_EDIT, OnChangeEditDefPassword)
	ON_EN_CHANGE(IDC_CONFIRM_EDIT, OnChangeEditDefConfirm)
	ON_WM_CONTEXTMENU()
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_BUTTON_BLESED_SELECT_TARGETS, OnButtonBlesedSelectTargets)
	ON_BN_CLICKED(IDC_BUTTON_UNBLESED_SELECT_TARGETS, OnButtonUnblesedSelectTargets)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstallDefinitions message handlers



void CInstallDefinitions::PostNcDestroy() 
{
	CPropertyPage::PostNcDestroy();
    Release();       
}

BOOL CInstallDefinitions::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
    CString  szDisplayName;
	USES_CONVERSION;
    m_dwRef++;	

	m_sCurrentDefVersion = m_pSConfigData->GetQSDefVersion();
	m_sCurrentDefDate = m_pSConfigData->GetQSDefDate();

    ((CWnd*) GetDlgItem(IDC_STATIC_VD_DATE))->SetWindowText(m_sCurrentDefDate);
    ((CWnd*) GetDlgItem(IDC_STATIC_VERSION))->SetWindowText(m_sCurrentDefVersion);

    // DEBUG Check current thread 12/29/99
    SaveCurrentThreadId();

    // SET THE DEFAULT LANG FONT
    mySetDefaultFont( GetSafeHwnd() );
	
    // INSERT DCOM CODE TO GET LIVE SETTINGS FROM THE SERVER
    if(m_pSConfigData!=NULL) 
    {
        // DID WE READ VALID DATA FROM THE SERVER?
        if( m_pSConfigData->ContainsValidData() )
        {
	        m_bCheckBlessedBroadcast    = m_pSConfigData->m_bCheckBlessedBroadcast   ;       
            m_bCheckUnblessedBroadcast  = m_pSConfigData->m_bCheckUnblessedBroadcast ;     
            m_bCheckUnblessedNarrowCast = m_pSConfigData->m_bCheckUnblessedNarrowCast;    
            m_bCheckUnblessedPointCast  = m_pSConfigData->m_bCheckUnblessedPointCast ;     
            // m_iDefDeliveryTimeout       = m_pSConfigData->m_iDefDeliveryTimeout      ;          
            m_iDefDeliveryInterval      = m_pSConfigData->m_iDefDeliveryInterval     ;         
            m_sDefBlessedTargets        = m_pSConfigData->m_sDefBlessedTargets;  
            m_sDefUnblessedTargets      = m_pSConfigData->m_sDefUnblessedTargets;
            m_sDefSecureUsername        = m_pSConfigData->m_sDefSecureUsername;
            m_sDefSecurePassword        = m_pSConfigData->m_sDefSecurePassword;
            m_sDefSecureConfirm         = m_pSConfigData->m_sDefSecurePassword;

            AvisInstallDefsConfig.bCheckBlessedBroadcast    = m_bCheckBlessedBroadcast;      
            AvisInstallDefsConfig.bCheckUnblessedBroadcast  = m_bCheckUnblessedBroadcast;    
            AvisInstallDefsConfig.bCheckUnblessedNarrowCast = m_bCheckUnblessedNarrowCast;   
            AvisInstallDefsConfig.bCheckUnblessedPointCast  = m_bCheckUnblessedPointCast;    
            // AvisInstallDefsConfig.iDefDeliveryTimeout       = m_iDefDeliveryTimeout;         
            AvisInstallDefsConfig.iDefDeliveryInterval      = m_iDefDeliveryInterval;        
            m_sLastDefBlessedTargets                        = m_sDefBlessedTargets;  
            m_sLastDefUnblessedTargets                      = m_sDefUnblessedTargets;
			m_sLastDefSecureUsername                        = m_sDefSecureUsername;
			m_sLastDefSecurePassword                        = m_sDefSecurePassword;
        }
    }

    // // SET UP SPIN CONTROL FOR   m_iDefDeliveryTimeout
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEFS_DELIVERY_TIMEOUT))->SetRange32(DEF_MIN_SPIN_DELIVERY_TIMEOUT, DEF_MAX_SPIN_DELIVERY_TIMEOUT);
    // // Set the buddy
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEFS_DELIVERY_TIMEOUT))->SetBuddy( GetDlgItem(IDC_EDIT_DEF_DELIVERY_TIMEOUT) );
    // // Set initial pos
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEFS_DELIVERY_TIMEOUT))->SetPos(m_iDefDeliveryTimeout);

    // SET UP SPIN CONTROL FOR  m_iDefDeliveryInterval     
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEFS_DELIVERY_INTERVAL))->SetRange32(DEF_MIN_SPIN_DELIVERY_INTERVAL, DEF_MAX_SPIN_DELIVERY_INTERVAL);
    // Set the buddy
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEFS_DELIVERY_INTERVAL))->SetBuddy( GetDlgItem(IDC_EDIT_DEF_DELIVERY_INTERVAL) );
    // Set initial pos
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEFS_DELIVERY_INTERVAL))->SetPos(m_iDefDeliveryInterval);



    UpdateData( FALSE );         // FORCE IT TO REDISPLAY THE DATA

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CInstallDefinitions::OnApply() 
{
    // Make sure data is good.
    UpdateData(TRUE);

    // DEBUG Check current thread 12/29/99
    SaveCurrentThreadId();

	if ( ! ( m_sDefSecurePassword == m_sDefSecureConfirm ) )
	{
		AfxMessageBox(IDS_CONFIRM_PASSWORD_NO_MATCH);

		// Empty the new password strings.
		m_sDefSecurePassword = _T("");
		m_sDefSecureConfirm = _T("");

		UpdateData( FALSE );

		// Set the focus back to the new password edit box.
		GetDlgItem(IDC_PASSWORD_EDIT)->SetFocus();

		// Return from this dialog without closing it.  
		// Allow the user to try again.
		return FALSE;
	}

    SaveChangedData(); 

    // 12/16/99
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
	
	return CPropertyPage::OnApply();
}

/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CInstallDefinitions::SaveChangedData() 
{
	DWORD dwDataChanged=0;

    // m_bCheckBlessedBroadcast   
    if(AvisInstallDefsConfig.bCheckBlessedBroadcast != m_bCheckBlessedBroadcast)
    {
        AvisInstallDefsConfig.bCheckBlessedBroadcast = m_bCheckBlessedBroadcast;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_bCheckBlessedBroadcast =  m_bCheckBlessedBroadcast;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEF_CHECK_BLESSED_BROADCAST );
    }
    // m_bCheckUnblessedBroadcast   
    if(AvisInstallDefsConfig.bCheckUnblessedBroadcast != m_bCheckUnblessedBroadcast)
    {
        AvisInstallDefsConfig.bCheckUnblessedBroadcast = m_bCheckUnblessedBroadcast;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_bCheckUnblessedBroadcast =  m_bCheckUnblessedBroadcast;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEF_CHECK_UNBLESSED_BROADCAST );
    }
    // m_bCheckUnblessedNarrowCast   
    if(AvisInstallDefsConfig.bCheckUnblessedNarrowCast != m_bCheckUnblessedNarrowCast)
    {
        AvisInstallDefsConfig.bCheckUnblessedNarrowCast = m_bCheckUnblessedNarrowCast;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_bCheckUnblessedNarrowCast =  m_bCheckUnblessedNarrowCast;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEF_CHECK_UNBLESSED_NARROWCAST );
    }
    // m_bCheckUnblessedPointCast   
    if(AvisInstallDefsConfig.bCheckUnblessedPointCast != m_bCheckUnblessedPointCast)
    {
        AvisInstallDefsConfig.bCheckUnblessedPointCast = m_bCheckUnblessedPointCast;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_bCheckUnblessedPointCast =  m_bCheckUnblessedPointCast;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEF_CHECK_UNBLESSED_POINTCAST );
    }

	// m_sDefSecureUsername
    if(m_sLastDefSecureUsername != m_sDefSecureUsername)
    {
        m_sLastDefSecureUsername = m_sDefSecureUsername;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_sDefSecureUsername =  m_sDefSecureUsername;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEF_SECURE_USERNAME );
    }

	// m_sDefSecurePassword
    if(m_sLastDefSecurePassword != m_sDefSecurePassword)
    {
        m_sLastDefSecurePassword = m_sDefSecurePassword;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_sDefSecurePassword =  m_sDefSecurePassword;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEF_SECURE_PASSWORD );
    }

	// // m_iDefDeliveryTimeout   
    // if(AvisInstallDefsConfig.iDefDeliveryTimeout != m_iDefDeliveryTimeout)
    // {
    //     AvisInstallDefsConfig.iDefDeliveryTimeout = m_iDefDeliveryTimeout;
    //     dwDataChanged = TRUE;
    //     // SAVE IT TO CServerConfigData
    //     m_pSConfigData->m_iDefDeliveryTimeout =  m_iDefDeliveryTimeout;
    //     // SET FLAG FOR THE WRITE ROUTINE
    //     m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEF_DELIVERY_TIMEOUT );
    // }
    //  m_iDefDeliveryInterval  
    if(AvisInstallDefsConfig.iDefDeliveryInterval != m_iDefDeliveryInterval)
    {
        AvisInstallDefsConfig.iDefDeliveryInterval = m_iDefDeliveryInterval;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_iDefDeliveryInterval =  m_iDefDeliveryInterval;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEF_DELIVERY_INTERVAL );
    }
    //  m_sDefBlessedTargets  
    if( m_sLastDefBlessedTargets != m_sDefBlessedTargets )
    {
        m_sLastDefBlessedTargets = m_sDefBlessedTargets;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_sDefBlessedTargets =  m_sDefBlessedTargets;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEF_BLESSED_TARGETS );
    }
    //  m_sDefUnblessedTargets  
    if( m_sLastDefUnblessedTargets != m_sDefUnblessedTargets )
    {
        m_sLastDefUnblessedTargets = m_sDefUnblessedTargets;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_sDefUnblessedTargets =  m_sDefUnblessedTargets;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEF_UNBLESSED_TARGETS );
    }



    // NEED TO WRITE IT?
    if(dwDataChanged)  
        m_pSConfigData->SetDirty(TRUE);

}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CInstallDefinitions::OnKillActive() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 

    BOOL   bRet  = 0;
    CString sTitle, sText;
    sTitle.LoadString( IDS_CONFIG_ERROR );
    int iControl = 0;
	
    //  m_iDefDeliveryTimeout  
    // int iDefDeliveryTimeout = GetDlgItemInt(IDC_EDIT_DEF_DELIVERY_TIMEOUT , &bRet, FALSE );
    // if( !bRet || ( iDefDeliveryTimeout < DEF_MIN_SPIN_DELIVERY_TIMEOUT || iDefDeliveryTimeout > DEF_MAX_SPIN_DELIVERY_TIMEOUT ) )
    // {
    //     // Inform the user 
    //     sText.Format( IDS_BADFMT_DEFS_DELIVERY_TIMEOUT, DEF_MIN_SPIN_DELIVERY_TIMEOUT, DEF_MAX_SPIN_DELIVERY_TIMEOUT );
    //     MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
    //     iControl = IDC_EDIT_DEF_DELIVERY_TIMEOUT;
    // }


    //  m_iDefDeliveryInterval  
    int iDefDeliveryInterval = GetDlgItemInt( IDC_EDIT_DEF_DELIVERY_INTERVAL, &bRet, FALSE );
    if( !bRet || ( iDefDeliveryInterval < DEF_MIN_SPIN_DELIVERY_INTERVAL || iDefDeliveryInterval > DEF_MAX_SPIN_DELIVERY_INTERVAL ) )
    {
        // Inform the user 
        sText.Format( IDS_BADFMT_DEFS_DELIVERY_INTERVAL, DEF_MIN_SPIN_DELIVERY_INTERVAL, DEF_MAX_SPIN_DELIVERY_INTERVAL );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_EDIT_DEF_DELIVERY_INTERVAL;
    }


    // RESET THE CONTROL
    if( iControl )
    {
        GetDlgItem( iControl )->SetFocus();
        ((CEdit*) GetDlgItem( iControl) )->SetSel( 0, -1 );
        return FALSE;
    }

	return CPropertyPage::OnKillActive();
}


/*----------------------------------------------------------------------------
    Written by: Jim Hill                     m_sDefBlessedTargets
 ----------------------------------------------------------------------------*/
void CInstallDefinitions::OnButtonBlesedSelectTargets() 
{

    CString              sTargetCommaList = m_sDefBlessedTargets;
    CSelectTargetDialog  dlg;

    dlg.Initialize(IDS_SELECT_BLESSED_TARGET_TITLE,
                   IDS_SELECT_BLESSED_TARGET_HEADING,
                   IDS_SELECT_BLESSED_TARGET_COL1, 
                   sTargetCommaList,
                   0,              // m_iTotalObjects,
                   0,              // dwDefaultHelpID
                   g_SelectTargetDialogHelpIdArray );         



    int nResponse = dlg.DoModal();

    if(nResponse == IDOK && dlg.m_bListModified ) //&& dlg.m_iTotalReturnedItems == 0
    {
        //sTargetCommaList = dlg.m_sTargetStrings;

        if( dlg.m_sTargetStrings != m_sDefBlessedTargets )
        {
            m_sDefBlessedTargets = dlg.m_sTargetStrings;
        	SetModified(TRUE);
        }
    }

    return;
}


/*----------------------------------------------------------------------------
    Written by: Jim Hill                   m_sDefUnblessedTargets  
 ----------------------------------------------------------------------------*/
void CInstallDefinitions::OnButtonUnblesedSelectTargets() 
{
    CString              sTargetCommaList = m_sDefUnblessedTargets;
    CSelectTargetDialog  dlg;

    dlg.Initialize(IDS_SELECT_UNBLESSED_TARGET_TITLE,
                   IDS_SELECT_UNBLESSED_TARGET_HEADING,
                   IDS_SELECT_UNBLESSED_TARGET_COL1, 
                   sTargetCommaList,
                   0,              // m_iTotalObjects,
                   0,              // dwDefaultHelpID
                   g_SelectTargetDialogHelpIdArray );      



    int nResponse = dlg.DoModal();

    if(nResponse == IDOK && dlg.m_bListModified ) //&& dlg.m_iTotalReturnedItems == 0
    {
        if( dlg.m_sTargetStrings != m_sDefUnblessedTargets )
        {
            m_sDefUnblessedTargets = dlg.m_sTargetStrings;
        	SetModified(TRUE);
        }
    }

	return;
}






void CInstallDefinitions::OnCheckBlessedBroadcast() 
{
	SetModified(TRUE);
}

void CInstallDefinitions::OnCheckUnblessedBroadcast() 
{
	SetModified(TRUE);
}

void CInstallDefinitions::OnCheckUnblessedNarrowcast() 
{
	SetModified(TRUE);
}

void CInstallDefinitions::OnCheckUnblessedPointcast() 
{
	SetModified(TRUE);
}

void CInstallDefinitions::OnChangeEditDefDeliveryInterval() 
{
	SetModified(TRUE);
}

void CInstallDefinitions::OnChangeEditDefUsername() 
{
	SetModified(TRUE);
}

void CInstallDefinitions::OnChangeEditDefPassword() 
{
	SetModified(TRUE);
}

void CInstallDefinitions::OnChangeEditDefConfirm() 
{
	SetModified(TRUE);
}

// void CInstallDefinitions::OnChangeEditDefDeliveryTimeout() 
// {
// 	SetModified(TRUE);
// }





// F1 KEY PRESSED OR WHAT-IS ? CLICKED ON A FIELD
BOOL CInstallDefinitions::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    DoHelpPropSheetF1( IDD, m_lpHelpIdArray, pHelpInfo );
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}



// PROP SHEET HELP BUTTON
BOOL CInstallDefinitions::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

void CInstallDefinitions::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    pContextWnd = pWnd;
    DoHelpDisplayContextMenuWhatsThis( IDD, &nResID, m_lpHelpIdArray, m_hWnd, pWnd, point, 0, &hTargetWnd );
    return;
}

#if 0
void CInstallDefinitions::OnWhatsThisMenuitem() 
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
