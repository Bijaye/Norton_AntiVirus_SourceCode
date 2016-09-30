// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: MainFrm.Cpp
//  Purpose: CMainFrame Implementation file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include <atlbase.h>
#include <new>
#include "WProtect32.h"
#include "TaskPadView.h"
#include "DefaultView.h"
#include "TaskpadView.h"
#include "MainFrm.h"
#include "TaskView.h"
#include "TaskPadView.h"
#include "WProtect32Doc.h"
#include "TaskDescriptionView.h"

#include "clientreg.h"
#include "comdef.h"
#include "ScanInf.h"

#include "ScheduleEnableDlg.h"
#include "admininfo.h"

#include "VirusListView.h"
#include "vphtmlhelp.h"	
#include "vdbversion.h"
#include "SymSaferRegistry.h"

#include "ScanShared.h"

#include "VPExceptionHandling.h"

#include "ResourceLoader.h"
extern CResourceLoader g_ResLoader;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString m_gHelpFileName;
extern CString m_gEncyclopediaName;
BOOL g_bEnableServicesCheckBox = FALSE;
static bool s_bReadonly = false;

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_RECORD_NEW, OnRecordNew)
	ON_UPDATE_COMMAND_UI(IDM_TASK_DELETE, OnUpdateTaskDelete)
	ON_COMMAND(IDM_TASK_DELETE, OnTaskDelete)
	ON_MESSAGE(WM_STOPSCANNING,OnStopScanning)
	ON_COMMAND(ID_NEW_SCHEDULED, OnNewScheduled)
	ON_COMMAND(IDM_CONTENTS, OnContents)
	ON_COMMAND(IDM_VIRUS_ENCYCLOPEDIA, OnVirusEncyclopedia)
	ON_UPDATE_COMMAND_UI(ID_RECORD_STARTUP, OnUpdateRecordStartup)
	ON_COMMAND(ID_RECORD_STARTUP, OnRecordStartup)
	ON_UPDATE_COMMAND_UI(ID_RECORD_NEW, OnUpdateRecordNew)
	ON_UPDATE_COMMAND_UI(ID_NEW_SCHEDULED, OnUpdateNewScheduled)
	ON_COMMAND(IDC_UPDATE_PATTERNFILE, OnUpdatePatternfile)
	ON_COMMAND(IDC_SCHEDULE_UPDATES, OnScheduleUpdates)
	ON_COMMAND(IDC_LOAD_SERVICES, OnLoadServices)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZING()
	ON_COMMAND(IDM_CONFIG_HISTORY, OnConfigHistory)
	ON_UPDATE_COMMAND_UI(IDC_UPDATE_PATTERNFILE, OnUpdateUpdatePatternfile)
	ON_UPDATE_COMMAND_UI(IDC_SCHEDULE_UPDATES, OnUpdateScheduleUpdates)
	ON_UPDATE_COMMAND_UI(IDC_LOAD_SERVICES, OnUpdateLoadServices)
	ON_WM_INITMENU()
	ON_COMMAND(IDM_VIRUSLIST, OnViruslist)
	ON_COMMAND(ID_CUT, OnCut)
	ON_COMMAND(ID_PASTE, OnPaste)
	ON_WM_EXITMENULOOP()
	//}}AFX_MSG_MAP
	ON_COMMAND( UWM_END_TASK, OnEndTask )
	ON_COMMAND( UWM_START_SCAN, ScanStarting )
	ON_COMMAND( UWM_END_SCAN, ScanEnding )
	ON_COMMAND( UWM_STOP_CURRENT_SCAN, StopCurrentScan )
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
	ON_MESSAGE( TASK_FOCUS, OnTaskFocus )
	ON_MESSAGE( UWM_TASKPAD_RESIZED, OnResizeTaskpad )
	ON_MESSAGE( UWM_POST_INITIALIZE, OnPostInitialize )
	ON_COMMAND( UWM_DELETESELTASK, DelSelTask )
//	ON_COMMAND( IDC_REGISTER, OnRegister )
	ON_MESSAGE( NAV_CHECK_DEF_VERSION, OnUpdateDisplayedDefVersion )
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

BOOL CheckRecording( DWORD dwRecordType )
{
	CWProtect32Doc *pDoc = (CWProtect32Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	BOOL			bRet = FALSE;

	//	Warn the user that they are about to
	//	cancel the recording
	if( ( pDoc->m_bRecordingStarted != 0 ) && ( pDoc->m_fRecording != dwRecordType ) )
	{
		// Displaying a dialog here messes up our drag/drop handling if the user
        // has clicked in a previously created scan item in the task view. The
        // agreed upon solution is to cancel the scan being created with no prompt.

        //if( IDYES == AfxMessageBox( IDS_CANCEL_RECORD, MB_YESNO ) )
	    pDoc->CancelRecord();
		//else
		//	return TRUE;
	}
	else
		pDoc->m_fRecording = 0;

	return FALSE;
}

// checks to see if this machine is an unmanaged client or not
BOOL IsUnmanagedClient()
{
	BOOL bRet = FALSE;

	DWORD dwClientType = GetClientType();
	if ((dwClientType != CLIENT_TYPE_SERVER) && (dwClientType != CLIENT_TYPE_CONNECTED))
			bRet = TRUE;

	return bRet;
}

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CMainFrame::CMainFrame()
{
	m_bScanning = FALSE;	
	m_bFirstTime = TRUE;
    m_bStartupInProgress = TRUE;
	m_wndSplitter = NULL;
	
	//Turn off the AutoMenuEnabling.
	//	This is required so I can get the
	//	messages from menu items that I 
	//	don't have handlers for - these
	//	are then sent to the Task Pad.
	m_bAutoMenuEnable = FALSE;

    // Init HTMLHelp
    m_dwHHCookie = 0;
	::HtmlHelp( NULL, NULL, HH_INITIALIZE, (DWORD)&m_dwHHCookie );
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CMainFrame::~CMainFrame()
{
    // Deinit HTMLHelp
	::HtmlHelp( NULL, NULL, HH_UNINITIALIZE, m_dwHHCookie );
}

//----------------------------------------------------------------
// OnCreate
//----------------------------------------------------------------
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CRect   rect( 0,0,0,0);
    HKEY    hKey;
    DWORD   dwHWND = 0;
    DWORD   dwDisableSplash = 0;

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD dwStyle = GetClassLong( m_hWnd, GCL_STYLE );
	::SetClassLong( m_hWnd, GCL_STYLE,
					dwStyle & ~(CS_VREDRAW | CS_HREDRAW ) );

//	CSplashWnd::ShowSplashScreen(this);

	m_strTitle.LoadString( IDS_LDVP_TITLE );

    //Open the reg key to get the display splash screen setting
    if( ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, szReg_Key_Main, 0, KEY_READ, &hKey))
        {
        DWORD   dwSize = sizeof(DWORD);
        SymSaferRegQueryValueEx(hKey, szReg_val_DisableSplashScreen, NULL, NULL, (BYTE*)&dwDisableSplash, &dwSize);
        RegCloseKey(hKey);
        }

    // Display splash screen if the user wants it
    if(!dwDisableSplash)
        {
	    if( m_AboutDlg.Create( "", WS_CHILD | WS_VISIBLE,
					    rect, this, ID_ABOUT_DLG,
					    NULL, FALSE ) )
    	    {
		    m_AboutDlg.SetType(0);
		    m_AboutDlg.SetSplashTimeOut( 1000 );
		    m_AboutDlg.ShowSplashScreen();
	        }
        }

    SetNumQuarantineItems();

	return 0;
}

//----------------------------------------------------------------
// PreCreateWindow
//----------------------------------------------------------------
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL	bRet = CFrameWnd::PreCreateWindow(cs);
	CRect	rect;
	HKEY	hKey;
	DWORD	dwSize = sizeof( CRect );


	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_CURRENT_USER,
						szReg_Key_Main,
						0,
						KEY_READ,
						&hKey ) )
	{

		if( ERROR_SUCCESS == SymSaferRegQueryValueEx(	hKey,
												szReg_Val_ClientPos,
												0,
												NULL,
												(BYTE*)&rect,
												&dwSize) )
		{
			cs.x = rect.left;
			cs.y = rect.top;
			cs.cx = rect.right - rect.left;
			cs.cy = rect.bottom - rect.top;
		}

		RegCloseKey( hKey );
	}

	cs.style &= ~FWS_PREFIXTITLE;

//	cs.dwExStyle |= WS_EX_CONTEXTHELP;

	// register a unique class name so we and do a findwindow and 
	// shut down from another app
	WNDCLASS wndcls;
	memset(&wndcls, 0, sizeof(WNDCLASS));   // start with NULL defaults
	wndcls.lpfnWndProc = ::DefWindowProc;
	wndcls.hInstance = AfxGetInstanceHandle();
	wndcls.hCursor =  AfxGetApp()->LoadStandardCursor(IDC_UPARROW); 
	wndcls.hIcon = ::LoadIcon( g_ResLoader.GetResourceInstance(), MAKEINTRESOURCE( IDR_MAINFRAME ) );
	wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wndcls.lpszClassName = _T(NAV_WIN_CLASS_NAME);
	VERIFY(AfxRegisterClass(&wndcls));
	cs.lpszClass = wndcls.lpszClassName;


	return bRet;
}

LRESULT CMainFrame::OnTaskFocus( WPARAM wparam, LPARAM lparam )
{
	m_wndSplitter->GetPane( 0, 1 )->SetFocus();

	return 1;
}

void CMainFrame::DelSelTask()
{
	CTaskView *ptrView = (CTaskView*)m_wndSplitter->GetPane( 0, 1 );

	if( ptrView )
		ptrView->DelTask();
}

//----------------------------------------------------------------
// OnCommand
//
//	NOTE: This is overriden so I can get the menu item message
//		meant for the Task Pad. I first call the base class
//		implementation to see if it is handled.
//		If the message is not handled, I can send it to the
//		task pad to see if it can be handled there.
//----------------------------------------------------------------
BOOL CMainFrame::OnCommand( WPARAM wParam, LPARAM lParam )
{
	//First call the base class implementation
	if( !CFrameWnd::OnCommand( wParam, lParam ) )
	{
		//If the command is out of my array bounds,
		//	simply return FALSE.
		if( wParam > 50 )
			return FALSE;

		//--------------------------------------------------//
		// HACK ALERT!!!!!!
		// In Windows95 & Windows98, the wParam is always
		//	chopping off the hiword of the command value.
		//	To compensate for this, I keep my own internal
		//	table of commands and map them to the proper
		//	value.
		//--------------------------------------------------//
		InterpretSnapInCommand( wParam );

		CString		strText;
		WORD		wordID = LOWORD( wParam ),
					wordGroupID = HIWORD( wParam );
		
		if( CheckRecording() )
			return 0;

		CTaskpad	*ptrTaskPad = &((CTaskpadView*)m_wndSplitter->GetPane( 0, 0 ))->m_taskpad;

		ptrTaskPad->SelectItem( ptrTaskPad->GetRootItem() );

		//Switch views so I can display the Task property sheet
		m_wndSplitter->ReplaceView( 0, 1, RUNTIME_CLASS(CTaskView), CSize(0,0));

		CTaskView *ptrView = (CTaskView*)m_wndSplitter->GetPane( 0, 1 );

		if (ptrView )
		{
			ptrView->SetTaskPadPtr( ptrTaskPad );
			ptrView->SelectTask( wordID, wordGroupID  );
		}
	}

	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

//----------------------------------------------------------------
// OnCreateClient
//----------------------------------------------------------------
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
    CSize   sizeLeft(0, 0), 
            sizeRight(0, 0),
            sizeNormal(220, 0),
            sizeSmall(190, 0);
	HKEY	hKey;
	DWORD	dwSize = sizeof( CSize );

    // Determine if we are running in 640x480 and if so, adjust the left-pane
    // so the client fits on the screen
    HWND hWnd = ::GetDesktopWindow();
    CRect rect;
    ::GetWindowRect(hWnd, rect);

    if(rect.Width() <= 640)
        sizeLeft += sizeSmall;
    else
        sizeLeft += sizeNormal;

	//Try to read the sizes out of the registry
	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_CURRENT_USER,
						szReg_Key_Main,
						0,
						KEY_READ,
						&hKey ) )
	{
		SymSaferRegQueryValueEx(	hKey,
							szReg_Val_LeftPane,
							0,
							NULL,
							(BYTE*)&sizeLeft,
							&dwSize );

		dwSize = sizeof( CSize );
		SymSaferRegQueryValueEx(	hKey,
							szReg_Val_RightPane,
							0,
							NULL,
							(BYTE*)&sizeRight,
							&dwSize );

        RegCloseKey( hKey );
	}


	try
	{
		m_wndSplitter = new CSplitter;
	}
	catch (std::bad_alloc &){}

	m_wndSplitter->CreateStatic(  this, 1, 2 );

	m_wndSplitter->CreateView( 0, 0, RUNTIME_CLASS( CTaskpadView ), sizeLeft,
								pContext );

	m_wndSplitter->CreateView( 0, 1, RUNTIME_CLASS( CDefaultView ), sizeRight,
								pContext );
	
	return TRUE;
}

LRESULT CMainFrame::OnPostInitialize( WPARAM wparam, LPARAM lparam )
{
	if( GetClientType() == CLIENT_TYPE_SERVER )
		GetMenu()->RemoveMenu( 1, MF_BYPOSITION);

	return( -1 );
}

//----------------------------------------------------------------
// OnDestroy
//----------------------------------------------------------------
void CMainFrame::OnDestroy() 
{
	CRect	rect;
	HKEY	hKey;
	DWORD	dwSize = sizeof( CRect );
    DWORD   dwHWND = 0;
	CSize	sizeLeft,
			sizeRight;
	int		iTemp;

	if( !IsIconic() )
	{

		if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_CURRENT_USER,
							szReg_Key_Main,
							0,
							KEY_WRITE,
							&hKey ) )
		{

			GetWindowRect( &rect );

			RegSetValueEx(	hKey,
							szReg_Val_ClientPos,
							0,
							REG_BINARY,
							(BYTE*)&rect,
							sizeof( CRect ) );

			if( m_wndSplitter )
			{
				m_wndSplitter->GetRowInfo( 0, (int&)sizeLeft.cy, iTemp );
				sizeRight.cy = sizeLeft.cy;

				m_wndSplitter->GetColumnInfo( 0, (int&)sizeLeft.cx, iTemp );
				m_wndSplitter->GetColumnInfo( 1, (int&)sizeRight.cx, iTemp );

				RegSetValueEx(	hKey,
								szReg_Val_LeftPane,
								0,
								NULL,
								(BYTE*)&sizeLeft,
								sizeof( CSize ) );

				RegSetValueEx(	hKey,
								szReg_Val_RightPane,
								0,
								NULL,
								(BYTE*)&sizeRight,
								sizeof( CSize ) );
			}

			RegCloseKey( hKey );
		}
	}

	CFrameWnd::OnDestroy();

	if (m_wndSplitter)
	{
		m_wndSplitter->DestroyWindow();
		delete m_wndSplitter;
	}
	
}

void CMainFrame::OnEndTask()
{
	SwitchToView( RUNTIME_CLASS( CDefaultView ) );
}

void CMainFrame::StopCurrentScan()
{
	CView* ptrView = (CView*)m_wndSplitter->GetPane( 0, 1 );

	//If the view is a TaskView, then I can call the method
	//	to kill the scan
	if( ptrView->IsKindOf( RUNTIME_CLASS( CTaskView ) ) )
	{
		((CTaskView*)ptrView)->EndTask();
	}
}

void CMainFrame::SwitchToView( CRuntimeClass *pNewView )
{
	CString strTitle;

	CView* ptrView = (CView*)m_wndSplitter->GetPane( 0, 1 );

	//Only change if the current view is NOT CDefaultView
	if( !ptrView->IsKindOf( pNewView ) )
	{
		m_wndSplitter->ReplaceView( 0, 1, pNewView, CSize(0,0));

		//If the pointer is a pointer to the Default View,
		if( pNewView == RUNTIME_CLASS( CDefaultView ) )
		{
			//then I need to set the focus to the Root item in the tree
			CTaskpadView *ptrTP = (CTaskpadView*)m_wndSplitter->GetPane( 0, 0 );

			if( ptrTP )
			{
				ptrTP->FocusToRoot();
			}
		}
	}

}

void CMainFrame::OnRecordNew() 
{
//	CWProtect32Doc	*ptrDoc = (CWProtect32Doc*)GetActiveView()->GetDocument();
	CTaskView		*ptrView = (CTaskView*)m_wndSplitter->GetPane( 0, 1 );
	CString			strCategory;

	if( CheckRecording( RECORDING_SCRIPTED ) )
		return;

	if (ptrView )
	{
		strCategory.LoadString( IDS_DEFTASK_SCRIPTED );
		ptrView->SetTaskPadPtr( &((CTaskpadView*)m_wndSplitter->GetPane( 0, 0 ))->m_taskpad );
		ptrView->SelectTask( (DWORD)-1, strCategory );
	}

}

void CMainFrame::OnUpdateTaskDelete(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	CTaskpadView *ptrTaskPadView = (CTaskpadView*)m_wndSplitter->GetPane( 0, 0 );

	//Figure out if 
	//	1. a task is selected
	//	2. the selected task can be deleted
	//Otherwise, disable the Delete
	if( ptrTaskPadView->CanDeleteSelTask() )
	{
		bEnable = TRUE;
	}
	
	pCmdUI->Enable( bEnable );
}

void CMainFrame::OnTaskDelete() 
{
	//We wouldn't get here if we could not delete
	// the selected task, so go ahead and do it.
	CTaskpadView *ptrTaskPadView = (CTaskpadView*)m_wndSplitter->GetPane( 0, 0 );

	ptrTaskPadView->DeleteSelTask();

}


LRESULT CMainFrame::OnStopScanning( WPARAM wparam, LPARAM lparam )
{
	ScanEnding();

	return( 0 );
}

void CMainFrame::ScanStarting()
{
	EnableMenu( FALSE );

	m_wndSplitter->GetPane( 0,0 )->EnableWindow( FALSE );

	m_bScanning = TRUE;
}

void CMainFrame::ScanEnding()
{
	EnableMenu( TRUE );

	m_wndSplitter->GetPane( 0,0 )->EnableWindow( TRUE );
	m_bScanning = FALSE;
}

void CMainFrame::EnableMenu( BOOL bEnable )
{
	CMenu		*ptrMenu = GetMenu();
	UINT		iMenus,
				iState;

	if( ptrMenu )
	{
		iMenus = ptrMenu->GetMenuItemCount();
		iState = (bEnable) ? MF_ENABLED : MF_GRAYED;
		
		for( UINT x = 0; x < iMenus; x++ )
		{
			ptrMenu->EnableMenuItem( x, iState | MF_BYPOSITION );
		}
	}

	DrawMenuBar();

}


void CMainFrame::OnNewScheduled() 
{
//	CWProtect32Doc	*ptrDoc = (CWProtect32Doc*)GetActiveView()->GetDocument();
	CTaskView		*ptrView = (CTaskView*)m_wndSplitter->GetPane( 0, 1 );
	CString			strCategory;

	if( CheckRecording( RECORDING_SCHEDULE ) )
		return;

	if (ptrView )
	{
		strCategory.LoadString( IDS_GROUP_SCHEDULED_TASKS );
		ptrView->SetTaskPadPtr( &((CTaskpadView*)m_wndSplitter->GetPane( 0, 0 ))->m_taskpad );
		ptrView->SelectTask( (DWORD)-1, strCategory );
	}
}

void CMainFrame::OnContents() 
{
	//Help call from the SAV Toolbar
    if ( !m_gHelpFileName.IsEmpty() ) 
    {
		::HtmlHelp( AfxGetMainWnd()->GetSafeHwnd(), (LPCTSTR)m_gHelpFileName, HH_DISPLAY_TOPIC, NULL );
    }
}

void CMainFrame::OnVirusEncyclopedia() 
{
	::WinHelp( AfxGetMainWnd()->m_hWnd, (LPCTSTR)m_gEncyclopediaName, HELP_FINDER, 0 );
}

void CMainFrame::OnRecordStartup() 
{
//	CWProtect32Doc	*ptrDoc = (CWProtect32Doc*)GetActiveView()->GetDocument();
	CTaskView		*ptrView = (CTaskView*)m_wndSplitter->GetPane( 0, 1 );
	CString			strCategory;

	if( CheckRecording( RECORDING_STARTUP ) )
		return;

	if (ptrView )
	{
		strCategory.LoadString( IDS_DEFTASK_STARTUP );
		ptrView->SetTaskPadPtr( &((CTaskpadView*)m_wndSplitter->GetPane( 0, 0 ))->m_taskpad );
		ptrView->SelectTask( (DWORD)-1, strCategory );
	}
}

void CMainFrame::OnUpdateRecordNew(CCmdUI* pCmdUI) 
{
//	CWProtect32Doc	*ptrDoc = (CWProtect32Doc*)GetActiveView()->GetDocument();
//	pCmdUI->Enable( ptrDoc->m_fRecording == 0 );
	pCmdUI->Enable( TRUE );
}

void CMainFrame::OnUpdateNewScheduled(CCmdUI* pCmdUI) 
{
//	CWProtect32Doc	*ptrDoc = (CWProtect32Doc*)GetActiveView()->GetDocument();
//	pCmdUI->Enable( ptrDoc->m_fRecording == 0 );
	pCmdUI->Enable( TRUE );
}

void CMainFrame::OnUpdateRecordStartup(CCmdUI* pCmdUI) 
{
	if (IsUnmanagedClient())
	{
		// always enable this for unmanaged clients
		pCmdUI->Enable( TRUE );
	}
	else
	{
		// are startup scans turned on, and not locked?
		BOOL bEnable = StartupScansEnabled() && !StartupScansLocked();
		pCmdUI->Enable( bEnable );
	}
}

//If the Update capability is locked, I need to grey out this menuitem
void CMainFrame::OnUpdateUpdatePatternfile(CCmdUI* pCmdUI) 
{
	CString     strSubKey;
	HKEY	    hKey;
    BOOL        bNormalUser = FALSE;
    BOOL        bCanUpdateRegKey = FALSE;
    CAdminInfo  AdminInfo;

    // TC - ported CRT fix from 7.61. Defect 374531
    //    bNormalUser = !AdminInfo.CanProcessUpdateRegKey( NULL, HKEY_LOCAL_MACHINE, szReg_Key_Main );
    // bug fix - check user class, not access to nav keys to launch lu through cliproxy
    // admin/owners later demoted to users sometimes fail access test - XP only?
 	AdminInfo.IsProcessNormalUser( &bNormalUser );
    bCanUpdateRegKey = AdminInfo.CanProcessUpdateRegKey( NULL, HKEY_LOCAL_MACHINE, szReg_Key_Main );

	//See if it is locked by the administrator
	BOOL bEnable = FALSE;
    BOOL bEnableAllUsers = FALSE;

    if ( bNormalUser )
    {
        // TC: STS Defect 366765
        // 
        // If the current user is "restricted", then we check the EnableAllUsers
        // value under the PatternManager key. If the value is 1, then we enable
        // the LU button.

        strSubKey.Format( "%s\\%s", szReg_Key_Main, szReg_Key_PatternManager );

	    if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										    strSubKey,
										    0,
										    KEY_READ,
										    &hKey) )
	    {
		    DWORD	dwSize = sizeof( DWORD );
            DWORD   dwLockUpdates = 0;
            DWORD   dwEnableAll = 0;

            //Can the user update the pattern file?
            SymSaferRegQueryValueEx( hKey,
							 szReg_Val_LockUpdatePattern,
							 NULL,
							 NULL,
							 (BYTE*)&dwLockUpdates,
							 &dwSize );

		    // Is EnableAllUsers turned on?
		    SymSaferRegQueryValueEx( hKey,
			                 szReg_Val_EnableAllUsers,
						     NULL,
						     NULL,
						     (BYTE*)&dwEnableAll,
						     &dwSize );

		    RegCloseKey( hKey );

            // If the pattern files are locked), then ...
            if ( dwLockUpdates )
			{
				bEnable = FALSE;
			}
			else
            {
                // Can we either update keys or use the update all users method?
                if ( bCanUpdateRegKey || dwEnableAll )
                {
                    bEnable = TRUE;
                }
            }
	    }
    }
    else
    {
  	    if( GetClientType() != CLIENT_TYPE_SERVER )
  	    {
  	        strSubKey.Format( "%s\\%s", szReg_Key_Main, szReg_Key_PatternManager );
  	        //Open the proper key for this value
  	        if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
  										        strSubKey,
  										        0,
  										        KEY_READ,
  										        &hKey) )
  	        {
  		        //Can the user update the pattern file?
  		        DWORD	dwSize = sizeof( DWORD ),
  				        dwTemp = 0;
  
  		        SymSaferRegQueryValueEx(	hKey,
  							        szReg_Val_LockUpdatePattern,
  							        NULL,
  							        NULL,
  							        (BYTE*)&dwTemp,
  							        &dwSize );
  
  		        bEnable = (dwTemp == 0);
  
  		        RegCloseKey( hKey );
  	        }
  	    }
		else
		{
			bEnable = AdminInfo.CanProcessUpdateRegKey( NULL, HKEY_LOCAL_MACHINE, szReg_Key_Main );
		}
    }

	pCmdUI->Enable( bEnable );
}

void CMainFrame::OnUpdatePatternfile() 
{
	CString strSubKey;
	HKEY	hKey = NULL;

	PROCESS_INFORMATION processInfo;
	STARTUPINFO			infoStartup;
	memset( &infoStartup, '\0', sizeof( STARTUPINFO ) );
	infoStartup.cb = sizeof( STARTUPINFO );

    CAdminInfo  AdminInfo;
    IUtil*      pUtil = NULL;
    BOOL        bEnableAllUsers = 0;
    BOOL        bNormalUser = 0;

    // See if we're a normal user (i.e. without reg write access)
    bNormalUser = !AdminInfo.CanProcessUpdateRegKey( NULL, HKEY_LOCAL_MACHINE, szReg_Key_Main );
    // bug fix (1-2SNQQ1) - original port from 7.6 was made to the wrong place - also check for Normal user here
	if (!bNormalUser)
		AdminInfo.IsProcessNormalUser( &bNormalUser );

    try
    {
        strSubKey.Format( "%s\\%s", szReg_Key_Main, szReg_Key_PatternManager );
    }
    VP_CATCH_MEMORYEXCEPTIONS( return; );

	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										strSubKey,
										0,
										KEY_READ,
										&hKey) )
	{
        DWORD   dwVal;

        // TC: STS Defect 366765
        // 
        // Get value of PatternManager/EnableAllUsers

		if(ERROR_SUCCESS == SymSaferRegQueryDWORDValue(hKey,
						                    szReg_Val_EnableAllUsers,
						                    &dwVal ))
        {
            if ( dwVal == 1 )
            {
                bEnableAllUsers = TRUE;
            }
        }

		RegCloseKey( hKey );
        hKey = NULL;
	}


    if ( bNormalUser && bEnableAllUsers )
    {
        // TC: STS Defect 366765
        // 
        // If we're running as a restricted user and EnableAllUsers is set, then
        // start LiveUpdate.

        // Create an instance of the IUtil interface from Cliproxy
        if ( S_OK == CoCreateLDVPObject( CLSID_CliProxy, IID_IUtil, (void**)&pUtil) )
        {        
            // Tell RTVScan to start LiveUpdate
            pUtil->LaunchLU();

            // Clean up
            pUtil->Release();
            pUtil = NULL;
        }
        else
        {
            // No Cliproxy/RTVScan communication

		    CString strError;
		    
            try
            {
		        strError.LoadString( IDS_ERROR_CANT_START_LIVEUPDATE );
    	        AfxMessageBox( strError );
            }
            VP_CATCH_MEMORYEXCEPTIONS(;)
        }
    }
    else
    {
        // set up filter format expected by vpdn_lu.exe, which will
        // process it into format expected by navlu.dll.
        try
        {
	        CString strCmdLine( "\"vpdn_lu.exe\"" );

            strSubKey.Format( "%s\\%s", szReg_Key_Main, szReg_Key_PatternManager );
	        //Open the proper key for this value
	        if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										        strSubKey,
										        0,
										        KEY_READ,
										        &hKey) )
	        {
                DWORD   dwVal = 0;

		        // should the user download product updates?
		        if( ERROR_SUCCESS != SymSaferRegQueryDWORDValue(hKey,
						                            szReg_Val_EnableProductUpdates,
						                            &dwVal ) ||
				    dwVal == 0 )
                {
				    // filter products
				    strCmdLine += " /fUpdate";
                }

			    // should the user download virusdefs?
			    dwVal = 0; // reset
		        if( ERROR_SUCCESS == SymSaferRegQueryDWORDValue(hKey,
						                            szReg_Val_DisableVirusDefUpdates,
						                            &dwVal ) &&
				    dwVal != 0 )
                {
				    // filter content updates
				    strCmdLine += " /fVirusDef";
                }

			    // should the user download SCF content (Trojans and IDSs)?
			    dwVal = 0; // reset
		        if( ERROR_SUCCESS == SymSaferRegQueryDWORDValue(hKey,
						                            szReg_Val_DisableSCFContentUpdates,
						                            &dwVal ) &&
				    dwVal != 0 )
                {
				    // filter content updates
				    strCmdLine += " /fSCFContent";
                }
	        }

            // Note: double-quotes already placed around application name in command line.

		    // Invoke VPDN_LU.EXE with the cmd line that we just built.
	        if( !CreateProcess(
						        NULL,
						        (LPSTR)(LPCSTR)strCmdLine,
						        NULL,
						        NULL,
						        FALSE,
						        CREATE_SEPARATE_WOW_VDM,
						        NULL,
						        NULL,
						        &infoStartup,
						        &processInfo ) )
	        {
		        CString strError;
    		    
		        strError.LoadString( IDS_ERROR_DOWNLOAD_PATTERN );
		        AfxMessageBox( strError );
	        }
            else
            {
                CloseHandle( processInfo.hThread );
                CloseHandle( processInfo.hProcess );
            }
        }
        VP_CATCH_MEMORYEXCEPTIONS(;)

        if ( NULL != hKey )
        {
		    RegCloseKey( hKey );
            hKey = NULL;
        }
    }
}

//If the Update Scheduling capability is locked, I need to grey out this menuitem
void CMainFrame::OnUpdateScheduleUpdates(CCmdUI* pCmdUI) 
{
	HKEY		hKey;
	CString		strSubKey;

	//See if it is locked by the administrator
	BOOL bEnable = TRUE;

	if( GetClientType() != CLIENT_TYPE_SERVER )
		{
		strSubKey.Format( "%s\\%s", szReg_Key_Main, szReg_Key_PatternManager );
		//Open the proper key for this value
		if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
											strSubKey,
											0,
											KEY_READ,
											&hKey) )
			{
			//Can the user schedule updates of the pattern file?
			DWORD	dwSize = sizeof( DWORD ),
					dwTemp = 0;

			SymSaferRegQueryValueEx(	hKey,
								szReg_Val_DisallowClientScheduling,
								NULL,
								NULL,
								(BYTE*)&dwTemp,
								&dwSize );

			bEnable = (dwTemp == 0);

			RegCloseKey( hKey );
			}		
		}

	//pCmdUI->Enable( bEnable );
	s_bReadonly = bEnable ? false : true;
}	


void CMainFrame::OnUpdateLoadServices(CCmdUI* pCmdUI) 
{
	HKEY		hKey;
	CString		strSubKey;
	CAdminInfo	adminInfo;

	BOOL bEnable = FALSE;

	if (g_bEnableServicesCheckBox)
		{
		if( GetClientType() != CLIENT_TYPE_SERVER )
			{
			strSubKey.Format( "%s\\%s\\%s", szReg_Key_Main, szReg_Key_AdminOnly, szReg_Key_Security );
			//Open the proper key for this value
			if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
												strSubKey,
												0,
												KEY_READ,
												&hKey) )
				{
				//Can the user turn off the services?
				DWORD	dwSize = sizeof( DWORD ),
						dwTemp = 0;

				SymSaferRegQueryValueEx(	hKey,
									szReg_Val_LockUnloadServices,
									NULL,
									NULL,
									(BYTE*)&dwTemp,
									&dwSize );

				bEnable = (dwTemp == 0);

				RegCloseKey( hKey );
				}
			}
		}

	long lServiceState = GetServicesState();

	// if the services are half loaded, disable the menu item
	// i.e. don't allow the user to unload them yet!
	if( lServiceState == 2 )
		bEnable = FALSE;

	CString strText;

	if ( lServiceState )
		strText.LoadString(IDS_UNLOAD_SERVICE);
	else
		strText.LoadString(IDS_LOAD_SERVICE);

	pCmdUI->SetText(strText);

	// now enable or disable the menu item

	pCmdUI->Enable( bEnable );
}	


void CMainFrame::OnScheduleUpdates() 
{
	IGenericConfig	*ptrGenConfig = NULL;
    DWORD   		dwRootID;

	dwRootID = HKEY_VP_MAIN_ROOT;

	if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IGenericConfig, (void**)&ptrGenConfig ) ) ) 
		{
			DWORD dwErr = ptrGenConfig->Open( NULL, dwRootID, szReg_Key_PatternManager, GC_MODEL_SINGLE /* flags ignored in Cli* objects */ );

			if( dwErr != 0 )
			{
				TRACE1("ERROR: Open on IGenericConfig reutrned 0x%X\n", dwErr );
				AfxMessageBox( IDS_ERROR_NO_OPEN_SCHEDULE );
			}
			else
                {
                CScheduleEnableDlg dlg(ptrGenConfig, NULL);
				dlg.SetReadonly(s_bReadonly);
                dlg.DoModal();
                }

			//I no longer need this interface
			ptrGenConfig->Release();
		}
}

void CMainFrame::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CFrameWnd::OnSizing(fwSide, pRect);

	TRACE("CMainFrame::OnSizing\n" );	
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CTaskDescriptionView *ptrView;

	CFrameWnd::OnSize(nType, cx, cy);
	
	if( m_wndSplitter && ( ptrView = (CTaskDescriptionView*)m_wndSplitter->GetDlgItem(m_wndSplitter->IdFromRowCol(0, 1)) ) ) 
	{
		//If we are displaying the Task Description view, then
		//	we need to manually tell it to resize itself
		if( ptrView->IsKindOf( RUNTIME_CLASS( CTaskDescriptionView ) ) )
		{
			ptrView->RedrawWindow();
		}
	}

}

void CMainFrame::OnGetMinMaxInfo( MINMAXINFO* lpMMI )
{
	CView *ptrView;
	int		cx = (220 * LOWORD( ::GetDialogBaseUnits() ))/4,
			cy = (230 * HIWORD( ::GetDialogBaseUnits() ))/8;


	//Now, add to the X coord the size of the right pane
	if( m_wndSplitter && ( ptrView = (CView*)m_wndSplitter->GetDlgItem(m_wndSplitter->IdFromRowCol(0, 0)) )  )
	{
		CRect rect;

		ptrView->GetClientRect( &rect );

		lpMMI->ptMinTrackSize.y = cy;
		lpMMI->ptMinTrackSize.x = cx + rect.right;
	}
}

LRESULT CMainFrame::OnResizeTaskpad( WPARAM, LPARAM )
{
	CRect		rect;
	MINMAXINFO	info;
	BOOL		bNeedToRedraw = FALSE;

	memset( &info, 0x00, sizeof( info ) );

	GetWindowRect( &rect );
	OnGetMinMaxInfo( &info );

	//If either position is too small, resize the frame
	if( (rect.bottom - rect.top ) < info.ptMinTrackSize.y )
	{
		bNeedToRedraw = TRUE;
		rect.bottom = info.ptMinTrackSize.y;
	}

	//I have no idea why I need to switch the coords here, but it
	//	makes it work, and I am too tired of messing around with
	//	this sizing crap, so I will leave it as is....
	ScreenToClient( &rect );
	//If either position is too small, resize the frame
	if( rect.right < info.ptMinTrackSize.x )
	{
		bNeedToRedraw = TRUE;
		rect.right = info.ptMinTrackSize.x;
	}

	//If either position is too small, resize the frame
	if( bNeedToRedraw )
	{
		SetWindowPos( &wndTop, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE );
	}

	return( 1 );
}

/*
void CMainFrame::OnRegister()
{
	// add the Intel HusDawg registration
	ILDVPRegister* pRegister;

	if ( SUCCEEDED( CoCreateInstance( CLSID_LDVPRegister, NULL, CLSCTX_INPROC_SERVER,
			IID_ILDVPRegister, (LPVOID*)&pRegister ) ) )
	{
		pRegister->RegisterUserWithIntel( (long)m_hWnd, TRUE );
		pRegister->Release();
	}
}
*/

void CMainFrame::OnConfigHistory() 
{
	//Get the IScanDlgs interface
	IScanDialogs	*pScanDlg = NULL;
	IScanConfig		*pConfig = NULL;
	HRESULT			hr = E_FAIL;

	//Get the IConfig
	if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IScanConfig, (void**)&pConfig ) ) )
	{
		hr = pConfig->Open(NULL,HKEY_VP_MAIN_ROOT,"");
	}

	//Try to create & get the interface
	if( SUCCEEDED( hr ) && 
		SUCCEEDED( CoCreateInstance( CLSID_ScanDialogs, NULL, CLSCTX_INPROC_SERVER,
										IID_IScanDialogs, (void**)&pScanDlg ) ) )
	{
		pScanDlg->ShowConfigHistoryDlg( m_hWnd, 0, pConfig, NULL );
		pScanDlg->Release();
	}
	else
		AfxMessageBox( IDS_SCAN_DIALOGS_ERROR );

	if( pConfig )
		pConfig->Release();
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_SETTEXT )
	{
		return CFrameWnd::WindowProc(message, wParam, (LPARAM)(LPCTSTR)m_strTitle);
		TRACE1( "Setting title to %s\n", (LPCTSTR)lParam );
	}
	else
		return CFrameWnd::WindowProc(message, wParam, lParam);
}


BOOL CMainFrame::GetStartupFlag()
{
    return m_bStartupInProgress;
}


BOOL CMainFrame::SetStartupFlag(BOOL bStartup)
{
    m_bStartupInProgress = bStartup;

    return TRUE;
}

void CMainFrame::SetNumQuarantineItems()
{
    m_dwNumQuarantineItems = CountQuarantineItems(VBIN_INFECTED);
}

DWORD CMainFrame::GetNumQuarantineItems()
{
    return m_dwNumQuarantineItems;
}

DWORD CMainFrame::CountQuarantineItems(DWORD dwQuarantineFlags)
{
    DWORD       dwNumItems = 0;
    DWORD       dwVBinIndex = 0;
    VBININFO    stInfo = {0};
    IVBin*      pVBin = NULL;
    HANDLE      hVBinFind = NULL;
    HCURSOR     hCurrentCursor = NULL;

    // Create an instance of the IVBin interface
    if ( S_OK == CoCreateLDVPObject( CLSID_Cliscan, IID_IVBin, (void**)&pVBin) )
    {        
        // Init the vbin info structure
        stInfo.Size = sizeof(VBININFO);

        // Enumerate all the items in the virus bin.
        hVBinFind = pVBin->FindFirstItem(&stInfo);

        // Did we get a valid handle?
        if ( hVBinFind )
        {
            // Yep. Change the cursor to an hourglass ... could take a while
            hCurrentCursor = GetCursor();
            SetCursor(LoadCursor(NULL, IDC_WAIT));

            // Loop through the files
            do
            {
                // Check the VBININFO flags against those passed in.
                // If it matches, we can count this one.
                if ( stInfo.Flags & dwQuarantineFlags )
                    dwNumItems++;

                // Reset the buffer
                memset(&stInfo, 0, sizeof(VBININFO));
                stInfo.Size = sizeof(VBININFO);

            }  while ( pVBin->FindNextItem(hVBinFind, &stInfo) );

            // Close down the find operation
            pVBin->FindClose( hVBinFind );

            // Restore the cursor
            SetCursor( hCurrentCursor );
        }

        // Release the IVBin object
        pVBin->Release();
    }

    // Return the number of items we counted
    return dwNumItems;
}

////////////////////////////////////////////////////////////////////////
//
// function name: CMainFrame::WinHelpInternal
//
// description: WinHelpInternal override to trap WinHelpInternal calls and redirect 
//				them to HTMLHelp
// return type: none
//
///////////////////////////////////////////////////////////////////////
// 8/1/03 JGEIGER - Inserted this function to implement HTMLHelp for SAV. 
///////////////////////////////////////////////////////////////////////

void CMainFrame::WinHelpInternal(DWORD dwData, UINT nCmd) 
{
	CWinApp* theApp = AfxGetApp();
	CString helpFilePath = theApp->m_pszHelpFilePath;
 
	::HtmlHelp(AfxGetMainWnd()->GetSafeHwnd(), helpFilePath, HH_HELP_CONTEXT, dwData);
}

void CMainFrame::OnInitMenu(CMenu* pMenu) 
{
	CFrameWnd::OnInitMenu(pMenu);
	
	// Manually restore focus to the mainframe when
	// the user activates a top-level menu item so that
	// the previous (child) dialog that had focus won't keep it.
	::SetFocus(m_hWnd);
	//EA 03/12/2000
	//Setting the taskpad view as active view when menu it activated
	//so that the focus is not on the task veiw this was causing a problem
	//in realtime scan dialog when on activation of menu the menu would freeze
	//as it's focus was still on the task view
	CTaskpadView* pView= (CTaskpadView*)m_wndSplitter->GetPane( 0, 0 );
	if(pView)
		SetActiveView(pView);
	//EA 03/12/2000
	
}

void CMainFrame::OnViruslist() 
{
	// April 2003 - rewriting this function so that the VPC32 virus list matches
	// the list for the client/server displayed in SSC, and to remove usage of old
	// Core / AVCore routines (NGUI, N32VList, N32Call, S32NavR, etc.). Now, new
	// CliProxy interfaces will SendCOM messages to the server to return the list
	// and total virus count.

	// Invoke the dialog (using shared code in SrvSnap)
	CVirusListView	cVirusList;


	// Set the virus def dsecription.

	HKEY hkey = NULL;
	DWORD size = sizeof(DWORD);
	DWORD defs = 0;
	CString defs_str;

	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE,
									   szReg_Key_Main,
									   0,
									   KEY_READ,
									   &hkey ) )
	{
		if( SymSaferRegQueryValueEx( hkey,
							 szReg_Val_PatVersion,
							 NULL,
							 NULL,
							 reinterpret_cast<BYTE*>(&defs),
							 &size ) == ERROR_SUCCESS )
		{
			VDBVersionString( defs, "NAV", defs_str.GetBuffer(MAX_PATH), MAX_PATH, 0 );
			defs_str.ReleaseBuffer();

			cVirusList.SetPattern( defs_str );
		}

		RegCloseKey( hkey );
	}

	cVirusList.SetIconID( IDR_MAINFRAME );

	cVirusList.DoModal();
}

void CMainFrame:: OnExitMenuLoop(BOOL bIsTrackPopupMenu)
{
	//EA 03/22/2000 added function so that after exiting menu
	//we get back focus to our rt hand side panes for key board
	//access
	CFrameWnd::OnExitMenuLoop(bIsTrackPopupMenu);
	CTaskView *ptrView = (CTaskView*)m_wndSplitter->GetPane( 0, 1 );
	if(ptrView)
	{
//		SetActiveView(ptrView);
		ptrView->SetFocus();
	}
	//EA 03/22/2000
}

// MH 08.27.00
// Since the CWnd pointer handed into the taskpad's TrackPopupMenu() is CMainFrame,
// we need to handle these command events and route them to CTaskView where
// it really gets handled, else it won't do anything.
// This bug is just something I noticed whilst fixing another bug...
void CMainFrame::OnCut() 
{
	CTaskpadView* pTaskpadView = (CTaskpadView*) m_wndSplitter->GetPane( 0, 0 );
	if( pTaskpadView)
		{
		pTaskpadView->Cut();
		}
}

void CMainFrame::OnPaste() 
{
	CTaskpadView* pTaskpadView = (CTaskpadView*) m_wndSplitter->GetPane( 0, 0 );
	if( pTaskpadView )
		{
		pTaskpadView->Paste();
		}
}

//----------------------------------------------------------------
// OnLoadServices
//----------------------------------------------------------------
void CMainFrame::OnLoadServices() 
{
	CUnloadDlg	dlg;
	long		lServiceState;
	BOOL		bFailed;
	HRESULT		hr = E_FAIL;

	lServiceState = GetServicesState();

	if( 1 == lServiceState )
	{
		//Display the dialog
		if( IDCANCEL == dlg.DoModal() )
			return;

        // Do not Unload Services if StartUp Scans are in progress.
        // UnloadServices() will terminate VPTRAY while startup scans are in
        // progress and leads to crashes.  This is due to the old (known)
        // problem of lack of communication between Scan Threads and the
        // process that initiated the scans.
        // Fix for STS #342428.
        if(CheckForStartUpScans())
        {
            CString strTitle;
            CString strMessage;
            strTitle.LoadString(AFX_IDS_APP_TITLE);
            strMessage.LoadString(IDS_ERROR_STARTUP_SCANS);

            MessageBox(strMessage, strTitle, MB_OK|MB_ICONSTOP);
            return;
        }

		//Go ahead with the unload
		hr = UnloadServices();
		lServiceState = GetServicesState();
		bFailed = (lServiceState != 0);
	}
	else
	{
		hr = LoadServices();
		lServiceState = GetServicesState();
		bFailed = (lServiceState != 1);
	}

	//Check the return value, and display an appropriate message
	if( bFailed )
	{
		LPSTR	szError = NULL;
		CString	strError;

		//First, see if it is a standard windows error message
		if( FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							(DWORD)hr,
							0,
							szError,
							256,
							NULL ) )
		{
			//Display the message
			AfxMessageBox( szError );
			LocalFree( szError );
		}
		else
		{
			//If I couldn't format the message, then it is probably a
			// LDVP-specific error code
			if( hr == ERROR_REG_FAIL )
				strError.LoadString( IDS_ERROR_REG_FAIL );
			else if( hr == ERROR_SERVICE_HUNG )
				strError.LoadString( IDS_ERROR_SERVICE_HUNG );
			else
				strError.LoadString( IDS_ERROR_UNKNOWN );

			AfxMessageBox( strError );
		}
	}
}

//----------------------------------------------------------------
// CheckForStartUpScans()
// Enumerates the registry for all the StartUp Scans and checks
// to see if they are in progress.
//----------------------------------------------------------------
BOOL CMainFrame::CheckForStartUpScans()
{
	IScanConfig *pConfig=NULL;
	HRESULT hr;
    BOOL bRet = FALSE;
    
	hr = CreateScanConfig (CLSID_Cliscan, HKEY_VP_USER_SCANS, szReg_Value_Startup, pConfig);
	if (hr==S_OK)
	{		
		int i=0;
		char* pKeyName=NULL;
        
        // Enumerate the StartUp Scans listed under TaskPadStartup
		hr = pConfig->EnumSubOptions(i,&pKeyName);

		while(hr==S_OK)			
		{
			i++;
			char FullKey[MAX_PATH];
			lstrcpy(FullKey,szReg_Value_Startup);
			lstrcat(FullKey,"\\");
			lstrcat(FullKey,pKeyName);

			if(IsStartUpScanInProgress(FullKey))
            {
                bRet = TRUE;
                CoTaskMemFree(pKeyName);
                break;
            }

			CoTaskMemFree(pKeyName);
			hr = pConfig->EnumSubOptions(i,&pKeyName);
		}
	
		pConfig->Release();
	}

    return bRet;
}

//----------------------------------------------------------------
// CreateScanConfig
// Creates and opens a ScanConfig in one call
//----------------------------------------------------------------
HRESULT CMainFrame::CreateScanConfig(const CLSID &clsidScanner, DWORD RootID,char *SubKey, IScanConfig*& pConfig)
{
	HRESULT hr;
	hr = CoCreateLDVPObject(clsidScanner,IID_IScanConfig,(void**)&pConfig);
	if (hr==S_OK)
	{
		// open the new config object
		hr = pConfig->Open(NULL,RootID,SubKey);
		if (hr != S_OK)
		{
			pConfig->Release();
			pConfig=NULL;
		}
	}

	return hr;
}

//----------------------------------------------------------------
// IsStartUpScanInProgress
// Determines if a StartUp Scan is in Progress
//----------------------------------------------------------------
BOOL CMainFrame::IsStartUpScanInProgress(LPSTR FullKey)
{	
	char *pGuid=NULL;
	IScanConfig *pConfig = NULL;
	HRESULT hr;
    BOOL bRet = FALSE;

	// create a new config object based on the scan key
	hr = CreateScanConfig (CLSID_Cliscan, HKEY_VP_USER_SCANS, FullKey, pConfig);
	if (hr==S_OK)
	{
		hr = pConfig->GetOption("",(BYTE**)&pGuid,1024,(BYTE*)"");
		pConfig->Release();
        pConfig = NULL;
	}

	if (hr == S_OK)
	{	
		// open the guid scan key
		hr = CreateScanConfig (CLSID_Cliscan, HKEY_VP_USER_SCANS, pGuid, pConfig);

		if (hr != S_OK) 
            goto Done;

		CoTaskMemFree(pGuid);

        // If the status is anything other than S_DONE, S_ABORTED, S_DELAYED
        // or S_NEVER_RUN, then assume that a scan is in progress. 
        DWORD dwStatus = S_NEVER_RUN;
        hr = pConfig->GetIntOption(szReg_Value_ScanStatus,(long*)&dwStatus, S_NEVER_RUN);
        if(hr == S_OK)
        {
            if( !(dwStatus == S_DONE || dwStatus == S_SUSPENDED || dwStatus == S_NEVER_RUN || dwStatus == S_ABORTED || dwStatus == S_DELAYED) )
            {
                bRet = TRUE;
            }
        }
	
		pConfig->Release();
	}

Done:
    return bRet;
}

//----------------------------------------------------------------
// GetServicesState
//----------------------------------------------------------------
int CMainFrame::GetServicesState()
{
	IScanDialogs *pScanDlgs = NULL;
	long		 lServicesState = 0;

	if( SUCCEEDED( CoCreateInstance( CLSID_ScanDialogs, NULL, CLSCTX_INPROC_SERVER, IID_IScanDialogs, (void**)&pScanDlgs) ) )
	{
		pScanDlgs->GetServicesState( &lServicesState );
		pScanDlgs->Release();
	}

	return lServicesState;
}

//----------------------------------------------------------------
// UnloadServices
//----------------------------------------------------------------
HRESULT CMainFrame::UnloadServices()
{
	IScanDialogs *pScanDlgs = NULL;
	HRESULT hr = E_FAIL;
	CWaitCursor	wait;


	if( SUCCEEDED( CoCreateInstance( CLSID_ScanDialogs, NULL, CLSCTX_INPROC_SERVER, IID_IScanDialogs, (void**)&pScanDlgs) ) )
	{
		hr = pScanDlgs->UnLoadServices();
		pScanDlgs->Release();
	}

	return hr;
}

//----------------------------------------------------------------
// LoadServices
//----------------------------------------------------------------
HRESULT CMainFrame::LoadServices()
{
	IScanDialogs *pScanDlgs = NULL;
	HRESULT hr = E_FAIL;
	CWaitCursor	wait;

	if( SUCCEEDED( CoCreateInstance( CLSID_ScanDialogs, NULL, CLSCTX_INPROC_SERVER, IID_IScanDialogs, (void**)&pScanDlgs) ) )
	{
		hr = pScanDlgs->LoadServices();
		pScanDlgs->Release();
	}

	return hr;
}

LRESULT CMainFrame::OnUpdateDisplayedDefVersion( WPARAM wparam, LPARAM lparam )
{
	// Only pass this along to the default view.

	CWnd* current_view = m_wndSplitter->GetPane( 0, 1 );

	if( current_view->IsKindOf( RUNTIME_CLASS( CDefaultView ) ) )
	{
		current_view->SendMessage( NAV_CHECK_DEF_VERSION );
	}

	return( 0 );
}

