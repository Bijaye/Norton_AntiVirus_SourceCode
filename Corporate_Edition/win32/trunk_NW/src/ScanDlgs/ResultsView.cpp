//////////////////////////////////////////////////////////////////////////////////
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.	//
// Copyright © 2004, 2005 Symantec Corporation.  All rights reserved.					//
//////////////////////////////////////////////////////////////////////////////////

// ResultsView.cpp : implementation file
//

#include "stdafx.h"
#include "scandlgs.h"
#include "ResultsView.h"
#include "oneline.h"
#include "GetStatus.h"
#include "scndlgex.h"
#include "ConfigHistory.h"
#include "ConfigQPurge.h"
#include "LDVPScnDlvr.h"
#include "clientreg.h"
#include "addfile.h"
#include <rpc.h>
#include "mscfuncs.h"
#include "wait.h"
#include "privileges.h"
#include "user.h"
#include "OSUtils.h"
#include "misc.h"
#include "vpstrutils.h"
#include "SymSaferRegistry.h"
#include "resultcontrolconstants.h"

#include "navcorph.h"
#include "navcorphstr.h"

#include "PauseDialog.h"

#include "SavrtModuleInterface.h"
#define INITGUID
#include "iquaran.h"
#include "iscandeliver.h"
#include "ccEraserInterface.h"

#include "ResultsActionDlg.h"
#include "LocalRegistryStorage.h"
#include "RiskConfiguration.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDM_PAUSE1HOUR	0x0010

#include <locale.h>
#include ".\resultsview.h"

///////////////////////////////////////////////////////////////////////////
// CResultsView Implementation of IScanCallback
interface IScanCallbackImp :  public IScanCallback
{
	BOOL closeview;
	CResultsView* m_pView;
	LONG m_refcount;

	IScanCallbackImp() : m_pView(NULL),m_refcount(1),closeview(FALSE) {};
	~IScanCallbackImp() {};

	ULONG STDMETHODCALLTYPE AddRef(void)
	{
		return InterlockedIncrement(&m_refcount);
	}

	ULONG STDMETHODCALLTYPE Release(void)
	{
		ULONG r = InterlockedDecrement(&m_refcount);
		if (r==0) delete this;
		return r;
	}
	HRESULT STDMETHODCALLTYPE QueryInterface(const IID & riid,void **ppvObj)
	{
		GUID iid = _IID_IScanCallback;
		*ppvObj = NULL;
		if (IsEqualIID(riid, iid) || IsEqualIID(riid, IID_IUnknown))
		{
    		*ppvObj =this;
			AddRef();
			return S_OK;
		}
   		return E_NOINTERFACE;
	}

	VI(ULONG) Progress(PPROGRESSBLOCK Progress)
	{
		if (closeview)
			return ERROR_STOP_CALLBACKS;

        if ( m_pView )
		    return m_pView->AddProgress(Progress)?ERROR_STOP_SCAN:0;
        else
		    return E_NOTIMPL;
	}

	VI(ULONG) RTSProgress(PREALTIMEPROGRESSBLOCK Progress)
	{
		return S_OK;
	}

	VI(ULONG) Virus(const char *line)
	{
		return S_OK;
	}

    VI(ULONG) ScanInfo(PEVENTBLOCK pEventBlk)
	{
		return S_OK;
    }
};



/////////////////////////////////////////////////////////////////////////////
// CResultsView dialog


CResultsView::CResultsView(CWnd* pParent /*=NULL*/) :
    CDialog(CResultsView::IDD, pParent),
    m_bShowOnFirstThreat (false),
    m_bDisplayDialogInViewThread (false),
    m_pViewThread (NULL),
    m_dwMinY(0),
    m_dwMinX(0),
	m_bRemovingRisks(FALSE),
    m_bAutoRebootOnDialogClose(false),
    m_bCloseDialogAfterRiskRepair(false)
{
	//{{AFX_DATA_INIT(CResultsView)
	//}}AFX_DATA_INIT
	m_bInitialized = FALSE;
	m_bDetails = FALSE;
	m_pDetailView = 0;
	m_pScanConfig = NULL;
	m_pScan = NULL;
	m_pScanCallback = NULL;
    m_pVBin = NULL;
	m_dwScanID = 0;
	m_bHasData=FALSE;
    m_hWndMain = NULL;
	m_time=0;
	memset(&m_Progress,0,sizeof(m_Progress));
	m_Progress.Status = S_STARTING;
    m_pRoot = NULL;
	m_dwScanDelay = 0;
	m_dwDelayCount = 0;
	m_dwMaxDelay = 2;
	m_dwAllow4Hour = 0;
	m_dwMaxPause = 0;
	m_bMaxedOut = FALSE;
	m_bMaxedOutUI = FALSE;
    m_bViewOpenedRemotely = FALSE;

	m_hIcon = AfxGetApp()->LoadIcon(IDI_SHIELD);

    TRACE("***Constructing a Results View: 0x%08p\n", this );

	CoInitialize(NULL);

	setlocale(LC_ALL, "");
}


CResultsView::~CResultsView()
{
    TRACE("***Destroying a Results View: 0x%08p\n", this );

	DWORD size = m_MyAllocs.GetSize();
	DWORD iter = 0;
	while( iter < size )
	{
		delete [] m_MyAllocs.GetAt( iter );
		iter++;
	}

    DeInitScanner();
	CoUninitialize();
}


void CResultsView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResultsView)
	DDX_Control(pDX, IDC_FILENAME, m_filename);
	DDX_Control(pDX, IDC_FILEPATH, m_filePath);
    DDX_Control(pDX, IDC_GENERIC_WARNING, m_GenericWarning);
	DDX_Control(pDX, IDC_AVI, m_Avi);
	DDX_Control(pDX, IDC_LDVPRESULTS, m_oLDVPResultsCtrl);
    DDX_Control(pDX, IDC_CLOSE, m_oClose);
    DDX_Control(pDX, IDC_TERMINATE_PROCESS, m_oTerminateProcess);
    DDX_Control(pDX, IDC_WARNING_ICON, m_oWarningIcon);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResultsView, CDialog)
	//{{AFX_MSG_MAP(CResultsView)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_REPORT, OnReport)
	ON_BN_CLICKED(IDC_DETAILS, OnDetails)
	ON_BN_CLICKED(IDC_FILTER, OnFilter)
	ON_BN_CLICKED(IDC_ACTIONS, OnActions)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_CONFIGURE_HISTORIES, OnConfigHistory)
	ON_BN_CLICKED(IDOK, OnClickClose )
	ON_BN_CLICKED(IDC_PAUSE, OnPause)
    ON_BN_CLICKED(IDC_TERMINATE_PROCESS, OnTerminateProcess)
    ON_BN_CLICKED(IDC_CLOSE, OnButtonClose)
	ON_WM_CLOSE()
	ON_WM_GETMINMAXINFO()
	ON_WM_PAINT()
	ON_COMMAND(IDC_SCANDELIVER, OnScandeliver)
	ON_COMMAND(IDC_REPAIR, OnRepair)
	ON_COMMAND(IDC_RESTORE, OnRestore)
	ON_COMMAND(IDC_DELETE, OnDelete)
	ON_COMMAND(IDC_ADD_TO_QUARANTINE, OnAddToQuarantine)
	ON_COMMAND(IDC_PURGE_QUARANTINE, OnPurgeQuarantine)
	ON_COMMAND(IDC_UNDO, OnUndo)
	ON_COMMAND(IDC_QUARANTINE_INFECTED_ITEM, OnQuarantineInfectedItem)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_START, OnPlay)
	ON_MESSAGE( CM_SHOWSCAN, OnShowScan )
	ON_MESSAGE( CM_RESIZECONTROLS, OnResizeControls )
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnTTT)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnTTT)
END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CResultsView, CDialog)
    //{{AFX_EVENTSINK_MAP(CResultsView)
	ON_EVENT(CResultsView, IDC_LDVPRESULTS, 1 /* DestroyingItem */, OnDestroyingItemLdvpresults, VTS_I4)
	ON_EVENT(CResultsView, IDC_LDVPRESULTS, 4 /* ActionRequested */, OnActionRequestedLdvpresults, VTS_I4)
	ON_EVENT(CResultsView, IDC_LDVPRESULTS, 2 /* ShowDetails */, OnShowDetailsLdvpresults, VTS_I4 VTS_BSTR VTS_BOOL VTS_I4)
	ON_EVENT(CResultsView, IDC_LDVPRESULTS, 3 /* SelectionChanged */, OnSelectionChangedLdvpresults, VTS_BOOL)
	ON_EVENT(CResultsView, IDC_LDVPRESULTS, 5 /* FilterEvents */, OnFilterEventsLdvpresults, VTS_NONE)
	ON_EVENT(CResultsView, IDC_LDVPRESULTS, 6 /* ScanDeliver */, OnScanDeliverLdvpresults, VTS_NONE)
	ON_EVENT(CResultsView, IDC_LDVPRESULTS, 7 /* AddToQuarantine */, OnAddToQuarantineLdvpresults, VTS_NONE)
	ON_EVENT(CResultsView, IDC_LDVPRESULTS, 8 /* StarttheScan */, OnStarttheScanLdvpresults, VTS_NONE)
	ON_EVENT(CResultsView, IDC_LDVPRESULTS, 9 /* PausetheScan */, OnPausetheScanLdvpresults, VTS_NONE)
	ON_EVENT(CResultsView, IDC_LDVPRESULTS, 10 /* StoptheScan */, OnStoptheScanLdvpresults, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResultsView message handlers
void CResultsView::ViewThread(CResultsView *pThis)
{
	// Currently, this thread is responsible for deleting this object.
	// When the scan finishes, this thread had better be resumed!

    // If the caller decided we did not need to display the dialog. Simply
    // return from the thread.
	// TODO:  We're making ourselves jump through hoops.
	// We should only create this thread when we need it, rather than having all
	// code paths that indicate that the scan is done resuming this thread.
	// We could also get rid of m_bDisplayDialogInViewThread.
	// Transfer the responsibility for deleting this object to ScanDone().  It
	// can know whether this thread will delete the object, or if it should do
	// it.
	// Also, all cleanup unrelated to UI (e.g. calling m_ViewClosed()) should be
	// in the destructor so that it's in one place.
    if (pThis->m_bDisplayDialogInViewThread == false)
	{
		if (pThis->m_ViewClosed)
			pThis->m_ViewClosed (pThis->m_Context);
		Sleep(128);
		delete pThis;
        return;
	}

	CoInitialize(NULL);
	AfxEnableControlContainer();

    HMODULE   hAPMod = NULL;
	SAVRT_PTR pSAVRT (CSavrtModuleInterface::Init());

	if (pSAVRT.Get() != NULL)
		pSAVRT->UnProtectProcess();

	int iErr = pThis->DoModal();

	if (pSAVRT.Get() != NULL)
		pSAVRT->ProtectProcess();

	if (iErr == -1 || iErr == IDABORT)
	{
		AfxMessageBox (IDS_CREATE_ERROR);
		if (pThis->m_ViewClosed)
			pThis->m_ViewClosed (pThis->m_Context);
	}

	Sleep(128);
	delete pThis;

	CoUninitialize();
}


//////////////////////////////////////////////////////////////////////////////////////////
DWORD CResultsView::Open(PRESULTSVIEW ResultsView)
{
	DWORD cc = RV_SUCCESS;

	m_bInitialized = FALSE;

	if (!ResultsView || ( ResultsView->Size != sizeof(RESULTSVIEW)) )
		return RV_ERROR_BAD_SIZE;

	m_Flags               = ResultsView->Flags;
	m_Parent              = ResultsView->hWndParent;
    m_hWndMain            = ResultsView->hWndMain;
	m_Type                = ResultsView->Type;
    m_GetFileStateForView = ResultsView->GetFileStateForView;
	m_TakeAction          = ResultsView->TakeAction;
	m_TakeAction2         = ResultsView->TakeAction2;
	m_GetCopyOfFile       = ResultsView->GetCopyOfFile;
	m_Context             = ResultsView->Context;
	m_ViewClosed          = ResultsView->ViewClosed;
    m_GetVBinData         = ResultsView->GetVBinData;
    m_pfnExcludeItem      = ResultsView->pfnExcludeItem;
	m_dwScanID            = ResultsView->dwScanID;
	m_dwDelayCount        = ResultsView->dwDelayCount;
	m_dwMaxDelay          = ResultsView->dwMaxDelay;
	m_dwAllow4Hour        = ResultsView->dwAllow4hour;
	m_bShowOnFirstThreat  = ResultsView->bShowOnFirstThreat;
    m_bViewOpenedRemotely = ResultsView->bViewOpenedRemotely;

	m_dwMaxPause = (ResultsView->dwMaxPause)*60; // convert from minutes to secs
	if (!(m_Flags & RV_FLAGS_LOCK_SCAN))
		m_dwMaxPause	 = 0; // don't honor the limit on the pause, if the scan is not locked

	m_bStopTheScan         = FALSE;
	m_bPaused              = FALSE;
	m_Modeless             = FALSE;
	m_bMaxedOut            = FALSE;
	m_bMaxedOutUI          = FALSE;
	m_strFilterSettingsKey = ResultsView->szFilterSettingsKey;

	if (ResultsView->Title)
		m_Title=ResultsView->Title;

	// If this is a new scan (rather than history view), add the start date and time to the window title.
    // Making the title modification here in the work thread which is impersonating the logged-on user
    // (in the case of local scans).

    if ( RV_TYPE_SCAN == m_Type )
    {
        auto    TCHAR       szShortDate[60];
        auto    TCHAR       szShortTime[60];
        auto    SYSTEMTIME  localTime;
        auto    CString     strTime;

        szShortDate[0] = _T('\0');
        szShortTime[0] = _T('\0');

        GetLocalTime( &localTime );

        // Small note on LOCALE_USER_DEFAULT vs. LOCALE_SYSTEM_DEFAULT:
        // Problems may occur when the user default locale selected in Control Panel | Regional settings
        // differs from the system default locale. I've chosen the ones that go along with user defaults...
        //
        // Using LOCALE_USER_DEFAULT it's possible garbled characters will be displayed -- for instance,
        // user selects Japanese dates but does not change the default ANSI code page of the machine to
        // one which supports kanji.
        //
        // Using LOCALE_SYSTEM_DEFAULT will (usually) display dates in the default format for the
        // system locale (no errors due to inability to display characters), but won't obey user
        // preferences for formatting.

        GetDateFormat( LOCALE_USER_DEFAULT, DATE_SHORTDATE, &localTime, NULL, szShortDate, sizeof(szShortDate)/sizeof(*szShortDate) );
        GetTimeFormat( LOCALE_USER_DEFAULT, 0, &localTime, NULL, szShortTime, sizeof(szShortTime)/sizeof(*szShortTime) );

        strTime.Format( IDS_SCAN_TIME_FORMAT, szShortDate, szShortTime );

        m_Title += strTime;
    }

	if (m_Parent || m_bDetails)
	{
		if (!CDialog::Create(IDD,CWnd::FromHandle(m_Parent)))
			return RV_ERROR_NO_CREATE;

		if (m_bDetails)
		{
			CRect r;

			GetWindowRect(r);
			SetWindowPos(0,r.left+20,r.top+20,0,0,SWP_NOZORDER|SWP_NOSIZE);
			//CenterWindow();
			ShowWindow(SW_SHOW);
		}
		m_Modeless = TRUE;
	}
	else
	{
        m_pViewThread = AfxBeginThread ((AFX_THREADPROC)ViewThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
        if (m_pViewThread)
        {
		    //Srikanth 12/01/04
		    //There is a new option "show scan progress if threat detected".
		    //In that case, we want to show the scan progress dialog at a later stage.
		    if (m_bShowOnFirstThreat == false)
            {
                m_bDisplayDialogInViewThread = true;
                m_pViewThread->ResumeThread ();
	            for (int i=0; !m_bInitialized && i < 40; i++)
		            Sleep(250);
            }
        }
        else
	        cc = RV_ERROR_NO_THREAD;
	}

	ResultsView->pResultsViewDlg = this;

    if ( cc == 0 )
    {
	    if ( m_pVBin )
        {
		    m_pVBin->Release();
            m_pVBin = NULL;
        }

		HRESULT hr = CoCreateLDVPObject( CLSID_Cliscan, IID_IVBin2, (void**)&m_pVBin );
    }

	return cc;
}


//////////////////////////////////////////////////////////////////////////////////////////
BOOL ScanRunning(DWORD Status)
{

	if (Status == S_DONE || Status == S_SUSPENDED || Status == S_NEVER_RUN || Status == S_ABORTED || Status == S_DELAYED)
		return FALSE;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Returns ERROR_SUCCESS in normal conditions.
// Other values indicate the scan stopped.
DWORD CResultsView::AddProgress(PPROGRESSBLOCK Block)
{
	// Add the progress.
	m_Progress = *Block;

	if (ScanRunning(Block->Status))
	{
		// If the scan was paused, wait for m_dwMaxPause seconds then unpause it.
		while(m_bPaused && !m_bMaxedOut)
		{
			Sleep(1000);  // in milliseconds
			if (m_dwMaxPause)  // if we're limiting the pause time
			{
				if ((m_dwMaxPause -= 1) == 0)
				{
					m_bPaused = FALSE;
					m_bMaxedOut = TRUE;
				}
			}
		}
	}
	else
	{
		m_time=0;
		// The caller is adding progress when the scan is not running.
		// That must mean the scan finished.
		SAVASSERT(m_Progress.Status != S_NEVER_RUN);
			// I don't think this will happen, but if it does, find out whether
			// AddProgress() might be called again.
			// If not, we should call ScanDone().
		if (m_Progress.Status != S_NEVER_RUN)
			ScanDone(); // Notify this object that the scan is done, so it can clean up.
	}

	// if we're stopping, set any scan delay
	if (m_bStopTheScan)
		Block->ScanDelay = m_dwScanDelay;

	return m_bStopTheScan;
}

//////////////////////////////////////////////////////////////////////////////////////////
DWORD CResultsView::AddLogLine( LPARAM Data, char* Line )
{

	// If no more data  enable some items
	if (Line==NULL)
	{
		AfxGetApp()->DoWaitCursor(-1 );
		m_oLDVPResultsCtrl.AddLogLine(0, NULL);
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_FILTER,TRUE);
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_EXPORT,TRUE);
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_REPORT,TRUE);

		return 0;
	}

    switch(m_Type)
    {
    case RV_TYPE_SCANHISTORY:
    case RV_TYPE_SWEEPHISTORY:
	{
		CString	str;
		str.Format("%u:%s", Data, Line);
		m_ItemStorage.AddToStorage((char *)(const char *)str);
        break;
	}
    case RV_TYPE_AUTOPROTECT:
    case RV_TYPE_SCAN:
		//Srikanth 12/01/04
		//At this stage, in the case of "show scan progress if threat...",
		//a threat has been detected and we want to show the scan progress dialog now.
		if(m_bShowOnFirstThreat && m_pViewThread)
		{
            m_bDisplayDialogInViewThread = true;
            DWORD dwPrevSuspendCnt = m_pViewThread->ResumeThread ();

            // If the thread was suspended, but now it is running, wait for it to initialize.
            if (dwPrevSuspendCnt == 1)
			{
				for (int i=0; !m_bInitialized && i < 40; i++)
					Sleep(250);
			}
		}
        break;
    default:
        break;
    }

	// if this is the first call stare the wait cursor
	if (!m_bHasData)
	{
		AfxGetApp()->DoWaitCursor( 1 );
		m_bHasData=TRUE;
	}

	// add the log list to ken
	DWORD dwResult = m_oLDVPResultsCtrl.AddLogLine(Line,Data);

    switch(m_Type)
    {
    case RV_TYPE_AUTOPROTECT:
    case RV_TYPE_SCAN:
		// We have to do this after AddLogLine().
        SetNotifications();
        break;
    }

	return dwResult;
}

//////////////////////////////////////////////////////////////////////////////////////////
DWORD CResultsView::AddLogLine( LPARAM Data, char *Line, int cSize )
{
	// This version of AddLogLine makes a copy of the LPARAM data
	// and takes ownership of the allocated memory.  The ptrs in
	// m_MyAllocs are deallocated in the destructor.
	BYTE* mycopy = NULL;
	try
	{
		mycopy = new BYTE[cSize];
		if( mycopy != NULL )
		{
			CopyMemory( mycopy, (BYTE*)Data, cSize );

			// Save the pointer to this memory in our vector.
			// This address is written to temp files, passed to other Active X controls,
			// and God knows what else.  The best way to deal with it is to just
			// keep it until we go out of scope.
			m_MyAllocs.Add( mycopy );

			return( AddLogLine( (LPARAM)mycopy, Line ) );
		}
	}
	catch (std::bad_alloc &) {}
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
DWORD CResultsView::ShowScan(LPCSTR sComputer,long ScanID)
{
	HRESULT herr;
	CString sComputerName;
	DWORD dwSize = IMAX_PATH;
	IVirusProtect* pRoot;

	GUID iid_root  = _IID_IVirusProtect,
		 iid_iscan = _IID_IScan,
		 iid_iscanconfig = _IID_IScanConfig,
		 remoteid = _CLSID_Transman,
		 localid  = _CLSID_Cliscan,
		 clsid;

	// Release any previous Scans
	if (m_pScanConfig)
    {
		m_pScanConfig->Release();
        m_pScanConfig = NULL;
    }

	if (m_pScan)
    {
		m_pScan->Release();
        m_pScan = NULL;
    }

	if (m_pScanCallback)
    {
		m_pScanCallback->Release();
        m_pScanCallback = NULL;
    }

	// get the computer name of this machine
	GetComputerName(sComputerName.GetBuffer(IMAX_PATH),&dwSize);
	sComputerName.ReleaseBuffer();

	// compare the computer name sent with this machine to determine local or remote
	if (sComputer == NULL || _tcsicmp(sComputer,sComputerName)==0)
		clsid = localid;
	else
		clsid = remoteid;


	// get the IScanConfig Interface
	herr = CoCreateInstance(clsid,NULL,CLSCTX_INPROC_SERVER,iid_root,(LPVOID*)&pRoot);

	ASSERT(herr==S_OK);

	if (SUCCEEDED(herr))
	{
		herr = pRoot->CreateByIID(iid_iscanconfig,(void**)&m_pScanConfig);
		if (SUCCEEDED(herr))
		{
			herr = pRoot->CreateByIID(iid_iscan,(void**)&m_pScan);
			if (SUCCEEDED(herr))
			{
				// open the scan on the computer
				herr = m_pScanConfig->OpenEx((LPSTR)(LPCSTR)sComputer,ScanID);

				ASSERT(herr==S_OK);

				if (herr==S_OK)
				{
					try
					{
						// create a IScanCallback Implemnation
						m_pScanCallback = new IScanCallbackImp;
						((IScanCallbackImp*)m_pScanCallback)->m_pView = this;
					}
					catch (std::bad_alloc &){}


					// get the scan progress for the scan
					herr = m_pScan->Open(m_pScanCallback,m_pScanConfig);
					if (herr == ERROR_IN_USE)
					{
						CString sTitle,
								sText;
						sTitle.LoadString(IDS_TITLE);
						sText.LoadString(IDS_SCAN_VIEWED);
						MessageBox(sText,sTitle);
					}

//					ASSERT(herr==S_OK);
				}
			} // IScan created

		} // IScanConfig created
	}// root created
	return herr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 1999.9.19  DALLEE - Changed to use regular Windows tooltips rather than MFC's tooltips.
// MFC tips don't work when this DLL's dialog is hosted in a dialog from a different module.
// Tips for this toolbar are always handled by CResultsView, so it's safe to use set
// the TBSTYLE_TOOLTIPS flag and use the standard windows toolbar control tooltips.
//
void CResultsView::InitToolbar(DWORD type)
{

    HANDLE hAccessToken = 0;
    int index=4;

    // TCashin 06/03/2002   Ported CRT fix for Siebel Defect # 1-3S7H4 to 8.0
    // BALJIAN 03/22/2002.	Fix for 1-3S7H4. Get the acces token for the
    //						currently logged on user (NT only).
    if (IsWinNT())
    {
    	hAccessToken = GetAccessTokenForLoggedOnUser();
    }
    // TCashin 06/03/2002   End CRT fix for Siebel Defect # 1-3S7H4

	if( type == RV_TYPE_SCAN || type == RV_TYPE_AUTOPROTECT )
	{
		m_toolBar.LoadToolBar( IDR_RESULTS_TOOLBAR );
		m_toolBar.SetBarStyle(m_toolBar.GetBarStyle() &~CBRS_BORDER_TOP|CBRS_SIZE_DYNAMIC);
		m_toolBar.ModifyStyle( 0, TBSTYLE_FLAT | TBSTYLE_TOOLTIPS );
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_DETAILS,FALSE );
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_ACTIONS,FALSE);
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_START,FALSE );
        m_toolBar.GetToolBarCtrl().EnableButton(IDOK, FALSE);
		m_oLDVPResultsCtrl.SetScanStartEnabled(FALSE); /*//EA 03/20/2000 the client(scandlgs.dll) will set the state of the
											scan start scan pause and scan stop buttons*/

		// remove report options
		index = m_toolBar.GetToolBarCtrl().CommandToIndex(IDC_REPORT);
		m_toolBar.GetToolBarCtrl().DeleteButton(index-1);
		m_toolBar.GetToolBarCtrl().DeleteButton(index-1);


        // TCashin 06/03/2002   Ported CRT fix for Siebel Defect # 1-3S7H4 to 8.0
        // BALJIAN 03/22/2002.	Fix for 1-3S7H4. Hide the help button if we
        //						couldn't get the access token for the
        //						currently logged on user (NT only).
        if (IsWinNT())
        {
            if (!hAccessToken)
            {
	            m_toolBar.GetToolBarCtrl().HideButton(IDHELP, TRUE);
            }
            else
            {
	            CloseHandle(hAccessToken);
	            hAccessToken = NULL;
            }
        }
        // TCashin 06/03/2002   End CRT fix for Siebel Defect # 1-3S7H4


		if( !(m_Flags & RV_FLAGS_ADMIN) && (m_Flags & RV_FLAGS_LOCK_SCAN) )
		{
			m_toolBar.GetToolBarCtrl().EnableButton(IDC_STOP,FALSE );
			m_oLDVPResultsCtrl.SetScanStopEnabled(FALSE); /*//EA 03/20/2000 the client(scandlgs.dll) will set the state of the
											scan start scan pause and scan stop buttons*/
			m_toolBar.GetToolBarCtrl().EnableButton(IDOK,FALSE );
		}

		// disable the pause button IF
		// (a) it's a remote manual admin scan
		// (b) it's a scheduled scan AND the pause feature is disabled AND it's a locked scan
		// (c) it's a scheduled scan AND the pause feature is on AND the scan has already been delayed twice AND it's a locked scan
		//     (if it's not a locked scan, then we don't need to disable the pause button)
		if ( (m_Flags & RV_FLAGS_ADMIN) ||
			 ((m_Flags & RV_FLAGS_SCHD_SCAN) && !(m_Flags & RV_FLAGS_DELAYABLE) && (m_Flags & RV_FLAGS_LOCK_SCAN)) ||
			 ((m_Flags & RV_FLAGS_SCHD_SCAN) && (m_Flags & RV_FLAGS_DELAYABLE) && (m_dwDelayCount >= m_dwMaxDelay) && (m_Flags & RV_FLAGS_LOCK_SCAN)) )
		{
			m_toolBar.GetToolBarCtrl().EnableButton(IDC_PAUSE,FALSE );
			m_oLDVPResultsCtrl.SetScanPauseEnabled(FALSE); /*//EA 03/20/2000 the client(scandlgs.dll) will set the state of the
											scan start scan pause and scan stop buttons*/
		}

        if( type == RV_TYPE_AUTOPROTECT )
        {
            m_toolBar.GetToolBarCtrl().EnableButton(IDC_STOP,FALSE );
            m_toolBar.GetToolBarCtrl().EnableButton(IDC_PAUSE,FALSE );
        }

		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
		return;
	}
	else
		m_toolBar.LoadToolBar( IDR_VIRUS_TOOLBAR );

    // TCashin 06/03/2002   Ported CRT fix for Siebel Defect # 1-3S7H4 to 8.0
    // BALJIAN 03/22/2002.	Fix for 1-3S7H4.  Hide the help button if we
    //						couldn't get the access token for the
    //						currently logged on user (NT only).
    if (IsWinNT())
    {
        if (!hAccessToken)
        {
            m_toolBar.GetToolBarCtrl().HideButton(IDHELP, TRUE);
        }
        else
        {
            CloseHandle(hAccessToken);
            hAccessToken = NULL;
        }
    }
    // TCashin 06/03/2002   End CRT fix for Siebel Defect # 1-3S7H4

    //
    // Hook up some of NAVs Quarantine buttons
    //
    switch ( type )
    {
        case RV_TYPE_VIRUS_BIN:

            // See if Scan & Deliver is enabled
            if ( IsScanDeliverEnabled() )
            {
				m_toolBar.GetToolBarCtrl().EnableButton(IDC_SCANDELIVER, FALSE);/*//EA 03/20/2000 the client(scandlgs.dll) will check if scan and deliver is allowed or not
																					depending on this in ldvpctls the Submit to SARC menu will be disabled or enabled*/
				m_oLDVPResultsCtrl.SetScanDeliverEnabled(TRUE);
			}
            else
            {
				m_toolBar.GetToolBarCtrl().HideButton(IDC_SCANDELIVER);
				m_oLDVPResultsCtrl.SetScanDeliverEnabled(FALSE);/*//EA 03/20/2000 the client(scandlgs.dll) will check if scan and deliver is allowed or not
																					depending on this in ldvpctls the Submit to SARC menu will be disabled or enabled*/
			}

    		m_toolBar.GetToolBarCtrl().EnableButton(IDC_REPAIR, FALSE);
		    m_toolBar.GetToolBarCtrl().EnableButton(IDC_RESTORE, FALSE);
		    m_toolBar.GetToolBarCtrl().EnableButton(IDC_DELETE, FALSE);

            m_toolBar.GetToolBarCtrl().HideButton(IDC_QUARANTINE_INFECTED_ITEM);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_UNDO);

            break;

        case RV_TYPE_VIRUSES:

    		m_toolBar.GetToolBarCtrl().EnableButton(IDC_REPAIR, FALSE);
		    m_toolBar.GetToolBarCtrl().EnableButton(IDC_DELETE, FALSE);
            m_toolBar.GetToolBarCtrl().EnableButton(IDC_UNDO, FALSE);
            m_toolBar.GetToolBarCtrl().EnableButton(IDC_QUARANTINE_INFECTED_ITEM, FALSE);

            m_toolBar.GetToolBarCtrl().HideButton(IDC_ADD_TO_QUARANTINE);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_SCANDELIVER);
		    m_toolBar.GetToolBarCtrl().HideButton(IDC_RESTORE);
		    m_toolBar.GetToolBarCtrl().HideButton(IDC_PURGE_QUARANTINE);

            break;

        case RV_TYPE_BACKUP:
        case RV_TYPE_REPAIR:

		    m_toolBar.GetToolBarCtrl().EnableButton(IDC_DELETE, FALSE);
            m_toolBar.GetToolBarCtrl().EnableButton(IDC_RESTORE, FALSE);
		    m_toolBar.GetToolBarCtrl().EnableButton(IDC_PURGE_QUARANTINE);


		    m_toolBar.GetToolBarCtrl().HideButton(IDC_UNDO);
		    m_toolBar.GetToolBarCtrl().HideButton(IDC_REPAIR);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_SCANDELIVER);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_ADD_TO_QUARANTINE);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_QUARANTINE_INFECTED_ITEM);

            break;

		case RV_TYPE_SCF_FW_VIOLATION_EVENT: //SES_EVENT_FIREWALL_VIOLATION_EVENT
		case RV_TYPE_SCF_CFG_CHANGE_EVENT:
		case RV_TYPE_SCF_INTRUSION_DETECTION_VIOLATION_EVENT:
		case RV_TYPE_SCF_INTRUSION_DETECTION_STATUS_EVENT:
		case RV_TYPE_SCF_UPDATEEVENT:
			m_toolBar.GetToolBarCtrl().HideButton(IDC_REPAIR);
		    m_toolBar.GetToolBarCtrl().HideButton(IDC_RESTORE);
		    m_toolBar.GetToolBarCtrl().HideButton(IDC_DELETE);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_SCANDELIVER);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_ADD_TO_QUARANTINE);
		    m_toolBar.GetToolBarCtrl().HideButton(IDC_PURGE_QUARANTINE);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_QUARANTINE_INFECTED_ITEM);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_UNDO);

			break;

		case RV_TYPE_SCF_ENVPROF_FW_EXCEPTION_EVENT:
		case RV_TYPE_SCF_ENVPROF_CONNECTION_EVENT:
			m_toolBar.GetToolBarCtrl().HideButton(IDC_REPAIR);
		    m_toolBar.GetToolBarCtrl().HideButton(IDC_RESTORE);
		    m_toolBar.GetToolBarCtrl().HideButton(IDC_DELETE);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_SCANDELIVER);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_ADD_TO_QUARANTINE);
		    m_toolBar.GetToolBarCtrl().HideButton(IDC_PURGE_QUARANTINE);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_QUARANTINE_INFECTED_ITEM);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_UNDO);
			m_toolBar.GetToolBarCtrl().HideButton(IDC_DETAILS);

			break;

		case RV_TYPE_TAMPERBEHAVIOR:
			m_toolBar.GetToolBarCtrl().HideButton(IDC_REPAIR);
		    m_toolBar.GetToolBarCtrl().HideButton(IDC_RESTORE);
		    m_toolBar.GetToolBarCtrl().HideButton(IDC_DELETE);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_SCANDELIVER);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_ADD_TO_QUARANTINE);
		    m_toolBar.GetToolBarCtrl().HideButton(IDC_PURGE_QUARANTINE);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_QUARANTINE_INFECTED_ITEM);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_UNDO);
			break;

        default:

		    m_toolBar.GetToolBarCtrl().HideButton(IDC_REPAIR);
		    m_toolBar.GetToolBarCtrl().HideButton(IDC_RESTORE);
		    m_toolBar.GetToolBarCtrl().HideButton(IDC_DELETE);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_SCANDELIVER);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_ADD_TO_QUARANTINE);
		    m_toolBar.GetToolBarCtrl().HideButton(IDC_PURGE_QUARANTINE);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_QUARANTINE_INFECTED_ITEM);
            m_toolBar.GetToolBarCtrl().HideButton(IDC_UNDO);

            break;
    }

	// MH 08.29.00
	// Yet again, we decide to place the reports functionality
	// back into the console.  But this time, add logic to only
	// display the toolbar item if a reg value is set.  And only if
	// we're on a console.
	// [HKLM\Software\...\CurrentVersion]
	// ShowVPReports=REG_DWORD
	// 1: show vpreports
	// This option will be off by default.

	// MH 08.07.00
	// Take out the reports functionality - again.  Leaving the
	// code commented for now since this issue seems to be
	// going back and forth.

	// We only want the reports functionality on the console.
	if( !(m_Flags & RV_FLAGS_ADMIN) )
	{
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_REPORT,FALSE);
		m_toolBar.GetToolBarCtrl().HideButton(IDC_REPORT);
	}
	else
	{
		// Check the registry to see if we *really* want to display this.
		HKEY hkey;
		BOOL bHideReports = TRUE;

		if( ERROR_SUCCESS == RegOpenKey( HKEY_LOCAL_MACHINE, szReg_Key_Main, &hkey ) )
		{
			DWORD dwRtn = 0;
			DWORD dwSize = sizeof(DWORD);

			if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hkey, _T("ShowVPReports"), NULL, NULL, (LPBYTE)&dwRtn, &dwSize ) )
			{
				if( dwRtn )
					bHideReports = FALSE;
			}

			RegCloseKey( hkey );
		}

		if( bHideReports )
		{
			m_toolBar.GetToolBarCtrl().EnableButton(IDC_REPORT,FALSE);
			m_toolBar.GetToolBarCtrl().HideButton(IDC_REPORT);
		}
	}

	if (type!=RV_TYPE_VIRUSES)
	{
		m_toolBar.GetToolBarCtrl().HideButton(IDC_ACTIONS);
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_UNDO,FALSE);
	}


	if (type != RV_TYPE_EVENTS)
		m_toolBar.GetToolBarCtrl().HideButton(IDC_FILTER);


	// disable detail buttons until somthing is selected
	m_toolBar.GetToolBarCtrl().EnableButton(IDC_DETAILS,FALSE );
	m_toolBar.GetToolBarCtrl().EnableButton(IDC_ACTIONS,FALSE);

	// disable file/export/report until data is added
	m_toolBar.GetToolBarCtrl().EnableButton(IDC_FILTER,FALSE );
	m_toolBar.GetToolBarCtrl().EnableButton(IDC_EXPORT,FALSE);
	m_toolBar.GetToolBarCtrl().EnableButton(IDC_REPORT,FALSE );

	// MH 08.30.00
	// Implement a reg value that will disallow a client to restore or undo
	// (potentially) infected files.
	if( !(m_Flags & RV_FLAGS_ADMIN) )
	{
		HKEY hkey;
		BOOL bHideUndo = FALSE;

		if( ERROR_SUCCESS == RegOpenKey( HKEY_LOCAL_MACHINE, szReg_Key_Main, &hkey ) )
		{
			DWORD dwRtn = 0;
			DWORD dwSize = sizeof(DWORD);

			if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hkey, szReg_Val_PreventRestore, NULL, NULL, (LPBYTE)&dwRtn, &dwSize ) )
			{
				if( dwRtn )
					bHideUndo = TRUE;
			}

			RegCloseKey( hkey );
		}

		if( bHideUndo )
		{
			m_toolBar.GetToolBarCtrl().EnableButton(IDC_UNDO, FALSE );
			m_toolBar.GetToolBarCtrl().HideButton(IDC_UNDO);
			m_toolBar.GetToolBarCtrl().EnableButton(IDC_RESTORE,FALSE );
			m_toolBar.GetToolBarCtrl().HideButton(IDC_RESTORE);
		}
	}

    // TCashin 2/11/2002 Ported CRT fix from 7.6 branch
    //
    // JS 10.09.01
    // Implement a reg value that will disable ability to add files to quarantine
    // STS defect 372172
    if( !(m_Flags & RV_FLAGS_ADMIN) )
    {
        CString strKeyString;
        HKEY    hKey;
        DWORD   dwEnabled = 0;
        DWORD   dwSize = sizeof(DWORD);
        BOOL bHideAddToQuarantine = FALSE;

        strKeyString.Format("%s\\%s",
                           szReg_Key_Main,
                           szReg_Key_Quarantine);

	    if( ERROR_SUCCESS == RegOpenKey( HKEY_LOCAL_MACHINE, strKeyString, &hKey ) )
	    {
		    if( ERROR_SUCCESS == SymSaferRegQueryValueEx( hKey, szReg_Val_DisableAddQuarantine, NULL, NULL, (LPBYTE)&dwEnabled, &dwSize ) )
		    {
			    if( dwEnabled )
				    bHideAddToQuarantine = TRUE;
		    }

		    RegCloseKey( hKey );
	    }

	    if( bHideAddToQuarantine )
	    {
		    m_toolBar.GetToolBarCtrl().EnableButton(IDC_ADD_TO_QUARANTINE, FALSE );
	    }
    }
    // TCashin: end 7.6 port

    // create the combobox
	CRect r(0,0,10,10),
		  startRect;
	CDC *pDc;
	CSize maxs(0,0);
	CSize sz;
	CString sText;
	int width;
	BOOL bInserted;

	m_toolBar.m_pResults = &m_oLDVPResultsCtrl;
	m_toolBar.m_pCombo = &m_datebox;
	m_datebox.Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|WS_VSCROLL, r, (CWnd*)&m_toolBar,DATE_COMBOBOX_ID);
	//m_datebox.SetFont(m_toolBar.GetFont());

	// We want to set the size of this font so that it fits
	// inside the combobox.
	//EA 03/24/2000 getting the font info from another control which has a small font irrespective of changing
	//the windows font to extra large so that we remain normal(i.e. of a reasonalble size) on diff language oses.
/*	LOGFONT lf;
	CFont* pFont;

	pFont = GetFont();
	pFont->GetLogFont(&lf);

	lf.lfHeight = 9;

	m_font.CreateFontIndirect(&lf);
	m_datebox.SetFont(&m_font);*/
	m_datebox.SetFont(m_filename.GetFont());
	//EA 03/24/2000
	// load all the string resources needed and populate the combobox
	// also check the width of each string and store the max
	pDc = m_datebox.GetDC();

	sText.LoadString(IDS_FILTER_TODAY);
	m_datebox.AddString(sText);
	sz = pDc->GetTextExtent( sText );
	if (sz.cx > maxs.cx) maxs.cx = sz.cx;

	sText.LoadString(IDS_FILTER_WEEK);
	m_datebox.AddString(sText);
	sz = pDc->GetTextExtent( sText );
	if (sz.cx > maxs.cx) maxs.cx = sz.cx;

	sText.LoadString(IDS_FILTER_MONTH);
	m_datebox.AddString(sText);
	sz = pDc->GetTextExtent( sText );
	if (sz.cx > maxs.cx) maxs.cx = sz.cx;

	sText.LoadString(IDS_FILTER_ALL);
	m_datebox.AddString(sText);
	sz = pDc->GetTextExtent( sText );
	if (sz.cx > maxs.cx) maxs.cx = sz.cx;

	sText.LoadString(IDS_FILTER_RANGE);
	m_datebox.AddString(sText);
	sz = pDc->GetTextExtent( sText );
	if (sz.cx > maxs.cx) maxs.cx = sz.cx;

	m_datebox.ReleaseDC(pDc);
	m_datebox.SetCurSel(3);


	// now we need to place enough separaters at the given index to handle
	// the combobox

	TBBUTTON tbb;
    tbb.iBitmap = NULL;
    tbb.idCommand=0;
    tbb.fsState=0;   // button state--see below
    tbb.fsStyle=TBSTYLE_SEP;   // button style--see below
    tbb.dwData=0;   // application-defined value
    tbb.iString=NULL;    // zero-based index of button label string

	index=4;

	// get the start location of the index we want
	m_toolBar.GetToolBarCtrl().InsertButton(index,&tbb);
	m_toolBar.GetToolBarCtrl().GetItemRect(index,startRect);
	width = startRect.Width();

	do {
		bInserted = m_toolBar.GetToolBarCtrl().InsertButton(index,&tbb);
		startRect.right += width;
	} while(startRect.Width() < maxs.cx && bInserted);

	// remove double spaces after combobox
	if ((type==RV_TYPE_VIRUSES) || (type==RV_TYPE_SCANHISTORY) || (type==RV_TYPE_TAMPERBEHAVIOR))
	{
		index = m_toolBar.GetToolBarCtrl().CommandToIndex(IDC_EXPORT);
		m_toolBar.GetToolBarCtrl().DeleteButton(index-1);
	}


	// set the combox rect the new position
	m_datebox.SetWindowPos(NULL,startRect.left,startRect.top,
		startRect.Width(),100,SWP_NOZORDER);

	m_toolBar.SetBarStyle(m_toolBar.GetBarStyle() &~CBRS_BORDER_TOP|CBRS_SIZE_DYNAMIC);
	m_toolBar.ModifyStyle( 0, TBSTYLE_FLAT | TBSTYLE_TOOLTIPS );
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	// adjust toolar size for new combobox
	m_toolBar.GetWindowRect(r);
	m_toolBar.SetWindowPos(NULL,0,0,
		r.Width()+startRect.Width(),r.Height(),SWP_NOZORDER|SWP_NOMOVE);
	m_toolBar.GetWindowRect(r);

	// Give m_htoolBar the WS_EX_CONTROLPARENT style so that we can
	// tab into the combobox.
	SetWindowLong( m_toolBar.m_hWnd, GWL_EXSTYLE, GetWindowLong( m_toolBar.m_hWnd, GWL_EXSTYLE ) | WS_EX_CONTROLPARENT );
}

/*******************************************************************************/
void insertfront(char* szShortPath,char* cp)
{
    char szTemp[IMAX_PATH];
    _tcscpy(szTemp,szShortPath);
    _tcscpy(szShortPath,cp);
    _tcscat(szShortPath,szTemp);
}
/**********************************************************************************************************/
void ShortPath(char* szPath,char* szShortPath,const int maxlen)
{

    int         len=0;
    int         bTruncated=0;
    char* cp,*start;
    int use;
    char temp[IMAX_PATH];

    if ( (int)strlen(szPath) <= maxlen )
    {
        _tcscpy(szShortPath,szPath);
        return;
    }

    szShortPath[0]=0;
    _tcscpy(temp,szPath);

    start = strchr(temp+3,'\\');
    if ( start )
    {
        use = start-temp;
        for ( ;; )
        {
            cp = strrchr(start,'\\');
            if ( cp==NULL )
                break;
            len += strlen(cp);
            if ( len+use+5>maxlen )
            {
                bTruncated=1;
                break;
            }
            insertfront(szShortPath,cp);
            *cp=0;
        }

        if ( bTruncated )
        {
            *start = 0;
            insertfront(szShortPath,"\\...");
        }

        insertfront(szShortPath,temp);
    }

}
//////////////////////////////////////////////////////////////////////////////////////////
// TODO:  Refactor this slighlty into an UpdateUI() function that updates the UI
//		  based only on this object's current state (not on the current UI state).
//		  It should not depend on some UI elements already being in a certain state.
void CResultsView::OnTimer(UINT nIDEvent)
{
	CString str2,
			strTemp;
	char    *q;
    BOOL    bTerminatedLogLine = FALSE;
    char    szShortenedFileName[_MAX_PATH];
    char    szTemp[IMAX_PATH + 1];


	if( !(m_Flags & RV_FLAGS_CHILD ) )
	{
		//EA 03/24/2000 setting status bar's font to a edit boxes font since it seems to have small
		//font so that when we have large font settings we will look ok
		m_Status.SetFont(m_filename.GetFont());
		//EA 03/24/2000

		// Display our progress:  # files scanned and viruses found.
		strTemp.Format( "%u", m_Progress.Scanned );
		AfxFormatString1( str2, IDS_FILES_SCANNED, strTemp );
		m_Status.SetPaneText(0,str2);

		strTemp.Format( "%u", m_Progress.Viruses );
		AfxFormatString1( str2, IDS_VIRUSES_FOUND, strTemp );
		m_Status.SetPaneText(1,str2);
	}

	// Set the time the scan started (if we haven't already)
	//	   or if it's not running, set m_time to 0 to indicate this.
	if (ScanRunning(m_Progress.Status))
	{
		if (m_time==0)
			m_time = GetTickCount();
	}
	else
    {
		m_time=0;
    }

	// Display how long the scan has been running.
	if (m_time)
	{
		UINT tick = GetTickCount()-m_time,
			   m=0,
			   s=0;
		strTemp = "0";
		str2="0";

		m = tick/60000;
		s =  (tick/1000) - (m*60);

		strTemp.Format("%02u",m);
		str2.Format("%02u",s);

		if( !(m_Flags & RV_FLAGS_CHILD ) && !(m_Flags& RV_FLAGS_ADMIN) )
		{
			CString sFinal;
			AfxFormatString2( sFinal, IDS_TIME, strTemp,str2 );

			if (s>=0 && s < 60 && m >=0)
			{
				//EA 03/24/2000 setting status bar's font to a edit boxes font since it seems to have small
				//font so that when we have large font settings we will look ok
				m_Status.SetFont(m_filename.GetFont());
				//EA 03/24/2000
				m_Status.SetPaneText(2,sFinal);
			}
		}
	}

	switch (m_Progress.Status)
	{
		// Just display a status message for these.
		case S_STARTING: 		str2.LoadString(IDS_S_STARTING); goto set;
		case S_STARTED: 		str2.LoadString(IDS_S_STARTED); goto set;
		case S_STOPPING:		str2.LoadString(IDS_S_STOPPING); goto set;
		case S_SCANNING_DIRS:	str2.LoadString(IDS_S_SCANNING_DIRS); goto set;
		case S_SCANNING_BOOT:	str2.LoadString(IDS_S_SCANNING_BOOT); goto set;
		case S_SCANNING_MEM:	str2.LoadString(IDS_S_SCANNING_MEM); goto set;
		case S_ABORTED:			str2.LoadString(IDS_S_INTERRUPTED);  goto set;
		case S_DELAYED:			str2.LoadString(IDS_S_DELAYED);  goto set;
		case S_NEVER_RUN:		str2.LoadString(IDS_S_NEVER_RUN);  goto set;
		case S_DONE:			str2.LoadString(IDS_S_DONE);  goto set;
		case S_SUSPENDED:		str2.LoadString(IDS_S_SUSPENDED);  goto set;
		case S_RESUMED:			str2.LoadString(IDS_S_RESUMED);  goto set;
        case S_QUEUED:          str2.LoadString(IDS_S_QUEUED); goto set;

set:
			m_filename.SetWindowText(str2);
			m_filePath.SetWindowText("");
			break;

		case S_SCANNING_PROCS:

			// Display the process currently being scanned.
			str2.LoadString( IDS_S_SCANNING_PROCESSES );
			m_filename.SetWindowText(str2);
            // Just display the file name in m_filePath.
			vpstrncpy (szTemp, m_Progress.CurrentFile, sizeof (szTemp));
			q = _tcsrchr(szTemp,'\\');
			if( q != NULL )
			{
				*q = 0;
				m_filePath.SetWindowText( q + 1 );
    			*q = '\\';
			}
			else
				m_filePath.SetWindowText( szTemp );
            break;

		case S_SCANNING_FILES:

			// Display the file currently being scanned.  Separate its path and filename.
		    vpstrncpy (szTemp, m_Progress.CurrentFile, sizeof (szTemp));

			q = _tcsrchr(szTemp,'\\');
			if (q)
			{
				*q = 0;
				m_filename.SetWindowText(q+1);

				// 80 is conservative guesstimate of what will fit.
				ShortPath ( szTemp, szShortenedFileName, 80);

				m_filePath.SetWindowText(szShortenedFileName);
				*q = '\\';
			}
			else
			{
				m_filename.SetWindowText(m_Progress.CurrentFile);
				m_filePath.SetWindowText("");
			}

			break;

		case S_SCANNING_LOADPOINTS:

			// Update the filename item to read "Scanning System Loadpoints"
			str2.LoadString(IDS_S_SCANNING_LOADPOINTS);
			m_filename.SetWindowText( str2 );

			// Set the file path to be the name of the item we are scanning.
			m_filePath.SetWindowText( m_Progress.CurrentFile );

			break;

		case S_SCANNING_ERASER_DEFS:

			// Update the filename item to read "Scanning System Loadpoints"
			str2.LoadString(IDS_S_SCANNING_ERASER_DEFS);
			m_filename.SetWindowText( str2 );

			// Set the file path to be the name of the item we are scanning.
			m_filePath.SetWindowText( m_Progress.CurrentFile );

			break;

		case S_ERASER_SCANS:

			// Update the filename item with a status description.
			str2.LoadString(IDS_S_ERASER_SCANS);
			m_filename.SetWindowText( str2 );

			// Set the file path to be the name of the item we are scanning.
			m_filePath.SetWindowText( m_Progress.CurrentFile );

			break;

        case S_SCANNING_PROCS_AND_LOADPOINTS:

            // Update the filename item with a status description.
            str2.LoadString(IDS_S_SCANNING_PROCS_AND_LOADPOINTS);
            m_filename.SetWindowText( str2 );

            // Set the file path to be the name of the item we are scanning.
            m_filePath.SetWindowText( m_Progress.CurrentFile );

            break;

        case S_SCANNING_PROCS_AND_ERASER_DEFS:

            // Update the filename item with a status description.
            str2.LoadString(IDS_S_SCANNING_PROCS_AND_ERASER_DEFS);
            m_filename.SetWindowText( str2 );

            // Set the file path to be the name of the item we are scanning.
            m_filePath.SetWindowText( m_Progress.CurrentFile );

            break;

        case S_SCANNING_LOADPOINTS_AND_ERASER_DEFS:

            // Update the filename item with a status description.
            str2.LoadString(IDS_S_SCANNING_LOADPOINTS_AND_ERASER_DEFS);
            m_filename.SetWindowText( str2 );

            // Set the file path to be the name of the item we are scanning.
            m_filePath.SetWindowText( m_Progress.CurrentFile );

            break;

        case S_SCANNING_PROCS_AND_LOADPOINTS_AND_ERASER_DEFS:

            // Update the filename item with a status description.
            str2.LoadString(IDS_S_SCANNING_PROCS_AND_LOADPOINTS_AND_ERASER_DEFS);
            m_filename.SetWindowText( str2 );

            // Set the file path to be the name of the item we are scanning.
            m_filePath.SetWindowText( m_Progress.CurrentFile );

            break;
	}

	// If the scan finished, enable/disable buttons appropriately.
	if (m_Progress.Status == S_ABORTED || m_Progress.Status == S_DONE || m_Progress.Status == S_SUSPENDED || m_Progress.Status == S_DELAYED)
	{
		if(!(m_Flags & RV_FLAGS_ADMIN) && (m_Flags & RV_FLAGS_LOCK_SCAN))
		{
			m_toolBar.GetToolBarCtrl().EnableButton(IDOK, TRUE);
		}

		m_toolBar.GetToolBarCtrl().EnableButton(IDC_PAUSE,FALSE );
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_STOP,FALSE );

		m_oLDVPResultsCtrl.SetScanPauseEnabled(FALSE); /*//EA 03/20/2000 the client(scandlgs.dll) will set the state of the
											scan start scan pause and scan stop buttons*/
		m_oLDVPResultsCtrl.SetScanStopEnabled(FALSE); /*//EA 03/20/2000 the client(scandlgs.dll) will set the state of the
											scan start scan pause and scan stop buttons*/

		KillTimer(m_Timer);
		AddLogLine(0, NULL);
        bTerminatedLogLine = TRUE;
	}

	if (ScanRunning(m_Progress.Status) && !m_bPaused)
	{
		if (!m_AviRunning)
		{
			m_Avi.Play(1,-1,-1);
			m_AviRunning = TRUE;
		}

		// if this scan maxed out its pause time, and it's a locked scan...
		if (!m_bMaxedOutUI && m_bMaxedOut && (m_Flags & RV_FLAGS_LOCK_SCAN))
			{
			m_bMaxedOutUI = TRUE;
			m_toolBar.GetToolBarCtrl().EnableButton(IDC_PAUSE, FALSE);
			m_toolBar.GetToolBarCtrl().EnableButton(IDC_START, FALSE);
			}
	}
	else
	{
		if (m_AviRunning)
		{
			m_Avi.Stop();
			m_Avi.Play(0,0,-1);
			m_Avi.Stop();
			m_AviRunning = FALSE;

            // If we're here and we have not added the
            // terminating log line, do it now.
            if ( !bTerminatedLogLine )
    		    AddLogLine(0, NULL);
		}

	}

    if( FALSE == ScanRunning(m_Progress.Status ) )
		SetNotifications();

	// If the option to force a close when done is set,
	// close the dialog automatically.
	if( (m_Progress.Status == S_DONE || m_Progress.Status == S_SUSPENDED) &&
		(m_Flags & RV_FLAGS_CLOSE_SCAN) &&
		!(m_Flags & RV_FLAGS_ADMIN) )
	{
		OnClose();
	}

	// close this dialog if the user delayed it
	// and it had no infections
	if( (m_Progress.Status == S_DELAYED) &&
		(m_Flags & RV_FLAGS_SCHD_SCAN)   &&
		(m_Progress.Infected == 0) )
	{
		OnClose();
	}

	CDialog::OnTimer(nIDEvent);

	Sleep(0);
}

void CResultsView::OnDestroy()
{
	TRACE("Destoying ResultsView\n" );
	CDialog::OnDestroy();

    if ( m_Type == RV_TYPE_SCAN )
    {
		if (m_bInitialized)
        {
            m_Avi.Close();
            KillTimer(m_Timer);
        }
    }

	if (m_ViewClosed)
		m_ViewClosed (m_Context);

	// free up live scanning

	if ( m_pScanConfig )
    {
		m_pScanConfig->Release();
    	m_pScanConfig=NULL;
    }

    if ( m_pVBin )
    {
        m_pVBin->Release();
        m_pVBin = NULL;
    }

	if ( m_pScanCallback )
	{
		((IScanCallbackImp*)m_pScanCallback)->closeview = TRUE;
		m_pScanCallback->Release();
        m_pScanCallback = NULL;
	}

	if ( m_pScan )
    {
		m_pScan->Release();
	    m_pScan=NULL;
    }

    if ( m_pRoot )
    {
        m_pRoot->Release();
        m_pRoot = NULL;
    }

	m_Context = NULL;

	// MH 08.10.00
	// Our hook is only placed when we're a popup dialog.
	if( !(m_Flags & RV_FLAGS_CHILD) && m_Parent )
	{
		// Only destroy the hook if our parent isn't a popup resultsview.
		CResultsView* pRV = (CResultsView*) FromHandle( m_Parent );

		if( FALSE == ( !(pRV->m_Flags & RV_FLAGS_CHILD) && pRV->m_Parent ) )
			UnHookWindow();
	}

	// end simulated modal state by enabling the parent
	if (!(m_Flags & RV_FLAGS_CHILD) && m_Parent && !m_bDetails)
		::SendMessage(m_Parent,WM_ENABLE,TRUE,0);

    // If the ViewThread is already running, this has no impact. If it is not
    // runing, however, resuming it here will simply cause the thread to startup
    // and return because m_bDisplayDialogInViewThread is false. Cleans up the
    // thread.
    if (m_pViewThread)
        m_pViewThread->ResumeThread ();
}


//////////////////////////////////////////////////////////////////////////////////////////
BOOL CResultsView::OnInitDialog()
{
	CDialog::OnInitDialog();

    m_hWndMain = ::FindWindow( _T(NAV_WIN_CLASS_NAME), NULL );

    DWORD dwStyle = GetClassLong( m_hWnd, GCL_STYLE );
    ::SetClassLong( m_hWnd, GCL_STYLE,
					dwStyle & ~(CS_VREDRAW | CS_HREDRAW ) );

    if (m_Flags&RV_FLAGS_ADMIN)
        m_oLDVPResultsCtrl.SetMode(1);

	//
	// Defect 1-3HOTNL --- ScanDlg still saying Norton Antivirus Corporate Edition.
	// This is caused by NAV 7.6 prefacing all scheduled scans and startup scan titles with "Norton Antivirus Corporate Edition - ".
	// Let's remove this if it's there. The only chance for this to fail is if a person actually named it "Norton Antivirus Corporate Edition - ".
	// But that's highly unlikely and more likely in 7.6 --- in which case there would be the preface noted above.
	//
	CString szTitle = m_Title;
	CString szPrefix = _T("Norton AntiVirus Corporate Edition - ");
	if (0 == szTitle.Find(szPrefix))
	{
		szTitle = m_Title.Mid(szPrefix.GetLength());
		m_Title = szTitle;
	}

    SetWindowText(m_Title);

	// IDM_PAUSE1HOUR must be in the system command range.
	ASSERT((IDM_PAUSE1HOUR & 0xFFF0) == IDM_PAUSE1HOUR);
	ASSERT(IDM_PAUSE1HOUR < 0xF000);

	DWORD dwGrayed = 0;

	if ( ((m_Flags & RV_FLAGS_SCHD_SCAN) && !(m_Flags & RV_FLAGS_DELAYABLE) && (m_Flags & RV_FLAGS_LOCK_SCAN)) ||
		 ((m_Flags & RV_FLAGS_SCHD_SCAN) && (m_Flags & RV_FLAGS_DELAYABLE) && (m_dwDelayCount >= m_dwMaxDelay) && (m_Flags & RV_FLAGS_LOCK_SCAN)) )
		{
		dwGrayed = MF_GRAYED;
		}

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strMenuItem;
	strMenuItem.LoadString(IDS_PAUSE1HOUR);
	if ( (m_Flags & RV_FLAGS_SCHD_SCAN) && NULL != pSysMenu && !strMenuItem.IsEmpty() )
    {
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING | dwGrayed, IDM_PAUSE1HOUR, strMenuItem);
    }

	if( !(m_Flags & RV_FLAGS_CHILD ) )
	{
		m_Status.Create(this);
		UINT ind[] = {0,0,0};
		m_Status.SetIndicators(ind,3);

		m_Status.SetPaneInfo(0,0,SBPS_NORMAL,150);
		m_Status.SetPaneInfo(1,0,SBPS_NORMAL,150);
		m_Status.SetPaneInfo(2,0,SBPS_STRETCH,100);
		m_Status.ShowWindow(SW_HIDE);
	}

	if ((m_Flags & RV_FLAGS_CHILD) && m_Parent && !m_bDetails)
	{
		::SetParent( m_hWnd, m_Parent );

		if (m_Flags & RV_FLAGS_CHILD)
		{

			ModifyStyle( WS_OVERLAPPEDWINDOW|WS_SIZEBOX|WS_POPUP, WS_CHILD, SWP_NOOWNERZORDER);
			::PostMessage( m_Parent, UWM_ADDCHILD, (WPARAM)m_hWnd, 0L );

		}
	}
	else if (!m_bDetails && !m_Parent)
		SetParent(NULL);

	// we are opened from as popup with a parent simulate modal my disabling the parent
	if (!(m_Flags & RV_FLAGS_CHILD) && m_Parent)
	{
		::SendMessage(m_Parent, WM_ENABLE, FALSE, 0);
		ModifyStyle( WS_MINIMIZEBOX|WS_MAXIMIZEBOX, 0 );
	}

    if( RV_TYPE_AUTOPROTECT != m_Type )
    {
        m_Avi.Open(IDR_AVI1);
	    m_Avi.Play(0,0,-1);
	    m_Avi.Stop();
	    m_AviRunning = FALSE;
    }

	//Create the toolbar
	m_toolBar.Create( this );

    //hide all new buttons and icon's unless they need to be shown
    m_GenericWarning.ShowWindow(SW_HIDE);
    m_oTerminateProcess.ShowWindow(SW_HIDE);
    m_oWarningIcon.ShowWindow(SW_HIDE);
    m_oClose.ShowWindow(SW_HIDE);
    m_oWarningIcon.ShowWindow(SW_HIDE);

	//and load the proper bar according to the type of the dialog
	BOOL	bTypeTransferred = FALSE;
	switch (m_Type)
	{
		case RV_TYPE_SCANHISTORY:
			m_oLDVPResultsCtrl.SetBehavior( BEHAVIOR_SCAN_HISTORY );
			m_oLDVPResultsCtrl.SetActionsAllowed(0);
			m_oLDVPResultsCtrl.SetFilterByID(m_dwScanID);
			bTypeTransferred = TRUE;

		case RV_TYPE_SWEEPHISTORY:
			if( !bTypeTransferred )
			{
				m_oLDVPResultsCtrl.SetBehavior( BEHAVIOR_SWEEP_HISTORY );
				m_oLDVPResultsCtrl.SetActionsAllowed(0);
				m_oLDVPResultsCtrl.SetFilterByID(m_dwScanID);
				bTypeTransferred = TRUE;
			}

		case RV_TYPE_EVENTS:
			if( !bTypeTransferred )
			{
				m_oLDVPResultsCtrl.SetBehavior( BEHAVIOR_EVENTS );
				m_oLDVPResultsCtrl.SetActionsAllowed(0);
				bTypeTransferred = TRUE;
			}
		case RV_TYPE_SCF_FW_VIOLATION_EVENT: //SES_EVENT_FIREWALL_VIOLATION_EVENT
			if( !bTypeTransferred )
			{
				m_oLDVPResultsCtrl.SetBehavior( BEHAVIOR_SCF_FW_VIOLATION_EVENT );
				m_oLDVPResultsCtrl.SetActionsAllowed(0);
				bTypeTransferred = TRUE;
			}

		case RV_TYPE_SCF_CFG_CHANGE_EVENT:
			if( !bTypeTransferred )
			{
				m_oLDVPResultsCtrl.SetBehavior( BEHAVIOR_SCF_CFG_CHANGE_EVENT );
				m_oLDVPResultsCtrl.SetActionsAllowed(0);
				bTypeTransferred = TRUE;
			}
		case RV_TYPE_SCF_INTRUSION_DETECTION_VIOLATION_EVENT:
			if( !bTypeTransferred )
			{
				m_oLDVPResultsCtrl.SetBehavior( BEHAVIOR_SCF_INTRUSION_DETECTION_VIOLATION_EVENT );
				m_oLDVPResultsCtrl.SetActionsAllowed(0);
				bTypeTransferred = TRUE;
			}
		case RV_TYPE_SCF_INTRUSION_DETECTION_STATUS_EVENT:
			if( !bTypeTransferred )
			{
				m_oLDVPResultsCtrl.SetBehavior( BEHAVIOR_SCF_INTRUSION_DETECTION_STATUS_EVENT );
				m_oLDVPResultsCtrl.SetActionsAllowed(0);
				bTypeTransferred = TRUE;
			}
		case RV_TYPE_SCF_UPDATEEVENT:
			if( !bTypeTransferred )
			{
				m_oLDVPResultsCtrl.SetBehavior( BEHAVIOR_SCF_UPDATE_EVENT );
				m_oLDVPResultsCtrl.SetActionsAllowed(0);
				bTypeTransferred = TRUE;
			}
		case RV_TYPE_SCF_ENVPROF_FW_EXCEPTION_EVENT:
			if( !bTypeTransferred )
			{
				m_oLDVPResultsCtrl.SetBehavior( BEHAVIOR_SCF_ENVPROF_FW_EXCEPTION_EVENT );
				m_oLDVPResultsCtrl.SetActionsAllowed(0);
				bTypeTransferred = TRUE;
			}
		case RV_TYPE_SCF_ENVPROF_CONNECTION_EVENT:
			if( !bTypeTransferred )
			{
				m_oLDVPResultsCtrl.SetBehavior( BEHAVIOR_SCF_ENVPROF_CONNECTION_EVENT );
				m_oLDVPResultsCtrl.SetActionsAllowed(0);
				bTypeTransferred = TRUE;
			}

		case RV_TYPE_VIRUS_BIN:
        	if( !bTypeTransferred )
			{
				m_oLDVPResultsCtrl.SetBehavior( BEHAVIOR_VIRUS_BIN );
				m_oLDVPResultsCtrl.SetActionsAllowed(1);
				bTypeTransferred = TRUE;
			}

		case RV_TYPE_REPAIR:/*//EA 03/20/2000 the client(scandlgs.dll) will need to differentiate between
								virus bin and repair so that for repair view in ldvpctls we will hide the submit to sarc and add
								new item to quarantine menus. NOTE: Hence if old client i.e.(scandlgs.dll) is used with new interface
								of ldvpctls.ocx it will show submit to sarc and add new item to quarantine menus on rt click or Shift+F10.*/

                             // TC 08/24/2000 Made the behavior for REPAIR and BACKUP the same(5), so the
                             // context menus match the toolbars for both views.

			if( !bTypeTransferred )
			{
				m_oLDVPResultsCtrl.SetBehavior( 5 );
				m_oLDVPResultsCtrl.SetActionsAllowed(1);
				bTypeTransferred = TRUE;
			}

        case RV_TYPE_BACKUP:
			if( !bTypeTransferred )
			{
				m_oLDVPResultsCtrl.SetBehavior( BEHAVIOR_BACKUP );
				m_oLDVPResultsCtrl.SetActionsAllowed(1);
				bTypeTransferred = TRUE;
			}

		case RV_TYPE_VIRUSES:
			if( !bTypeTransferred )
			{
				m_oLDVPResultsCtrl.SetBehavior( BEHAVIOR_VIRUS );
				m_oLDVPResultsCtrl.SetActionsAllowed(1);
				bTypeTransferred = TRUE;
			}

			InitToolbar(m_Type);

            // Always turn on web links for virus info.
			m_oLDVPResultsCtrl.InitWebLinkView();

            m_Avi.ShowWindow(SW_HIDE);
			m_filename.ShowWindow(SW_HIDE);
			m_filePath.ShowWindow(SW_HIDE);

			//Now, resize the Results control so it takes up the
			//	entire area of the dialog
			ExpandResultsCtrlVertically();
			break;

		case RV_TYPE_AUTOPROTECT:
            {
            m_oTerminateProcess.EnableWindow(FALSE);
            m_oTerminateProcess.ShowWindow(SW_SHOW);
            m_oClose.ShowWindow(SW_SHOW);
            m_Avi.ShowWindow(SW_HIDE);
            m_oWarningIcon.ShowWindow(SW_SHOW);

            m_oLDVPResultsCtrl.SetBehavior( BEHAVIOR_AUTOPROTECT );
			m_oLDVPResultsCtrl.SetActionsAllowed(ACTION_TYPE_ALL);
            InitToolbar(m_Type);
			m_oLDVPResultsCtrl.InitWebLinkView();

			// Shift a few controls down because AP doesn't have a status bar.
			// First, figure how much much to shift it by looking at the Remove Risks button.
			CWnd *pwndControl;
            CRect rControl;
            CRect rWindow;
			CRect rResultsCtrl;
			int   iShiftDown;

			pwndControl = GetDlgItem(IDC_TERMINATE_PROCESS);
			pwndControl->GetWindowRect(&rControl);
			ScreenToClient(&rControl);
			GetClientRect(&rWindow);
            m_oLDVPResultsCtrl.GetWindowRect(&rResultsCtrl);
		    ScreenToClient(&rResultsCtrl);
			iShiftDown = rWindow.bottom - rControl.bottom
							 - (rControl.top - rResultsCtrl.bottom);
							   // Leave a margin from the bottom equal to the space
							   // between the results control and this button.
			// Shift IDC_TERMINATE_PROCESS down.
			rControl.OffsetRect(0, iShiftDown);
			pwndControl->MoveWindow(&rControl);
			// Shift IDC_CLOSE down.
			pwndControl = GetDlgItem(IDC_CLOSE);
			pwndControl->GetWindowRect(&rControl);
		    ScreenToClient(&rControl);
			rControl.OffsetRect(0, iShiftDown); // rControl already has data for IDC_TERMINATE_PROCESS.
			pwndControl->MoveWindow(&rControl); // pwndControl already points to IDC_TERMINATE_PROCESS.
			// Expand the results control.
			rResultsCtrl.InflateRect(0, 0, 0, iShiftDown);
		    m_oLDVPResultsCtrl.MoveWindow(&rResultsCtrl);
            }
            break;

		case RV_TYPE_SCAN:
			{
			/*//EA 03/20/2000 the client(scandlgs.dll) will need to differentiate between
			RV_TYPE_VIRUS and RV_TYPE_SCAN so that for scan view in ldvpctls we will hide
			the report and export views. NOTE: Hence if old client i.e.(scandlgs.dll) is
			used with new interface of ldvpctls.ocx it will show the report and export views.*/
			//m_oLDVPResultsCtrl.SetBehavior( 0 );
            m_oTerminateProcess.EnableWindow(FALSE);
            m_oTerminateProcess.ShowWindow(SW_SHOW);
            m_oClose.ShowWindow(SW_SHOW);
            m_oWarningIcon.ShowWindow(SW_HIDE);
            m_Status.ShowWindow(SW_SHOW);

            m_oLDVPResultsCtrl.SetBehavior( BEHAVIOR_SCAN );
			m_oLDVPResultsCtrl.SetActionsAllowed(ACTION_TYPE_ALL);
			InitToolbar(m_Type);
			RepositionBars(AFX_IDW_STATUS_BAR, AFX_IDW_STATUS_BAR, 0);

			m_oLDVPResultsCtrl.InitWebLinkView();



            // Post a message to disable vpc32 while the scan is running.
            // This allows our Window to be displayed before disabling
            // the main window, so we get established in the z-order and
            // don't appear behind other windows that happen to be up.
            PostMessage(WM_COMMAND, MAKEWPARAM(ID_DISABLE_MAINWND, 0));
			}
			break;

		case RV_TYPE_TAMPERBEHAVIOR:
			m_oLDVPResultsCtrl.SetBehavior(BEHAVIOR_TAMPERBEHAVIOR);
			m_oLDVPResultsCtrl.SetActionsAllowed(0);
			m_oLDVPResultsCtrl.SetFilterByID(m_dwScanID);
			InitToolbar(m_Type);
			bTypeTransferred = TRUE;
			m_Avi.ShowWindow(SW_HIDE);
			m_filename.ShowWindow(SW_HIDE);
			m_filePath.ShowWindow(SW_HIDE);

			//Now, resize the Results control so it takes up the
			//	entire area of the dialog
			ExpandResultsCtrlVertically();
			break;
	}

    // Set up window sizer.  (Do this after we've resized m_oLDVPResultsCtrl.)
    m_oWndSizer.SetParent(m_hWnd);
    m_oWndSizer.RegisterWindow(IDC_TERMINATE_PROCESS, 1, 1, 0, 0, SWP_NOZORDER);
    m_oWndSizer.RegisterWindow(IDC_CLOSE, 1, 1, 0, 0, SWP_NOZORDER);
	m_oWndSizer.RegisterWindow(m_oLDVPResultsCtrl.m_hWnd, 0, 0, 1, 1, SWP_NOZORDER);
		// For some reason, GetDlgItem(m_hWnd, IDC_LDVPRESULTS) doesn't work.
	if (m_Type == RV_TYPE_SCAN || RV_TYPE_AUTOPROTECT == m_Type)
	{
		m_oWndSizer.RegisterWindow(IDC_FILEPATH, 0, 0, 1, 0, SWP_NOZORDER);
		//NOTE: I don't resize the Filename window for two reasons:
		// 1. It only holds the filenames, and unless it is a very long filename,
		//	it should fit without resizing, and
		// 2. I use the Filename field in the GETMINMAX message to determine the
		//	smallest horizontal size of the dialog.
	}
    m_oWndSizer.UseSizeGrip(FALSE);
    m_oWndSizer.SetInitialized(TRUE);

	// don't allow the user to do anything if this is a readonly dialog
	if (m_Flags & RV_FLAGS_READONLY)
		m_oLDVPResultsCtrl.SetActionsAllowed(0);

	m_oLDVPResultsCtrl.SetFilterSettingsKey( m_strFilterSettingsKey );

	if (m_Type == RV_TYPE_SCAN)
		m_Timer = SetTimer(1,125,NULL);

	m_bInitialized = TRUE;

	SetIcon(AfxGetApp()->LoadIcon(IDI_SHIELD),FALSE);

	UpdateWindow();
	CenterWindow();
	ShowWindow(SW_SHOW);
	SetForegroundWindow();

	if (m_dwScanID)
        m_oLDVPResultsCtrl.SetFilterByID(m_dwScanID);

	// MH 08.10.00
	// Install our windows hook - the hook is needed for tabbing to
	// work when the resultsview is opened as a popup dialog.
	// Our hook is only placed when we're a popup dialog.
	if( !(m_Flags & RV_FLAGS_CHILD) && m_Parent )
		HookWindow();


    //get minimums
    RECT oClientRect = {0};
    GetClientRect(&oClientRect);
    m_dwMinY = oClientRect.bottom;
    m_dwMinX = oClientRect.right;

    if (m_Type == RV_TYPE_SCAN || RV_TYPE_AUTOPROTECT == m_Type)
    {
        CMenu* pMenu = GetSystemMenu( FALSE );
        if( NULL != pMenu )
            pMenu->EnableMenuItem(SC_CLOSE, MF_GRAYED|MF_BYCOMMAND);
    }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////
// purpose: Resize the Results control so it takes up the
//			entire area of the dialog.
void CResultsView::ExpandResultsCtrlVertically()
{
	//Now, resize the Results control so it takes up the
	//	entire area of the dialog
	//This entails changing the TOP and BOTTOM of the control.
	CRect rControl;
	CRect rFinalPosition;
	//		I will use the AVI control to tell me where to
	//		place the new top of the OCX.
	m_Avi.GetWindowRect(&rControl);
	ScreenToClient(&rControl);
	rFinalPosition.top = rControl.top;
	// Keep the control's left and right.
	m_oLDVPResultsCtrl.GetWindowRect(&rControl);
	ScreenToClient(&rControl);
	rFinalPosition.left  = rControl.left;
	rFinalPosition.right = rControl.right;
	// It can go down to the bottom of the dialog.
	GetClientRect(&rControl);
	rFinalPosition.bottom = rControl.bottom;
	m_oLDVPResultsCtrl.MoveWindow(rFinalPosition);
}


//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnSize(UINT nType, int cx, int cy)
{
    if (!m_bInitialized)
		return;

    CDialog::OnSize(nType, cx, cy);

    m_oWndSizer.UpdateSize( nType, cx, cy );

	if ( ::IsWindow( m_Status.m_hWnd ))
		RepositionBars(AFX_IDW_STATUS_BAR, AFX_IDW_STATUS_BAR, 0);
}


//////////////////////////////////////////////////////////////////////////////////////////
LONG CResultsView::OnResizeControls( WPARAM wUnused, LPARAM lUnused )
{
	// Force the sizer to resize all controls.
	CRect rWindowSize;

	GetClientRect(rWindowSize);
	m_oWndSizer.UpdateSize(SIZE_RESTORED,
						   rWindowSize.Width(),
						   rWindowSize.Height(),
						   true);

	return NULL;
}


//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnGetMinMaxInfo( MINMAXINFO* lpMMI )
{
#if 0
	if (m_bInitialized)
	{
		CRect	rectTemp,
				rectMinSize;

		if( ::IsWindow( m_oLDVPResultsCtrl.m_hWnd ) )
		{
			m_oLDVPResultsCtrl.GetWindowRect( &rectTemp );
			ScreenToClient( &rectTemp );
			info->ptMinTrackSize.y = rectTemp.top + 50;

			m_filename.GetWindowRect( &rectTemp );
			ScreenToClient( &rectTemp );
			info->ptMinTrackSize.x = rectTemp.right + 5;
		}
	}
#endif

    // Restrict the minimum size of the dialog.
    lpMMI->ptMinTrackSize.x = m_dwMinX;
    lpMMI->ptMinTrackSize.y = m_dwMinY;

    CDialog::OnGetMinMaxInfo(lpMMI);
}


//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnReport()
{
	m_oLDVPResultsCtrl.ShowReportDlg();
}


//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnDetails()
{
	m_oLDVPResultsCtrl.ShowDetails();
}


//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnFilter()
{
	m_oLDVPResultsCtrl.ShowFilterDlg();

	CString sLine;
	long i = 0;
	LLSTUFF *stuff;
	sLine = m_oLDVPResultsCtrl.EnumVisibleItems(&i, (long*)&stuff);
	if (sLine.IsEmpty())
	{
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_FILTER,TRUE);
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_EXPORT,TRUE);
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_REPORT,TRUE);

	}
	else
	{
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_FILTER,TRUE);
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_EXPORT,TRUE);
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_REPORT,TRUE);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnActions()
{
	CRect r;
	int index =3;
    //do we need this for AutoProtect
	if ( m_Type==RV_TYPE_SCAN )
        index = 6;

	m_toolBar.GetItemRect(index,r);
	m_toolBar.ClientToScreen(r );
	//EA 03/15/2000 added code so that we can use hotkeys
	//on popup menu which we get on click of action item in virus
	//history pane basically disable all keyboard and mouse
	//access if any window has focus on toolbar so that it goes
	//to the pop up menu
	CWnd* pWnd = NULL;
	pWnd = m_toolBar.GetFocus();
	if(pWnd)
	{
		pWnd->EnableWindow(FALSE);
	}
	//EA 03/15/2000

	m_oLDVPResultsCtrl.ShowActionsMenu(r.left,r.bottom);

	//EA 03/15/2000 after menu operation resetting the focus
	//and key board and mouse access to that window
	if(pWnd)
	{
		if(::IsWindow(pWnd->m_hWnd))
		{
			pWnd->EnableWindow(TRUE);
			pWnd->SetFocus();

		}
	}
	//EA 03/15/2000

}


//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnExport()
{
	m_oLDVPResultsCtrl.SaveLog(NULL);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnHelp()
{
	if (IsHTMLHelpEnabled())
		OnHTMLHelp();
	else
		AfxGetApp()->WinHelpInternal(m_Type);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnConfigHistory()
{
	CConfigHistory dlg;
	dlg.DoModal();
}


//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnStop()
{

	m_bPaused = FALSE;

	m_toolBar.GetToolBarCtrl().EnableButton(IDC_PAUSE,FALSE);
	m_toolBar.GetToolBarCtrl().EnableButton(IDC_STOP,FALSE);
	m_toolBar.GetToolBarCtrl().EnableButton(IDC_START,FALSE);

	m_oLDVPResultsCtrl.SetScanStartEnabled(FALSE); /*//EA 03/20/2000 the client(scandlgs.dll) will set the state of the
											scan start scan pause and scan stop buttons*/
	m_oLDVPResultsCtrl.SetScanPauseEnabled(FALSE); /*//EA 03/20/2000 the client(scandlgs.dll) will set the state of the
											scan start scan pause and scan stop buttons*/
	m_oLDVPResultsCtrl.SetScanStopEnabled(FALSE); /*//EA 03/20/2000 the client(scandlgs.dll) will set the state of the
											scan start scan pause and scan stop buttons*/

    if(m_Progress.Status == S_QUEUED)
    {
		CString sTitle,
				sText;
		sTitle.LoadString(IDS_TITLE);

        sText.LoadString(IDS_QUEUED_SCAN_WILL_ABORT);
        MessageBox(sText,sTitle,MB_OK | MB_ICONINFORMATION);
    }

	m_bStopTheScan = TRUE;
	Sleep(2000);
		// TODO:  This needs to be fixed.  We should wait until we're sure
		//		  that the scanning thread knows it should stop.
		//		  We could wait on an event that AddProgress() would set.

	if (( m_Flags&RV_FLAGS_CHILD) && m_Parent )
	{
	//		change stop icon to start
		::SendMessage( m_Parent, UWM_SCAN_STOPPED, 1, 0L );
	}

}

//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnPlay()
{

	m_toolBar.GetToolBarCtrl().EnableButton(IDC_START,FALSE );
	m_toolBar.GetToolBarCtrl().EnableButton(IDC_PAUSE);

	m_oLDVPResultsCtrl.SetScanStartEnabled(FALSE); /*//EA 03/20/2000 the client(scandlgs.dll) will set the state of the
											scan start scan pause and scan stop buttons*/
	m_oLDVPResultsCtrl.SetScanPauseEnabled(TRUE); /*//EA 03/20/2000 the client(scandlgs.dll) will set the state of the
											scan start scan pause and scan stop buttons*/
	m_bPaused = FALSE;


	if (( m_Flags&RV_FLAGS_CHILD) && m_Parent )
			::SendMessage( m_Parent, UWM_SCAN_WANTS_RESTART, 1, 0L );
}


//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnClickClose()
{

	if (( m_Flags&RV_FLAGS_CHILD) && m_Parent )
		::SendMessage( m_Parent, UWM_CLOSING, 1, 0L );
	else
		PostMessage(WM_CLOSE,0,0L);
//		DestroyWindow();
}


//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnPause()
{
	CPauseDialog dPause;
	int button = IDC_PAUSE_PAUSE_BUTTON;

	if ( !(m_Flags & RV_FLAGS_DELAYABLE) && (m_Flags & RV_FLAGS_SCHD_SCAN) && (m_Flags & RV_FLAGS_LOCK_SCAN))
	{ // if we can't delay/pause this scheduled scan, then we shouldn't be here!
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_PAUSE, FALSE);
		m_oLDVPResultsCtrl.SetScanPauseEnabled(FALSE);
		return;
	}

	if ((m_Flags & RV_FLAGS_DELAYABLE) && (m_Flags & RV_FLAGS_SCHD_SCAN))
	{
		m_bPaused = TRUE;

		// set the text of the dialog
		DWORD dwDelaysLeft = m_dwMaxDelay - m_dwDelayCount;

		if (m_Flags & RV_FLAGS_LOCK_SCAN)
		{
			if (dwDelaysLeft == 1)
				dPause.strDelayCountText.LoadString(IDS_DELAY_ONEMORE);
			else
				dPause.strDelayCountText.Format(IDS_DELAY_NTIMES, dwDelaysLeft);
		}

		// disable the stop button if needed
		dPause.m_bStopEnabled = (m_Flags & RV_FLAGS_LOCK_SCAN) ? FALSE: TRUE;

		// disable the long snooze button if needed
		dPause.m_b4HourEnabled = (m_Flags & RV_FLAGS_LOCK_SCAN) ? m_dwAllow4Hour : TRUE;

		button = dPause.DoModal();
	}

	switch (button)
	{
		case IDC_PAUSE_PAUSE_BUTTON:
			m_toolBar.GetToolBarCtrl().EnableButton(IDC_PAUSE, FALSE);
			m_toolBar.GetToolBarCtrl().EnableButton(IDC_START);
			m_oLDVPResultsCtrl.SetScanPauseEnabled(FALSE); /*//EA 03/20/2000 the client(scandlgs.dll) will set the state of the
												scan start scan pause and scan stop buttons*/
			m_oLDVPResultsCtrl.SetScanStartEnabled(TRUE); /*//EA 03/20/2000 the client(scandlgs.dll) will set the state of the
												scan start scan pause and scan stop buttons*/
			m_bPaused = TRUE;
			break;

		case IDC_PAUSE_STOP_BUTTON:
			OnStop();
			break;

		case IDC_PAUSE_DELAY_1HOUR_BUTTON:
			m_dwScanDelay = 60;
			OnStop();
			break;

		case IDC_PAUSE_DELAY_4HOUR_BUTTON:
			// it is now a 3 hour snooze button!
			m_dwScanDelay = 180;
			OnStop();
			break;

		case IDC_PAUSE_CONTINUE_BUTTON:
		default:
			m_bPaused = FALSE;
			break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnDestroyingItemLdvpresults( long lParam )
{
	// This is the LPARAM from the CResultItem that is being destroyed by
	// the CLDVPResultsCtrl in our dialog (whoi is firing the event).
	// That LPARAM might be memory that
	// we allocated in AddLogLine, and this used to be the place where we
	// freed it.  However, that led to lots of memory leaks and double
	// deallocs, because the AddLogLine fn didn't always know when CResultsView
	// actually took ownership of the memory.  So, we've moved all of that
	// memory mgmt to the destructor and the m_MyAllocs member.
}

//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnClose()
{
	if (m_Progress.Status != S_ABORTED &&
		m_Progress.Status != S_DONE && m_Progress.Status != S_SUSPENDED &&
		m_Progress.Status != S_DELAYED &&
		m_Type == RV_TYPE_SCAN)
	{
		if (!(m_Flags&RV_FLAGS_ADMIN) && (m_Flags&RV_FLAGS_LOCK_SCAN))
			return;

		CString sTitle,
				sText;
		sTitle.LoadString(IDS_TITLE);

        if (m_Progress.Status == S_QUEUED)
        {
            sText.LoadString(IDS_CANNOT_ABORT_QUEUED_SCANS);
            MessageBox(sText,sTitle,MB_OK | MB_ICONINFORMATION);
        }
        else
        {
		    sText.LoadString(IDS_ABORT_SCAN);

            // Currently, the UI runs within the context of RTVScan.  This causes a security vulnerability
            // due to the Task Manager's ability to kill an application that it believes is nonresponsive.
            // (Must be done on app tab, not on Process tab.  All users, even guests, can do this)
            // This takes down RTVScan except for 1 thread and may introduce a brief (few dozen accesses)
            // outage in NAVAP protection.
            // Workaround is to eliminate the confirmation dialog, which prevents Task Manager from ever being
            // able to End Task us (1-2HAK1O)
		    // int y = MessageBox(sText,sTitle,MB_YESNO);
            //int y = IDYES;
		    //if(y == IDYES)
			    OnStop();
		    //else if(!(m_Flags&RV_FLAGS_ADMIN) && !m_bDetails)
			//    return;
        }
	}

    // Re-Enable vpc32 now that the scan is finished
    if((m_Type == RV_TYPE_SCAN) && m_hWndMain)
    {
        ::ShowWindow(m_hWndMain, SW_SHOW);
        ::ShowWindow(m_hWndMain, SW_RESTORE);
        ::EnableWindow(m_hWndMain, TRUE);
    }

	if (!(m_Flags&RV_FLAGS_CHILD) && m_Parent)
		::SetForegroundWindow(m_Parent);

	KillTimer(m_Timer);
	ExcludeCheckedItems();

	m_oLDVPResultsCtrl.SendMessage( WM_CLOSE, 0, 0L );

//	CDialog::OnClose();

    // Are we supposed to automatically reboot the machine after this dialog closes?
    if ( true == GetAutoRebootOnDialogClose() )
        RebootSystem();

	if (m_Modeless)
		DestroyWindow();
	else
		EndDialog(0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::PostNcDestroy()
{
	CDialog::PostNcDestroy();

	// null out the m_pDetailView in the parent if we are an we are closing
	if (m_bDetails && m_Parent)
	{
		CWnd *pParent = CWnd::FromHandle(m_Parent);
		ASSERT(pParent);
		((CResultsView*)pParent)->m_pDetailView=NULL;

	}
	// if we are the parent make sure any detail views are destroyied
	else if (m_pDetailView&& m_pDetailView->m_hWnd)
		m_pDetailView->DestroyWindow();

	//I need to free the memory associated with 'this'
	if (m_Modeless)
		delete this;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnActionRequestedLdvpresults(long lAction)
{

    HMODULE hAPMod = NULL;
    SAVRT_PTR pSAVRT( CSavrtModuleInterface::Init() );
	if( pSAVRT.Get() != NULL ){
		pSAVRT->UnProtectProcess();
	}

	CGetStatus dlg(this);

	dlg.Action = lAction;

	dlg.DoModal();

    // If we processed anything, repaint the view
    if ( dlg.GetNumberOfItemsProcessed() )
    {
        UpdateView();
    }

	if( pSAVRT.Get() != NULL ){
		pSAVRT->ProtectProcess();
	}
}



//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnShowDetailsLdvpresults(long ID, LPCTSTR ComputerName, BOOL bInProgress, long lParam)
{

	RESULTSVIEW ScanView;
	CWaitCursor;

	// if a detail view already exists destroy it
	if (m_pDetailView)
		m_pDetailView->DestroyWindow();


	// Fix for STS #339384. Do not show details of a scan in progress when viewed
	// via the console (just like the client history views cannot show details of
	// a scan in progress when viewed from the client).
	if (bInProgress)
	{
		CString str, strTitle;
		str.LoadString(IDS_NO_VIEW_SCAN_IN_PROGRESS);
		strTitle.LoadString(IDS_TITLE);

		MessageBox(str, strTitle, MB_OK|MB_ICONINFORMATION);
		return;
	}


	try
	{
	// create a new results new
	m_pDetailView = new CResultsView;
	}
	catch (std::bad_alloc &) {}
	if(m_pDetailView)
		m_pDetailView->m_bDetails = TRUE;

	ZeroMemory(&ScanView,sizeof(RESULTSVIEW));
	ScanView.Size=sizeof(RESULTSVIEW);

	ScanView.hWndParent=m_hWnd;
	ScanView.Flags=0;

	// show scan results if in progress else show virus history
	if (bInProgress)
		ScanView.Type=RV_TYPE_SCAN;
	else
		ScanView.Type=RV_TYPE_VIRUSES;

	// make sure admin flag is passed on
	if (m_Flags & RV_FLAGS_ADMIN)
		ScanView.Flags = RV_FLAGS_ADMIN;

	ScanView.GetFileStateForView = m_GetFileStateForView;
	ScanView.TakeAction          = m_TakeAction;
	ScanView.TakeAction2         = m_TakeAction2;
	ScanView.GetCopyOfFile       = m_GetCopyOfFile;
	ScanView.ViewClosed          = m_ViewClosed;
	ScanView.Context             = m_Context;
	ScanView.hWndParent          = this->m_hWnd;

	// build the title from computername and date/time
	CString sTitle(ComputerName);
	CString sLogLine, sDate,sTime;
	long i=0;
	LLSTUFF stuff;
	sLogLine = m_oLDVPResultsCtrl.EnumSelectedItems(&i,(long*)&stuff);
	if (!sLogLine.IsEmpty())
	{
		CResultItem rItem(sLogLine);
		rItem.GetDate(sDate);
		rItem.GetTime(sTime);
		sTitle = sTitle + CString(" ") + sDate + CString(" ") + sTime;
	}



	ScanView.Title = (LPSTR)(LPCSTR)sTitle;
	if(m_pDetailView)
		m_pDetailView->Open(&ScanView);

	// open up the store and feed the infections to the new results view
	CString sKey;
	CStringList sList;

	sKey = m_ItemStorage.GetKey(ComputerName,ID);

	m_ItemStorage.LoadStorage(sKey,sList);
	for( POSITION pos = sList.GetHeadPosition(); pos != NULL; )
	{
		CString
			strLog,
			strData,
			str = sList.GetNext(pos);
		int
			index = -1;
		DWORD
			dwData;

		index = str.Find(':');

		if (index != -1)
		{
			strData = str.Left(index);
			strLog = str.Mid(index + 1);
			dwData = atoi((char *)(const char *)strData);
		}
		else
		{
			strLog = str;
			dwData = 0;
		}
		if(m_pDetailView)
			m_pDetailView->AddLogLine( dwData, (LPSTR)(LPCSTR)strLog );
	}

	// if its live open scan object to feed the window all new viruses and current progress
	if (bInProgress && (m_Flags & RV_FLAGS_ADMIN))
	{
		if(m_pDetailView)
			m_pDetailView->ShowScan(ComputerName,ID);
	}
	else if (bInProgress)
	{
		 EnumWindows(EnumWindowsProc,(LPARAM)ID);
	}

	// Done adding items, calling with NULL to enable appropriate toolbar buttons
	if(m_pDetailView)
		m_pDetailView->AddLogLine( NULL, NULL );
}


BOOL CALLBACK CResultsView::EnumWindowsProc( HWND hwnd,LPARAM lParam)
{
	::PostMessage(hwnd,CM_SHOWSCAN,0,lParam);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CResultsView::OnSelectionChangedLdvpresults(BOOL IsOldLogLine)
{
	BOOL bEnable=TRUE;
	BOOL bNotReadOnly = !(m_Flags & RV_FLAGS_READONLY);

	long cCount = m_oLDVPResultsCtrl.GetSelectedItemCount();

	// no Items selected
	if (cCount==0 || cCount > 1)
	{
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_DETAILS,FALSE );
		// m_toolBar.GetToolBarCtrl().EnableButton(IDC_ACTIONS,FALSE);
	}
	else	// at least one item is selected
	{
		if (IsOldLogLine) bEnable=FALSE;
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_ACTIONS,bEnable && bNotReadOnly);
		m_toolBar.GetToolBarCtrl().EnableButton(IDC_DETAILS,TRUE );
	}

    // Special handling for Virus Bin toolbar buttons
	// if (m_Type == RV_TYPE_VIRUS_BIN )
    switch ( m_Type )
	{
        case RV_TYPE_VIRUS_BIN:
			/*//EA 03/20/2000 the client(scandlgs.dll) will decide if scan deliver is enabled or not depending on
			this the ldvpctls.ocx will display the menu item submit to sarc*/
			if ( IsScanDeliverEnabled() )
				m_oLDVPResultsCtrl.SetScanDeliverEnabled(TRUE);
			else
				m_oLDVPResultsCtrl.SetScanDeliverEnabled(FALSE);
			//EA 03/20/2000
            if (cCount==0)
            {
                if ( IsScanDeliverEnabled() )
                {
                    m_toolBar.GetToolBarCtrl().EnableButton(IDC_SCANDELIVER, FALSE);
                }

   		        m_toolBar.GetToolBarCtrl().EnableButton(IDC_RESTORE, FALSE);
            }
            else
            {
                if ( IsScanDeliverEnabled() )
                {
                    m_toolBar.GetToolBarCtrl().EnableButton(IDC_SCANDELIVER, bNotReadOnly);
                }

   		        m_toolBar.GetToolBarCtrl().EnableButton(IDC_RESTORE, bNotReadOnly);
            }

        case RV_TYPE_BACKUP:
        case RV_TYPE_REPAIR:
        case RV_TYPE_VIRUSES:

            if (cCount==0)
            {
		        m_toolBar.GetToolBarCtrl().EnableButton(IDC_REPAIR, FALSE);
		        m_toolBar.GetToolBarCtrl().EnableButton(IDC_RESTORE, FALSE);
		        m_toolBar.GetToolBarCtrl().EnableButton(IDC_UNDO, FALSE);
		        m_toolBar.GetToolBarCtrl().EnableButton(IDC_DELETE, FALSE);
                m_toolBar.GetToolBarCtrl().EnableButton(IDC_QUARANTINE_INFECTED_ITEM,FALSE);
            }
            else
            {
                // Enable the "Repair" option only if the selection contains at
                // least 1 viral threat.  If we only have expanded threats, we
                // cannot perform any repair action.
		        m_toolBar.GetToolBarCtrl().EnableButton(IDC_REPAIR, bNotReadOnly && SelectionContainsViralThreat() );

		        m_toolBar.GetToolBarCtrl().EnableButton(IDC_RESTORE, bNotReadOnly);
		        m_toolBar.GetToolBarCtrl().EnableButton(IDC_UNDO, bNotReadOnly);
		        m_toolBar.GetToolBarCtrl().EnableButton(IDC_DELETE, bNotReadOnly);
                m_toolBar.GetToolBarCtrl().EnableButton(IDC_QUARANTINE_INFECTED_ITEM,bNotReadOnly);
            }
			break;
    }
	// Post a message to resize the results view to its current size so that
	//	   OnSize() will resize the results control appropriately.
	// This is necessary because after the results control gets focus, windows
	//	   resizes it to its original size.  I don't know why.
	// It happens after this function exits, so we need to post the message.
	PostMessage(CM_RESIZECONTROLS);
}

//////////////////////////////////////////////////////////////////////////////////////////
BOOL CResultsView::OnTTT(UINT,NMHDR *pNMHDR,LRESULT *Result)  {

	// allow top level routing frame to handle the message
	if (GetRoutingFrame() != NULL)
		return FALSE;

	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString str;

	UINT nID = pNMHDR->idFrom;

	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) || pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
		// idFrom is actually the HWND of the tool
		nID = ((UINT)(WORD)::GetDlgCtrlID((HWND)nID));

	if (nID != 0) // will be zero on a separator
		str.LoadString(nID);

	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, str,(sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
	else
		_mbstowcsz(pTTTW->szText, str,(sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));

	*Result = 0;

	// bring the tooltip window above other popup windows
	::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	return TRUE;    // message was handled
}

//////////////////////////////////////////////////////////////////////////////////////////
LONG CResultsView::OnShowScan( WPARAM wparam, LPARAM lparam )
{
	if (lparam==(LPARAM)m_dwScanID)
		SetForegroundWindow();
	return 0;
}


/////////////////////////////////////////////////////////////////////
// Handles Toolbar Custom ComboBox SelChange
BOOL CCustomBar::OnCommand( WPARAM wParam, LPARAM lParam )
{
	if (LOWORD(wParam)==DATE_COMBOBOX_ID && HIWORD(wParam)==CBN_SELCHANGE && m_pResults)
	{
		switch (m_pCombo->GetCurSel())
		{
			case 0:	// today
				m_pResults->FilterDates(1);
				break;
			case 1:	// this week
				m_pResults->FilterDates(2);
				break;
			case 2:	// this Month
				m_pResults->FilterDates(3);
				break;
			case 3:	// All Entries
				m_pResults->FilterDates(4);
				break;
			case 4:	// Seleted Range
				m_pResults->FilterDates(5);
				break;
		}

	}
	return CToolBar::OnCommand(wParam,lParam);
}

BOOL CResultsView::OnCommand(WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDCANCEL)
	{
		if((m_Flags&RV_FLAGS_ADMIN) || !(m_Flags&RV_FLAGS_LOCK_SCAN))
			PostMessage(WM_CLOSE,0,0L);

		return TRUE;
	}

    else if(LOWORD(wParam) == ID_DISABLE_MAINWND)
    {
        if(m_hWndMain)
        {
            ::EnableWindow(m_hWndMain, FALSE);
			if (GetParent() == NULL)
			{
				/* Certain cases (defect 1-3NRPSF) make m_hWndMain the parent of
				 * this dialog.  (As far as I can tell, that's how MFC should
				 * work.  When MFC creates this dialog, it calls
				 * ::GetActiveWindow() to find a parent. Normally, this returns
				 * NULL, but in this defect, the SAV main window becomes the
				 * parent.)
				 * If we minimize the SAV UI when this dialog is its child,
				 * we'll also minimize this dialog. */
				::ShowWindow(m_hWndMain, SW_MINIMIZE);
			}
            ::ShowWindow(m_hWndMain, SW_HIDE);
        }
		return TRUE;
    }

	return CDialog::OnCommand(wParam, lParam);
}

void CResultsView::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


void CResultsView::OnScandeliver()
{
    DWORD           dwData=0;
	long            iIndex=0;
    int             iNumSelectedItems = 0;
    int             iNumItemsToSend = 0;
    CResultItem*    pResultItem = NULL;


    // Make sure Scan & Deliver is enabled before we continue.
    if ( !IsScanDeliverEnabled() )
        return;

    // Get the number of selected items
    iNumSelectedItems = m_oLDVPResultsCtrl.GetSelectedItemCount();

    if (iNumSelectedItems)
    {
        HINSTANCE hResourceInst = AfxGetResourceHandle();
        // Go through the selected items and create
        // a CResultItem object for each one.

        GetSelectedItems();

        // If we have a valid pointer to the VirusBinPage callback
        // then call it.
        if (m_GetVBinData)
        {
            m_GetVBinData(m_Context, (LPARAM)&m_arSelectedItems,NULL,NULL);
        }

        // Free the CResultItems
        FreeSelectedItems();

        AfxSetResourceHandle( hResourceInst );
    }
}


BOOL CResultsView::IsScanDeliverEnabled()
{
    CString strKeyString;
    HKEY    hKey;
    DWORD   dwEnabled = 0;
    DWORD   dwSize = sizeof(DWORD);


    // Is the Scan & Deliver COM object flag set?
    if ( m_Flags & RV_FLAGS_SCANDELIVER )
    {
        // Yes. Check the reg value
        strKeyString.Format("%s\\%s",
                            szReg_Key_Main,
                            szReg_Key_Quarantine);

	    if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										    strKeyString,
										    0,
										    KEY_READ,
										    &hKey) )
	    {
		    SymSaferRegQueryValueEx(	hKey,
							    szReg_Val_ScanDeliverEnabled,
							    NULL,
							    NULL,
							    (BYTE*)&dwEnabled,
							    &dwSize	);

		    RegCloseKey( hKey );
	    }
    }

    return ((BOOL)dwEnabled);
}

DWORD CResultsView::GetSelectedItems()
{
    DWORD           dwData=0;
	long            iIndex=0;
    int             iNumSelectedItems = 0;


    // Get the number of selected items
    iNumSelectedItems = m_oLDVPResultsCtrl.GetSelectedItemCount();

    if (iNumSelectedItems)
    {
        // Go through the selected items and create
        // a CResultItem object for each one.

	    for (int i=0;i<iNumSelectedItems;i++)
	    {
            CResultItem *pResultItem = NULL;
		    CString logline = m_oLDVPResultsCtrl.EnumSelectedItems(&iIndex,(long*)&dwData);

		    if (logline.IsEmpty())
			    break;

			try
			{
				// get info from the log line
				pResultItem = new CResultItem(logline,0);
			}
			catch (std::bad_alloc &){}

            // Save the pointer in a CPtrArray
            m_arSelectedItems.Add((void*)pResultItem);

            // Clean up the logline
            logline.Empty();
        }
    }

    return (iNumSelectedItems);
}

DWORD CResultsView::FreeSelectedItems()
{
    int             iNumItems;
    int             i;
    DWORD           dwNumReleased = 0;
    CResultItem*    pResultItem = NULL;


    iNumItems = m_arSelectedItems.GetSize();

    for (i=0;i<iNumItems;i++)
    {
        pResultItem = (CResultItem*)m_arSelectedItems.GetAt(i);

        delete pResultItem;

        dwNumReleased++;
    }

    m_arSelectedItems.RemoveAll();

    return (dwNumReleased);
}

void CResultsView::OnRepair()
{
    InitScanner(szReg_Key_CScan_Repair);
    OnActionRequestedLdvpresults(AC_CLEAN);
}

void CResultsView::OnRestore()
{
	CGetStatus dlg(this);

	dlg.Action = AC_MOVE_BACK;

	dlg.DoModal();

    // If we processed anything, repaint the view
    if ( dlg.GetNumberOfItemsProcessed() )
    {
        UpdateView();
    }
}

void CResultsView::OnDelete()
{
	CGetStatus dlg(this);

	dlg.Action = AC_DEL;

	dlg.DoModal();

    // If we processed anything, repaint the view
    if ( dlg.GetNumberOfItemsProcessed() )
    {
        UpdateView();
    }
}

void CResultsView::OnAddToQuarantine()
{
    VBININFO            vbi = {0};
    EVENTBLOCK          eb = {0};
    DECOMPFILEINFO      decompinfo = {0};
    DWORD               dwError = ERROR_SUCCESS;
    char                szStringBuf[MAX_PATH] = {0};
	HRESULT             herr = NULL;
    BOOL                bClose = TRUE;
	DWORD               dwSize = IMAX_PATH;
	CWait*              waitDlg = NULL; //EA fix for STS defect 362806 VPC32 crash do not need to create the window now
	CWnd*               pMainWnd = FromHandle( m_hWndMain );

    // Set the filter to "*.*"
    CString sFilter( (LPCTSTR) IDS_ADD_FILTER_STRING );

   	SAVRT_PTR pSAVRT (CSavrtModuleInterface::Init());
	if (pSAVRT.Get() != NULL)
		pSAVRT->UnProtectProcess();

    // Create the dialog
    CAddFile  dlg ( TRUE,                  // Open
                    NULL,                  // No default extension
                    NULL,                  // Initial file name
                    OFN_FILEMUSTEXIST |    // Flags
                    OFN_HIDEREADONLY |
                    OFN_PATHMUSTEXIST |
					OFN_NOCHANGEDIR,
                    sFilter,               // Filter string
                    this );                // Parent window

    // Fire off dialog.
    if( dlg.DoModal() != IDOK )
    {
        dwError = CommDlgExtendedError();
        goto Done;
    }

	// MH 08.22.00
	// Fix for VPC32 crash occurring when the user clicks around
	// in the main UI while we are performing our scanning tasks here.
	// Ideally, we'd want to use CWaitCursor to put up an hourglass, but
	// its going out of scope while the scan is happening for zip files.
	// So instead, disable the UI and popup a modeless messagebox indicating
	// that we're busy.
	try
	{
		waitDlg = new CWait; //EA fix for STS defect 362806 VPC32 crash create the window now coz this is where we need the wait
		if( waitDlg )
		{
			waitDlg->CenterWindow( this );
			waitDlg->ShowWindow( SW_SHOW );
			pMainWnd->EnableWindow( FALSE );
		}
	}
	catch (std::bad_alloc &) {}

	// Shut down the scanner
	DeInitScanner();

	// Restart using the default scan options
    //if we can't init the scanner just update the view...
    //because we can't add file
	if( InitScanner(szReg_Key_CScan_Scan) != TRUE )
    {
        // Error adding the file to Quarantine
        AfxMessageBox( IDS_ERROR_LOADING_SCAN_ENGINE, MB_ICONSTOP | MB_OK );

        goto Done;
    }

    // Make sure we set up this results view and save the scan info
	((IScanCallbackImp*)m_pScanCallback)->m_pView = this;

	DWORD dwScanFlags = IScan2::SOFEX_FLAG_REVERSELOOKUPIFCLEAN | IScan2::SOFEX_FLAG_QUARANTINEIFCLEAN | IScan2::SOFEX_FLAG_MANUAL_QUARANTINE;
	DWORD dwScanResultFlags = 0;

	if (!dlg.m_bDelete)
		dwScanFlags |= IScan2::SOFEX_FLAG_BACKUP_ONLY;

	// This will quarantine scan and quarantine the file regardless of whether it's infected or not.
	// It'll also scan for side effects and quarantine those regardless of whether it's infected or not.
	dwError = m_pScan->ScanOneFileEx(dlg.GetPathName().GetBuffer(0), dwScanFlags, &dwScanResultFlags);
    if (dwError != ERROR_SUCCESS)
    {
		// Error adding the file to Quarantine
		AfxMessageBox( IDS_ERROR_ADDING_TO_QUARANTINE, MB_ICONSTOP | MB_OK );
	}

    // Shut down the scanner since we are done with adding the file to Quarantine.
    DeInitScanner();

Done:
    if (pSAVRT.Get() != NULL)
		pSAVRT->ProtectProcess();

	// MH 08.22.00
	// Restore the main UI and do some cleanup.
	if( waitDlg )
	{
		pMainWnd->EnableWindow( TRUE );
		waitDlg->ShowWindow( FALSE );
		delete waitDlg;
		waitDlg = NULL; //EA fix for STS defect 362806 vpc32 crash just cleanup
	}

    UpdateView();

}

void CResultsView::OnPurgeQuarantine()
{
	CConfigQPurge dlg;
	dlg.m_QuarantineType = this->m_Type;
	dlg.DoModal();
}

void CResultsView::OnUndo()
{
	CGetStatus dlg(this);

    // Special case: The Quarantine/Virus Bin views should
    // say specificly state restore rather than Undo.
    if ( m_Type == RV_TYPE_VIRUS_BIN ||
		 m_Type == RV_TYPE_REPAIR    ||
         m_Type == RV_TYPE_BACKUP    )
    {
	    dlg.Action = AC_MOVE_BACK;
    }
    else
    {
	    dlg.Action = AC_UNDO;
    }

	dlg.DoModal();

    // If we processed anything, repaint the view
    if ( dlg.GetNumberOfItemsProcessed() )
    {
        UpdateView();
    }
}

void CResultsView::OnQuarantineInfectedItem()
{
	CGetStatus dlg(this);

	dlg.Action = AC_MOVE;

	dlg.DoModal();

    // If we processed anything, repaint the view
    if ( dlg.GetNumberOfItemsProcessed() )
    {
        UpdateView();
    }
}

BOOL CResultsView::IsHTMLHelpEnabled()
{
	TCHAR lpBuffer [ _MAX_PATH ];

	GetEnvironmentVariable ( NAVCORP_HTML_HELP_ENABLED_VAR, lpBuffer, sizeof (lpBuffer) / sizeof (TCHAR) );

	if ( 0 == _tcscmp (lpBuffer, NAVCORP_HTML_HELP_ENABLED_VALUE))
		return TRUE;
	else
		return FALSE;
}


void CResultsView::OnHTMLHelp()
{
	INavCorpConsoleHelp *pIConsoleHTMLHelp;
	CString sHelpFile;
	BSTR bstrHelpFile;

	sHelpFile = AfxGetApp()->m_pszHelpFilePath;
	bstrHelpFile = sHelpFile.AllocSysString ();
	if( S_OK == CoCreateInstance( CLSID_NavCorpConsoleHelp, NULL, CLSCTX_INPROC_SERVER, IID_INavCorpConsoleHelp, (void**)&pIConsoleHTMLHelp ) )
	{
		// Invoke the help.
		TRACE1( _T("ShowContextHelp ( %d )\n"),m_Type);
		pIConsoleHTMLHelp->ShowContextHelp ( bstrHelpFile, m_Type );
		pIConsoleHTMLHelp->Release();
	}
	else
	{
		// Error.
		AfxMessageBox(IDS_HTML_HELP_ERROR);
	}
	SysFreeString ( bstrHelpFile );
}

// Initializes the CLISCAN COM OBJECTS for the current ResultsView
BOOL CResultsView::InitScanner(LPTSTR lpszScanOptions)
{
    DWORD               dwError;
    BOOL                bReturn = FALSE;
	HRESULT             hr;


	// IScanConfig: used for given scan options
	hr = CoCreateLDVPObject(CLSID_CliProxy,IID_IScanConfig,(void**)&m_pScanConfig);
	if (hr != S_OK) goto All_Done;

	// IScan: the scanner
	hr = CoCreateLDVPObject(CLSID_CliProxy,IID_IScan2,(void**)&m_pScan);
	if (hr != S_OK) goto All_Done;

	try
	{
		// IScanCallback implementation: see above
		m_pScanCallback = new IScanCallbackImp;
		((IScanCallbackImp*)m_pScanCallback)->m_pView = this;
	}
	catch (std::bad_alloc &){}

    // Get the Scan options.
    dwError = m_pScanConfig->Open( NULL, HKEY_VP_ADMIN_SCANS, lpszScanOptions );

	// Open the scan object
	dwError = m_pScan->Open( m_pScanCallback, m_pScanConfig );

    bReturn = TRUE;

All_Done:

    return (bReturn);
}

void CResultsView::DeInitScanner()
{
	// Release Scanner Interfaces
	if ( m_pScanConfig )
    {
		m_pScanConfig->Release();
        m_pScanConfig = NULL;
    }

	if ( m_pScan )
    {
        m_pScan->ControlDLL( CD_UNLOAD_ALL,0);
		m_pScan->Release();
        m_pScan = NULL;
    }

	if ( m_pScanCallback )
    {
		m_pScanCallback->Release();
        m_pScanCallback = NULL;
    }

}

void CResultsView::UpdateView()
{
    if ( m_Type == RV_TYPE_VIRUS_BIN ||
		 m_Type == RV_TYPE_REPAIR    ||
         m_Type == RV_TYPE_BACKUP    )
    {
        // Force it to enumerate the quarantine items.
        m_oLDVPResultsCtrl.ResetContent();
        GetParent()->SendMessage(5025,0,0);
    }

    // Update the view.
    Invalidate();
    OnPaint();

}

void CResultsView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDown(nFlags, point);

	if(m_Parent && (m_Flags & RV_FLAGS_CHILD))
		SetFocus();
}

void CResultsView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CDialog::OnRButtonDown(nFlags, point);

	if(m_Parent && (m_Flags & RV_FLAGS_CHILD))
		SetFocus();
}

BOOL CResultsView::ShowWindow(int nCmdShow )
{

	//EA 03/13/2000
	//activate the results view window with a mouse click my sending the mouse activate
	//message to it main window so that we can get focus even after returning from help
	int nResult = CDialog::ShowWindow(nCmdShow);
	if(m_hWndMain)
		PostMessage(WM_MOUSEACTIVATE, (WPARAM)m_hWndMain,(MAKELONG(HTCLIENT,WM_LBUTTONDOWN)) );
	return nResult;
	//EA 03/13/2000

}

//EA 03/20/2000 these are the new events trapped for the ldvpctls.ocx
void CResultsView::OnFilterEventsLdvpresults()
{
	OnFilter();
}

void CResultsView::OnScanDeliverLdvpresults()
{
	OnScandeliver();
}

void CResultsView::OnAddToQuarantineLdvpresults()
{
	OnAddToQuarantine();
}

void CResultsView::OnStarttheScanLdvpresults()
{
	OnPlay();
}

void CResultsView::OnPausetheScanLdvpresults()
{
	OnPause();
}

void CResultsView::OnStoptheScanLdvpresults()
{
	OnStop();
}
//EA 03/20/2000

BOOL CResultsView::PreTranslateMessage(MSG* pMsg)
{


	// IsDialogMessage() returns TRUE for messages
	// that do not get processed in the case where an embedded
	// (active-x) control is present, which causes certain messages
	// to never reach its target window.
	// The tab key is the only key that we need to be processed by
	// HookWindow(), so catch all VK_TAB's and send the rest for default
	// processing.
	if( pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_TAB )
	{
		return ::IsDialogMessage(m_hWnd, pMsg);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CResultsView::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_PAUSE1HOUR)
		{
		m_dwScanDelay = 60;
		OnStop();
		}
	else
		CDialog::OnSysCommand(nID, lParam);
}


BOOL CResultsView::SelectionContainsViralThreat( void )
{
    BOOL    bViralThreatSelected    = FALSE;
    int     iNumSelectedItems       = 0;
    DWORD   dwVirusType             = 0;
	long    iIndex                  = 0;
    LLSTUFF llStuff                 = {0};

    // Get the number of selected items
    iNumSelectedItems = m_oLDVPResultsCtrl.GetSelectedItemCount();

    if (iNumSelectedItems)
    {
        // Go through the selected items and create
        // a CResultItem object for each one.

	    for (int i=0;i<iNumSelectedItems;i++)
	    {
		    CString logline = m_oLDVPResultsCtrl.EnumSelectedItems(&iIndex,(long*)&llStuff);

		    if (logline.IsEmpty())
			    break;

            CResultItem cResultItem(logline);

            // Get the virus type for this threat.
            cResultItem.GetVirusType( dwVirusType );

			// Is this a viral threat?
			if ( IsViralCategory( dwVirusType ) )
			{
                bViralThreatSelected = TRUE;
				break;
			}
        }
    }

    return bViralThreatSelected;
}

HANDLE CResultsView::GetViewThreadHandle () const
{
    if (m_pViewThread)
        return m_pViewThread->m_hThread;
    else
        return NULL;
}

void CResultsView::ScanDone()
{
    // Release the ViewThread to ensure that things cleanup if the dialog was
    // never displayed.
    if (m_pViewThread)
        m_pViewThread->ResumeThread();
	// TODO:  else delete this;
	// I'm too chicken to make a change like this right before beta.
	// This function should assume responsibility for deleting this object.
	// (See comments in ViewThread().)  "else delete this;" is how it would do it.
}

BOOL CResultsView::InitCliscanScanner(void)
{
	HRESULT hr;

	if(!m_pScan)
	{
		// IScanConfig: Scan configuration
		hr = CoCreateLDVPObject( CLSID_Cliscan, IID_IScanConfig, (void**)&m_pScanConfig );
		if (hr != S_OK)
			return(FALSE);
		// IScan: Scan engine
		hr = CoCreateLDVPObject( CLSID_Cliscan, IID_IScan2, (void**)&m_pScan );
		if (hr != S_OK)
			return(FALSE);
	}

	return(TRUE);
}

void CResultsView::DeInitCliscanScanner(BOOL bInitScanEngine)
{
	if(bInitScanEngine)
	{
		if(m_pScanConfig)
		{
			m_pScanConfig->Release();
			m_pScanConfig = NULL;
		}

		if(m_pScan)
		{
			m_pScan->Release();
			m_pScan = NULL;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
//	Method  : CResultsView::GetItemsNeedingPrompt
//  Purpose : Recieve a list of log lines of those items which need a prompt.
//            We do this first, since when we start taking action on these items,
//            the log lines can shift, causing our enumeration of items to not
//            function.  Using this method, we get all of the items first, then
//            take action on all of them later in ActOnPromptItems.
//
//	Returns   :	None.
///////////////////////////////////////////////////////////////////////////////
//	3/09/05	KSACKIN  - Method created
///////////////////////////////////////////////////////////////////////////////
void CResultsView::GetItemsNeedingPrompt( )
{
	CString sLine;
	long index = 0;
	LLSTUFF* pStuff;

    m_strlistPromptItems.RemoveAll();

	//Get all log lines currently being shown in the scan results view.
	//Check if any of the log line(resultitem)s have the stop service/terminate proc flag as set.
	//If yes, take action for that anomaly.
    sLine = m_oLDVPResultsCtrl.EnumAllItems(&index, (long*)&pStuff);
	while(index != 0)
	{
		CResultItem viewItem(sLine);
        DWORD dwActionTaken(AC_INVALID);
        viewItem.GetActionTaken(dwActionTaken);
		if(AC_TERMINATE == dwActionTaken )
		{
            // Add this item to our list of items to prompt for.
            m_strlistPromptItems.AddTail( sLine.GetBuffer() );
		}

        sLine = m_oLDVPResultsCtrl.EnumAllItems(&index, (long*)&pStuff);
	}
}

///////////////////////////////////////////////////////////////////////////////
//	Method  : CResultsView::ActOnPromptItems
//  Purpose :
//
//	Arguments :	CStringList &strlistPromptItems
//	Returns :	None.
///////////////////////////////////////////////////////////////////////////////
//	3/09/05	KSACKIN  - Method created
///////////////////////////////////////////////////////////////////////////////
void CResultsView::ActOnPromptItems( CResultsView* pThis )
{
#if defined _USRDLL
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	bool	bSvcStopOrProcTerminate = false;

    // Make sure we have a take action function pointer.
    SAVASSERT( pThis->m_TakeAction2 );

    //if we have taken action on an item reset the index because we have
    //invalidated the position pointer
	if( pThis->m_TakeAction2 )
    {
        for ( POSITION rPos = pThis->m_strlistPromptItems.GetHeadPosition(); rPos; )
        {
            CString     strPromptItem       = pThis->m_strlistPromptItems.GetNext( rPos );
            CResultItem rviPromptItem(strPromptItem);
			DWORD	    State               = RV_STATE_INFECTED | RV_STATE_NORMAL;

            DWORD       dwPrimaryAction;
            DWORD       dwSecondaryAction;

            // Get the primary and secondary actions to take from the log line.
            rviPromptItem.GetPrimaryAction( dwPrimaryAction );
            rviPromptItem.GetSecondaryAction( dwSecondaryAction );

            // Take action on this item.
            DWORD dwReturn = pThis->m_TakeAction2( pThis->m_Context,
                                                    NULL,
                                                    strPromptItem.GetBuffer(),
                                                    dwPrimaryAction,     // Use primary action from log line.
                                                    dwSecondaryAction,   // Use the secondary action from log line.
                                                    State,
                                                    bSvcStopOrProcTerminate );
        }
    }

    // Rtvscan should send us a log line that says the risks have been acted on.
	// This will clear our need-to-reboot status.
	pThis->m_bRemovingRisks = FALSE;

    
	// Update the UI.
	pThis->SetNotifications();

    //re-enable the close button
    pThis->m_oClose.EnableWindow();

    // Are we supposed to automatically reboot the machine after risk repair?
    // This is set as a result of the "Close" button being pressed, and the user
    // selecting the option to "Remove Risks and Reboot Machine".
    if ( true == pThis->GetCloseDialogAfterRiskRepair() )
    {
        // The user wanted to automatically close this dialog after risk repair was complete.
        pThis->OnClose();
    }
}


/////////////////////////////////////////////
//
// purpose: Prompt for confirmation,  take actions on the items that need
//			processes terminated, and update the UI.
//
// returns: nothing
void CResultsView::OnTerminateProcess()
{
	// Prompt for confirmation.
    CString strText;
    CString strCaption;
    strCaption.LoadString(IDS_REMOVE_RISK);
    strText.LoadString(IDS_REMOVE_RISK_TEXT);

    DWORD dwRet = MessageBox(strText, strCaption, MB_YESNO | MB_ICONWARNING );
    if( dwRet == IDNO )
        return;
	// Do the work!
	TerminateProcesses();
	// Update the UI.
	OnTimer(0); // Update icons and messages.
}


/////////////////////////////////////////////
//
// purpose: Iterate through items in results ctrl list and
//          take actions on the items that need processes terminated.
//			Don't update the UI (other than displaying a wait cursor).
//
// returns: nothing
void CResultsView::TerminateProcesses()
{
    CWaitCursor oWaitCursor;

	m_bRemovingRisks = TRUE;
	// Change button states.
    m_oTerminateProcess.EnableWindow(FALSE);
    m_oClose.EnableWindow(FALSE);
    
    GetItemsNeedingPrompt();

    // Since we prompted, we should always have at least 1 item in our list.
    SAVASSERT( m_strlistPromptItems.GetCount() );

    // Iterate all of these items, and call TakeAction on each.
    AfxBeginThread((AFX_THREADPROC)ActOnPromptItems, this, THREAD_PRIORITY_NORMAL, 0, 0, 0);
}


////////////////////////////////////////////
//
// purpose: Reboots the system.
void CResultsView::RebootSystem()
{
    CTempPrivileges TempPriv;

	TempPriv.SetPrivilege(SE_SHUTDOWN_NAME, true);
    InitiateSystemShutdown(NULL, NULL, 0, FALSE, TRUE);
}


//close dialog and show any warning when they are necessary
void CResultsView::OnButtonClose()
{
	// First, figure out what types of risk states are in the results control.
	bool bRemoveRisk = false;
	bool bMustReboot = false;

	GetOverallStatus(&bMustReboot, &bRemoveRisk, NULL, NULL);
	// Don't reboot this machine if we're in SSC viewing results of a remote scan.
	if (m_bViewOpenedRemotely) // Is this flag even relevant any more?
		bMustReboot = false;
#ifdef _DEBUG
/*	// For testing only!  Set this key to
	// 1 for Remove Risks
	// 2 for Reboot
	// 3 for both.
	CRegKey regFlags;

	if (regFlags.Open(HKEY_LOCAL_MACHINE,
					  _T(szReg_Key_Main)
						  _T("\\") _T(szReg_Key_Storages)
						  _T("\\") _T(szReg_Key_Storage_File)
						  _T("\\") _T(szReg_Key_Storage_RealTime)
						  _T("\\") _T(szReg_Key_Threat_Configuration))
			== ERROR_SUCCESS)
	{
		DWORD dwFlag = 0;

		if (regFlags.QueryDWORDValue("RebootRemedFlag", dwFlag) == ERROR_SUCCESS)
		{
			if (dwFlag & 1)
				m_bTerminateProcess = true;
			if (dwFlag & 2)
				m_bRebootRequired = true;
		}
	}
*/
#endif
	// Take remaining actions, if there are any.
    if (bRemoveRisk || bMustReboot)
	{
        CResultsActionDlg::EAction eActionRequired = CResultsActionDlg::evInvalidAction;
	
		if (bRemoveRisk && bMustReboot)
			eActionRequired = CResultsActionDlg::evRemoveRisksAndReboot;
		else if (bRemoveRisk)
			eActionRequired = CResultsActionDlg::evRemoveRisks;
		else if (bMustReboot)
			eActionRequired = CResultsActionDlg::evReboot;
		// Prompt the user.
		CResultsActionDlg dlgActionRequired(eActionRequired, this);

		if (dlgActionRequired.DoModal() != IDOK)
			return;
		// Take the chosen action.
		switch (dlgActionRequired.GetChosenAction())
		{
		case CResultsActionDlg::evRemoveRisksAndReboot:
            // Set a flag to let our terminate process threat know to
            // close the dialog after it completes a round of risk repair.
            SetCloseDialogAfterRiskRepair( true );
            // Set a flag to let our worker thread for terminating risks
            // know to auto-reboot the machine after risk repair completes.
            SetAutoRebootOnDialogClose( true );
            // Remove risks and don't update the UI.
			TerminateProcesses();
            // Return now so that OnClose is not processed.  We will call
            // OnClose() as soon as the risk processing is done so that we can
            // cleanup from this dialog correctly.
            return;
		case CResultsActionDlg::evReboot:
            // Set a flag to state that we want to reboot after the OnClose is processed.
            SetAutoRebootOnDialogClose( true );
			break;
		case CResultsActionDlg::evRemoveRisks:
			// Remove risks.
			TerminateProcesses();
			// See whether to show the results.
			if (CResultsActionDlg::evRemoveRisks == eActionRequired)
			{
				// If the action originally required was just to remove risks,
				//	   we should show the results, in case a reboot becomes required.
				OnTimer(0); // Update icons and messages.
				return;
			}
			// else the user knew a reboot was necessary and chose not to,
			//		so close the dialog.
			break;
		case CResultsActionDlg::evNoAction:
			// Just close the dialog.
			break;
		default:
			ASSERT(false);
		}
	}
    OnClose();
}


/** This function is responsible for display updates based on risk status (e.g.
  * whether processes need termination or risks are still being analyzed).
  * Note:  We used to only update the display on receiving a
  * GL_EVENT_ANOMALY_START or GL_EVENT_ANOMALY_FINISH event.  I just removed
  * this logic, but if it's needed, add it back in as an if statement that only
  * calls this function on those event types.
  */
void CResultsView::SetNotifications()
{
	// See if this results dialog is in a mode where we actually want to update
	// the display based on risk status.
	   if (!((RV_TYPE_AUTOPROTECT == m_Type) ||
		  (RV_TYPE_SCAN == m_Type && !ScanRunning(m_Progress.Status))))
		return;
	// First, figure out what types of risk states are in the results control.
	bool bAnalyzing  = false;
	bool bTookAction = false;
	bool bRemoveRisk = false;

	GetOverallStatus(NULL, &bRemoveRisk, &bTookAction, &bAnalyzing);
	// Figure out what status message to display.
	UINT nIDMessage = 0;

	if (bRemoveRisk)
	{
		if (bAnalyzing)
			nIDMessage = IDS_STATUS_MUSTREMOVE_ANALYZING;
		else
			nIDMessage = IDS_STATUS_MUSTREMOVE;
	}
	else if (bTookAction)
	{
		if (bAnalyzing)
			nIDMessage = IDS_STATUS_ACTED_ANALYZING;
		else
			nIDMessage = IDS_STATUS_ACTED;
	}
	else if (bAnalyzing)
		nIDMessage = IDS_STATUS_ANALYZING;
	// else we'll display no message
	// Display the message.
	if (nIDMessage != 0)
	{
		// Display the message.
		CString strStatusMessage;

		try
		{
			strStatusMessage.LoadString(nIDMessage);
		}
		VP_CATCH_MEMORYEXCEPTIONS(;)
		m_GenericWarning.SetWindowText(strStatusMessage);
	}
	// Figure out whether to show the status message and warning icon or scan fields.
	bool bDisplayMessage = ((RV_TYPE_AUTOPROTECT == m_Type) || bRemoveRisk);

	m_GenericWarning.ShowWindow(bDisplayMessage ? SW_SHOW : SW_HIDE);
	m_oWarningIcon  .ShowWindow(bDisplayMessage ? SW_SHOW : SW_HIDE);
	m_filename		.ShowWindow(bDisplayMessage ? SW_HIDE : SW_SHOW);
	m_filePath		.ShowWindow(bDisplayMessage ? SW_HIDE : SW_SHOW);
	m_Avi			.ShowWindow(bDisplayMessage ? SW_HIDE : SW_SHOW);
	// Enable/disable the Remove Risks button.
	// m_bRemovingRisks is actually not a complete solution to the problem of
	// not re-enabling the Remove Risks button after it's clicked.
	// For example, suppose two risks require removal, the user clicks Remove
	// Risks, TerminateProcesses() finishes and sets m_bRemovingRisks to FALSE,
	// and then Rtvscan sends a log line for each risk.  After the first log
	// line, the other risk's status still says it needs a reboot, so we'll
	// re-enable the Remove Risks button until we get the second log line.
	// The way to solve this is to keep an internal list of risks that needed
	// removal when the user hit the button, and then change GetOverallStatus()
	// to check that internal list.
	// However, the problem seems small enough that we can leave this code as-is.
	m_oTerminateProcess.EnableWindow(bRemoveRisk && !m_bRemovingRisks);
	if (bRemoveRisk && !m_bRemovingRisks)
	    m_oTerminateProcess.SetFocus();
}


/** Looks at risk action statuses in our results control and figures out what
  * we've done to them and what needs to be done.
  * @param pbReboot Do we need to be reboot?  May be NULL if you don't care.
  * @param pbRemoveRisk Do we need to terminate processes?  May be NULL if you
  * don't care.
  * @param pbTookAction Did we do anything to any risk?  This is only false if
  * there are no risks or all are pending analysis (or have AC_INVALID).  May be
  * NULL if you don't care.
  * @param pbAnalyzing Are any risks pending analysis?  May be NULL if you don't
  * care.
  */
void CResultsView::GetOverallStatus(bool *pbReboot,
									bool *pbRemoveRisk,
									bool *pbTookAction,
									bool *pbAnalyzing)
{
	// Check and initialize parameters.
	if (NULL == pbReboot	 &&
		NULL == pbRemoveRisk &&
		NULL == pbTookAction &&
		NULL == pbAnalyzing)
	{
		SAVASSERT(!"Bad call to GetOverallStatus().");
			// What's the point if you don't want any flags back?
	}
	bool bLocalReboot = false;
	bool bLocalRemoveRisk = false;
	bool bLocalTookAction = false;
	bool bLocalAnalyzing = false;

	// Iterate through the risks.
	long	 lItemIndex = 0;
	LLSTUFF *pStuff = NULL; // We don't use this data

	for (CString sLine = m_oLDVPResultsCtrl.EnumAllItems(&lItemIndex, (long*)&pStuff);
		 lItemIndex != 0;
				 sLine = m_oLDVPResultsCtrl.EnumAllItems(&lItemIndex, (long*)&pStuff))
	{
		CResultItem oViewItem(sLine);
		DWORD		dwActionTaken(AC_INVALID);

		oViewItem.GetActionTaken(dwActionTaken);
		switch (dwActionTaken)
		{
		case AC_PENDING:
			bLocalAnalyzing = true;
			break;
		case AC_INVALID:
			SAVASSERT(!"We don't know what action we took on a risk.");
			break;
		case AC_TERMINATE:
			bLocalRemoveRisk = true;
			bLocalTookAction = true;
			break;
		case AC_REBOOT_PROCESSING:
			bLocalReboot = true;
			bLocalTookAction = true;
			break;
		default: // AP took any other action (e.g. Clean, Quarantine, Leave alone)
			bLocalTookAction = true;
			break;
		}
		// If AC_REBOOT_PROCESSING then reboot should be set to true.
		SAVASSERT((AC_REBOOT_PROCESSING != dwActionTaken) 
					  || bLocalReboot);
		// An item can have an action taken of quarantine - AC_MOVE (1) and
		// still have a status of reboot required - set the proper reboot state.
		if (ccEraser::RebootRequired == oViewItem.GetStatus())
		{
			bLocalReboot = true;
		}
	}
	if (pbReboot)
		*pbReboot = bLocalReboot;
	if (pbRemoveRisk)
		*pbRemoveRisk = bLocalRemoveRisk;
	if (pbTookAction)
		*pbTookAction = bLocalTookAction;
	if (pbAnalyzing)
		*pbAnalyzing = bLocalAnalyzing;
}


//////////////////////////////////////////////////
//
// ExcludeCheckeditems
//
// purpose: Create exclusions for all items whose exclude checkbox is checked.
void CResultsView::ExcludeCheckedItems()
{
    // See if any exclude checkboxes were checked.
	CString sItemLine;
	long	lItemPos = 0;

	try
	{
		sItemLine = m_oLDVPResultsCtrl.EnumExcludeCheckedItems(&lItemPos, NULL);
	}
	catch (CAtlException &)
	{
		return;
	}
	if (0 == lItemPos)
		return;

    SAVASSERT( !(m_Flags&RV_FLAGS_ADMIN) );	
    
	// Iterate through checked items.
	while (lItemPos != 0)
	{
        //call back into RTVScan/CliScan to create the exclusion...
        m_pfnExcludeItem(sItemLine, m_Type);
					
		// Get the next item.
		sItemLine = m_oLDVPResultsCtrl.EnumExcludeCheckedItems(&lItemPos, NULL);
	}
}
