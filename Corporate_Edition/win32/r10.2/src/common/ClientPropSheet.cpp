// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: ClientPropSheet.cpp
//  Purpose: Base classes for Schedule property Sheet & Page.
//
//	Date: 2-19-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------
#include "stdafx.h"
#include <afxcmn.h>

#include "ClientPropSheet.h"
#include "MscFuncs.h"
#include "WizardPage.H"
#include "SymSaferRegistry.h"
#include "acta.h"
#include "LS.h"


#define SCAN_DLGS_CLSID_STRING		"4DEF8DD1-C4D1-11d1-82DA-00A0C9749EEF"

tCreateResultsView			CreateResultsView		= NULL;
tAddLogLineToResultsView	AddLogLineToResultsView = NULL;

IMPLEMENT_DYNAMIC(CClientPropSheet, CPropertySheet)

BEGIN_MESSAGE_MAP(CClientPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CClientPropSheet)
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_MSC, OnMscButton )
END_MESSAGE_MAP()

void CClientPropSheet::OnScanStarting() 
{
	SetWizardButtons( PSWIZB_DISABLEDFINISH );
	//Fire the even to inform my container that a scan is starting
	m_ptrCtrl->SendMessage( WM_COMMAND, MAKEWPARAM( UWM_SCAN_STARTING, 0 ), 0L );
}

void CClientPropSheet::OnScanEnding() 
{
	//Fire the even to inform my container that a scan is ending
	m_ptrCtrl->SendMessage( WM_COMMAND, MAKEWPARAM( UWM_SCAN_ENDING, 0 ), 0L );
	//Enable the Close button
	SetWizardButtons( PSWIZB_FINISH );
}

//----------------------------------------------------------------
// 
// Class CClientPropSheet
//
//----------------------------------------------------------------

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CClientPropSheet::CClientPropSheet( LPCTSTR szTitle, CWnd *pParent, UINT flags, IConfig *ptrEngine)
: CLDVPPropSheet(szTitle, pParent, flags )
{
	InitMembers( pParent, ptrEngine );
}


//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CClientPropSheet::CClientPropSheet( DWORD dwTitle, CWnd *pParent, UINT flags, IConfig *ptrEngine)
: CLDVPPropSheet(dwTitle, pParent, flags)
{
	InitMembers( pParent, ptrEngine  );
}

//----------------------------------------------------------------
// InitMembers
//----------------------------------------------------------------
void CClientPropSheet::InitMembers( CWnd *pParent, IConfig *ptrConfig )
{
	HKEY	hKey;
	CString strSubKey,
			strScanDlgs;
	BYTE	*szTemp;
	DWORD	dwSize;

	m_bDeleteOnClose = FALSE;
	m_ptrCtrl        = (CTaskBase*)pParent;
	m_ptrConfig      = ptrConfig;
	m_dwScheduleType = DAILY;
	m_bStartedEngine = FALSE;
	m_bScanOpened    = (BOOL)m_ptrConfig;
    m_ptrScanConfig  = NULL;

	if( m_ptrConfig )
		m_ptrConfig->AddRef();

	//Load the Scan Dialogs DLL
	strSubKey.Format( "CLSID\\{%s}\\InProcServer32", SCAN_DLGS_CLSID_STRING );

	//OK, open the key
	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_CLASSES_ROOT,
										strSubKey,
										0,
										KEY_QUERY_VALUE,
										&hKey ) )
	{
		//Calculate the size of the string
		SymSaferRegQueryValueEx(	hKey,
							NULL,	//I want the default value
							NULL,
							NULL,
							NULL,
							&dwSize );

		//Get the actual filename
		szTemp = (BYTE*)strScanDlgs.GetBuffer( dwSize );
		SymSaferRegQueryValueEx(	hKey,
							NULL,	//I want the default value
							NULL,
							NULL,
							szTemp,
							&dwSize );
		strScanDlgs.ReleaseBuffer();

		RegCloseKey( hKey );
	}

	if(	m_hScanDialogs = LoadLibrary( strScanDlgs ) )
	{
		//Load the pointer to the Message method
		CreateResultsView = (tCreateResultsView)GetProcAddress( m_hScanDialogs, "CreateResultsView" );
		AddLogLineToResultsView = (tAddLogLineToResultsView)GetProcAddress( m_hScanDialogs, "AddLogLineToResultsView" );

		if( !CreateResultsView && !AddLogLineToResultsView)
		{
			//Unload the DLL and set my handle to NULL
			FreeLibrary( m_hScanDialogs );
			m_hScanDialogs = NULL;
		}
	}
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CClientPropSheet::~CClientPropSheet()
{
	DeInitEngine();

	if( m_hScanDialogs )
	{
		//Unload the DLL and set my handle to NULL
		FreeLibrary( m_hScanDialogs );
		m_hScanDialogs = NULL;
	}
}

//----------------------------------------------------------------
// InitEngine
//----------------------------------------------------------------
BOOL CClientPropSheet::InitEngine()
{
	CWnd	*ptrWnd;
	RECT	rect,
			buttonRect;

	//hide the Cancel button
	ptrWnd = GetDlgItem( IDCANCEL );
	ptrWnd->ShowWindow( SW_HIDE );
	ptrWnd->GetWindowRect( &buttonRect );
	ScreenToClient( &buttonRect );

	//Get the rect of the NEXT/FINISH/SCAN/SAVE button
	ptrWnd = GetDlgItem( 0x3025 );
	ptrWnd->SetWindowPos( &wndTop, buttonRect.left, buttonRect.top, buttonRect.right - buttonRect.left, buttonRect.bottom - buttonRect.top, SWP_SHOWWINDOW );

	//Create the Edit button
	buttonRect.left -= ( buttonRect.right - buttonRect.left ) - 6;
	m_btnMsc.Create( "", WS_CHILD|WS_TABSTOP, buttonRect, ptrWnd->GetParent(), IDC_MSC );
	m_btnMsc.SetFont( GetFont() );
	
	//Move the Help button so it appears to the left
	//	of the NEXT/FINISH/SCAN/SAVE button.
	ptrWnd = GetDlgItem( IDHELP );
	ptrWnd->SetWindowPos( &wndTop, buttonRect.left - ( buttonRect.right - buttonRect.left ) - 6, buttonRect.top, buttonRect.right - buttonRect.left, buttonRect.bottom - buttonRect.top, SWP_SHOWWINDOW );

	//Shorten the 3D line so it fits my View
	ptrWnd = NULL;
	ptrWnd = GetDlgItem( 0x3026 );
	ptrWnd->GetWindowRect( &rect );
	ScreenToClient( &rect );
	ptrWnd->SetWindowPos( &wndTop, rect.left, rect.top, (buttonRect.right + 10) - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW ); 

	memset( &rect, '\0', sizeof( RECT ) );

	HookWindow();

    return m_ptrScanConfig == NULL? FALSE : TRUE;
}

//----------------------------------------------------------------
// GetScan
//----------------------------------------------------------------
IScan*	CClientPropSheet::GetScan( IScanCallback* pCallback /* = NULL */)
{ 
	IScan	*ptrScan = NULL; 

	if( m_ptrScanConfig ) 
	{ 
		if( SUCCEEDED( m_ptrScanConfig->CreateByIID( IID_IScan, (void**)&ptrScan ) ) )
		{
			if( FAILED( ptrScan->Open( pCallback, m_ptrScanConfig ) ) )
			{
				ptrScan->Release();
				ptrScan = NULL;
			}
		}
	} 
		
	return ptrScan; 
}

//----------------------------------------------------------------
// CloseScan
//----------------------------------------------------------------
void CClientPropSheet::CloseScan()
{

	//Now, check if I have a configuration interface to release
	if( m_ptrConfig )
		m_ptrConfig->Release();

	//Set my interface pointers to NULL
	m_ptrScanConfig = NULL;
	m_ptrConfig = NULL;

	m_bScanOpened = FALSE;
}

//----------------------------------------------------------------
// DeInitEngine
//----------------------------------------------------------------
void CClientPropSheet::DeInitEngine()
{
	CloseScan();

	UnHookWindow();
}

//----------------------------------------------------------------
// OpenScan
//----------------------------------------------------------------
void CClientPropSheet::OpenScan( BOOL bDeleteOnClose, CString strCopyOf /* = ""*/ , DWORD dwRoot /* = HKEY_VP_USER_SCANS */ )
{
	if( m_bScanOpened )
	{
		TRACE("**ERROR: Scan already opened!\nCannot open another scan until CloseScan is called!\n");
		return;
	}

	ASSERT( m_ptrConfig == NULL );

	IVirusProtect	*ptrVirusProtect = NULL;

	//Create the Cliscan COM object if I didn't have an interface passed in
	if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IScanConfig, (void**)&m_ptrScanConfig ) ) )
	{
		m_ptrConfig = (IConfig*)m_ptrScanConfig;

		m_ptrScanConfig->Open( NULL, dwRoot, m_strTaskName.GetBuffer( m_strTaskName.GetLength() ) );
		m_strTaskName.ReleaseBuffer();

		//Set the CloseMode
		m_ptrScanConfig->ChangeCloseMode( bDeleteOnClose );

		//And if we are to copy a scan, do it now
		if( !strCopyOf.IsEmpty() )
		{
			m_ptrScanConfig->CopyFromName( dwRoot, strCopyOf.GetBuffer( strCopyOf.GetLength() )  );
			strCopyOf.ReleaseBuffer();
		}
	}

	if( m_ptrConfig )
		m_bScanOpened = TRUE;
}

//----------------------------------------------------------------
// OnMscButton
//----------------------------------------------------------------
void CClientPropSheet::OnMscButton()
{
	CWizardPage	*pPage = (CWizardPage*)GetActivePage();

	if( pPage )
		pPage->OnMscButton();
}

//----------------------------------------------------------------
// HideBack
//----------------------------------------------------------------
void CClientPropSheet::HideBack()
{
	CWnd *ptrWnd;
	RECT rect;

//	ptrWnd = GetDlgItem( 0x3023 );
//	ptrWnd->ShowWindow( SW_HIDE );

	//Find out where the Scan button is
	if(ptrWnd = GetDlgItem( 0x3025 ))
        {
	    ptrWnd->GetWindowRect( &rect );
	    ScreenToClient( &rect );
        }

	//Hide the Back button
	if( ptrWnd = GetDlgItem( 0x3023 ) )
	    {
		ptrWnd->ShowWindow( SW_HIDE );
	    }

	//Move the Help button so it is next to the Scan button
	if(ptrWnd = GetDlgItem( IDHELP ))
	    ptrWnd->SetWindowPos( &wndTop, rect.left - ( rect.right - rect.left ) - 6, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW );

}

//----------------------------------------------------------------
// ShowBack
//----------------------------------------------------------------
void CClientPropSheet::ShowBack()
{
	CWnd *ptrWnd;
	RECT rect;

	//Find out where the Scan button is
	if( ptrWnd = GetDlgItem( 0x3025 ) )
	{
		ptrWnd->GetWindowRect( &rect );
		ScreenToClient( &rect );
	}
	else
		return;	//if I couldn't get the button, then something is wrong - get out

	//Move the Back button and display it
	if( ptrWnd = GetDlgItem( 0x3023 ) )
	{
		ptrWnd->SetWindowPos( &wndTop, rect.left - ( rect.right - rect.left ) - 6, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW );
		ptrWnd->ShowWindow( SW_SHOW );
		ptrWnd->GetWindowRect( &rect );
		ScreenToClient( &rect );
	}

	//If the Edit button is not hidden, move it as well
	if( ( ptrWnd = GetDlgItem( IDC_MSC ) ) && ( ptrWnd->IsWindowVisible() ) )
	{
		ptrWnd->SetWindowPos( &wndTop, rect.left - ( rect.right - rect.left ) - 6, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW );
		ptrWnd->ShowWindow( SW_SHOW );
		ptrWnd->GetWindowRect( &rect );
		ScreenToClient( &rect );
	}

	//Place the help button next to the back\msc button	
	if( ptrWnd = GetDlgItem( IDHELP ) )
		ptrWnd->SetWindowPos( &wndTop, rect.left - ( rect.right - rect.left ) - 6, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW );
}

//----------------------------------------------------------------
// ShowEdit
//----------------------------------------------------------------
void CClientPropSheet::ShowMscButton( DWORD dwTitleId)
{
	CWnd *ptrWnd;
	RECT rect;
	CString strTitle;

	strTitle.LoadString( dwTitleId );

	//Find out where the Scan button is
	ptrWnd = GetDlgItem( 0x3025 );
	ptrWnd->GetWindowRect( &rect );
	ScreenToClient( &rect );

	//If the Back button is not hidden, move it as well
	if( ( ptrWnd = GetDlgItem( 0x3023 ) ) && ( ptrWnd->IsWindowVisible() ) )
	{
		ptrWnd->SetWindowPos( &wndTop, rect.left - (rect.right - rect.left), rect.top - 1, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW );
		ptrWnd->ShowWindow( SW_SHOW );
		ptrWnd->GetWindowRect( &rect );
		ScreenToClient( &rect );
	}
	
	//Move the Msc button
	ptrWnd = &m_btnMsc;
	ptrWnd->SetWindowText( strTitle );
	ptrWnd->SetWindowPos( &wndTop, rect.left - (rect.right - rect.left), rect.top - 1, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW );
	ptrWnd->ShowWindow( SW_SHOW );
	ptrWnd->GetWindowRect( &rect );
	ScreenToClient( &rect );

	//Place the help button next to the edit button	
	ptrWnd = GetDlgItem( IDHELP );
	ptrWnd->SetWindowPos( &wndTop, rect.left - ( rect.right - rect.left ) - 6, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW );
}

//----------------------------------------------------------------
// SetFinishText
//----------------------------------------------------------------
void CClientPropSheet::SetFinishText( int ID )
{
	CString strText;

	strText.LoadString( ID );

	CPropertySheet::SetFinishText( (LPCTSTR)strText );
}

//----------------------------------------------------------------
// OnSetFocus
//----------------------------------------------------------------
void CClientPropSheet::OnSetFocus(CWnd* pOldWnd) 
{
	CPropertySheet::OnSetFocus(pOldWnd);
	
	if( GetActivePage() )
		GetActivePage()->SetFocus();
}

//----------------------------------------------------------------
// OnSize
//----------------------------------------------------------------
void CClientPropSheet::OnSize(UINT nType, int cx, int cy) 
{
	CWizardPage	*ptrPage;

	if( Initialized() )
	{
		//First, hide the tab control. This will stop
		//	it from re-appearing as is seems to do every so often.
		CTabCtrl *pTab = GetTabControl();
		
		if( pTab )
			pTab->ShowWindow( SW_HIDE );

		MoveWindow( 0, 0, cx, cy, TRUE );

		SetButtonsPos();

		//Remove all of my pages
		for( int iPage = GetPageCount() -1; iPage >= 0; iPage-- )
		{
			ptrPage = (CWizardPage*)GetPage( iPage );
			if( ptrPage && ::IsWindow( ptrPage->m_hWnd ) )
			{
				ptrPage->SizePage( cx, cy );
			}
		}	
	}
}

//----------------------------------------------------------------
// SetButtonsPos
//----------------------------------------------------------------
void CClientPropSheet::SetButtonsPos()
{
	
	CWnd *ptrWnd;
	RECT buttonRect,
		 dlgRect;

	//Figure out the current client rect
	GetClientRect( &dlgRect );

	//Move the Scan button
	if( ptrWnd = GetDlgItem( 0x3025 ) )
	{
		ptrWnd->GetWindowRect( &buttonRect );
		ScreenToClient( &buttonRect );
		ptrWnd->MoveWindow( dlgRect.right - ( buttonRect.right - buttonRect.left ) - 20, dlgRect.bottom - ( buttonRect.bottom - buttonRect.top ) - 10, buttonRect.right - buttonRect.left, buttonRect.bottom - buttonRect.top );
	}
	//Move the Next button
	if( ptrWnd = GetDlgItem( 0x3024 ) )
	{
		//ptrWnd->GetWindowRect( &buttonRect );
		//ScreenToClient( &buttonRect );
		ptrWnd->MoveWindow( dlgRect.right - ( buttonRect.right - buttonRect.left ) - 20, dlgRect.bottom - ( buttonRect.bottom - buttonRect.top ) - 10, buttonRect.right - buttonRect.left, buttonRect.bottom - buttonRect.top );
	}

	//Make the line adjust properly
	if( ptrWnd = GetDlgItem( 0x3026 ) )
	{
		CRect	tempRect;

		ptrWnd->GetWindowRect( &tempRect );
		ScreenToClient( &tempRect );
		ptrWnd->MoveWindow( tempRect.left, buttonRect.top - 12, buttonRect.right - tempRect.left, tempRect.bottom - tempRect.top );
	}

	//Move the Back button
	if( (ptrWnd = GetDlgItem( 0x3023 ))  && ptrWnd->IsWindowVisible() )
	{
		ptrWnd->MoveWindow( buttonRect.left - ( buttonRect.right - buttonRect.left ) - 6, buttonRect.top, buttonRect.right - buttonRect.left, buttonRect.bottom - buttonRect.top);
		if( ptrWnd->IsWindowVisible() )
		{
			ptrWnd->GetWindowRect( &buttonRect );
			ScreenToClient( &buttonRect );
		}
	}

	if( (ptrWnd = &m_btnMsc) && ptrWnd->IsWindowVisible() )
	{
		ptrWnd->MoveWindow( buttonRect.left - ( buttonRect.right - buttonRect.left ) - 6, buttonRect.top, buttonRect.right - buttonRect.left, buttonRect.bottom - buttonRect.top);
		if( ptrWnd->IsWindowVisible() )
		{
			ptrWnd->GetWindowRect( &buttonRect );
			ScreenToClient( &buttonRect );
		}
	}

	
	//Move the help button
	if( ptrWnd = GetDlgItem( IDHELP ) )
		ptrWnd->MoveWindow( buttonRect.left - ( buttonRect.right - buttonRect.left ) - 6, buttonRect.top, buttonRect.right - buttonRect.left, buttonRect.bottom - buttonRect.top );
}

//----------------------------------------------------------------
// OnPaint
//----------------------------------------------------------------
void CClientPropSheet::OnPaint() 
{
	CPaintDC	dc(this); // device context for painting
	CBrush		brushBackground;
	RECT		rect;

	GetClientRect( &rect );
	brushBackground.CreateSolidBrush( (COLORREF)GetSysColor( COLOR_BTNFACE ) );
	dc.FillRect( &rect, &brushBackground );

	dc.SetBkColor( (COLORREF)GetSysColor( COLOR_BTNFACE ) );

	brushBackground.DeleteObject();
	//Paint the background with the dialog color

}

//----------------------------------------------------------------
// SetRecordedData
//----------------------------------------------------------------
void CClientPropSheet::SetRecordedData( CEdit *pName, CEdit *pDesc)
{
#if !(defined NORECORD)
	pName->GetWindowText( m_strTitle );
	pDesc->GetWindowText( m_strDescription );

	//Now, if either is empty, use defaults
	if( m_strTaskName.IsEmpty() )
		m_strTaskName.LoadString( IDS_TASK_NAME_DEFAULT );

	if( m_strDescription.IsEmpty() )
		m_strDescription = LS( IDS_TASK_DESCRIPTION_DEFAULT );
#endif
}

//----------------------------------------------------------------
// RenameTask
//----------------------------------------------------------------
void CClientPropSheet::RenameTask( CEdit *pName, CEdit *pDesc)
{
	SetRecordedData( pName, pDesc );

	m_ptrCtrl->NotifyNameDescription( m_strTitle, m_strDescription );
}

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/

