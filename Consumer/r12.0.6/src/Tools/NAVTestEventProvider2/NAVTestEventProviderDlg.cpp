// NAVTestEventProviderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NAVTestEventProvider.h"
#include "NAVTestEventProviderDlg.h"

#include "ccSymCommonClientInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNAVTestEventProviderDlg dialog



CNAVTestEventProviderDlg::CNAVTestEventProviderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNAVTestEventProviderDlg::IDD, pParent)
    , m_strTestText(_T(""))
    , m_dwSendDelay(0)
    , m_bLoopForever(FALSE)
    , m_bAllTypes(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNAVTestEventProviderDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EVENTID, m_cboEventType);
    DDX_Text(pDX, IDC_TESTTEXT, m_strTestText);
    DDX_Text(pDX, IDC_SENDDELAY, m_dwSendDelay);
    DDV_MinMaxUInt(pDX, m_dwSendDelay, 0, 99999);
    DDX_Check(pDX, IDC_LOOP, m_bLoopForever);
    DDX_Check(pDX, IDC_ALLTYPES, m_bAllTypes);
    DDX_Control(pDX, IDC_STOP, m_btnStop);
    DDX_Control(pDX, IDC_GO, m_btnGo);
    DDX_Control(pDX, IDC_TESTTEXT, m_edtTestText);
    DDX_Control(pDX, IDC_SENDDELAY, m_edtSendDelay);
    DDX_Control(pDX, IDC_LOOP, m_chkLoop);
    DDX_Control(pDX, IDC_ALLTYPES, m_chkAllTypes);
}

BEGIN_MESSAGE_MAP(CNAVTestEventProviderDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_GO, OnBnClickedGo)
    ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
    ON_BN_CLICKED(IDC_LAUNCHLOGVIEWER, OnBnClickedLaunchlogviewer)
    ON_WM_DESTROY()
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CNAVTestEventProviderDlg message handlers

BOOL CNAVTestEventProviderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

#ifdef _DEBUG
    AfxMessageBox ( "Break here" );
#endif

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_pProvManager = new CProviderManager;
    
    // Start the thread.
    m_pProvManager->Run ();

    // Put the event types into the combo box
    //
    for ( long lIndex = 0;
          lIndex < m_EventDescriptions.m_vecEventTypes.size();
          lIndex++)
    {
        m_cboEventType.AddString ( m_EventDescriptions.m_vecEventTypes[lIndex] );
        m_cboEventType.SetItemData ( lIndex, lIndex );
    }

    m_cboEventType.SetCurSel (0);
    stopUI();
    UpdateData (TRUE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNAVTestEventProviderDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNAVTestEventProviderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CNAVTestEventProviderDlg::OnBnClickedGo()
{
    UpdateData (TRUE);

    if ( m_bAllTypes )
        m_iSendEvent = 0;
    else
        m_iSendEvent = (long) m_cboEventType.GetItemData ( m_cboEventType.GetCurSel ());

    // Disable the UI
    //
    runningUI ();

    // Send the first event
    //
    SendEvent ();

    if ( m_bLoopForever || m_bAllTypes )
    {
        // Start the waiting timer
        //
        SetTimer (1, m_dwSendDelay*1000, 0 );
    }
}

void CNAVTestEventProviderDlg::OnBnClickedStop()
{
    KillTimer (1);
    stopUI();
}

void CNAVTestEventProviderDlg::OnBtnClickedLaunchLogViewer()
{
    // TODO: Add your control notification handler code here
}

void CNAVTestEventProviderDlg::SendEvent(void)
{
    CEventData event;
    CEventDescriptions::MakeEvent ( event, m_iSendEvent, m_strTestText );

    // Put the event into the queue
    m_pProvManager->SendEvent (event);

    if ( !m_bLoopForever && !m_bAllTypes )
    {
        // Unless we are looping, stop here
        OnBnClickedStop();
        return;
    }

    if ( m_bAllTypes )
    {
        m_iSendEvent++;
        if ( m_iSendEvent == m_EventDescriptions.m_vecEventTypes.size())
        {
            if ( m_bLoopForever )
                m_iSendEvent = 0;
            else
            {
                // Unless we are looping, stop here
                OnBnClickedStop();
                return;
            }
        }
    }
}

void CNAVTestEventProviderDlg::stopUI()
{
    // Turn everything back on
    m_btnStop.EnableWindow (FALSE);
    m_btnGo.EnableWindow (TRUE);
    m_cboEventType.EnableWindow (TRUE);
    m_edtTestText.EnableWindow (TRUE);
    m_edtSendDelay.EnableWindow (TRUE);
    m_chkLoop.EnableWindow (TRUE);
    m_chkAllTypes.EnableWindow (TRUE);
}

void CNAVTestEventProviderDlg::runningUI()
{
    // Turn everything off except the Stop button
    m_btnStop.EnableWindow (TRUE);
    m_btnGo.EnableWindow (FALSE);
    m_cboEventType.EnableWindow (FALSE);
    m_edtTestText.EnableWindow (FALSE);
    m_edtSendDelay.EnableWindow (FALSE);
    m_chkLoop.EnableWindow (FALSE);
    m_chkAllTypes.EnableWindow (FALSE);
}

void CNAVTestEventProviderDlg::OnTimer(UINT nIDEvent)
{
    if ( 1 == nIDEvent )
    {
        SendEvent ();
    }
}

void CNAVTestEventProviderDlg::OnDestroy()
{
    CDialog::OnDestroy();

    if ( m_pProvManager )
    {
        // Start the thread.
        m_pProvManager->Stop ();

        delete m_pProvManager;
        m_pProvManager = NULL;
    }
}

void CNAVTestEventProviderDlg::OnBnClickedLaunchlogviewer()
{
    ccLib::CString strPath;

    ccSym::CInstalledApps::GetCCDirectory ( strPath );
    strPath += _T("\\ccLgView.exe");

    ::ShellExecute ( NULL, "open", strPath, NULL, NULL, SW_SHOWNORMAL );
}

