// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ConfigureSnapIn.cpp : implementation file
//

#include "stdafx.h"
#include "wprotect32.h"
#include "ConfigureSnapIn.h"
#include "MainFrm.h"
#include <comdef.h>

#include "ldvpsnapin.h"
#include "clientreg.h"
#include "DefaultView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigureSnapIn

IMPLEMENT_DYNCREATE(CConfigureSnapIn, CFormView)

CConfigureSnapIn::CConfigureSnapIn()
	: CFormView(CConfigureSnapIn::IDD)
{
	//{{AFX_DATA_INIT(CConfigureSnapIn)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CConfigureSnapIn::~CConfigureSnapIn()
{
}

void CConfigureSnapIn::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigureSnapIn)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigureSnapIn, CFormView)
	//{{AFX_MSG_MAP(CConfigureSnapIn)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_HELP_BUTTON, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigureSnapIn diagnostics

#ifdef _DEBUG
void CConfigureSnapIn::AssertValid() const
{
	CFormView::AssertValid();
}

void CConfigureSnapIn::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConfigureSnapIn message handlers

void CConfigureSnapIn::OnInitialUpdate() 
{
	CRect			rcClientRect;
	CWnd			*ptrWnd = NULL;
	LOGFONT			lf;
	CFont			*pFont;

	//Base-class implementation
	CFormView::OnInitialUpdate();
	
	//Size the frame
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit( !((CMainFrame*)GetParentFrame())->GetFirstTime() );
	
	//Set the font of my title
	if( !m_titleFont.m_hObject )
	{
		pFont = GetFont();
		pFont->GetLogFont( &lf );
		//lf.lfWeight = 700;
		lf.lfHeight += 35;
		m_titleFont.CreateFontIndirect( &lf );

		//Set the font into the text control
		if( ptrWnd = GetDlgItem( IDC_TITLE ) )
			ptrWnd->SetFont( &m_titleFont );
	}
	
	
	//On the OnInitialUpdate, I need to create my OCX
	//	and make sure I have the proper IConfig loaded
	
	//Figure out the client rect for the control
	if( ptrWnd = GetDlgItem( IDC_PLACE_HOLDER ) )
	{
		ptrWnd->GetWindowRect( &rcClientRect );
		ScreenToClient( &rcClientRect );
	}

	InitSnapIn( rcClientRect );
}

void CConfigureSnapIn::InitSnapIn( CRect rcClientRect )
{
//	IVirusProtect	*ptrVirusProtect = NULL;
	IGenericConfig	*ptrGenConfig = NULL;
	CString			strName;
	CString			strPageType,
					strDescription;
	CSubTask		*ptrTask = ((CWProtect32App*)AfxGetApp())->m_ptrTask;
	DWORD			dwType = -1;
	DWORD			dwRootID;
	CWnd			*ptrWnd;

	if( ptrTask == NULL )
		return;

	//First, get the SnapIn information
	CLDVPSnapIn	snapIn( ptrTask->m_strInternalName );
	snapIn.Read();

	//Figure out the name of the task
	switch( ptrTask->m_identifier )
	{
	case SNAP_IN_PAGE_REALTIME:
		dwType = CONFIG_TYPE_REALTIME;
		dwRootID = HKEY_VP_STORAGE_REALTIME;
		strName = snapIn.GetStorage();
		strPageType.LoadString( IDS_REALTIME_PROTECTION );
		break;
	case SNAP_IN_PAGE_IDLE:
		dwType = CONFIG_TYPE_IDLE;
		dwRootID = HKEY_VP_ADMIN_SCANS;
		strName = szReg_Key_Idle;
		strPageType.LoadString( IDS_IDLE_PROTECTION );
		break;
	}

	if( ptrWnd = GetDlgItem( IDC_TITLE ) )
	{
		CString strTitle = ptrTask->m_text;

		RemoveChars( strTitle, _T('&') );
		ptrWnd->SetWindowText( strTitle );
	}

	m_configOCX.SetClsid( ptrTask->m_guid );
	if(m_configOCX.Create( _T(""), WS_VISIBLE | WS_CHILD, rcClientRect, 
									this, IDC_CONFIG_OCX, NULL, FALSE ) )
	{
		if ( dwType != -1 )  
			m_configOCX.SetType( dwType );

		//Always in client mode
		m_configOCX.SetMode( CONFIG_MODE_CLIENT );

		SetWindowLong( m_configOCX.m_hWnd, GWL_EXSTYLE, GetWindowLong( m_configOCX.m_hWnd, GWL_EXSTYLE ) | WS_EX_CONTROLPARENT );


		if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IGenericConfig, (void**)&ptrGenConfig ) ) ) 
			{
				DWORD dwErr = ptrGenConfig->Open( NULL, dwRootID, strName.GetBuffer( strName.GetLength() ) , GC_MODEL_SINGLE /* flags ignored in Cli* objects */ );
				strName.ReleaseBuffer();					

				if( dwErr != 0 )
				{
					TRACE1("ERROR: Open on IGenericConfig reutrned 0x%X\n", dwErr );
					AfxMessageBox( IDS_ERROR_NO_OPEN );
				}
				else
					m_configOCX.SetConfigInterface(ptrGenConfig);

				m_configOCX.Load();
				//I no longer need this interface
				ptrGenConfig->Release();
			}

			//I no longer need the Virus Protect object - it is only a helper
			//	object to get other objects.
//			ptrVirusProtect->Release();
//		}
	}
}

void CConfigureSnapIn::OnSize(UINT nType, int cx, int cy) 
{
	CRect	buttonRect,
			dlgRect;
	CWnd	*ptrWnd;

	CFormView::OnSize(nType, cx, cy);

	//Figure out the current client rect
	GetWindowRect( &dlgRect );
	ScreenToClient( &dlgRect );

	//Figure out the current client rect
	if( ::IsWindow( m_configOCX.m_hWnd ))
	{
		m_configOCX.GetWindowRect( &buttonRect );
		ScreenToClient( &buttonRect );
		m_configOCX.MoveWindow( buttonRect.left, buttonRect.top, buttonRect.Width(), m_configOCX.GetDlgHeight() );

		//Move the Close button
		if( ptrWnd = GetDlgItem( IDC_CLOSE ) )
		{
			ptrWnd->GetWindowRect( &buttonRect );
			ScreenToClient( &buttonRect );
			ptrWnd->MoveWindow( dlgRect.right - ( buttonRect.right - buttonRect.left ) - 10, dlgRect.bottom - ( buttonRect.bottom - buttonRect.top ) - 10, buttonRect.right - buttonRect.left, buttonRect.bottom - buttonRect.top );
			ptrWnd->GetWindowRect( &buttonRect );
			ScreenToClient( &buttonRect );
		}

		//Move the Help button
		if( ptrWnd = GetDlgItem( IDC_HELP_BUTTON ) )
		{
			int iWidth = buttonRect.right - buttonRect.left;

			buttonRect.left -= iWidth + 5;
			buttonRect.right = buttonRect.left + iWidth;

			ptrWnd->MoveWindow( buttonRect.left, buttonRect.top, buttonRect.right - buttonRect.left, buttonRect.bottom - buttonRect.top );
            // defect 1-3GKG14: Sometimes the close button is draw on top 
            // of the help button before the window is moved, this leaves 
            // artifacts after the move. So invalidate the help button, so 
            // it gets redrawn completely.
            ptrWnd->Invalidate(TRUE);
		}
	}
}

void CConfigureSnapIn::OnClose() 
{
    //Store out the results of the configuration
	m_configOCX.Store();
	((CMainFrame*)GetParentFrame())->SwitchToView( RUNTIME_CLASS( CDefaultView ) );
}

void CConfigureSnapIn::OnDestroy() 
{
	
	CFormView::OnDestroy();

}

void CConfigureSnapIn::OnHelp() 
{
	HELPINFO	helpInfo;

	memset( &helpInfo, 0x00, sizeof( HELPINFO ) );
	helpInfo.cbSize = sizeof( HELPINFO );

	//Send the help message to the control
	m_configOCX.SendMessage( WM_HELP, 0, (LPARAM)&helpInfo );
	//EA 03/12/2000
	//Setting the taskview as active view so that we get the focus
	//back after quitting help
	CView* pView;
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if(pFrame)
	{
		pView = (CView*)pFrame->m_wndSplitter->GetPane( 0, 1 );
		if(pView)
		pFrame->SetActiveView(pView);
	}
	//EA 03/12/2000
	//AfxGetApp()->WinHelp( IDD, HELP_CONTEXT );	
	//AfxMessageBox("Help was pressed" );
}

