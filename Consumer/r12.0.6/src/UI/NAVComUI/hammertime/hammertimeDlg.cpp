// hammertimeDlg.cpp : implementation file
//

#include "stdafx.h"

//#define INITIIDS
#include "commonuiinterface.h"
#include "InocUIInterface.h"
#include "MemScanUIInterface.h"
//#include "SmtpUIInterface.h"
//#include "SMTPProcess.h"
#include "navscan.h"

#include "hammertime.h"
#include "hammertimeDlg.h"
#include "UISink.h"
#include "Results.h"
#include <process.h>    

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



enum {MODE_SCAN_UI, MODE_MEM_UI, MODE_SMTP_UI, MODE_INNOC_UI};



/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();


// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHammertimeDlg dialog

CHammertimeDlg::CHammertimeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHammertimeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHammertimeDlg)
	m_iRepCount = 1;
	m_iQuarCount = 1;
	m_iDelCount = 1;
	m_iNonDelCount = 1;
	m_iCompRepCount = 1;
	m_iCompQuarCount = 1;
	m_iCompDeleteCount = 1;
	m_iCompNonDeleteCount = 1;
	m_iCompressionDepth = 2;
	m_iNumUI = 1;
	m_szDriveLetter = _T("C");
	m_iDriveNumber = 1;
	m_iInocInfectedBR = 0;
	m_iInocInfectedMBR = 0;
	m_iInocNonInfectedBR = 0;
	m_iInocNonInfectedMBR = 0;
	m_bInocNotifyOnly = FALSE;
	m_iSeconds = 10;
	m_iProgressTime = 0;
	m_bShowProgress = FALSE;
	m_bSetEmailInfo = FALSE;
	m_iHandleTime = 1;
	m_iMessages = 1;
	m_iMBRRep = 1;
	m_iMBRUnRep = 1;
	m_iBRRep = 1;
	m_iBRUnRep = 1;
	m_bRepWizbeforeSMTP = FALSE;
    m_iNonViralThreatNotDeletableCount = 0;
    m_iNonViralThreatCount = 0;
    m_iNonViralThreatCount = 0;
    m_iNonViralThreatNotDeletableCount = 0;
    m_bThreatCatEnabled = TRUE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_iMode = MODE_SCAN_UI;
	m_strIniFile = new char[MAX_PATH + 15];
	if(m_strIniFile)
	{
		GetCurrentDirectory(MAX_PATH,  m_strIniFile);
		_tcsncat(m_strIniFile, "\\hammertime.ini", 15); 
	}
	else
		throw runtime_error("Memory Allocation Failure");

	bCommandLineMode = false;
}

CHammertimeDlg::~CHammertimeDlg()
{
	if(m_strIniFile)
		delete[] m_strIniFile;
}

void CHammertimeDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CHammertimeDlg)
    DDX_Text(pDX, IDC_EDIT1, m_iRepCount);
    DDV_MinMaxInt(pDX, m_iRepCount, 0, 999999);
    DDX_Text(pDX, IDC_EDIT2, m_iQuarCount);
    DDX_Text(pDX, IDC_EDIT3, m_iDelCount);
    DDX_Text(pDX, IDC_EDIT4, m_iNonDelCount);
    DDX_Text(pDX, IDC_EDIT5, m_iCompRepCount);
    DDX_Text(pDX, IDC_EDIT6, m_iCompQuarCount);
    DDX_Text(pDX, IDC_EDIT7, m_iCompDeleteCount);
    DDX_Text(pDX, IDC_EDIT8, m_iCompNonDeleteCount);
    DDX_Text(pDX, IDC_EDIT9, m_iCompressionDepth);
    DDV_MinMaxInt(pDX, m_iCompressionDepth, 2, 10);
    DDX_Text(pDX, IDC_EDIT10, m_iNumUI);
    DDV_MinMaxInt(pDX, m_iNumUI, 1, 1000);
    DDX_Text(pDX, IDC_EDIT11, m_szDriveLetter);
    DDV_MaxChars(pDX, m_szDriveLetter, 1);
    DDX_Text(pDX, IDC_EDIT12, m_iDriveNumber);
    DDX_Check(pDX, IDC_CHECK1, m_bInocNotifyOnly);
    DDX_Text(pDX, IDC_EDIT13, m_iSeconds);
    DDV_MinMaxInt(pDX, m_iSeconds, 1, 10000);
    DDX_Text(pDX, IDC_EDIT14, m_iProgressTime);
    DDV_MinMaxInt(pDX, m_iProgressTime, 0, 10000000);
    DDX_Check(pDX, IDC_CHECK2, m_bShowProgress);
    DDX_Check(pDX, IDC_CHECK3, m_bSetEmailInfo);
    DDX_Text(pDX, IDC_EDIT15, m_iHandleTime);
    DDV_MinMaxInt(pDX, m_iHandleTime, 0, 10);
    DDX_Text(pDX, IDC_EDIT16, m_iMessages);
    DDV_MinMaxInt(pDX, m_iMessages, 1, 100000);
    DDX_Text(pDX, IDC_EDIT17, m_iMBRRep);
    DDV_MinMaxInt(pDX, m_iMBRRep, 0, 50);
    DDX_Text(pDX, IDC_EDIT18, m_iMBRUnRep);
    DDV_MinMaxInt(pDX, m_iMBRUnRep, 0, 50);
    DDX_Text(pDX, IDC_EDIT19, m_iBRRep);
    DDV_MinMaxInt(pDX, m_iBRRep, 0, 50);
    DDX_Text(pDX, IDC_EDIT20, m_iBRUnRep);
    DDV_MinMaxInt(pDX, m_iBRUnRep, 0, 50);
    DDX_Check(pDX, IDC_CHECK4, m_bRepWizbeforeSMTP);
    DDX_Text(pDX, IDC_EDIT21, m_iNonViralThreatCount);
    DDX_Text(pDX, IDC_EDIT22, m_iNonViralThreatNotDeletableCount);
    DDX_Check(pDX, IDC_CHECK5, m_bThreatCatEnabled);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHammertimeDlg, CDialog)
	//{{AFX_MSG_MAP(CHammertimeDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_COMMAND(ID_UIDISPLAY_SMTPEMAILSENDINGUI, OnUidisplaySmtpemailsendingui)
	ON_COMMAND(ID_UIDISPLAY_MEMORYINFECTION, OnUidisplayMemoryinfection)
	ON_COMMAND(ID_UIDISPLAY_INNOCULATIONUI, OnUidisplayInnoculationui)
	ON_COMMAND(ID_UIDISPLAY_COMMONUISCANNING, OnUidisplayCommonuiscanning)
	ON_COMMAND(ID_FILE_QUIT, OnFileQuit)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHammertimeDlg message handlers

BOOL CHammertimeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	SetUI(m_iMode);	
	LoadSettings();

	// Also check the correct radio button on the Innoculation UI
	if(m_iInocInfectedBR)
	{
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(1);
		OnRadio1();
	}
	else if(m_iInocInfectedMBR)
	{
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(1);
		OnRadio2();
	}
	else if(m_iInocNonInfectedBR)
	{
		((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(1);
		OnRadio3();
	}
	else if(m_iInocNonInfectedMBR)
	{
		((CButton*)GetDlgItem(IDC_RADIO4))->SetCheck(1);
		OnRadio4();
	}

	CheckCommandLine();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHammertimeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHammertimeDlg::OnPaint() 
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHammertimeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CHammertimeDlg::OnStart() 
{
	// Fetch data from controls.
	UpdateData();

	if(bCommandLineMode) // Only Bring up One UI if we are in command line mode
	{
		ThreadDisplayUI((LPVOID)this);
	}
	else
	{
		for(int i =0; i < m_iNumUI; i++)
		{
			unsigned uID;
			HANDLE hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadDisplayUI, this, 0, &uID);
		}
	}	
}

unsigned __stdcall CHammertimeDlg::ThreadDisplayUI( LPVOID pParam )
{
	CHammertimeDlg* pThis = (CHammertimeDlg*) pParam;
	ICommonUI* pCommonUI = NULL;
	CoInitialize(NULL);


	// Handle the Scan UI
	if(pThis->m_iMode == MODE_SCAN_UI)
	{
        CResults* results = NULL;
		// Create some results.
        if( pThis->m_bThreatCatEnabled )
        {
		    results = new CResults(pThis->m_iRepCount, pThis->m_iQuarCount, pThis->m_iDelCount, pThis->m_iNonDelCount,
						    pThis->m_iCompRepCount, pThis->m_iCompQuarCount, pThis->m_iCompDeleteCount, 
						    pThis->m_iCompNonDeleteCount, pThis->m_iCompressionDepth, pThis->m_iMBRRep, pThis->m_iMBRUnRep, pThis->m_iBRRep, pThis->m_iBRUnRep,
                            pThis->m_iNonViralThreatCount, pThis->m_iNonViralThreatNotDeletableCount);
        }
        else
        {
            results = new CResults(pThis->m_iRepCount, pThis->m_iQuarCount, pThis->m_iDelCount, pThis->m_iNonDelCount,
						    pThis->m_iCompRepCount, pThis->m_iCompQuarCount, pThis->m_iCompDeleteCount, 
						    pThis->m_iCompNonDeleteCount, pThis->m_iCompressionDepth, pThis->m_iMBRRep, pThis->m_iMBRUnRep, pThis->m_iBRRep, pThis->m_iBRUnRep,
                            /*NonViralThreat Deleteable count =*/0, /*NonViralThreat non-deleteable count =*/ 0);
        }
        if(!results)
	        throw runtime_error("Memory allocation failure");
		results->Initialize();
		
		// Create our sink object.
		CUISink *sink = new CUISink( *results, pThis->m_iHandleTime );
		if(!sink)
			throw runtime_error("Memory allocation failure");
		
		// Finally, Create the Common UI stuff, giving it our bogus sink and results
		// objects.
		CreateCommonUI( pThis->GetSafeHwnd(), sink, &pCommonUI ); 

        // Turn on/off threat categorization
        pCommonUI->SetScanNonViralThreats((pThis->m_bThreatCatEnabled ? true : false));

        // Turn on gse scanning for non-email scans
		if(pThis->m_bSetEmailInfo)
			pCommonUI->SetEmailInfo("Infected Subject", "Infected Sender", "Infected Recipient");
        else
            pCommonUI->SetSideEffectsEnabled(true);

		if(pThis->m_bShowProgress)
		{
			ThreadParams* params = new ThreadParams;
			params->pCommonUI = pCommonUI;
			params->iProgressTime = pThis->m_iProgressTime;
			params->pResults = results;
			params->pUISink = sink;
			AfxBeginThread(ThreadProgress, (LPVOID)params);

			pCommonUI->ShowCUIDlg(CommonUI_Complete, false, NULL );
		}


		// No progress
		else
			pCommonUI->ShowCUIDlg(CommonUI_RepairAndSummary, false, results );
	
		if(sink)
			delete sink;
        if(results)
            delete results;
		pCommonUI->DeleteObject();
	}


	// Handle the Innoculation UI
	else if(pThis->m_iMode == MODE_INNOC_UI)
	{
		
		IInocUIInterface* pInocUI = NULL;
		if (CreateInocUI(&pInocUI) == true)
		{
			if(pThis->m_iInocInfectedBR)
			{
				pInocUI->DisplayBootRecInfectionDlg((pThis->m_szDriveLetter)[0], "Your BR is Toast");
				pInocUI->DeleteObject();
			}
			else if(pThis->m_iInocInfectedMBR)
			{
				pInocUI->DisplayMasterBootRecInfectionDlg(pThis->m_iDriveNumber, "Your MBR is Toast");
				pInocUI->DeleteObject();
			}


			else if(pThis->m_iInocNonInfectedBR || pThis->m_iInocNonInfectedMBR)
			{

				InocType Type;
				InocAction Action = InocAction_Ignore;
				
				if (pThis->m_iInocNonInfectedBR)
				{
					Type = InocType_BootRecord;
				}
				else
				{
					Type = InocType_MasterBootRecord;
				}
                pInocUI->DisplayInocActionDlg(Type, &Action, (pThis->m_bInocNotifyOnly ? true : false));
				pInocUI->DeleteObject();
			}

			else
			{
				AfxMessageBox("You must select a UI type for Inculation");
			}
		}
	}


	/* Handle the SMTP UI
	else if(pThis->m_iMode == MODE_SMTP_UI)
	{

		// Show the Repair Wizard part first if option is set
		if(pThis->m_bRepWizbeforeSMTP)
		{

			// Create some results.
			CResults results(pThis->m_iRepCount, pThis->m_iQuarCount, pThis->m_iDelCount, pThis->m_iNonDelCount,
							 pThis->m_iCompRepCount, pThis->m_iCompQuarCount, pThis->m_iCompDeleteCount, 
							 pThis->m_iCompNonDeleteCount, pThis->m_iCompressionDepth, pThis->m_iMBRRep, pThis->m_iMBRUnRep, pThis->m_iBRRep, pThis->m_iBRUnRep );
			results.Initialize();
			
			// Create our sink object.
			CUISink sink( results, pThis->m_iHandleTime );
			
			// Finally, Create the Common UI stuff, giving it our bogus sink and results
			// objects.
			CreateCommonUI( pThis->GetSafeHwnd(), &sink, &pCommonUI ); 

			// Set Email info
			pCommonUI->SetEmailInfo("Infected Subject", "Infected Sender", "Infected Recipient");

			// No progress
			pCommonUI->ShowCUIDlg(CommonUI_RepairAndSummary, false, &results );
		
			pCommonUI->DeleteObject();
		}

	
		// Now do the SMTP UI
		SMTPProcess object(pThis->m_iSeconds, pThis->m_iMessages);
		object.Process();
	}*/

	// Handle the Memory infection UI
	else
	{
		IMemScanUIInterface* pMemScanUI = NULL;
		if (CreateMemScanUI(&pMemScanUI) == true)
		{
			pMemScanUI->DisplayMemoryInfectionDlg();
			pMemScanUI->DeleteObject();
		}
	}

	return 1;
}


void CHammertimeDlg::OnUidisplaySmtpemailsendingui() 
{
	m_iMode = MODE_SMTP_UI;
	SetUI(m_iMode);	
}

void CHammertimeDlg::OnUidisplayMemoryinfection() 
{
	m_iMode = MODE_MEM_UI;
	SetUI(m_iMode);	
}

void CHammertimeDlg::OnUidisplayInnoculationui() 
{
	m_iMode = MODE_INNOC_UI;
	SetUI(m_iMode);
}

void CHammertimeDlg::OnUidisplayCommonuiscanning() 
{
	m_iMode = MODE_SCAN_UI;
	SetUI(m_iMode);	
}

void CHammertimeDlg::OnFileQuit() 
{
	PostMessage(WM_QUIT);
	
}

void CHammertimeDlg::SetUI(int setting)
{
	// Code to Set the Check Next to the Menu Item
	CWnd* pMain = AfxGetMainWnd();
	CMenu* pMenu;

	// The main window _can_ be NULL, so this code
	// doesn't ASSERT and actually tests.
	if (pMain != NULL)
	{
		// Get the main window's menu
		pMenu = pMain->GetMenu();
		if(!pMenu)
			throw runtime_error("Error in Set UI setting the Menu");
	}


	if(setting==MODE_SCAN_UI)
	{
		GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT2)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT3)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT4)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT5)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT6)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT7)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT8)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT9)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT14)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT15)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK3)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT17)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT18)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT19)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT20)->EnableWindow(TRUE);
		pMenu->CheckMenuItem(ID_UIDISPLAY_COMMONUISCANNING, MF_CHECKED);
	}

	else
	{
		GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT3)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT5)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT6)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT7)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT8)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT9)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT14)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT15)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK3)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT17)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT18)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT19)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT20)->EnableWindow(FALSE);
		pMenu->CheckMenuItem(ID_UIDISPLAY_COMMONUISCANNING, MF_UNCHECKED);
	}


	if(setting ==MODE_INNOC_UI)
	{
		GetDlgItem(IDC_EDIT11)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT12)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO3)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK1)->EnableWindow(TRUE);
		pMenu->CheckMenuItem(ID_UIDISPLAY_INNOCULATIONUI, MF_CHECKED);
	}
	else
	{
		GetDlgItem(IDC_EDIT11)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT12)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO3)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK1)->EnableWindow(FALSE);
		pMenu->CheckMenuItem(ID_UIDISPLAY_INNOCULATIONUI, MF_UNCHECKED);
	}

	if(setting==MODE_SMTP_UI)
	{
		GetDlgItem(IDC_EDIT13)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT16)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK4)->EnableWindow(TRUE);
		pMenu->CheckMenuItem(ID_UIDISPLAY_SMTPEMAILSENDINGUI, MF_CHECKED);
	}
	else
	{
		GetDlgItem(IDC_EDIT13)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT16)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK4)->EnableWindow(FALSE);
		pMenu->CheckMenuItem(ID_UIDISPLAY_SMTPEMAILSENDINGUI, MF_UNCHECKED);
	}

	if(setting==MODE_MEM_UI)
		pMenu->CheckMenuItem(ID_UIDISPLAY_MEMORYINFECTION, MF_CHECKED);
	else
		pMenu->CheckMenuItem(ID_UIDISPLAY_MEMORYINFECTION, MF_UNCHECKED);

}

void CHammertimeDlg::OnRadio1() 
{
	m_iInocInfectedBR = 1;
	m_iInocInfectedMBR = 0;
	m_iInocNonInfectedBR = 0;
	m_iInocNonInfectedMBR = 0;
}

void CHammertimeDlg::OnRadio2() 
{
	m_iInocInfectedBR = 0;
	m_iInocInfectedMBR = 1;
	m_iInocNonInfectedBR = 0;
	m_iInocNonInfectedMBR = 0;	
}

void CHammertimeDlg::OnRadio3() 
{
	m_iInocInfectedBR = 0;
	m_iInocInfectedMBR = 0;
	m_iInocNonInfectedBR = 1;
	m_iInocNonInfectedMBR = 0;	
}

void CHammertimeDlg::OnRadio4() 
{
	m_iInocInfectedBR = 0;
	m_iInocInfectedMBR = 0;
	m_iInocNonInfectedBR = 0;
	m_iInocNonInfectedMBR = 1;
}

UINT ThreadProgress(LPVOID pParam)
{
	ThreadParams* params = (ThreadParams*)pParam;
	CoInitialize(NULL);
	unsigned long scanned = 0;
	unsigned long infected = 0;
	unsigned long repaired = 0;
    unsigned long deleted = 0;
	for(int i =0; i < params->iProgressTime; i++)
	{
		scanned++;
		Sleep(1000);
		if(params->pUISink->m_bAbort)
			break;
		//params->pCommonUI->SetCurrentStatus("C:\\MR.T\\Rocks", scanned, infected, repaired, deleted);
	}

	params->pCommonUI->SetScanComplete(params->pResults);
	delete pParam;
	return 1;
}

void CHammertimeDlg::SaveSettings() 
{
	UpdateData(TRUE);
	CString tempval;


	// General Options
	tempval.Empty();
	tempval.Format("%d", m_iNumUI);
	WritePrivateProfileString("General", "Number of UI", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iMode);
	WritePrivateProfileString("General", "UI Mode", tempval, m_strIniFile);

	// Repair Wizard options
	tempval.Empty();
	tempval.Format("%d", m_iRepCount);
	WritePrivateProfileString("Scanner", "File Repairable Count", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iQuarCount);
	WritePrivateProfileString("Scanner", "File Quarantinable Count", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iDelCount);
	WritePrivateProfileString("Scanner", "File Deletable Count", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iNonDelCount);
	WritePrivateProfileString("Scanner", "File Non-Deleteable Count", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iCompRepCount);	
	WritePrivateProfileString("Scanner", "Compressed File Repairable Count", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iCompQuarCount);	
	WritePrivateProfileString("Scanner", "Compressed File Quarantinable Count", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iCompDeleteCount);	
	WritePrivateProfileString("Scanner", "Compressed File Deleteable Count", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iCompNonDeleteCount);	
	WritePrivateProfileString("Scanner", "Compressed File Non-Deletable Count", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iMBRRep);	
	WritePrivateProfileString("Scanner", "MBR Repairable Count", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iMBRUnRep);	
	WritePrivateProfileString("Scanner", "MBR UnRepairable Count", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iBRRep);	
	WritePrivateProfileString("Scanner", "BR Repairable Count", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iBRUnRep);	
	WritePrivateProfileString("Scanner", "BR UnRepairable Count", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iProgressTime);	
	WritePrivateProfileString("Scanner", "Scan Progress Time", tempval, m_strIniFile);
	tempval.Empty();
	if(m_bShowProgress)
		tempval = "1";
	else
		tempval = "0";
	WritePrivateProfileString("Scanner", "Show Progress", tempval, m_strIniFile);
	tempval.Empty();
	if(m_bSetEmailInfo)
		tempval = "1";
	else
		tempval = "0";
	WritePrivateProfileString("Scanner", "Set Email Info", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iHandleTime);
	WritePrivateProfileString("Scanner", "Handle Time", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iCompressionDepth);	
	WritePrivateProfileString("Scanner", "Compression Depth", tempval, m_strIniFile);

	// Inoculation Options

	WritePrivateProfileString("Inoculation", "Drive Letter", m_szDriveLetter, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iDriveNumber);	
	WritePrivateProfileString("Inoculation", "Drive Number", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iInocInfectedBR);	
	WritePrivateProfileString("Inoculation", "Infected BR", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iInocInfectedMBR);	
	WritePrivateProfileString("Inoculation", "Infected MBR", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iInocNonInfectedBR);	
	WritePrivateProfileString("Inoculation", "NonInfected BR", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iInocNonInfectedMBR);
	WritePrivateProfileString("Inoculation", "NonInfected MBR", tempval, m_strIniFile);
	tempval.Empty();
	if(m_bInocNotifyOnly)
		tempval = "1";
	else
		tempval = "0";
	WritePrivateProfileString("Inoculation", "Notify Only On Infection", tempval, m_strIniFile);


	// SMTP UI options
	tempval.Empty();
	tempval.Format("%d", m_iSeconds);	
	WritePrivateProfileString("SMTP", "Seconds Per Message", tempval, m_strIniFile);
	tempval.Empty();
	tempval.Format("%d", m_iMessages);	
	WritePrivateProfileString("SMTP", "Messages", tempval, m_strIniFile);
	tempval.Empty();
	if(m_bRepWizbeforeSMTP)
		tempval = "1";
	else
		tempval = "0";
	WritePrivateProfileString("SMTP", "Show Repair Wizard Before SMTP", tempval, m_strIniFile);
}

void CHammertimeDlg::LoadSettings()
{	
	char buffer[MAX_PATH];
	int temp;
	
	// General Options
	m_iNumUI = GetPrivateProfileInt("General", "Number of UI", m_iNumUI, m_strIniFile);
	m_iMode = GetPrivateProfileInt("General", "UI Mode", m_iMode, m_strIniFile);

	// Repair Wizard options
	m_iRepCount = GetPrivateProfileInt("Scanner", "File Repairable Count", m_iRepCount, m_strIniFile);
	m_iQuarCount = GetPrivateProfileInt("Scanner", "File Quarantinable Count", m_iQuarCount, m_strIniFile);
	m_iDelCount = GetPrivateProfileInt("Scanner", "File Deletable Count", m_iDelCount, m_strIniFile);
	m_iNonDelCount = GetPrivateProfileInt("Scanner", "File Non-Deleteable Count", m_iNonDelCount, m_strIniFile);
	m_iCompRepCount = GetPrivateProfileInt("Scanner", "Compressed File Repairable Count", m_iCompRepCount, m_strIniFile);
	m_iCompQuarCount = GetPrivateProfileInt("Scanner", "Compressed File Quarantinable Count", m_iCompQuarCount, m_strIniFile);
	m_iCompDeleteCount = GetPrivateProfileInt("Scanner", "Compressed File Deleteable Count", m_iCompDeleteCount, m_strIniFile);
	m_iCompNonDeleteCount = GetPrivateProfileInt("Scanner", "Compressed File Non-Deletable Count", m_iCompNonDeleteCount, m_strIniFile);
	m_iMBRRep = GetPrivateProfileInt("Scanner", "MBR Repairable Count", m_iMBRRep, m_strIniFile);
	m_iMBRUnRep = GetPrivateProfileInt("Scanner", "MBR UnRepairable Count", m_iMBRUnRep, m_strIniFile);
	m_iBRRep = GetPrivateProfileInt("Scanner", "BR Repairable Count", m_iBRRep, m_strIniFile);
	m_iBRUnRep = GetPrivateProfileInt("Scanner", "BR UnRepairable Count", m_iBRUnRep, m_strIniFile);
	m_iProgressTime = GetPrivateProfileInt("Scanner", "Scan Progress Time", m_iProgressTime, m_strIniFile);
	temp = GetPrivateProfileInt("Scanner", "Show Progress", 0, m_strIniFile);
	if(temp)
		m_bShowProgress = TRUE;
	else
		m_bShowProgress = FALSE;
	temp = GetPrivateProfileInt("Scanner", "Set Email Info", 0, m_strIniFile);
	if(temp)
		m_bSetEmailInfo = TRUE;
	else
		m_bSetEmailInfo = FALSE;
	m_iHandleTime = GetPrivateProfileInt("Scanner", "Handle Time", m_iHandleTime, m_strIniFile);
	m_iCompressionDepth = GetPrivateProfileInt("Scanner", "Compression Depth", m_iCompressionDepth, m_strIniFile);

	// Inoculation Options

	GetPrivateProfileString("Inoculation", "Drive Letter", m_szDriveLetter, buffer, MAX_PATH, m_strIniFile);
	m_szDriveLetter = buffer;
	m_iDriveNumber = GetPrivateProfileInt("Inoculation", "Drive Number", m_iDriveNumber, m_strIniFile);
	m_iInocInfectedBR = GetPrivateProfileInt("Inoculation", "Infected BR", m_iInocInfectedBR, m_strIniFile);
	m_iInocInfectedMBR = GetPrivateProfileInt("Inoculation", "Infected MBR", m_iInocInfectedMBR, m_strIniFile);
	m_iInocNonInfectedBR = GetPrivateProfileInt("Inoculation", "NonInfected BR", m_iInocNonInfectedBR, m_strIniFile);
	m_iInocNonInfectedMBR = GetPrivateProfileInt("Inoculation", "NonInfected MBR", m_iInocNonInfectedMBR, m_strIniFile);
	temp = GetPrivateProfileInt("Inoculation", "Notify Only On Infection", 0, m_strIniFile);
	if(temp)
		m_bInocNotifyOnly = TRUE;
	else
		m_bInocNotifyOnly = FALSE;

	// SMTP UI options
	m_iSeconds = GetPrivateProfileInt("SMTP", "Seconds Per Message", m_iSeconds, m_strIniFile);
	m_iMessages = GetPrivateProfileInt("SMTP", "Messages", m_iMessages, m_strIniFile);
	m_bRepWizbeforeSMTP = GetPrivateProfileInt("SMTP", "Show Repair Wizard Before SMTP", m_bRepWizbeforeSMTP, m_strIniFile);

    // NonViral Threat options
    m_iNonViralThreatCount = 1;
    m_iNonViralThreatNotDeletableCount = 1;

	UpdateData(FALSE);
	SetUI(m_iMode);
}

// Button to Save the settings out to an INI file
void CHammertimeDlg::OnButton1() 
{
	SaveSettings();	
}


// Function to Get the command Line parameters, parse them and run the desired action 
void CHammertimeDlg::CheckCommandLine()
{
	// no command Line Parameters were given, so just return
	if(__argc < 2)
	{
		return;
	}

	// We have parameters, so lets party
	else
	{
		bCommandLineMode = true;
		for (int i=1; i<__argc; i++)
		{
			// Handle the "/help" Argument
			if(0 == _tcsicmp(__argv[i], "/help"))
			{
				AfxMessageBox("Help\nAvailable Command Line options\n/stress - Exits after bringing up settings from ini file\n/pat - A cool quote from a cool guy");
			}

			// Handle the "/pat" Argument
			else if(0 == _tcsicmp(__argv[i], "/pat"))
			{
				AfxMessageBox("All hail Lord Helmet!");
			}

			// Handle the "/stress" Argument
			else if(0 ==( _tcsicmp(__argv[i], "/stress")))
			{
				OnStart();
			}
			else
			{
				CString temp;
				temp.Format("Invalid Argument \"%s\".\nTry /help for more info", __argv[i]);
				AfxMessageBox(temp);
			}
		}

		// Since the Command line is for Automation, exit when we are done
		Sleep(1000);
		EndDialog(1);
	}
}

