/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


// AvisServerConfig1.cpp : implementation file
//
/*--------------------------------------------------------------------
   AvisServerConfig1.cpp : implementation file

   Property Page Tab Title: "Avis Policies"

   Written by: Jim Hill                 
--------------------------------------------------------------------*/

#include "stdafx.h" 
#include "resource.h"
#include "AvisConsole.h"
#include "AServerDefPolicyConfig.h"
#include "InputRange.h"
#include "mmc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





// Persistant data
// Define
typedef struct tagAVISDATADEFPOLICYCONFIG
{
	TCHAR   szDefLibraryPath[POLICY_DEFAULT_DEFLIBRARY_PATH_MAXLEN+1];
	TCHAR   szActiveDefsSeqNum[255];
	DWORD	dwActiveDefsSeqNum;
    BOOL    bDefinitionActiveBlessed;
	int		iDefHeuristicLevel;
	int		iUnpackTimeout;
	int		iDefsBlessedInterval;
	int		iDefsNeededInterval;
	int		iPruneDefs;
    BOOL    bPruneDefs;

}   AVIS_DATA_DEF_POLICY_CONFIG;
// Declare
AVIS_DATA_DEF_POLICY_CONFIG  AvisDefPolicyConfig={0};



/////////////////////////////////////////////////////////////////////////////
// CAvisServerDefPolicyConfig property page

IMPLEMENT_DYNCREATE(CAvisServerDefPolicyConfig, CPropertyPage)

CAvisServerDefPolicyConfig::CAvisServerDefPolicyConfig() : CPropertyPage(CAvisServerDefPolicyConfig::IDD)
{
	//{{AFX_DATA_INIT(CAvisServerDefPolicyConfig)
	m_iDefsBlessedInterval = 0;
	m_sActiveDefsSeqNum = _T("");
	//}}AFX_DATA_INIT

    // INITIALIZE
    m_pSnapin       =NULL;
    m_dwRef         = 1;
    m_pSConfigData  = NULL;
    m_lpHelpIdArray = g_DefinitionPolicyHelpIdArray;  // LOAD THE HELP MAP


    //fWidePrintString("CAvisServerDefPolicyConfig Constructor called  ObjectID= %d", m_iExtDataObjectID);

}

CAvisServerDefPolicyConfig::~CAvisServerDefPolicyConfig()
{

	// Free up the memory from initializing the strings  3/31
	//m_szDefLibraryPath.Empty();

    //fWidePrintString("CAvisServerDefPolicyConfig Destructor called  ObjectID= %d", m_iExtDataObjectID);
}


void CAvisServerDefPolicyConfig::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAvisServerDefPolicyConfig)
	DDX_Control(pDX, IDC_SPIN_DEFS_BLESSED_INTERVAL, m_SpinDefsBlessedInterval);
	DDX_Text(pDX, IDC_EDIT_DEFS_BLESSED_INTERVAL, m_iDefsBlessedInterval);
	DDX_Text(pDX, IDC_ACTIVE_DEFS_SEQ_NUM, m_sActiveDefsSeqNum);
	DDV_MaxChars(pDX, m_sActiveDefsSeqNum, 259);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAvisServerDefPolicyConfig, CPropertyPage)
	//{{AFX_MSG_MAP(CAvisServerDefPolicyConfig)
	ON_WM_HELPINFO()
	ON_WM_CONTEXTMENU()
	ON_EN_CHANGE(IDC_EDIT_DEFS_BLESSED_INTERVAL, OnChangeEditDefsBlessedInterval)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAvisServerDefPolicyConfig message handlers



/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerDefPolicyConfig::PostNcDestroy() 
{
	CPropertyPage::PostNcDestroy();
    Release();       
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAvisServerDefPolicyConfig::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// TODO: Add extra initialization here
    CString  szDisplayName;
    m_dwRef++;	

    // SET THE DEFAULT LANG FONT
    mySetDefaultFont( GetSafeHwnd() );

    // INSERT DCOM CODE TO GET LIVE SETTINGS FROM THE SERVER
    if(m_pSConfigData!=NULL) 
    {
        // hr = m_pSConfigData->ReadConfigDataFromServer();
        // DID WE READ VALID DATA FROM THE SERVER?
        if( m_pSConfigData->ContainsValidData() )
        {
            m_iDefsBlessedInterval       = m_pSConfigData->m_iDefsBlessedInterval;    
            m_sActiveDefsSeqNum          = m_pSConfigData->m_sActiveDefsSeqNum;
            // m_szDefLibraryPath           = m_pSConfigData->m_szDefLibraryPath           ;
            // m_iDefHeuristicLevel         = m_pSConfigData->m_iDefHeuristicLevel;      
            // m_iUnpackTimeout             = m_pSConfigData->m_iUnpackTimeout;          
            // m_iDefsNeededInterval        = m_pSConfigData->m_iDefsNeededInterval;     
            // if( m_pSConfigData->m_iPruneDefs)    
            //        m_bPruneDefs = TRUE;
            // m_sCurrentBlessedDefsSeqNum  = m_pSConfigData->m_sCurrentBlessedDefsSeqNum;


            // CHECK THE READ ONLY STRINGS
            if( m_sActiveDefsSeqNum.IsEmpty() )
                m_sActiveDefsSeqNum.LoadString(IDS_NA_ACTIVE_DEFS_SEQ_NUM);
            // if( m_sCurrentBlessedDefsSeqNum.IsEmpty() )
            //     m_sCurrentBlessedDefsSeqNum.LoadString(IDS_NA_ACTIVE_DEFS_SEQ_NUM);


            // UPDATE THE GLOBALS TO CHECK FOR CHANGES ON OK
            AvisDefPolicyConfig.iDefsBlessedInterval      = m_iDefsBlessedInterval;    
            // _tcscpy(AvisDefPolicyConfig.szDefLibraryPath, m_szDefLibraryPath);              
            // AvisDefPolicyConfig.iDefHeuristicLevel        = m_iDefHeuristicLevel;      
            // AvisDefPolicyConfig.iUnpackTimeout            = m_iUnpackTimeout;          
            // AvisDefPolicyConfig.iDefsNeededInterval       = m_iDefsNeededInterval;     
            // AvisDefPolicyConfig.bPruneDefs                = m_bPruneDefs;              

        }
    }

    // SET UP SPIN CONTROL FOR    m_iDefsBlessedInterval
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEFS_BLESSED_INTERVAL))->SetRange32(DEF_MIN_SPIN_DEFS_BLESSED_INTERVAL, DEF_MAX_SPIN_DEFS_BLESSED_INTERVAL);
    // Set the buddy
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEFS_BLESSED_INTERVAL))->SetBuddy( GetDlgItem(IDC_EDIT_DEFS_BLESSED_INTERVAL) );
    // Set initial pos
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEFS_BLESSED_INTERVAL))->SetPos(m_iDefsBlessedInterval);

    // // SET UP SPIN CONTROL FOR   m_iDefHeuristicLevel
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEF_HEURISTIC_LEVEL))->SetRange32(DEF_MIN_SPIN_DEFS_HEURISTIC_LEVEL, DEF_MAX_SPIN_DEFS_HEURISTIC_LEVEL);
    // // Set the buddy
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEF_HEURISTIC_LEVEL))->SetBuddy( GetDlgItem(IDC_EDIT_DEF_HEURISTIC_LEVE2) );
    // // Set initial pos
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEF_HEURISTIC_LEVEL))->SetPos(m_iDefHeuristicLevel);
    // 
    // // SET UP SPIN CONTROL FOR   m_iUnpackTimeout
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEF_UNPACK_TIMEOUT))->SetRange32(DEF_MIN_SPIN_UNPACK_TIMEOUT, DEF_MAX_SPIN_UNPACK_TIMEOUT);
    // // Set the buddy
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEF_UNPACK_TIMEOUT))->SetBuddy( GetDlgItem(IDC_EDIT_DEF_UNPACK_TIMEOUT) );
    // // Set initial pos
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEF_UNPACK_TIMEOUT))->SetPos(m_iUnpackTimeout);
    // 
    // // SET UP SPIN CONTROL FOR    m_iDefsNeededInterval
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEFS_NEEDED_INTERVAL2))->SetRange32(WEB_MIN_SPIN_DEFS_NEEDED_INTERVAL, WEB_MAX_SPIN_DEFS_NEEDED_INTERVAL);
    // // Set the buddy
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEFS_NEEDED_INTERVAL2))->SetBuddy( GetDlgItem(IDC_EDIT_DEFS_NEEDED_INTERVAL2) );
    // // Set initial pos
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_DEFS_NEEDED_INTERVAL2))->SetPos(m_iDefsNeededInterval);


    UpdateData( FALSE );         // FORCE IT TO REDISPLAY THE DATA


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerDefPolicyConfig::OnOK() 
{
	CPropertyPage::OnOK();
}   


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAvisServerDefPolicyConfig::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class

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


	return CPropertyPage::OnApply();
}


 
/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerDefPolicyConfig::SaveChangedData() 
{
	DWORD dwDataChanged=0;

    // SAVE THE PERSISTANT DATA THAT HAS BEEN CHANGED
    //  m_iDefsBlessedInterval  
    if(AvisDefPolicyConfig.iDefsBlessedInterval != m_iDefsBlessedInterval)
    {
        AvisDefPolicyConfig.iDefsBlessedInterval = m_iDefsBlessedInterval;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_iDefsBlessedInterval = m_iDefsBlessedInterval;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEFS_BLESSED_INTERVAL );
    }


    // // DEFINITION LIBRARY PATH
    // if(m_szDefLibraryPath.Compare((LPCTSTR) AvisDefPolicyConfig.szDefLibraryPath) != 0)
    // {
    //     //fWidePrintString("DATA SAVED CAvisServerDefPolicyConfig::m_szDefLibraryPath  Old=%s  New=%s",
    //     //                 (LPCTSTR) AvisDefPolicyConfig.szDefLibraryPath,(LPCTSTR) m_szDefLibraryPath);
    //     _tcscpy(AvisDefPolicyConfig.szDefLibraryPath, m_szDefLibraryPath);
    //     dwDataChanged = TRUE;
    //     // SAVE IT TO CServerConfigData
    //     m_pSConfigData->m_szDefLibraryPath = m_szDefLibraryPath;
    //     // SET FLAG FOR THE WRITE ROUTINE
    //     m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEFLIBRARY_FOLDER );
    // }
    // 
    // //  m_iDefHeuristicLevel
    // if(AvisDefPolicyConfig.iDefHeuristicLevel != m_iDefHeuristicLevel)
    // {
    //     AvisDefPolicyConfig.iDefHeuristicLevel = m_iDefHeuristicLevel;
    //     dwDataChanged = TRUE;
    //     // SAVE IT TO CServerConfigData
    //     m_pSConfigData->m_iDefHeuristicLevel = m_iDefHeuristicLevel;
    //     // SET FLAG FOR THE WRITE ROUTINE
    //     m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEFS_HEURISTIC_LEVEL );
    // }
    // 
    // //  m_iUnpackTimeout
    // if(AvisDefPolicyConfig.iUnpackTimeout != m_iUnpackTimeout)
    // {
    //     AvisDefPolicyConfig.iUnpackTimeout = m_iUnpackTimeout;
    //     dwDataChanged = TRUE;
    //     // SAVE IT TO CServerConfigData
    //     m_pSConfigData->m_iUnpackTimeout = m_iUnpackTimeout;
    //     // SET FLAG FOR THE WRITE ROUTINE
    //     m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEFS_UNPACK_TIMEOUT );
    // }
    // 
    // //  m_iDefsNeededInterval   
    // if(AvisDefPolicyConfig.iDefsNeededInterval != m_iDefsNeededInterval)
    // {
    //     AvisDefPolicyConfig.iDefsNeededInterval = m_iDefsNeededInterval;
    //     dwDataChanged = TRUE;
    //     // SAVE IT TO CServerConfigData
    //     m_pSConfigData->m_iDefsNeededInterval = m_iDefsNeededInterval;
    //     // SET FLAG FOR THE WRITE ROUTINE
    //     m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEFS_NEEDED_INTERVAL );
    // }
    // 
    // //  m_iPruneDefs  
    // if(AvisDefPolicyConfig.bPruneDefs != m_bPruneDefs)
    // {
    //     AvisDefPolicyConfig.bPruneDefs = m_bPruneDefs;
    //     dwDataChanged = TRUE;
    //     // SAVE IT TO CServerConfigData
    //     m_pSConfigData->m_iPruneDefs = (INT) m_bPruneDefs;
    //     // SET FLAG FOR THE WRITE ROUTINE
    //     m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEFS_DEFS_PRUNE );
    // }
     
    // NEED TO WRITE IT?
     if(dwDataChanged)  
        m_pSConfigData->SetDirty(TRUE);

    return;
}  




#if 0
/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerDefPolicyConfig::OnFiledialogButton1() 
{
	// TODO: Add your control notification handler code here
	BROWSEINFO  bi;
	TCHAR       szPath[MAX_PATH];
	CString     csCaption;
	IMalloc     *pMalloc;
	HRESULT     res, hr;
	LPITEMIDLIST lpiil;
	LPITEMIDLIST lpiilNetNeighborhood = NULL;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());	  // jhill 4/8/99
    szPath[0] = '\0';

	res = SHGetMalloc(&pMalloc);
	ASSERT(res == NOERROR);
	csCaption.LoadString(IDS_BROWSEDIRS_STRING);
	bi.hwndOwner = GetSafeHwnd();
	bi.pidlRoot = NULL;                  

    // SPECIFY NETWORK NEIGHBORHOOD ONLY 7/27/99
    // hr = SHGetSpecialFolderLocation( bi.hwndOwner, CSIDL_NETWORK, &lpiilNetNeighborhood );
    // LOCAL DRIVE ONLY 12/18/99
    hr = SHGetSpecialFolderLocation( bi.hwndOwner, CSIDL_DRIVES, &lpiilNetNeighborhood );
	if( hr == NOERROR)
        bi.pidlRoot = lpiilNetNeighborhood;

	bi.pszDisplayName = szPath;
	bi.lpszTitle = (LPCTSTR)csCaption;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX ;
	bi.lpfn = NULL;
	bi.lParam = NULL;
	lpiil = SHBrowseForFolder(&bi);
	if(!lpiil)
		return;
	SHGetPathFromIDList(lpiil, szPath);

	if ( szPath[0] != '\0') 
    {
	    m_szDefLibraryPath = szPath;
		GetDlgItem( IDC_DEFINITION_LIBRARY_PATH )->SetWindowText( szPath );
    }

	//UpdateData(FALSE);   jhill 4/20/99
	SetModified(TRUE);

    // SET FLAG FOR THE WRITE ROUTINE
    // if( m_pSConfigData )
    //     m_pSConfigData->SetFieldDirty( FIELD_CHANGE_DEFLIBRARY_FOLDER );

	if(res != NOERROR)
		return;
	pMalloc->Free(lpiil);
    if( lpiilNetNeighborhood != NULL )
    	pMalloc->Free( lpiilNetNeighborhood );

    return;
}
#endif



/*----------------------------------------------------------------------------

   Written by: Jim Hill
   Code borrowed from QConsole
----------------------------------------------------------------------------*/
BOOL CAvisServerDefPolicyConfig::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 


    BOOL   bRet  = 0;
    CString sTitle, sText;
    sTitle.LoadString( IDS_CONFIG_ERROR );
    int iControl = 0;
    
    // Now, get the data
    UpdateData(TRUE);

    //  m_iDefsBlessedInterval  
    int iDefsBlessedInterval = GetDlgItemInt( IDC_EDIT_DEFS_BLESSED_INTERVAL, &bRet, FALSE );
    if( !bRet || ( iDefsBlessedInterval < DEF_MIN_SPIN_DEFS_BLESSED_INTERVAL || iDefsBlessedInterval > DEF_MAX_SPIN_DEFS_BLESSED_INTERVAL ) )
    {
        // Inform the user 
        sText.Format( IDS_BADFMT_DEFS_BLESSED_INTERVAL, DEF_MIN_SPIN_DEFS_BLESSED_INTERVAL, DEF_MAX_SPIN_DEFS_BLESSED_INTERVAL );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_EDIT_DEFS_BLESSED_INTERVAL;
    }


    
    // // Make sure the library path has a valid form
    // if( m_pSConfigData!=NULL && m_pSConfigData->IsValidFolderPath(&m_szDefLibraryPath) == FALSE )
    // {
    //     // Inform the user
    //     sTitle.LoadString( IDS_BAD_FOLDER_TITLE );
    //     sText.LoadString( IDS_BAD_FOLDER_PATH );
    //     MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
    //     iControl = IDC_DEFINITION_LIBRARY_PATH;
    // }
    // 
    // //  m_iDefHeuristicLevel  
    // int iDefHeuristicLevel = GetDlgItemInt( IDC_EDIT_DEF_HEURISTIC_LEVE2, &bRet, FALSE );
    // if( !bRet || ( iDefHeuristicLevel < DEF_MIN_SPIN_DEFS_HEURISTIC_LEVEL || iDefHeuristicLevel > DEF_MAX_SPIN_DEFS_HEURISTIC_LEVEL ) )
    // {
    //     // Inform the user 
    //     sText.Format( IDS_BADFMT_DEFS_HEURISTIC_LEVEL, DEF_MIN_SPIN_DEFS_HEURISTIC_LEVEL, DEF_MAX_SPIN_DEFS_HEURISTIC_LEVEL );
    //     MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
    //     iControl = IDC_EDIT_DEF_HEURISTIC_LEVE2;
    // }
    // 
    // //  m_iUnpackTimeout   
    // int iUnpackTimeout = GetDlgItemInt( IDC_EDIT_DEF_UNPACK_TIMEOUT, &bRet, FALSE );
    // if( !bRet || ( iUnpackTimeout < DEF_MIN_SPIN_UNPACK_TIMEOUT || iUnpackTimeout > DEF_MAX_SPIN_UNPACK_TIMEOUT ) )
    // {
    //     // Inform the user 
    //     sText.Format( IDS_BADFMT_DEFS_UNPACK_TIMEOUT, DEF_MIN_SPIN_UNPACK_TIMEOUT, DEF_MAX_SPIN_UNPACK_TIMEOUT );
    //     MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
    //     iControl = IDC_EDIT_DEF_UNPACK_TIMEOUT;
    // }
    // 
    // //  m_iDefsNeededInterval   "Unblessed definitions interval"
    // int iDefsNeededInterval = GetDlgItemInt( IDC_EDIT_DEFS_NEEDED_INTERVAL2, &bRet, FALSE );
    // if( !bRet || ( iDefsNeededInterval < WEB_MIN_SPIN_DEFS_NEEDED_INTERVAL || iDefsNeededInterval > WEB_MAX_SPIN_DEFS_NEEDED_INTERVAL ) )
    // {
    //     // Inform the user 
    //     sText.Format( IDS_BADFMT_DEFS_NEEDED_INTERVAL, WEB_MIN_SPIN_DEFS_NEEDED_INTERVAL, WEB_MAX_SPIN_DEFS_NEEDED_INTERVAL );
    //     MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
    //     iControl = IDC_EDIT_DEFS_NEEDED_INTERVAL2;
    // }


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

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerDefPolicyConfig::OnChangeEditDefsBlessedInterval() 
{
	SetModified(TRUE);
}


// void CAvisServerDefPolicyConfig::OnChangeDefinitionLibraryPath() 
// {
// 	SetModified(TRUE);
// }
// 
// void CAvisServerDefPolicyConfig::OnChangeEditDefHeuristicLeve2() 
// {
// 	SetModified(TRUE);
// }
// 
// void CAvisServerDefPolicyConfig::OnChangeEditDefUnpackTimeout() 
// {
// 	SetModified(TRUE);
// }
// 
// void CAvisServerDefPolicyConfig::OnChangeEditDefsNeededInterval2() 
// {
// 	SetModified(TRUE);
// }
// 
// void CAvisServerDefPolicyConfig::OnCheckPruneDefs() 
// {
// 	SetModified(TRUE);
// }










// HELP HANDLERS AND ROUTINES 

/*----------------------------------------------------------------------------
   CAvisServerDefPolicyConfig::OnHelpInfo()
   F1 KEY PRESSED OR WHAT-IS ? CLICKED ON A FIELD
   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAvisServerDefPolicyConfig::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
    DoHelpPropSheetF1( IDD_AVIS_POLICY_PAGE, m_lpHelpIdArray, pHelpInfo );
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}


/*----------------------------------------------------------------------------
   CAvisServerDefPolicyConfig::OnNotify()
   PROP SHEET HELP BUTTON
   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAvisServerDefPolicyConfig::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    NMHDR* pNMHDR = (NMHDR*)lParam;
    DoHelpPropSheetHelpButton(IDD_AVIS_POLICY_PAGE, m_lpHelpIdArray, pNMHDR );
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}


static DWORD nResID = 0;
static CWnd* pContextWnd = NULL;
static DWORD hTargetWnd = 0;

/*----------------------------------------------------------------------------
   CAvisServerDefPolicyConfig::OnContextMenu()

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerDefPolicyConfig::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    pContextWnd = pWnd;
    DoHelpDisplayContextMenuWhatsThis( IDD_AVIS_POLICY_PAGE, &nResID, m_lpHelpIdArray, m_hWnd, pWnd, point, 0, &hTargetWnd );
    return;
}
#if 0
/*----------------------------------------------------------------------------
   CAvisServerDefPolicyConfig::OnWhatsThisMenuitem()

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerDefPolicyConfig::OnWhatsThisMenuitem() 
{
	// TODO: Add your command handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HWND hWnd = (HWND) hTargetWnd;
    hTargetWnd= 0;
    int nResourceID = nResID;
    nResID = 0;                  // CLEAR IT
    if( nResourceID == 0)
        nResourceID = IDD_AVIS_POLICY_PAGE;
    DoHelpPropSheetContextHelp( nResourceID, m_lpHelpIdArray, pContextWnd, hWnd );
    pContextWnd = NULL;
    return;
}

#endif
