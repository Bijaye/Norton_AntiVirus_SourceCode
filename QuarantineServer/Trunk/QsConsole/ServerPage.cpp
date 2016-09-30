/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// ServerPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ServerPage.h"
#include "qsregval.h"
#include "qscon.h"
#include "qsconsole.h"
#include "mmc.h"
#include "qshelp.h"
#include "qscommon.h"
#include "qsregval.h"
#include "PurgeLog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifndef  STRTOUL
    //#define STRTOUL(n,e,b)      wcstoul(n,e,b)
    #define STRTOUL(n,e,b)        _tcstoul(n,e,b)
#endif

// 1-3CPYEN - define literal for max samples
#define MAX_FILES_MAX		5000


DWORD CServerPage::m_dwWhatsThisMap[] = { 
	IDC_FOLDER_STATIC, IDH_Quar_Server_Folder,
    IDC_Q_FOLDER, IDH_Quar_Server_Folder, 
	IDC_MAX_STATIC, IDH_Quar_Server_Max_Size,
	IDC_SPIN1, IDH_Quar_Server_Max_Size,
	IDC_QFOLDER_SIZE, IDH_Quar_Server_Max_Size,
	IDC_MEGABYTES_STATIC, IDH_Quar_Server_Max_Size,
	IDC_IP_STATIC, IDH_Quar_Server_PortIP,
	IDC_IP_PORT, IDH_Quar_Server_PortIP,
	IDC_IP_CHECK, 	IDH_Quar_Server_ListenIP,
	IDC_IPX_CHECK, IDH_Quar_Server_ListenSPX,
	IDC_IPX_STATIC, IDH_Quar_Server_PortSPX,
	IDC_SPX_PORT, IDH_Quar_Server_PortSPX,
    IDC_EDIT_QFOLDER_WARNING_SIZE, IDH_Quar_Server_Warning_size,
    IDC_MEGABYTES_STATIC2, IDH_Quar_Server_Warning_size,
    IDC_QFOLDER_WARNING_SIZE_STATIC, IDH_Quar_Server_Warning_size,
    IDC_SPIN_QFOLDER_WARNING_SIZE, IDH_Quar_Server_Warning_size,
    IDC_REFRESH_EDIT,IDH_Quar_Server_Refresh_Edit,
	IDC_REFRESH_STATIC, IDH_Quar_Server_Refresh_Edit,
	IDC_REFRESH_SPIN, IDH_Quar_Server_Refresh_Edit,
    IDC_QFOLDER_NO_SAMPLES,IDH_Quar_No_Samples_limit,
	IDC_SPIN_NOSAMPLES, IDH_Quar_No_Samples_limit,
	IDC_MAX_NOSAMPLES_STATIC, IDH_Quar_No_Samples_limit,
	IDC_CHECK_PURGE, IDH_Quar_Purge,
	0,0 };


/////////////////////////////////////////////////////////////////////////////
// CServerPage property page

IMPLEMENT_DYNCREATE(CServerPage, CQPropPage)

CServerPage::CServerPage() : CQPropPage(CServerPage::IDD)
{
	//{{AFX_DATA_INIT(CServerPage)
	m_bListenIP = FALSE;
	m_bListenSPX = FALSE;
	m_iIPPort = 4185;
	m_sFolder = _T("");
	m_iSPXPort = 4186;
	m_iRefreshInteveral = 5;
	m_uMaxSize = 500;
	m_uWarningSize = 450;
	m_uMaxNoSamples = MAX_FILES_MAX;
	m_bPurge = FALSE;
	//}}AFX_DATA_INIT

    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    dwFreeSpace = 0;
	m_ifMoveQFolder = -1;
}

CServerPage::~CServerPage()
{
}

void CServerPage::DoDataExchange(CDataExchange* pDX)
{
	CQPropPage::DoDataExchange(pDX);

	//
	// Make sure number text fields have data.
	//
	CString s;
	GetDlgItemText( IDC_QFOLDER_SIZE, s );
	if( s.GetLength() == 0 )
		GetDlgItem( IDC_QFOLDER_SIZE)->SetWindowText(_T("0"));
	GetDlgItemText( IDC_IP_PORT, s );
	if( s.GetLength() == 0 )
		GetDlgItem( IDC_IP_PORT)->SetWindowText(_T("0"));
	GetDlgItemText( IDC_SPX_PORT, s );
	if( s.GetLength() == 0 )
		GetDlgItem( IDC_SPX_PORT)->SetWindowText(_T("0"));

    // jhill 7/18/00
    if( m_iRefreshInteveral < 5 )
        m_iRefreshInteveral = 5;
    if( m_uMaxSize == 0 )
        m_uMaxSize = m_uWarningSize = 1;
    if( m_uWarningSize == 0 )
        m_uWarningSize = 1;


	//{{AFX_DATA_MAP(CServerPage)
	DDX_Control(pDX, IDC_SPIN_NOSAMPLES, m_NoSampleSpin);
	DDX_Control(pDX, IDC_REFRESH_SPIN, m_RefreshSpin);
	DDX_Control(pDX, IDC_REFRESH_EDIT, m_RefreshEdit);
	DDX_Check(pDX, IDC_IP_CHECK, m_bListenIP);
	DDX_Check(pDX, IDC_IPX_CHECK, m_bListenSPX);
	DDX_Text(pDX, IDC_IP_PORT, m_iIPPort);
	DDX_Text(pDX, IDC_Q_FOLDER, m_sFolder);
	DDV_MaxChars(pDX, m_sFolder, 259);
	DDX_Text(pDX, IDC_QFOLDER_SIZE, m_uMaxSize);
	DDX_Text(pDX, IDC_SPX_PORT, m_iSPXPort);
	DDX_Text(pDX, IDC_REFRESH_EDIT, m_iRefreshInteveral);
	DDV_MinMaxInt(pDX, m_iRefreshInteveral, 5, 60);
	DDX_Text(pDX, IDC_EDIT_QFOLDER_WARNING_SIZE, m_uWarningSize);
	DDX_Text(pDX, IDC_QFOLDER_NO_SAMPLES, m_uMaxNoSamples);
	DDV_MinMaxUInt(pDX, m_uMaxNoSamples, 10, MAX_FILES_MAX);
	DDX_Check(pDX, IDC_CHECK_PURGE, m_bPurge);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CServerPage, CQPropPage)
	//{{AFX_MSG_MAP(CServerPage)
	ON_BN_CLICKED(IDC_IP_CHECK, OnIpCheck)
	ON_BN_CLICKED(IDC_IPX_CHECK, OnIpxCheck)
	ON_EN_CHANGE(IDC_Q_FOLDER, OnChangeQFolder)
	ON_EN_CHANGE(IDC_QFOLDER_SIZE, OnChangeQfolderSize)
	ON_EN_CHANGE(IDC_REFRESH_EDIT, OnChangeRefreshEdit)
	ON_EN_CHANGE(IDC_EDIT_QFOLDER_WARNING_SIZE, OnChangeEditQfolderWarningSize)
	ON_EN_CHANGE(IDC_IP_PORT, OnChangeIpPort)
	ON_EN_CHANGE(IDC_SPX_PORT, OnChangeSpxPort)
	ON_EN_CHANGE(IDC_QFOLDER_NO_SAMPLES, OnChangeQfolderNoSamples)
	ON_BN_CLICKED(IDC_CHECK_PURGE, OnCheckPurge)
	ON_BN_CLICKED(IDC_BUTTON_PURGE_LOG, OnButtonPurgeLog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerPage message handlers


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CServerPage::OnIpCheck
//
// Description   : 
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CServerPage::OnIpCheck() 
{
	BOOL bEnable = IsDlgButtonChecked( IDC_IP_CHECK );

    GetDlgItem( IDC_IP_STATIC )->EnableWindow( bEnable );
    GetDlgItem( IDC_IP_PORT )->EnableWindow( bEnable );
	SetModified(TRUE);
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CServerPage::OnIpxCheck
//
// Description   : 
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CServerPage::OnIpxCheck() 
{
	BOOL bEnable = IsDlgButtonChecked( IDC_IPX_CHECK );

    GetDlgItem( IDC_IPX_STATIC )->EnableWindow( bEnable );
    GetDlgItem( IDC_SPX_PORT )->EnableWindow( bEnable );
	SetModified(TRUE);
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CServerPage::OnInitDialog
//
// Description   : 
//
// Return type   : BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CServerPage::OnInitDialog() 
{
    CQPropPage::OnInitDialog();
    BSTR bstrS;


    // Check to see if we have a valid pointer to the interfaces
	_ASSERTE(((CQSConsoleData*)m_pSnapin)->m_cQSInterfaces);
	if (((CQSConsoleData*)m_pSnapin)->m_cQSInterfaces == NULL)
		return FALSE;

	// 
    // Unmarshal server interface.
    // 
    CoGetInterfaceAndReleaseStream( m_pServerStream, __uuidof( IQserverConfig ), (LPVOID*)&m_pConfig );
    ((CQSConsoleData*)m_pSnapin)->m_cQSInterfaces->m_pQServer.DCO_SetProxyBlanket( m_pConfig );


	//
	// Set text limits
	//
	((CEdit*)GetDlgItem( IDC_QFOLDER_SIZE ))->SetLimitText( 4 );
	((CEdit*)GetDlgItem( IDC_IP_PORT ))->SetLimitText( 5 );
	((CEdit*)GetDlgItem( IDC_SPX_PORT ))->SetLimitText( 5 );
	((CEdit*)GetDlgItem( IDC_QFOLDER_NO_SAMPLES))->SetLimitText( 4 );

	//
	// Disable purge functionality if connected to a 1.0 server
	//
	if (((CQSConsoleData*)m_pSnapin)->m_dwQSVersion < QSVERSION2X)// terrym 9-21-01 added support for version 3.0
	{
		((CWnd*)GetDlgItem(IDC_QFOLDER_NO_SAMPLES))->EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_SPIN_NOSAMPLES))->EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_CHECK_PURGE))->EnableWindow(FALSE);
	}
    // 
    // Set up data members.
    // 
	m_iRefreshInteveral = GetRefreshInterval();	
    try
        {
        VARIANT v;
		USES_CONVERSION;

        VariantInit( &v );
        bstrS = SysAllocString(T2COLE(REGVALUE_QUARANTINE_FOLDER));
        if( SUCCEEDED( m_pConfig->GetValue( bstrS, &v ) ) )
            {
            m_sFolder = v.bstrVal;
            m_sOrigQFolder = m_sFolder;
            VariantClear( &v );
            }

        VariantInit( &v );
        SysReAllocString(&bstrS,T2COLE(REGVALUE_PORT));
        if( SUCCEEDED( m_pConfig->GetValue( bstrS, &v ) ) )
            {
            m_iIPPort = (int) v.ulVal;
            }

        VariantInit( &v );
        SysReAllocString(&bstrS,T2COLE(REGVALUE_SPXSOCKET));
        if( SUCCEEDED( m_pConfig->GetValue( bstrS, &v ) ) )
            {
            m_iSPXPort = (int) v.ulVal;
            }

        VariantInit( &v );
        SysReAllocString(&bstrS,T2COLE(REGVALUE_LISTEN_IP));
        if( SUCCEEDED( m_pConfig->GetValue( bstrS, &v ) ) )
            {
            m_bListenIP = (BOOL) (v.ulVal > 0 );
            }

        VariantInit( &v );
        SysReAllocString(&bstrS,T2COLE(REGVALUE_LISTEN_SPX));
        if( SUCCEEDED( m_pConfig->GetValue( bstrS, &v ) ) )
            {
            m_bListenSPX = (BOOL) (v.ulVal > 0 );
            }

        VariantInit( &v );
        SysReAllocString(&bstrS,T2COLE(REGVALUE_MAX_SIZE));
        if( SUCCEEDED( m_pConfig->GetValue( bstrS, &v ) ) )
            {
            m_uMaxSize = v.ulVal;
            }

        VariantInit( &v );
        SysReAllocString(&bstrS,T2COLE(REGVALUE_QFOLDER_WARNING_SIZE));
        if( SUCCEEDED( m_pConfig->GetValue( bstrS, &v ) ) )
        {
            if( v.vt == VT_BSTR )
            {
                CString s = v.bstrVal;
                TCHAR*  endptr=NULL;
                m_uWarningSize = STRTOUL( (LPCTSTR) s, &endptr, 10 );
	            VariantClear( &v );
            }
            else
                m_uWarningSize = (m_uMaxSize * 9)/10;    // jhill 7/18/00

        }
        else
            m_uWarningSize = (m_uMaxSize * 9)/10;    // jhill 7/18/00
        if( m_uWarningSize == 0 )
            m_uWarningSize = 1;

        VariantInit( &v );
        SysReAllocString(&bstrS,T2COLE(REGVALUE_MAX_FILES));
        if( SUCCEEDED( m_pConfig->GetValue( bstrS, &v ) ) )
        {
	        m_uMaxNoSamples = v.ulVal;
        }

        VariantInit( &v );
        SysReAllocString(&bstrS,T2COLE(REGVALUE_PURGE));
        if( SUCCEEDED( m_pConfig->GetValue( bstrS, &v ) ) )
        {
            m_bPurge = v.ulVal;
        }

//         VariantInit( &v );
//         s = REGVALUE_QFOLDER_FREE_SPACE;
//         if( SUCCEEDED( m_pConfig->GetValue( s.AllocSysString(), &v ) ) )
//         {
//             if( v.vt == VT_BSTR )
//             {
//                 CString s = v.bstrVal;
//                 TCHAR*  endptr=NULL;
//                 dwFreeSpace = STRTOUL( (LPCTSTR) s, &endptr, 10 );
//             }
//         }


        // jhill 4/28/00 IF NOT CONFIGURED USE THE CLIENT SETTINGS IF THEY EXIST
        //if( m_iIPPort == 0 && m_iSPXPort == 0 )
        //{
        //}


        }
    catch(...)
        {
        // 
        // Tell the user that something bad happened.
        // 
        CString sError( (LPCTSTR) IDS_ERROR );
        CString sText( (LPCTSTR) IDS_CONFIG_ERROR );
        MessageBox( sText, sError, MB_ICONSTOP | MB_OK );
        }


	SysFreeString(bstrS);
    UpdateData( FALSE );

	// 
    // Set states of controls correctly
    // 
    OnIpxCheck();
    OnIpCheck();
	
    // 
    // Set up spin control.
    // 
    ((CSpinButtonCtrl*) GetDlgItem( IDC_SPIN1 ) )->SetRange(1, 4095 );
    ((CSpinButtonCtrl*) GetDlgItem( IDC_SPIN_QFOLDER_WARNING_SIZE ) )->SetRange(1, 4095 );


    // Set up spin control.
    m_RefreshSpin.SetRange(5,60);
    m_RefreshSpin.SetPos( m_iRefreshInteveral );
    m_NoSampleSpin.SetRange(10,MAX_FILES_MAX);
    m_NoSampleSpin.SetPos( m_uMaxNoSamples );


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CServerPage::OnOK
//
// Description   : 
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CServerPage::OnOK()
{
    // 
    // Save values to server
    //
	SetRefreshInterval(m_iRefreshInteveral);	
    BSTR bstrS=NULL;


    try
        {
        HRESULT hr;
        VARIANT v;
		USES_CONVERSION;
        // 
        // Trim leading and trailing spaces
        // 
        m_sFolder.TrimLeft();
        m_sFolder.TrimRight();

        // 
        // Strip off trailing backslashes
        // 
        m_sFolder.TrimRight( _T('\\') );    

		// check to see if folder has changed.
		if (m_ifMoveQFolder == MOVE_SAMPLES_ON_QFOLDER_CHANGE || m_ifMoveQFolder == DELETE_SAMPLES_ON_QFOLDER_CHANGE)
		{
			VariantInit( &v );
			v.vt = VT_UI4;
			v.ulVal = m_ifMoveQFolder;


			bstrS = SysAllocString(T2COLE(REGVALUE_MOVE_SAMPLE));
			hr = m_pConfig->SetValue( bstrS, v );
			if( FAILED( hr ) )            
				{
				_com_issue_error( hr );
				}
		    VariantClear( &v );
			VariantInit( &v );
			v.vt = VT_BSTR;
			v.bstrVal = m_sFolder.AllocSysString();
			SysReAllocString(&bstrS,T2COLE(REGVALUE_QUARANTINE_FOLDER));
			hr = m_pConfig->SetValue( bstrS, v );
			if( FAILED( hr ) )            
				{
				_com_issue_error( hr );
				}
			// Now that new folder name has been written out, save it off as orginal to make sure if it changes again we write it out again.
			m_sOrigQFolder = m_sFolder;
		}

        VariantClear( &v );
        v.vt = VT_UI4;
        v.ulVal = m_iIPPort;
		if (bstrS == NULL)
			bstrS = SysAllocString(T2COLE(REGVALUE_PORT));
		else
			SysReAllocString(&bstrS,T2COLE(REGVALUE_PORT));
        hr = m_pConfig->SetValue( bstrS, v );
        if( FAILED( hr ) )            
            {
            _com_issue_error( hr );
            }

        VariantClear( &v );
        v.vt = VT_UI4;
        v.ulVal = m_iSPXPort;
		SysReAllocString(&bstrS,T2COLE(REGVALUE_SPXSOCKET));
        hr = m_pConfig->SetValue( bstrS, v );
        if( FAILED( hr ) )            
            {
            _com_issue_error( hr );
            }

        VariantClear( &v );
        v.vt = VT_UI4;
        v.ulVal = m_bListenIP;
		SysReAllocString(&bstrS,T2COLE(REGVALUE_LISTEN_IP));
        hr = m_pConfig->SetValue( bstrS, v );
        if( FAILED( hr ) )            
            {
            _com_issue_error( hr );
            }

        VariantClear( &v );
        v.vt = VT_UI4;
        v.ulVal = m_bListenSPX;
		SysReAllocString(&bstrS,T2COLE(REGVALUE_LISTEN_SPX));
        hr = m_pConfig->SetValue( bstrS, v );
        if( FAILED( hr ) )            
            {
            _com_issue_error( hr );
            }

        VariantClear( &v );
        v.vt = VT_UI4;
        v.ulVal = m_uMaxSize;
		SysReAllocString(&bstrS,T2COLE(REGVALUE_MAX_SIZE));
        hr = m_pConfig->SetValue( bstrS, v );
        if( FAILED( hr ) )
            {
            _com_issue_error( hr );
            }

        VariantClear( &v );
        v.vt = VT_UI4;
        v.ulVal = m_uMaxNoSamples;
		SysReAllocString(&bstrS,T2COLE(REGVALUE_MAX_FILES));
        hr = m_pConfig->SetValue( bstrS, v );
        if( FAILED( hr ) )
            {
            _com_issue_error( hr );
            }

        VariantClear( &v );
        v.vt = VT_UI4;
        v.ulVal = m_bPurge;
		SysReAllocString(&bstrS,T2COLE(REGVALUE_PURGE));
        hr = m_pConfig->SetValue( bstrS, v );
        if( FAILED( hr ) )
            {
            _com_issue_error( hr );
            }

        VariantClear( &v );
        CString sTemp;
        sTemp.Format( _T("%d"), m_uWarningSize );
        v.vt = VT_BSTR;
        v.bstrVal = sTemp.AllocSysString();
		SysReAllocString(&bstrS,T2COLE(REGVALUE_QFOLDER_WARNING_SIZE));
        hr = m_pConfig->SetValue( bstrS, v );
        if( FAILED( hr ) )
        {
            _com_issue_error( hr ); 
        }
        }
    catch( _com_error e )
        {
		CString s;
        // 
        // Tell user what happened.
        //
        s.LoadString( IDS_ERROR );
        MessageBox( e.ErrorMessage(), s, MB_ICONSTOP | MB_OK ); 
        }
	SysFreeString(bstrS);
	SetModified(FALSE);
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CServerPage::PostNcDestroy
//
// Description   : Need to delete this now.
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CServerPage::PostNcDestroy() 
{
	CQPropPage::PostNcDestroy();

    // 
    // Tell main snapin that the page is gone.
    // 
    ((CQSConsoleData*)m_pSnapin)->CancelPropPage();

    // 
    // Release configuration interface
    // 
    m_pConfig->Detach();
    m_pConfig->Release();

    // 
    // If quarantine folder has changed, notify caller.
    //
    if( m_sOrigQFolder != m_sFolder )   
        {
        MMCPropertyChangeNotify( m_lNotifyHandle, (long) m_pSnapin );
        }

    // 
    // This is the only real opportunity to delete this property page.
    // 
    delete this; 
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CServerPage::OnKillActive
//
// Description   : 
//
// Return type   : BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/3/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CServerPage::OnKillActive() 
{
	// 
    // Make sure data is good.
    // 
    if( CQPropPage::OnKillActive() == FALSE )
        return FALSE;

    CString sTitle, sText;
    sTitle.LoadString( IDS_CONFIG_ERROR );
    int iControl = 0;
	int iIsValidFolder = 0;
    // 
    // Check quarantine folder field.
    // 
	iIsValidFolder = IsValidQserverFolder();
    if( iIsValidFolder  == FALSE )
        {
        // 
        // Inform the user of the errors of his ways.
        // 
        sText.LoadString( IDS_BAD_QSERVER_FOLDER );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_Q_FOLDER;
        }
	else if (iIsValidFolder == -1)
	{
        // 
        // Inform the user of the errors of his ways.
        // 
        sText.LoadString( IDS_ERROR_QSERVER_FOLDER_INVALID );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_Q_FOLDER;
	}
	else if (iIsValidFolder == 2)
	{
		// the user canceled the move lets go back to the control
        iControl = IDC_Q_FOLDER;
	}
    else if( m_bListenIP && ( m_iIPPort < 1 || m_iIPPort > 65535 ) )
        {
        // 
        // Inform the user of the errors of his ways.
        // 
        sText.LoadString( IDS_BAD_PORT );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_IP_PORT;
        }
    else if( m_bListenSPX && ( m_iSPXPort < 1 || m_iSPXPort > 65535 ) )
        {
        // 
        // Inform the user of the errors of his ways.
        // 
        sText.LoadString( IDS_BAD_PORT );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_SPX_PORT;
        }
    else if( m_uMaxSize < 1 || m_uMaxSize > 4095 )
        {
        // 
        // Inform the user of the errors of his ways.
        // 
        sText.LoadString( IDS_BAD_MAX_SIZE );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_QFOLDER_SIZE;
        }

    else if( m_uMaxNoSamples< 10 || m_uMaxNoSamples > MAX_FILES_MAX)
        {
        // 
        // Inform the user of the errors of his ways.
        // 
        sText.LoadString( IDS_BAD_NO_SAMPLE_MAX_SIZE );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_QFOLDER_NO_SAMPLES;
        }

    else if( m_uWarningSize < 1 || m_uWarningSize > 4095 )
    {
        // Inform the user of the errors of his ways.
        sText.LoadString( IDS_BAD_WARNING_SIZE2 );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_EDIT_QFOLDER_WARNING_SIZE;
    }

    else if( m_uMaxSize < m_uWarningSize )
    {
        // Inform the user of the errors of his ways.
        sText.LoadString( IDS_BAD_WARNING_SIZE );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_EDIT_QFOLDER_WARNING_SIZE;
    }

// 4/29
//     else if( m_uMaxSize > dwFreeSpace && dwFreeSpace != 0 )
//     {
//         // Inform the user of the errors of his ways.
//         //sText.LoadString( IDS_QFOLDER_NOT_ENOUGH_DISK_SPACE_FMT );
//         //sText.Format( IDS_QFOLDER_NOT_ENOUGH_DISK_SPACE_FMT );
//         MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
//         iControl = IDC_QFOLDER_SIZE;
//     }




    // 
    // Select the offending control.
    // 
    if( iControl )
        {
        GetDlgItem( iControl )->SetFocus();
        ((CEdit*) GetDlgItem( iControl ) )->SetSel( 0, -1 );
        return FALSE;
        }

    // 
    // Looks good.  Ok to bail out now.
    // 
    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CServerPage::IsValidQserverFolder
//
// Description   : Makes sure the quarantine folder path looks valid.
//
// Return type   : BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/3/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
int CServerPage::IsValidQserverFolder()
{

	int iRc = FALSE;

	// check to see if folder has changed.
	if (m_sFolder != m_sOrigQFolder)
	{
		// 
		// Check the length
		//     
		if( m_sFolder.GetLength() < 3 || m_sFolder.IsEmpty() )
			return FALSE;

		// 
		// Convert forward slashes.
		// 
		m_sFolder.Replace( _T('/'), _T( '\\' ) );

		// 
		// Trim leading and trailing spaces
		// 
		m_sFolder.TrimLeft();
		m_sFolder.TrimRight();

		// 
		// Strip off trailing backslashes
		// 
		m_sFolder.TrimRight( _T('\\') );    

		// 
		// Make sure there are no wildcard characters.
		// 
		if( -1 != m_sFolder.FindOneOf( _T("*?") ) )
			return FALSE;

		// 
		// Make sure the folder is in the form X:\...
		// 
		CString sTemp = m_sFolder.Left( 3 );
		if( _T( ':' ) == sTemp.GetAt( 1 ) && 
			_T( '\\' ) == sTemp.GetAt( 2 ) )
			{
			iRc = TRUE;
			}
		if(TRUE == iRc)
		{
			CString sText, sTitle;
			int iTemp=0;
			sText.LoadString(IDS_Q_MOVE_SAMPLES);
			sTitle.LoadString(IDS_WARNING_TITLE);
			// Ask user if he wants to move the quarantined items.
			iTemp = MessageBox( sText, sTitle, MB_ICONQUESTION | MB_YESNOCANCEL );
			if (IDCANCEL != iTemp)
			{
				if(IDYES == iTemp)
				{
				m_ifMoveQFolder = MOVE_SAMPLES_ON_QFOLDER_CHANGE;
				}
				else if (IDNO == iTemp)
				{
				m_ifMoveQFolder = DELETE_SAMPLES_ON_QFOLDER_CHANGE;
				}
			}
			else
			{
				iRc = 2;
			}
		}
	}
	else
	{
		iRc = TRUE;
	}

    return iRc;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CServerPage::SetRefreshInterval
//
// Description   : Writes out the refreshInterval to the registry
//
// Return type   : None	
//
//
///////////////////////////////////////////////////////////////////////////////
// 01-05-2000 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CServerPage::SetRefreshInterval(DWORD dwRefreshInteveral)
{

	CRegKey reg;
	LONG lResult;
	// 
	// Read in values from registry
	// 
	lResult = reg.Open( HKEY_LOCAL_MACHINE,
						REGKEY_QSCONSOLE);
    if( ERROR_SUCCESS != lResult )
	{
		lResult = reg.Create( HKEY_LOCAL_MACHINE,
							REGKEY_QSCONSOLE);
	}

	reg.SetDWORDValue (REGVAL_REFRESH_INTERVAL, dwRefreshInteveral);
	reg.Close();
    return;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CServerPage::GetRefreshInterval
//
// Description   : reads in the refreshInterval from the registry
//
// Return type   : int refresh interval
//
//
///////////////////////////////////////////////////////////////////////////////
// 01-05-2000 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD CServerPage::GetRefreshInterval(void)
{
	DWORD iRefresh =0;
	CRegKey reg;
	LONG lResult;
	// 
	// Read in values from registry
	// 
	lResult = reg.Open( HKEY_LOCAL_MACHINE,
						REGKEY_QSCONSOLE);
    if( ERROR_SUCCESS != lResult )
	{
		lResult = reg.Create( HKEY_LOCAL_MACHINE,
							REGKEY_QSCONSOLE);
		if(ERROR_SUCCESS == lResult )
			reg.SetDWORDValue (REGVAL_REFRESH_INTERVAL, REFRESH_TIME_DEFAULT);
	}
	lResult = reg.QueryDWORDValue(REGVAL_REFRESH_INTERVAL, iRefresh);
	if(ERROR_SUCCESS != lResult )
	{
		iRefresh = REFRESH_TIME_DEFAULT;
		reg.SetDWORDValue (REGVAL_REFRESH_INTERVAL,REFRESH_TIME_DEFAULT);
	}
	reg.Close();
    return iRefresh;
}


#if 0
// jhill 3/24/00
/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CServerPage::OnFiledialogButton() 
{
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
	csCaption.LoadString(IDS_BROWSEQDIRS_STRING);
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
	    m_sFolder = szPath;
		GetDlgItem( IDC_Q_FOLDER )->SetWindowText( szPath );
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

	

void CServerPage::OnChangeQFolder() 
{
	SetModified(TRUE);
	
}

void CServerPage::OnChangeQfolderSize() 
{
	SetModified(TRUE);
}

void CServerPage::OnChangeRefreshEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CQPropPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
	
}

void CServerPage::OnChangeEditQfolderWarningSize() 
{
	SetModified(TRUE);
}

void CServerPage::OnChangeIpPort() 
{
	SetModified(TRUE);
}

void CServerPage::OnChangeSpxPort() 
{
	SetModified(TRUE);
}

void CServerPage::OnChangeQfolderNoSamples() 
{
	SetModified(TRUE);
	
}

void CServerPage::OnCheckPurge() 
{
	SetModified(TRUE);
	
}

void CServerPage::OnButtonPurgeLog() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	  
	CPurgeLog PurgeDlg(CWnd::FromHandle(GetSafeHwnd()));


	PurgeDlg.DoModal();


}
BOOL CServerPage::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
//    DisplayHtmlContextHelp( (HWND)hWinHandle, dwControlID, dwHtmlHelpID, 0, sTopicName, lpHelpIdArray );   // HH_TP_HELP_CONTEXTMENU  //  CallHtmlHelp
//    DoHelpPropSheetF1( IDD_AVIS_POLICY_PAGE, m_lpHelpIdArray, pHelpInfo );
//    hHelpWin =HtmlHelpA(
//                    hwndCaller,
//                    pszFile,                // (LPCTSTR)sTopic,    // "c:\\Help.chm::/Intro.htm"
//                    HH_TP_HELP_CONTEXTMENU,
//                    dwData) ;
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}
BOOL CServerPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
    char szPath[512];
    CString sTopic;
    // INITIALIZE
    HWND hHelpWin = NULL;
    sTopic = _T(" ");

	AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    NMHDR* pNMHDR = (NMHDR*)lParam;

	if( pNMHDR->code == PSN_HELP)
	{
	     CWnd *pwnd = GetActiveWindow();
	    sTopic.Format( _T("%s::/%s"), m_sHelpFile, QCONSOLE_HELPTOPIC_GEN_PROP);
#ifdef _UNICODE
		int iRet = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)sTopic.GetBuffer(0),-1,
			                           (LPSTR)szPath,sizeof(szPath),NULL,NULL);
		sTopic.ReleaseBuffer(-1);
	     if( iRet == FALSE )
		    return(FALSE);
#else
	    vpstrncpy( szPath, sTopic.GetBuffer(0), sizeof(szPath) );
		sTopic.ReleaseBuffer(-1);
#endif

		hHelpWin =HtmlHelpA(
			            pwnd->m_hWnd,
				        szPath,                // (LPCTSTR)sTopic,    // "c:\\Help.chm::/Intro.htm"
					    HH_DISPLAY_TOPIC,         // 
						NULL) ;
	}
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}
