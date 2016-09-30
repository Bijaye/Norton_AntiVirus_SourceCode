// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: LDVPTaskFSCtl.cpp 
//  Purpose: LDVPTaskFS OCX control Implementation file
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------
#include "stdafx.h"
#include "LDVPTaskFS.h"
#include "LDVPTaskFSCtl.h"
#include "LDVPTaskFSPpg.h"
#include "Wizard.h"
#include "ModalConfig.h"
#include "password.h"
#include <new>
#include "ci.h"
#include "SymSaferRegistry.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// Local defines
#define POLICY_KEY_EXPLORER "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"
#define POLICY_VALUE_HIDE_SPECIFIED_DRIVE   "NoDrives"
#define MAX_DRIVES 26


IMPLEMENT_DYNCREATE(CLDVPTaskFSCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CLDVPTaskFSCtrl, COleControl)
	//{{AFX_MSG_MAP(CLDVPTaskFSCtrl)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_COMMAND( ID_HELP, OnHelp ) 
	ON_COMMAND( UWM_SCAN_STARTING, ScanStarting )
	ON_COMMAND( UWM_SCAN_ENDING, ScanEnding )
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CLDVPTaskFSCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CLDVPTaskFSCtrl)
	DISP_PROPERTY_NOTIFY(CLDVPTaskFSCtrl, "RecordingSchedule", m_recordingSchedule, OnRecordingScheduleChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CLDVPTaskFSCtrl, "SkipFirst", m_bSkipFirst, OnSkipFirstChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CLDVPTaskFSCtrl, "HelpFilename", m_helpFilename, OnHelpFilenameChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY(CLDVPTaskFSCtrl, "Recording", m_recording, OnRecordingChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CLDVPTaskFSCtrl, "TaskName", m_taskName, OnTaskNameChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY(CLDVPTaskFSCtrl, "TaskDescription", m_taskDescription, OnTaskDescriptionChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY(CLDVPTaskFSCtrl, "TaskDescriptiveName", m_taskDescriptiveName, OnTaskDescriptiveNameChanged, VT_BSTR)
	DISP_FUNCTION(CLDVPTaskFSCtrl, "Deinitialize", Deinitialize, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CLDVPTaskFSCtrl, "EndTask", EndTask, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CLDVPTaskFSCtrl, "StartTask", StartTask, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CLDVPTaskFSCtrl, "Initialize", Initialize, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CLDVPTaskFSCtrl, "DoModalSchedule", DoModalSchedule, VT_BOOL, VTS_BSTR VTS_I2)
	DISP_FUNCTION(CLDVPTaskFSCtrl, "DoModalConfig", DoModalConfig, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CLDVPTaskFSCtrl, "DeleteTask", DeleteTask, VT_BOOL, VTS_NONE)
	DISP_STOCKPROP_CAPTION()
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CLDVPTaskFSCtrl, COleControl)
	//{{AFX_EVENT_MAP(CLDVPTaskFSCtrl)
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
BEGIN_PROPPAGEIDS(CLDVPTaskFSCtrl, 1)
	PROPPAGEID(CLDVPTaskFSPropPage::guid)
END_PROPPAGEIDS(CLDVPTaskFSCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CLDVPTaskFSCtrl, "LDVPTASKFS.LDVPTaskFSCtrl.1",
//	0xba0c6364, 0x7218, 0x11d0, 0x88, 0x65, 0x44, 0x45, 0x53, 0x54, 0, 0)
	0x64b4a5ae, 0x799, 0x11d1, 0x81, 0x2a, 0x0, 0xa0, 0xc9, 0x5c, 0x7, 0x56);



/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CLDVPTaskFSCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DLDVPTaskFS =
//		{ 0xba0c6362, 0x7218, 0x11d0, { 0x88, 0x65, 0x44, 0x45, 0x53, 0x54, 0, 0 } };
		{ 0x64b4a5ac, 0x799, 0x11d1, { 0x81, 0x2a, 0x0, 0xa0, 0xc9, 0x5c, 0x7, 0x56 } };

const IID BASED_CODE IID_DLDVPTaskFSEvents =
//		{ 0xba0c6363, 0x7218, 0x11d0, { 0x88, 0x65, 0x44, 0x45, 0x53, 0x54, 0, 0 } };
		{ 0x64b4a5ad, 0x799, 0x11d1, { 0x81, 0x2a, 0x0, 0xa0, 0xc9, 0x5c, 0x7, 0x56 } };



/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwLDVPTaskFSOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CLDVPTaskFSCtrl, IDS_LDVPTASKFS, _dwLDVPTaskFSOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CLDVPTaskFSCtrl::CLDVPTaskFSCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CLDVPTaskFSCtrl

BOOL CLDVPTaskFSCtrl::CLDVPTaskFSCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_LDVPTASKFS,
			IDB_LDVPTASKFS,
			afxRegApartmentThreading,
			_dwLDVPTaskFSOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CLDVPTaskFSCtrl::CLDVPTaskFSCtrl()
{
	InitializeIIDs(&IID_DLDVPTaskFS, &IID_DLDVPTaskFSEvents);

	m_dwActiveTask = TASK_INVALID;
	m_ptrWizard = NULL;
	m_recordingSchedule = FALSE;

	((CLDVPTaskFSApp*)AfxGetApp())->m_ptrCtl = this;

	m_bModalConfigDisplayed = FALSE;
}


//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CLDVPTaskFSCtrl::~CLDVPTaskFSCtrl()
{
}


//----------------------------------------------------------------
// OnDraw
//----------------------------------------------------------------
void CLDVPTaskFSCtrl::OnDraw(
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
void CLDVPTaskFSCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
}


//----------------------------------------------------------------
// OnResetState
//----------------------------------------------------------------
void CLDVPTaskFSCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
}


//----------------------------------------------------------------
// Initialize
//----------------------------------------------------------------
BOOL CLDVPTaskFSCtrl::Initialize() 
{
	CString strGroup;
	CString strTask;
	CString strDescription;
	BOOL	bFloppyInstalled = FALSE;
    BOOL    bDrivesToScan = FALSE;

	//Determine if a floppy is installed so I know whether or not to
	//	add the Scan a Floppy task
    DWORD   dwMask = 1;
	DWORD	dwLogicalDrives = GetLogicalDrives();
    DWORD   dwHiddenDrives = GetHiddenDrives();
    DWORD   dwDrives = 0;
	CString strFirstFloppy;
	CString strDrive;
    int     iDrive = 0;
    DWORD   dwDriveType;

    dwDrives = dwLogicalDrives &~ dwHiddenDrives;

    while ( iDrive < MAX_DRIVES )
    {
		if( dwMask & dwDrives )
		{
			strDrive.Format( "%c:\\", iDrive + 'A' );

            dwDriveType = GetDriveType( (LPCTSTR)strDrive );

			if( dwDriveType == DRIVE_REMOVABLE )
			{
				bFloppyInstalled = TRUE;
			}
			else if( dwDriveType == DRIVE_FIXED )
			{
				bDrivesToScan = TRUE;
			}
			else if( dwDriveType == DRIVE_CDROM )
			{
				bDrivesToScan = TRUE;
			}
			else if( dwDriveType == DRIVE_REMOTE )
			{
				bDrivesToScan = TRUE;
			}
        }

        iDrive++;
        dwMask *= 2;
    }

	/*---------------------------------------*/
	// Scanning tasks
	/*---------------------------------------*/
	if( bFloppyInstalled || bDrivesToScan )
    {
	    strGroup.LoadString(IDS_GROUP_HEADER);
	    FireCreateGroup( (LPCTSTR)strGroup, TRUE, 6 );
	    
	    if( bFloppyInstalled )
	    {
		    //Now load each task heading and add it to the TaskPad
		    strTask.LoadString(IDS_SCAN_FLOPPY);
		    strDescription.LoadString(IDS_SCAN_FLOPPY_DESCRIPTION);
		    FireCreateTask( (LPCTSTR)strGroup, (LPCTSTR)strTask, (LPCTSTR)strDescription, TASK_ID_SCAN_FLOPPY, 9 );
	    }
    
        if ( bDrivesToScan )
        {
	        strTask.LoadString(IDS_SCAN_SELECTED);
	        strDescription.LoadString(IDS_SCAN_DRIVE_DESCRIPTION );
	        FireCreateTask( (LPCTSTR)strGroup, (LPCTSTR)strTask, (LPCTSTR)strDescription, TASK_ID_SCAN_SELECTED, 8 );

			//quickscan
			strTask.LoadString(IDS_QUICKSCAN);
	        strDescription.LoadString(IDS_QUICKSCAN_DESCRIPTION);
			FireCreateTask( (LPCTSTR)strGroup, (LPCTSTR)strTask, (LPCTSTR)strDescription, TASK_ID_QUICK_SCAN, 33 );

			//fullscan
			strTask.LoadString(IDS_FULL_SCAN);
	        strDescription.LoadString(IDS_FULL_SCAN_DESCRIPTION);
			FireCreateTask( (LPCTSTR)strGroup, (LPCTSTR)strTask, (LPCTSTR)strDescription, TASK_ID_FULL_SCAN, 34 );
		}

	}

	// LDCM specific scan
	HKEY hDMIkey = NULL;
	HKEY hLDCMkey = NULL;
	RegOpenKey( HKEY_LOCAL_MACHINE, REG_DMI_BASE, &hDMIkey );
	RegOpenKey( HKEY_LOCAL_MACHINE, LDCM_REG_PATH, &hLDCMkey );
	if ( hDMIkey && hLDCMkey )
	{
		::LoadString( AfxGetResourceHandle(), IDS_SCAN_PCHEALTH, strTask.GetBuffer( 1024 ), 1024 );
		strTask.ReleaseBuffer();
		::LoadString( AfxGetResourceHandle(), IDS_SCAN_PCHEALTH_DESCRIPTION, strDescription.GetBuffer( 1024 ), 1024 );
		strDescription.ReleaseBuffer();
		FireCreateTask( (LPCTSTR)strGroup, (LPCTSTR)strTask, (LPCTSTR)strDescription, TASK_ID_SCAN_PCHEALTH, 18 );
	}
	if ( hDMIkey )
		RegCloseKey( hDMIkey );
	if ( hLDCMkey )
		RegCloseKey( hLDCMkey );

	//And the special "Record task" task
	FireCreateTask( (LPCTSTR)"", (LPCTSTR)"", (LPCTSTR)"", TASK_ID_SCAN_RECORD, 0 );

	/*---------------------------------------*/
	// Configuration tasks
	/*---------------------------------------*/
	if( GetClientType() != CLIENT_TYPE_LIGHT )
	{
		strGroup.LoadString(IDS_GROUP_CONFIGURATION);
		FireCreateGroup( (LPCTSTR)strGroup, FALSE, 3 );
	}

	return TRUE;
}

//----------------------------------------------------------------
// DeInitialize
//----------------------------------------------------------------
BOOL CLDVPTaskFSCtrl::Deinitialize() 
{
	return TRUE;
}

//----------------------------------------------------------------
// EndTask
//----------------------------------------------------------------
BOOL CLDVPTaskFSCtrl::EndTask() 
{

	switch( m_dwActiveTask )
	{
	//If it is a valid task, delete it
	case TASK_ID_SCAN_FLOPPY:
	case TASK_ID_SCAN_SELECTED:
	case TASK_ID_QUICK_SCAN:
	case TASK_ID_FULL_SCAN:
	case TASK_ID_SCAN_CUSTOM:
	case TASK_ID_SCAN_RECORD:
	case TASK_ID_SCAN_PCHEALTH:
	if( m_ptrWizard )
		{
			m_ptrWizard->DestroyWindow();
			//Free the memory for the Wizard
			delete m_ptrWizard;
			m_ptrWizard = NULL;
		}
		break;

	default:
		m_dwActiveTask = TASK_INVALID;
	}

	return TRUE;
}


//----------------------------------------------------------------
// StartTask
//----------------------------------------------------------------
BOOL CLDVPTaskFSCtrl::StartTask(long dwID) 
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
	case TASK_ID_SCAN_RECORD:
	case TASK_ID_SCAN_SELECTED:
	case TASK_ID_QUICK_SCAN:
	case TASK_ID_FULL_SCAN:
		m_bCanSkipFirst = FALSE;
		break;
	case TASK_ID_SCAN_FLOPPY:
	case TASK_ID_SCAN_CUSTOM:
	case TASK_ID_SCAN_PCHEALTH:
		m_bCanSkipFirst = TRUE;
		break;
	default:
		m_dwActiveTask = TASK_INVALID;
		FireTaskEnding();
		return FALSE;
	}

	//I used to use this variable, but I no longer do.
	//	Everything in this task OCX is now a user task.
	m_bUserTask = TRUE;

	try
	{
		//Create the wizard
		m_ptrWizard = new CWizard( dwID, this );
		m_dwActiveTask = dwID;

		//Now set the Wizard screen and create it modelessly
		if( m_ptrWizard )
		{
			m_ptrWizard->Create( this, WS_CHILD);

			//Position the window
			m_ptrWizard->SetWindowPos( &wndTop, 0, 0, 0, 0,
										SWP_NOZORDER | SWP_NOSIZE );
			// And show it
			m_ptrWizard->ShowWindow( SW_NORMAL );

			//Init the Wizard
	//		m_ptrWizard->InitWizard();
			
			bReturn = TRUE;
		}
	}
	catch (std::bad_alloc &) {}
	FireTaskStarting();

	return bReturn;
}

void CLDVPTaskFSCtrl::ScanStarting()
{
	FireScanStarting();
}

void CLDVPTaskFSCtrl::ScanEnding()
{
	FireScanEnding();
}

//----------------------------------------------------------------
// NotifyNameDescription
//----------------------------------------------------------------
void CLDVPTaskFSCtrl::NotifyNameDescription( const CString &strName, const CString &strDescription )
{
	FireSaveScan( strName, strDescription, m_taskName );
}

//----------------------------------------------------------------
// NotifyEndTask
//	This method notifies the owner of the OCX that the task has 
//	ended
//----------------------------------------------------------------
void CLDVPTaskFSCtrl::NotifyEndTask()
{
	FireTaskEnding();
}

//----------------------------------------------------------------
// OnSize
//----------------------------------------------------------------
void CLDVPTaskFSCtrl::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx-100, cy-100);

	TRACE0("Size message in the OCX\n" );

	if( m_ptrWizard )
		m_ptrWizard->SendMessage( WM_SIZE, (WPARAM)nType, (LPARAM)MAKELPARAM( cx, cy ));
}

//----------------------------------------------------------------
// OnRecordingScheduleChanged
//----------------------------------------------------------------
void CLDVPTaskFSCtrl::OnRecordingScheduleChanged() 
{
	SetModifiedFlag();
}

//----------------------------------------------------------------
// DoModalSchedule
//----------------------------------------------------------------
BOOL CLDVPTaskFSCtrl::DoModalSchedule(LPCTSTR lpstrTaskname, short iKey) 
{
	IScanConfig		*ptrScan = NULL;
	IVirusProtect	*ptrVirusProtect = NULL;
	BOOL			bRet = FALSE;

	m_bUserTask = (iKey != 0 );
	m_taskName = lpstrTaskname;

	//Open the scan to configure it's schedule options
	if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IScanConfig, (void**)&ptrScan ) ) )
	{
		//First, open the scan
		ptrScan->Open( NULL, HKEY_VP_USER_SCANS, (LPSTR)lpstrTaskname );

		//Now, open the configuration dialog
		CModalConfig	sheet(IDS_SCHEDULE_OPTIONS, PAGE_SCHEDULE, ptrScan, this);

		bRet = ( IDOK == sheet.DoModal() );

		//Release my object
		ptrScan->Release();		//Show the property sheet as a Modal dialog box
	}

	return bRet;
}


//----------------------------------------------------------------
// DoModalConfig
//----------------------------------------------------------------
BOOL CLDVPTaskFSCtrl::DoModalConfig(long dwPagemask) 
{
	BOOL	bRet = FALSE;
/*	UINT	ID = IDS_CONFIGURATION;

	//If a 0 is passed in for the Pagemask,
	//	that means that we simply want to ensure
	//	the scan options are in the registry.
	// To do this, I will simply read and save each
	//	object. This will make no changes if the
	//	items exist in the registry, and will
	//	create default values if they don't.
	if( dwPagemask == 0 )
	{
		//Create the Engine OCX
		CLDVPScan	scan;
		RECT		rect;

		memset( &rect, 0, sizeof( RECT ) );

		if( scan.Create( "", WS_CHILD, rect, this, ID_ENGINE_OCX ) )
		{
			scan.SetProduct( szReg_Product );

			scan.OpenScan( szReg_Key_Scan_Defaults, TRUE, TYPE_STANDARD );
			scan.CloseScan( FALSE );
	
			scan.OpenScan( szReg_Key_RTS, FALSE, TYPE_REALTIME );
			scan.CloseScan( FALSE );

			scan.OpenScan( szReg_Key_Idle, TRUE, TYPE_IDLE );
			scan.CloseScan( FALSE );

		}
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
	
*/	return bRet;
}

void CLDVPTaskFSCtrl::OnSkipFirstChanged() 
{
	//See if the Finish button is enabled
	if( m_ptrWizard )
	{
	
		CWnd *ptrWnd = m_ptrWizard->GetDlgItem( 0x3025 );
		if( !ptrWnd->IsWindowEnabled() )
			return;

		if( m_bCanSkipFirst && m_bSkipFirst )
		{
			//Make sure the First page is active
			m_ptrWizard->SetActivePage( 0 );
			m_ptrWizard->PressButton( PSBTN_FINISH );
		}
		else
		{
			m_bSkipFirst = FALSE;
		}

	}

	SetModifiedFlag();
}

void CLDVPTaskFSCtrl::OnHelpFilenameChanged() 
{
	SetModifiedFlag();
}

void CLDVPTaskFSCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	COleControl::OnSetFocus(pOldWnd);
	
	if( m_ptrWizard )
	{
		m_ptrWizard->SetFocus();
	}
}


void CLDVPTaskFSCtrl::OnRecordingChanged() 
{
	SetModifiedFlag();
}

void CLDVPTaskFSCtrl::OnTaskNameChanged() 
{
	SetModifiedFlag();
}

void CLDVPTaskFSCtrl::OnTaskDescriptionChanged() 
{
	SetModifiedFlag();
}

void CLDVPTaskFSCtrl::OnTaskDescriptiveNameChanged() 
{
	SetModifiedFlag();
}

BOOL CLDVPTaskFSCtrl::DeleteTask() 
{
	if( m_ptrWizard )
		return (BOOL)m_ptrWizard->SendMessage( WM_COMMAND, MAKEWPARAM( UWM_DESTROYSCAN, 0 ), 0L );
	else
		return FALSE;
}

DWORD CLDVPTaskFSCtrl::GetHiddenDrives()
{
    HKEY    hKey = NULL;
    DWORD   dwHiddenDrives = 0;
    DWORD   dwType = REG_DWORD;
    DWORD   dwSize = sizeof(DWORD);
    long    lResult;

    lResult = RegOpenKeyEx( HKEY_CURRENT_USER,
                            POLICY_KEY_EXPLORER,
                            0,
                            KEY_READ,
                            &hKey );

    if( ERROR_SUCCESS == lResult )
    {
        // Read data.
        lResult = SymSaferRegQueryValueEx( hKey,
                                   POLICY_VALUE_HIDE_SPECIFIED_DRIVE,
                                   NULL,
                                   &dwType,
                                   (LPBYTE) &dwHiddenDrives,
                                   &dwSize );

        if( ERROR_SUCCESS != lResult )
        {
            // See if there was no registry value. Then the policy is not
            // configured and no drives are hidden

            dwHiddenDrives = 0;
        }

            // Cleanup.
        RegCloseKey(hKey);
    }
    else
    {
        // Couldn't read the reg key
        dwHiddenDrives = 0;
    }

    return dwHiddenDrives;
}

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/

