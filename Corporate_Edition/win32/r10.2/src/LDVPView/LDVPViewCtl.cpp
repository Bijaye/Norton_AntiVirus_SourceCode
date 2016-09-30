// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: LDVPViewCtl.cpp 
//  Purpose: LDVPView OCX control Implementation file
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include "LDVPView.h"
#include "LDVPViewCtl.h"
#include "LDVPViewPpg.h"
#include "Wizard.h"
#include "ModalConfig.h"
#include <atlbase.h>
#include "licensehelper.h"
#include "wow64helpers.h"
#include <new>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CLDVPViewCtrl, COleControl)

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CLDVPViewCtrl, COleControl)
	//{{AFX_MSG_MAP(CLDVPViewCtrl)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_MESSAGE(WM_LICENSE_MODE_CHANGE, OnLicenseModeChange)
	//}}AFX_MSG_MAP
	ON_COMMAND( ID_HELP, OnHelp ) 
	ON_COMMAND( UWM_SCAN_STARTING, ScanStarting )
	ON_COMMAND( UWM_SCAN_ENDING, ScanEnding )
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CLDVPViewCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CLDVPViewCtrl)
	DISP_PROPERTY_NOTIFY(CLDVPViewCtrl, "RecordingSchedule", m_recordingSchedule, OnRecordingScheduleChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CLDVPViewCtrl, "SkipFirst", m_bSkipFirst, OnSkipFirstChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CLDVPViewCtrl, "HelpFilename", m_helpFilename, OnHelpFilenameChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY(CLDVPViewCtrl, "Recording", m_recording, OnRecordingChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CLDVPViewCtrl, "TaskName", m_taskName, OnTaskNameChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY(CLDVPViewCtrl, "TaskDescription", m_taskDescription, OnTaskDescriptionChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY(CLDVPViewCtrl, "TaskDescriptiveName", m_taskDescriptiveName, OnTaskDescriptiveNameChanged, VT_BSTR)
	DISP_FUNCTION(CLDVPViewCtrl, "Deinitialize", Deinitialize, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CLDVPViewCtrl, "EndTask", EndTask, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CLDVPViewCtrl, "StartTask", StartTask, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CLDVPViewCtrl, "Initialize", Initialize, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CLDVPViewCtrl, "DoModalSchedule", DoModalSchedule, VT_BOOL, VTS_BSTR VTS_I2)
	DISP_FUNCTION(CLDVPViewCtrl, "DoModalConfig", DoModalConfig, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CLDVPViewCtrl, "DeleteTask", DeleteTask, VT_BOOL, VTS_NONE)
	DISP_STOCKPROP_CAPTION()
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CLDVPViewCtrl, COleControl)
	//{{AFX_EVENT_MAP(CLDVPViewCtrl)
	EVENT_CUSTOM("CreateGroup", FireCreateGroup, VTS_BSTR  VTS_BOOL  VTS_I4)
	EVENT_CUSTOM("CreateTask", FireCreateTask, VTS_BSTR  VTS_BSTR  VTS_BSTR  VTS_I4  VTS_I4)
	EVENT_CUSTOM("TaskEnding", FireTaskEnding, VTS_NONE)
	EVENT_CUSTOM("TaskStarting", FireTaskStarting, VTS_NONE)
	EVENT_CUSTOM("ScanStarting", FireScanStarting, VTS_NONE)
	EVENT_CUSTOM("ScanEnding", FireScanEnding, VTS_NONE)
	EVENT_CUSTOM("SaveScan", FireSaveScan, VTS_BSTR  VTS_BSTR  VTS_BSTR)
	EVENT_CUSTOM("BeginRecording", FireBeginRecording, VTS_I4)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CLDVPViewCtrl, 1)
	PROPPAGEID(CLDVPViewPropPage::guid)
END_PROPPAGEIDS(CLDVPViewCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CLDVPViewCtrl, "LDVPView.LDVPViewCtrl.1",
	0x8e9145bd, 0x703d, 0x11d1, 0x81, 0xc9, 0x00, 0xa0, 0xc9, 0x5c, 0x07, 0x56);



/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CLDVPViewCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DLDVPView =
		{ 0x8e9145bf, 0x703d, 0x11d1, { 0x81, 0xc9, 0x00, 0xa0, 0xc9, 0x5c, 0x07, 0x56 } } ;

const IID BASED_CODE IID_DLDVPViewEvents =
		{ 0x8e9145c0, 0x703d, 0x11d1, { 0x81, 0xc9, 0x00, 0xa0, 0xc9, 0x5c, 0x07, 0x56 } } ;



/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwLDVPViewOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CLDVPViewCtrl, IDS_LDVPVIEW, _dwLDVPViewOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CLDVPViewCtrl::CLDVPViewCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CLDVPViewCtrl

BOOL CLDVPViewCtrl::CLDVPViewCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_LDVPVIEW,
			IDB_LDVPVIEW,
			afxRegApartmentThreading,
			_dwLDVPViewOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CLDVPViewCtrl::CLDVPViewCtrl()
{
	InitializeIIDs(&IID_DLDVPView, &IID_DLDVPViewEvents);

	m_dwActiveTask = TASK_INVALID;
	m_ptrWizard = NULL;
	m_recordingSchedule = FALSE;

	((CLDVPViewApp*)AfxGetApp())->m_ptrCtl = this;

	m_bModalConfigDisplayed = FALSE;
}


//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CLDVPViewCtrl::~CLDVPViewCtrl()
{
}


//----------------------------------------------------------------
// OnDraw
//----------------------------------------------------------------
void CLDVPViewCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if( m_ptrWizard )
	{
		m_ptrWizard->InvalidateRect( rcInvalid );
	}
}


//----------------------------------------------------------------
// DoPropExchange
//----------------------------------------------------------------
void CLDVPViewCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
}


//----------------------------------------------------------------
// OnResetState
//----------------------------------------------------------------
void CLDVPViewCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
}


//----------------------------------------------------------------
// Initialize
//----------------------------------------------------------------
BOOL CLDVPViewCtrl::Initialize() 
{
	CString strGroup;
	CString strTask;
	CString strDescription;
	
	/*---------------------------------------*/
	// View group
	/*---------------------------------------*/
	strGroup.LoadString(IDS_GROUP_HEADER);
	FireCreateGroup( (LPCTSTR)strGroup, FALSE, 4 );
	
	strTask.LoadString(IDS_TASK_RTS);
	strDescription.LoadString(IDS_TASK_RTS_DESCRIPTION);
	FireCreateTask( (LPCTSTR)strGroup, (LPCTSTR)strTask, (LPCTSTR)strDescription, TASK_ID_RTS, 21 );

	strTask.LoadString(IDS_TASK_SCHEDULE);
	strDescription.LoadString(IDS_TASK_SCHEDULE_DESCRIPTION);
	FireCreateTask( (LPCTSTR)strGroup, (LPCTSTR)strTask, (LPCTSTR)strDescription, TASK_ID_SCHEDULE, 20 );

	strTask.LoadString(IDS_TASK_VIRUS_BIN);
	strDescription.LoadString(IDS_TASK_VIRUS_BIN_DESCRIPTION);
	FireCreateTask( (LPCTSTR)strGroup, (LPCTSTR)strTask, (LPCTSTR)strDescription, TASK_ID_VIRUS_BIN, 29 );

	strTask.LoadString(IDS_TASK_BACKUP);
	strDescription.LoadString(IDS_TASK_BACKUP_DESCRIPTION);
	FireCreateTask( (LPCTSTR)strGroup, (LPCTSTR)strTask, (LPCTSTR)strDescription, TASK_ID_VIEW_BACKUP, 30 );

	strTask.LoadString(IDS_TASK_REPAIRED);
	strDescription.LoadString(IDS_TASK_REPAIR_DESCRIPTION);
	FireCreateTask( (LPCTSTR)strGroup, (LPCTSTR)strTask, (LPCTSTR)strDescription, TASK_ID_VIEW_REPAIR, 31 );

    // If this is a license enforcing installation of SAVF or SCS, the we
    // also need to show the licensing UI panel.

    if( License_IsLicenseEnforcingInstall() )
    {
		strTask.LoadString(IDS_TASK_LICENSING);
		strDescription.LoadString(IDS_TASK_LICENSING_DESCRIPTION);
		FireCreateTask( (LPCTSTR)strGroup, (LPCTSTR)strTask, (LPCTSTR)strDescription, TASK_ID_LICENSING, 12 );
	}

	/*---------------------------------------*/
	// Histories group
	/*---------------------------------------*/
	strGroup.LoadString(IDS_GROUP_HISTORIES);
	FireCreateGroup( (LPCTSTR)strGroup, FALSE, 23 );

	strTask.LoadString(IDS_TASK_VIRUS_HISTORY);
	strDescription.LoadString(IDS_TASK_VIRUS_HISTORY_DESCRIPTION);
	FireCreateTask( (LPCTSTR)strGroup, (LPCTSTR)strTask, (LPCTSTR)strDescription, TASK_ID_VIRUS_HISTORY, 24 );

	strTask.LoadString(IDS_TASK_SCAN_HISTORY);
	strDescription.LoadString(IDS_TASK_SCAN_HISTORY_DESCRIPTION);
	FireCreateTask( (LPCTSTR)strGroup, (LPCTSTR)strTask, (LPCTSTR)strDescription, TASK_ID_SCAN_HISTORY, 25 );

	strTask.LoadString(IDS_TASK_EVENT_LOG);
	strDescription.LoadString(IDS_TASK_EVENT_LOG_DESCRIPTION);
	FireCreateTask( (LPCTSTR)strGroup, (LPCTSTR)strTask, (LPCTSTR)strDescription, TASK_ID_EVENT_LOG, 26 );

	// Final argument to FireCreateTask is the ID of the icon in VPC32's image list
	if (!IsWin64())
	{
		strTask.LoadString(IDS_TASK_TAMPERBEHAVIOR_HISTORY);
		strDescription.LoadString(IDS_TASK_TAMPERBEHAVIOR_HISTORY_DESCRIPTION);
		FireCreateTask( (LPCTSTR)strGroup, (LPCTSTR)strTask, (LPCTSTR)strDescription, TASK_ID_TAMPERBEHAVIOR_HISTORY, 32 );
	}

	return TRUE;
}

//----------------------------------------------------------------
// DeInitialize
//----------------------------------------------------------------
BOOL CLDVPViewCtrl::Deinitialize() 
{
	return TRUE;
}

//----------------------------------------------------------------
// EndTask
//----------------------------------------------------------------
BOOL CLDVPViewCtrl::EndTask() 
{

	switch( m_dwActiveTask )
	{
	//If it is a valid task, delete it
	case TASK_ID_RTS:
	case TASK_ID_SCHEDULE:
	case TASK_ID_VIRUS_HISTORY:
	case TASK_ID_EVENT_LOG:
	case TASK_ID_VIRUS_BIN:
	case TASK_ID_SCAN_HISTORY:
    case TASK_ID_VIEW_BACKUP:
    case TASK_ID_VIEW_REPAIR:
	case TASK_ID_LICENSING:
	case TASK_ID_TAMPERBEHAVIOR_HISTORY:

		if( m_ptrWizard )
		{
			//IF a scan is in progress, end it gracefully.
			m_ptrWizard->SendMessage( WM_COMMAND, MAKEWPARAM( UWM_END_SCAN, 0 ), 0L );
			m_ptrWizard->DestroyWindow();
			//Free the memory for the Wizard
			delete m_ptrWizard;
			m_ptrWizard = NULL;
		}
		break;

	default:
		m_dwActiveTask = TASK_INVALID;
		break;
	}

	return TRUE;
}


//----------------------------------------------------------------
// StartTask
//----------------------------------------------------------------
BOOL CLDVPViewCtrl::StartTask(long dwID) 
{
	BOOL bReturn = FALSE;

	//If there is a task already running, end it
	if( m_dwActiveTask != TASK_INVALID )
	{
		EndTask();
	}

	//Figure out which task we are supposed to run
	switch( dwID )
	{
	case TASK_ID_RTS:
	case TASK_ID_SCHEDULE:
	case TASK_ID_VIRUS_HISTORY:
	case TASK_ID_EVENT_LOG:
	case TASK_ID_VIRUS_BIN:
	case TASK_ID_SCAN_HISTORY:
    case TASK_ID_VIEW_BACKUP:
    case TASK_ID_VIEW_REPAIR:
	case TASK_ID_LICENSING:
	case TASK_ID_TAMPERBEHAVIOR_HISTORY:

		m_dwActiveTask = dwID;
		m_bCanSkipFirst = FALSE;

		break;

	default:

		m_dwActiveTask = TASK_INVALID;
		FireTaskEnding();

		break;
	}

	if( m_dwActiveTask != TASK_INVALID )
	{
		try
		{
			//Create the wizard
			m_ptrWizard = new CWizard( dwID, this );

			//Now set the Wizard screen and create it modelessly
			if( m_ptrWizard )
			{
				m_ptrWizard->Create( this, WS_CHILD);

				//Position the window
				m_ptrWizard->SetWindowPos( &wndTop, 0, 0, 0, 0,
											SWP_NOZORDER | SWP_NOSIZE );
				// And show it
				m_ptrWizard->ShowWindow( SW_NORMAL );

				FireTaskStarting();

				bReturn = TRUE;
			}
			else
			{
				m_dwActiveTask = TASK_INVALID;
			}
		}
		catch (std::bad_alloc &) {m_dwActiveTask = TASK_INVALID;}
	}

	return bReturn;
}

void CLDVPViewCtrl::ScanStarting()
{
	FireScanStarting();
}

void CLDVPViewCtrl::ScanEnding()
{
	FireScanEnding();
}

//----------------------------------------------------------------
// NotifyNameDescription
//----------------------------------------------------------------
void CLDVPViewCtrl::NotifyNameDescription( const CString &strName, const CString &strDescription )
{
	FireSaveScan( strName, strDescription, m_taskName );
}

//----------------------------------------------------------------
// NotifyEndTask
//	This method notifies the owner of the OCX that the task has 
//	ended
//----------------------------------------------------------------
void CLDVPViewCtrl::NotifyEndTask()
{
	FireTaskEnding();
}

//----------------------------------------------------------------
// OnSize
//----------------------------------------------------------------
void CLDVPViewCtrl::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx-100, cy-100);

	TRACE0("Size message in the OCX\n" );

	if( m_ptrWizard )
		m_ptrWizard->SendMessage( WM_SIZE, (WPARAM)nType, (LPARAM)MAKELPARAM( cx, cy ));
}

//----------------------------------------------------------------
// OnRecordingScheduleChanged
//----------------------------------------------------------------
void CLDVPViewCtrl::OnRecordingScheduleChanged() 
{
	SetModifiedFlag();
}

//----------------------------------------------------------------
// DoModalSchedule
//----------------------------------------------------------------
BOOL CLDVPViewCtrl::DoModalSchedule(LPCTSTR lpstrTaskname, short iKey) 
{
	m_bUserTask = (iKey != 0 );
	m_taskName = lpstrTaskname;

//	CModalSchedule	sheet(IDS_SCHEDULE_OPTIONS, this);
	AfxMessageBox( "Implement a Modal Schedule page if needed!" );

	//Show the property sheet as a Modal dialog box
//	return ( IDOK == sheet.DoModal() );
	return TRUE;
}


//----------------------------------------------------------------
// DoModalConfig
//----------------------------------------------------------------
BOOL CLDVPViewCtrl::DoModalConfig(long dwPagemask) 
{
	BOOL	bRet;
	UINT	ID = IDS_CONFIGURATION;

	//If a 0 is passed in for the Pagemask,
	//	that means that we simply want to ensure
	//	the scan options are in the registry.
	// To do this, I will simply read and save each
	//	object. This will make no changes if the
	//	items exist in the registry, and will
	//	create default values if they don't.
	if( dwPagemask == 0 )
	{
	}	
	else if( m_bModalConfigDisplayed )
	{
		CString strTitle;

		strTitle.LoadString( ID );
		//The property sheet is already displayed.
		//Find it and activate it
		CWnd *ptrWnd = FindWindow( NULL, strTitle );

		if( ptrWnd )
		{
			ptrWnd->SetActiveWindow();
		}

	}
	else
	{
		//Construct the property sheet
		CModalConfig	sheet( ID, dwPagemask, this );
		
		m_bModalConfigDisplayed = TRUE;
		
		//Show the property sheet as a Modal dialog box
		bRet = ( IDOK == sheet.DoModal() );
		
		m_bModalConfigDisplayed = FALSE;
	}
	
	return bRet;
}

void CLDVPViewCtrl::OnSkipFirstChanged() 
{
	if( m_bCanSkipFirst && m_ptrWizard && m_bSkipFirst )
	{
		//Make sure the First page is active
		m_ptrWizard->SetActivePage( 0 );
		m_ptrWizard->PressButton( PSBTN_FINISH );
	}
	else
	{
		m_bSkipFirst = FALSE;
	}

	SetModifiedFlag();
}

void CLDVPViewCtrl::OnHelpFilenameChanged() 
{
	AfxGetApp()->m_pszHelpFilePath = m_helpFilename;

	SetModifiedFlag();
}

void CLDVPViewCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	COleControl::OnSetFocus(pOldWnd);
	
	if( m_ptrWizard )
	{
		m_ptrWizard->SetFocus();
	}
}


void CLDVPViewCtrl::OnRecordingChanged() 
{
	SetModifiedFlag();
}

void CLDVPViewCtrl::OnTaskNameChanged() 
{
	SetModifiedFlag();
}

void CLDVPViewCtrl::OnTaskDescriptionChanged() 
{
	SetModifiedFlag();
}

void CLDVPViewCtrl::OnTaskDescriptiveNameChanged() 
{
	SetModifiedFlag();
}

BOOL CLDVPViewCtrl::DeleteTask() 
{
	if( m_ptrWizard )
		return (BOOL)m_ptrWizard->SendMessage( WM_COMMAND, MAKEWPARAM( UWM_DESTROYSCAN, 0 ), 0L );
	else
		return FALSE;
}

LRESULT CLDVPViewCtrl::OnLicenseModeChange(WPARAM wParam, LPARAM lParam)
{
	StartTask(TASK_ID_LICENSING);
	CRect rect;
	GetClientRect(&rect);
	SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM( rect.right, rect.bottom ));
	return (LRESULT) 0;
}

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
