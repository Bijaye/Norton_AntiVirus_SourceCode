// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// CustomPage.cpp : implementation file
//

#include "stdafx.h"
#include "LDVPTaskFS.h"
#include "ScanCustomPage.h"
#include "Wizard.h"
#include "ModalConfig.h"
#include <new>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern IID	IID_IScanConfig;

/////////////////////////////////////////////////////////////////////////////
// CScanCustomPage property page

IMPLEMENT_DYNCREATE(CScanCustomPage, CWizardPage)

CScanCustomPage::CScanCustomPage() : CWizardPage(CScanCustomPage::IDD)
{
	//{{AFX_DATA_INIT(CScanCustomPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_psp.dwFlags |= PSP_HASHELP;
	//Set the Help Context ID
	SetHelpID( IDD );
	m_bDeleted = FALSE;
}

BOOL CScanCustomPage::OnKillActive()
{
	return TRUE;
}

CScanCustomPage::~CScanCustomPage()
{
}

void CScanCustomPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScanCustomPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScanCustomPage, CWizardPage)
	//{{AFX_MSG_MAP(CScanCustomPage)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_COMMAND( UWM_DESTROYING, OnDestroying )
	ON_COMMAND( UWM_DESTROYSCAN, OnDeleteTask )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScanCustomPage message handlers

BOOL CScanCustomPage::OnSetActive() 
{
	CString			strSchedule;
	CWnd			*ptrWnd;
	CLDVPTaskFSCtrl	*ptrCtrl = ((CLDVPTaskFSApp*)AfxGetApp())->m_ptrCtl;

	CWizardPage::OnSetActive();

	//If the control has been destroyed, don't attempt
	//	to access it
	if( !::IsWindow( ptrCtrl->m_hWnd ) )
		return FALSE;

	//Set up the Context-sensitive help
	SetHelpID( IDD_CUSTOM_SCAN );

	CClientPropSheet *ptrWizParent = (CClientPropSheet*)GetParent();

	ptrWizParent->SetFinishText( IDS_SCAN );
	ptrWizParent->SetWizardButtons( PSWIZB_BACK | PSWIZB_FINISH );
	ptrWizParent->ShowMscButton( IDS_EDIT );
	
	ptrWizParent->m_strTitle = ptrCtrl->GetTaskDescriptiveName();
	ptrWizParent->m_strDescription = ptrCtrl->GetTaskDescription();

	ptrWizParent->OpenScan( FALSE );

	ptrWnd = GetDlgItem( IDC_SCHEDULE_INFO );
	if( ptrWnd )
	{
		//Get the Schedule information and fill it in
		GetScheduleInfo( strSchedule );

		ptrWnd->SetWindowText( strSchedule );
	}

	return TRUE;
}

void CScanCustomPage::OnDestroying()
{
}

void CScanCustomPage::OnDeleteTask()
{
	IConfig		*pConfig = GetScanConfig();
	IScanConfig	*pScanConfig;

	ASSERT( pConfig );

	//Reset the Autodelete flag
	if( pConfig )
	{
		pConfig->AddRef();
		if( S_OK == pConfig->QueryInterface( IID_IScanConfig, (void**)&pScanConfig ) )
		{
			pScanConfig->ChangeCloseMode( TRUE );
			pScanConfig->Release();
		}
		
		pConfig->Release();
	}

	CClientPropSheet *ptrWizParent = (CClientPropSheet*)GetParent();
	
	//Now, tell the scan to close ( release the remaining interface reference )
	ptrWizParent->CloseScan();
	m_bDeleted = TRUE;
}

void CScanCustomPage::OnMscButton()
{
	CWizard	*ptrWizParent = (CWizard*)GetParent();
	CModalConfig		*pConfig = NULL;
	long				lType = 0;
	long				selectedScanType = 0;
	CConfigObj			config( GetScanConfig() );
	CString				strSchedule;

	//If this is a Scheduled scan, add the Schedule page
	config.SetSubOption( szReg_Key_ScheduleKey );
	lType = config.GetOption( szReg_Val_HowOften, (DWORD)0 );
	config.SetSubOption( "" );

	//Get the saved selected scan type
	selectedScanType = config.GetOption(szReg_Val_SelectedScanType, SCAN_TYPE_DEFAULT);

    try
    {
    	switch( selectedScanType )
    	{
    	default:

            SAVASSERT( FALSE ); // add handling for new types
            // intentionally fall through to handle corrupt settings ... the default s/b SCAN_TYPE_CUSTOM

	    case SCAN_TYPE_CUSTOM:

		    ptrWizParent->m_dwSelectedScanType = SCAN_TYPE_CUSTOM;
    		if( lType != 0 )
    			pConfig = new CModalConfig( IDS_TASK_NAME_DEFAULT, PAGE_MANUAL_CONFIG | PAGE_SCHEDULE | PAGE_SAVE_TASK | PAGE_SCAN_SELECTED, GetScanConfig(), ((CWizard*)GetParent())->m_ptrCtrl );
    		else
    			pConfig = new CModalConfig( IDS_TASK_NAME_DEFAULT, PAGE_MANUAL_CONFIG | PAGE_SAVE_TASK | PAGE_SCAN_SELECTED, GetScanConfig(), ((CWizard*)GetParent())->m_ptrCtrl );
    		break;

    	case SCAN_TYPE_QUICK:

    		ptrWizParent->m_dwSelectedScanType = SCAN_TYPE_QUICK;
    		if( lType != 0 )
    			pConfig = new CModalConfig( IDS_TASK_NAME_DEFAULT, PAGE_MANUAL_CONFIG | PAGE_SCHEDULE | PAGE_SAVE_TASK | PAGE_QUICK_SCAN, GetScanConfig(), ((CWizard*)GetParent())->m_ptrCtrl );
    		else
    			pConfig = new CModalConfig( IDS_TASK_NAME_DEFAULT, PAGE_MANUAL_CONFIG | PAGE_SAVE_TASK | PAGE_QUICK_SCAN, GetScanConfig(), ((CWizard*)GetParent())->m_ptrCtrl );
    		break;

    	case SCAN_TYPE_FULL:

    		ptrWizParent->m_dwSelectedScanType = SCAN_TYPE_FULL;
    		if( lType != 0 )
    			pConfig = new CModalConfig( IDS_TASK_NAME_DEFAULT, PAGE_MANUAL_CONFIG | PAGE_SCHEDULE | PAGE_SAVE_TASK | PAGE_FULL_SCAN, GetScanConfig(), ((CWizard*)GetParent())->m_ptrCtrl );
    		else
    			pConfig = new CModalConfig( IDS_TASK_NAME_DEFAULT, PAGE_MANUAL_CONFIG | PAGE_SAVE_TASK | PAGE_FULL_SCAN, GetScanConfig(), ((CWizard*)GetParent())->m_ptrCtrl );
    		break;
    	}
    }
    catch (std::bad_alloc &)
    {} 

	if( pConfig )
	{
		pConfig->DoModal();
		delete pConfig;
		SetFocus();

		// We're losing our windows hook after we delete
		// the CClientPropSheet object, so reinstall our
		// hook here.
		UnHookWindow();
		HookWindow();
	}

	CWnd *ptrWnd = GetDlgItem( IDC_SCHEDULE_INFO );
	if( ptrWnd )
	{
		//Get the Schedule information and fill it in
		GetScheduleInfo( strSchedule );

		ptrWnd->SetWindowText( strSchedule );
	}
}

void CScanCustomPage::GetScheduleInfo( CString &strSchedule )
{
	CString		strDay,
				strScheduleKey;
	DWORD		dwTime;
	CWizard		*ptrParent = (CWizard*)GetParent();
	DWORD		dwHour = 0;
	DWORD		resumeableScanDuration = 0;
	CWnd		*ptrWnd = GetDlgItem( IDC_SCHEDULE );
	CConfigObj	config( GetScanConfig() );
    

	config.SetSubOption( szReg_Key_ScheduleKey );
	m_dwSchedule = config.GetOption( szReg_Val_HowOften, (DWORD)0 );
	dwTime = config.GetOption( szReg_Val_MinOfDay, (DWORD)0 );
	// szReg_Val_ScanDuration is stored in seconds, convert to hours
	resumeableScanDuration = config.GetOption(szReg_Val_ScanDuration, 0)/3600;

	//Depending on the type, I may need to read a different value
	if( m_dwSchedule == 2 )
		m_dwDay = config.GetOption( szReg_Val_DayOfWeek, 1 );
	else if ( m_dwSchedule == 3 )
		m_dwDay = config.GetOption( szReg_Val_DayOfMonth, 1 );

	config.SetSubOption( "" );

	if( 0 != m_dwSchedule )
	{
        LPTSTR		pszTime = NULL;
        SYSTEMTIME tScheduleTime;
        WORD wHours, wMinutes;
        
        // Take the schedule time and format it for the locale settings on the machine
        // it's running on
        wHours = (WORD)(dwTime/60);
        wMinutes = (WORD)(dwTime%60);
        memset( &tScheduleTime, 0x00, sizeof( tScheduleTime ) );
        
        if( (wHours >= 0 && wHours < 24) &&  (wMinutes >= 0  && wMinutes < 60))
        {
            tScheduleTime.wHour = wHours;
            tScheduleTime.wMinute = wMinutes;
        }
        pszTime = m_strTime.GetBuffer( MAX_PATH );
        GetTimeFormat(LOCALE_SYSTEM_DEFAULT, TIME_NOSECONDS, &tScheduleTime, NULL, pszTime, MAX_PATH);
        m_strTime.ReleaseBuffer();
        pszTime = NULL;

        
		//Format the string
		switch( m_dwSchedule )
		{
			case 1:
				if (resumeableScanDuration == 0)
					AfxFormatString1( strSchedule, IDS_SCAN_PROMPT_DAILY, m_strTime );
				else
					strSchedule.FormatMessage(IDS_SCAN_PROMPT_DAILY_RESUME, (LPCTSTR) m_strTime, resumeableScanDuration);
				break;
			case 2:
				if (resumeableScanDuration == 0)
					AfxFormatString2( strSchedule, IDS_SCAN_PROMPT_WEEKLY, GetDayName(m_dwDay), m_strTime );
				else
					strSchedule.FormatMessage(IDS_SCAN_PROMPT_WEEKLY_RESUME, (LPCTSTR) GetDayName(m_dwDay), (LPCTSTR) m_strTime, resumeableScanDuration);
				break;
			case 3:
				strDay.Format( "%d", m_dwDay );
				if (resumeableScanDuration == 0)
					AfxFormatString2( strSchedule, IDS_SCAN_PROMPT_MONTHLY, strDay, m_strTime );
				else
					strSchedule.FormatMessage(IDS_SCAN_PROMPT_MONTHLY_RESUME, (LPCTSTR) strDay, (LPCTSTR) m_strTime, resumeableScanDuration);
				break;
			default:
				//there is no schedule info here
				strSchedule.LoadString( IDS_NO_SCHEDULE );
				//Disable the Schedule button if there is no
				//	schedule information
				if( ptrWnd )
				{
					ptrWnd->EnableWindow( FALSE );
				}
		}
	}	
	else
	{
		//there is no schedule info here
		strSchedule.LoadString( IDS_NO_SCHEDULE );
		//Disable the Schedule button if there is no
		//	schedule information
		if( ptrWnd )
		{
			ptrWnd->EnableWindow( FALSE );
		}
	}
}

BOOL CScanCustomPage::OnWizardFinish() 
{
	CClientPropSheet *ptrParent = (CClientPropSheet*)GetParent();

	//Go to the next page
	// and start the scan
	((CWizard*)ptrParent)->Scan();

	return FALSE;
}

void CScanCustomPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CClientPropSheet *ptrWizParent = (CClientPropSheet*)GetParent();
	
	ptrWizParent->SetWizardButtons( PSWIZB_FINISH );

	if( InWizardMode() )
		PaintTitle( &dc );
	
}

void CScanCustomPage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWizardPage::OnLButtonDown(nFlags, point);

	SetFocus();
}

void CScanCustomPage::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CWizardPage::OnRButtonDown(nFlags, point);

	SetFocus();
}
