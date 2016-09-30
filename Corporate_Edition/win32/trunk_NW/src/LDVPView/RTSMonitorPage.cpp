// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// CRTSMonitor.cpp : implementation file
//

#include "stdafx.h"
#include "LDVPView.h"
#include "RTSMonitorPage.h"
#include "Wizard.h"
#include "atlbase.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRTSMonitor property page

IMPLEMENT_DYNCREATE(CRTSMonitor, CPropertyPage)

CRTSMonitor::CRTSMonitor() : CWizardPage(CRTSMonitor::IDD)
{
	//{{AFX_DATA_INIT(CRTSMonitor)
	m_strLastFileScanned = _T("");
	m_strLastUser = _T("");
	m_strLastAction = _T("");
	m_strLastVirusFound = _T("");
	m_strTimeStarted = _T("");
	m_strTotalScanned = _T("");
	m_strVirusesFound = _T("");
	m_strTimeOfLastInfection = _T("");
	//}}AFX_DATA_INIT

	m_psp.dwFlags |= PSP_HASHELP;
	//Set the Help Context ID
	SetHelpID( IDD );

	m_ptrRts = NULL;
}

CRTSMonitor::~CRTSMonitor()
{
	if( m_ptrRts )
	{
		m_ptrRts->Release();
		m_ptrRts = NULL;
	}
}

void CRTSMonitor::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRTSMonitor)
	DDX_Control(pDX, IDC_LOCATION, m_ctlLocation);
	DDX_Control(pDX, IDC_LAST_VIRUS_FOUND_FILE, m_ctlLastVirusFile);
	DDX_Text(pDX, IDC_LAST_FILE_SCANED, m_strLastFileScanned);
	DDX_Text(pDX, IDC_LAST_USER, m_strLastUser);
	DDX_Text(pDX, IDC_LAST_VIRUS_ACTION, m_strLastAction);
	DDX_Text(pDX, IDC_LAST_VIRUS_FOUND, m_strLastVirusFound);
	DDX_Text(pDX, IDC_TIME_STARTED, m_strTimeStarted);
	DDX_Text(pDX, IDC_TOTAL_SCANED, m_strTotalScanned);
	DDX_Text(pDX, IDC_VIRUSES_FOUND, m_strVirusesFound);
	DDX_Text(pDX, IDC_TIME_OF_LAST_INFECTION, m_strTimeOfLastInfection);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRTSMonitor, CPropertyPage)
	//{{AFX_MSG_MAP(CRTSMonitor)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CString GetActionNames(int i) 
{
	CString strReturn,
			strTemp;
	DWORD	ActionNames[] = { IDS_MOVE, IDS_RENAME, IDS_DELETE, IDS_LEAVEALONE, IDS_CLEAN };

	if( (i < 1) || (i > 5) )
	{
		//Invalid action
		strTemp.Format( "%d", i );
		AfxFormatString1( strReturn, IDS_INVALID, strTemp );
	}
	else 
		strReturn.LoadString( ActionNames[--i] );

	return strReturn;
}

CString GetTimeString( VTIME vTime )
{
	CString		strDate,
				strTime,
				strReturn;
	SYSTEMTIME	time;

	memset( &time, 0x00, sizeof( time ) );

	time.wYear = vTime.year + 1970;
	time.wMonth = vTime.month + 1;
	time.wDay = vTime.day;
	time.wHour = vTime.hour;
	time.wMinute = vTime.min;
	time.wSecond = vTime.sec;

	GetDateFormat( LOCALE_SYSTEM_DEFAULT, DATE_SHORTDATE, &time, NULL, strDate.GetBuffer( 256 ), 256 );
	strDate.ReleaseBuffer();

	GetTimeFormat( LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE, &time, NULL, strTime.GetBuffer( 256 ), 256 );
	strTime.ReleaseBuffer();

	if( !strDate.IsEmpty() && !strTime.IsEmpty() )
		AfxFormatString2( strReturn, IDS_DATE_AT, strDate, strTime );

	//If the year is 1970, then I don't have a valid date - return the empty string
	if( time.wYear == 1970 )
		strReturn.Empty();

	return strReturn;
}

/////////////////////////////////////////////////////////////////////////////
// CRTSMonitor message handlers

BOOL CRTSMonitor::OnSetActive() 
{
	CWizardPage::OnSetActive();
	// terrym 8-28-00 have to parse the raw resouce string now since
	// both the menu and resource string are enbedded
	CString sMenu,sTaskpad,sRaw;

	CWizard	*ptrParent = (CWizard*)GetParent();

	//Set the Context-sensitive ID
	SetHelpID( IDD_RTS_MONITOR );

	// terrym 8-28-00 have to parse the raw resouce string now since
	// both the menu and resource string are enbedded
	sRaw.LoadString( IDS_TASK_RTS  );
	ConvertRawMenuString(sRaw,sMenu,sTaskpad);
	
	//Load the options
	ptrParent->HideBack();
	ptrParent->m_strTitle=sTaskpad;
	ptrParent->SetFinishText( IDS_CLOSE );
	ptrParent->SetWizardButtons( PSWIZB_FINISH );

	return TRUE;
}

void CRTSMonitor::SizePage( int cx, int cy )
{
	//First, call tha base class version of this
	//		virtual method
	CWizardPage::SizePage( cx, cy );

	//Now, size the fields

}

BOOL CRTSMonitor::OnWizardFinish() 
{
	//Fire an event telling the App to close the task
	((CLDVPViewApp*)AfxGetApp())->m_ptrCtl->NotifyEndTask();

	return CPropertyPage::OnWizardFinish();
}


void CRTSMonitor::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	KillTimer(1);
}

BOOL CRTSMonitor::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	//Open up the RealtimeScan interface
	if( SUCCEEDED( CoCreateLDVPObject( CLSID_Cliscan, IID_IRealTimeScan, (void**)&m_ptrRts ) ) )
	{
		//Open the scan
		m_ptrRts->Open( NULL, NULL );
	}
	
	SetTimer(1,256,NULL);

	OnTimer(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRTSMonitor::OnTimer(UINT nIDEvent) 
{
	REALTIMEPROGRESSBLOCK	rtsBlock;
	int						iIndex;
	CString					strTemp;	//I need a temp string to put
										//	values into my StaticPath objects

	memset( &rtsBlock, 0x00, sizeof( rtsBlock ) );
	rtsBlock.Size = sizeof( rtsBlock );
	//
	//	Notes: To fix 1-3D7PUA: Client AP Status from  SSC and from client UI are different
	//
	//	About the fix:  (1) The two dialogs are different, should be the same
	//					(2) This is after ET2, UI frozen. So we can not change it, just make it
	//						looks like the same as far as possible
	//					(3) The UI elements were not even aligned, again, no change is made due to UI frozen
	//					(4) m_ptrRts->GetStatus() is bogus. so, get the AP status from the registry directly
	//

	//
	//	Find the path to the AP on/off regkey
	//
	DWORD dwAP = 1;
	CRegKey reg;
	CString strKey = szReg_Key_Main;
	strKey += "\\";
	strKey += szReg_Key_Storages;
	strKey += "\\";
	strKey += szReg_Key_Storage_File;
	strKey += "\\";
	strKey += szReg_Key_Storage_RealTime;
	//
	//	Get Current AP Status
	//
	if(reg.Open (HKEY_LOCAL_MACHINE,strKey,KEY_READ) == ERROR_SUCCESS)
	{
		reg.QueryDWORDValue(szReg_Val_RTSScanOnOff,dwAP);
	}
	CString strText;
	
	if(!dwAP)
	{
		//
		//	If AP is disabled, change "Start:" to "Status:", add "disabled"
		//	(see the above note for the limitation)
		//
		strText.LoadString (IDS_STATUS);
		::SetWindowText(::GetDlgItem(m_hWnd,IDC_STARTED_STATUS),strText);
		m_strTotalScanned = _T("");
		m_strTimeStarted.LoadString(IDS_NONE);
		m_strLastFileScanned = _T("");
		m_ctlLocation.SetWindowText(_T("")); 
		m_strVirusesFound  = _T(""); 
		m_ctlLastVirusFile.SetWindowText (_T("")); 
		m_strLastUser = _T("");
		m_strLastAction = _T("");
		m_strTimeOfLastInfection = _T("");
		m_strLastVirusFound = _T("");
		UpdateData( FALSE );
	}
	else if( SUCCEEDED( m_ptrRts->GetLastProgress( &rtsBlock ) ) )
	{
		//
		//	business as usual.
		//
		strText.LoadString (IDS_STARTED2);
		::SetWindowText(::GetDlgItem(m_hWnd,IDC_STARTED_STATUS),strText);

		//DWORD TotalScaned;
		m_strTotalScanned.Format( "%u", rtsBlock.TotalScaned );

		//VTIME TimeStarted;
		m_strTimeStarted = GetTimeString( rtsBlock.TimeStarted );
	
		//char LastScanned[IMAX_PATH];
		strTemp = rtsBlock.LastScanned;

		//Parse out the filename & location
		if( !strTemp.IsEmpty() )
		{
			if( -1 != (iIndex = strTemp.ReverseFind( _T('\\') ) ) )
			{
				m_strLastFileScanned = strTemp.Right( strTemp.GetLength() - (iIndex + 1));
				strTemp = strTemp.Left( iIndex );
				//EA 03/15/2000 made changes so that we dont
				//keep displaying even if the text in the window is the same
				//to display this was not allowing the edit
				//control to scroll changes control from cstatic to cedit
				//so we can scroll for long file names
				CString szCurrLocText;
				m_ctlLocation.GetWindowText(szCurrLocText);
				if(stricmp(szCurrLocText, strTemp) != 0 )
					m_ctlLocation.SetWindowText( strTemp );
				//EA 03/15/2000
			
			}
			else
			{
				m_strLastFileScanned = strTemp;
				m_ctlLocation.SetWindowText("");
			}
		}
		else
			m_ctlLocation.SetWindowText("");

		//DWORD VirusesFound;
		m_strVirusesFound.Format( "%u", rtsBlock.VirusesFound );

		//char LastVirusFoundName[MAX_VIRUS_NAME];
		m_strLastVirusFound = rtsBlock.LastVirusFoundName;

		if( !m_strLastVirusFound.IsEmpty() )
		{
			//char LastVirusFoundFile[IMAX_PATH];
			strTemp = rtsBlock.LastVirusFoundFile;
			//EA 03/15/2000 made changes so that we dont
			//keep displaying even if the text in the window is the same
			//to display this was not allowing the edit
			//control to scroll changes control from cstatic to cedit
			//so we can scroll for long file names
			CString szCurrLastVirusFile;
			m_ctlLastVirusFile.GetWindowText(szCurrLastVirusFile);
			if(stricmp(szCurrLastVirusFile, strTemp) != 0  )
			m_ctlLastVirusFile.SetWindowText( strTemp );
    		//EA 03/15/2000
	    	//char LastVirusFoundUserName[NAME_SIZE];
		    m_strLastUser = rtsBlock.LastScannedUserName;
		
			//DWORD LastVirusFoundAction;
			m_strLastAction = GetActionNames( rtsBlock.LastVirusFoundAction );

			//VTIME LastVirusFoundTime;
			m_strTimeOfLastInfection = GetTimeString( rtsBlock.LastVirusFoundTime );
		}
        

		//Update my fields with the latest data
		UpdateData( FALSE );
	}
}

void CRTSMonitor::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	PaintTitle( &dc );	
}

void CRTSMonitor::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnLButtonDown(nFlags, point);

	SetFocus();
}

void CRTSMonitor::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnRButtonDown(nFlags, point);

	SetFocus();
}

BOOL CRTSMonitor::ShowWindow(int nCmdShow )
{
	//MH 08.08.00
	//Brought over EA's earlier fix for a similar problem.
	//Activate the results view window with a mouse click by sending the mouse activate
	//message to its main window so that we can get focus even after returning from help.
	int nResult = CPropertyPage::ShowWindow(nCmdShow);
	HWND hWndMain = AfxGetMainWnd()->GetSafeHwnd();

	if(hWndMain)
		PostMessage(WM_MOUSEACTIVATE, (WPARAM)hWndMain,(MAKELONG(HTCLIENT,WM_LBUTTONDOWN)) );
	return nResult;
}
