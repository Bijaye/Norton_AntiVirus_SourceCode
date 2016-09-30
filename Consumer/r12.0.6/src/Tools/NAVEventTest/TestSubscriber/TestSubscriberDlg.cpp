#include "StdAfx.h"

#include "TestSubscriber.h"
#include "TestSubscriberDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTestSubscriberDlg::CTestSubscriberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestSubscriberDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestSubscriberDlg)
	m_nSubscriberId = 0;
	m_nPriority = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestSubscriberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestSubscriberDlg)
	DDX_Control(pDX, IDC_EVENT_LIST, m_EventList);
	DDX_Text(pDX, IDC_SUBSCRIBERID_EDIT, m_nSubscriberId);
	DDX_Text(pDX, IDC_PRIORITY_EDIT, m_nPriority);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestSubscriberDlg, CDialog)
	//{{AFX_MSG_MAP(CTestSubscriberDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDC_CONNECT_BUTTON, OnConnectButton)
	ON_BN_CLICKED(IDC_REGISTER_BUTTON, OnRegisterButton)
	ON_BN_CLICKED(IDC_UNREGISTER_BUTTON, OnUnregisterButton)
	ON_BN_CLICKED(IDC_COMMIT_BUTTON, OnCommitButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	//}}AFX_MSG_MAP
    ON_MESSAGE(UM_EVENT, OnEvent)
END_MESSAGE_MAP()

BOOL CTestSubscriberDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
    m_EventList.InsertColumn(EventTypeColumn, _T("Type"));
    ResizeColumn(EventTypeColumn, _T("Type"));
    m_EventList.InsertColumn(EventIdColumn, _T("Id"));
    ResizeColumn(EventIdColumn, _T("Id"));
    m_EventList.InsertColumn(BroadcastColumn, _T("Broadcast"));
    ResizeColumn(BroadcastColumn, _T("Broadcast"));
    m_EventList.InsertColumn(CancelColumn, _T("Cancel"));
    ResizeColumn(CancelColumn, _T("Cancel"));
    m_EventList.InsertColumn(UserNameColumn, _T("User Name"));
    ResizeColumn(UserNameColumn, _T("User Name"));
    m_EventList.InsertColumn(ComputerNameColumn, _T("Computer Name"));
    ResizeColumn(ComputerNameColumn, _T("Computer Name"));
	
    GetDlgItem(IDC_UNREGISTER_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_REGISTER_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(TRUE);
    GetDlgItem(IDC_COMMIT_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_CANCEL_BUTTON)->EnableWindow(FALSE);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestSubscriberDlg::OnPaint() 
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

HCURSOR CTestSubscriberDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTestSubscriberDlg::OnOk() 
{
	// Do nothing
}

void CTestSubscriberDlg::OnCancel() 
{
	m_Subscriber.Disconnect();
	
    if (m_pEvent != NULL)
    {
        m_Subscriber.DeleteEvent(m_pEvent);
        m_pEvent = NULL;
    }

    CDialog::OnCancel();
}

void CTestSubscriberDlg::OnCommitButton() 
{
	if (m_pEvent == NULL)
    {
        return;
    }

    if (m_Subscriber.CommitEvent(m_pEvent) == FALSE)
    {
        MessageBox(_T("Failed to commit event"));
    }
	
    m_Subscriber.DeleteEvent(m_pEvent);
    m_pEvent = NULL;

    GetDlgItem(IDC_COMMIT_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_CANCEL_BUTTON)->EnableWindow(FALSE);
}

void CTestSubscriberDlg::OnCancelButton() 
{
	if (m_pEvent == NULL)
    {
        return;
    }

    if (m_Subscriber.CancelEvent(m_pEvent) == FALSE)
    {
        MessageBox(_T("Failed to cancel event"));
    }
	
    m_Subscriber.DeleteEvent(m_pEvent);
    m_pEvent = NULL;

    GetDlgItem(IDC_COMMIT_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_CANCEL_BUTTON)->EnableWindow(FALSE);
}

void CTestSubscriberDlg::ResizeColumn(int nColumn, LPCTSTR szString)
{
    int nStringWidth = m_EventList.GetStringWidth(szString);
    nStringWidth += 16;
    int nColumnWidth = m_EventList.GetColumnWidth(nColumn);
    if (nColumnWidth < nStringWidth)
    {
        m_EventList.SetColumnWidth(nColumn, nStringWidth);
    }
}

LRESULT CTestSubscriberDlg::OnEvent(WPARAM wParam, LPARAM lParam)
{
    //m_pEvent = reinterpret_cast<CNAVEventCommon*>(lParam);
    m_pEvent = (CEventEx*)lParam;

    if (m_pEvent != NULL)
    {    
        int nItem = m_EventList.GetItemCount();
        nItem = m_EventList.InsertItem(nItem + 1, _T(""));
        CString sString;
        sString.Format(_T("%ld"), m_pEvent->GetType());
        m_EventList.SetItemText(nItem, EventTypeColumn, sString);
        ResizeColumn(EventTypeColumn, sString);

        sString.Format(_T("%ld"), m_pEvent->GetEventId());
        m_EventList.SetItemText(nItem, EventIdColumn, sString);
        ResizeColumn(EventIdColumn, sString);

        sString.Format(_T("%ld"), m_pEvent->GetBroadcast());
        m_EventList.SetItemText(nItem, BroadcastColumn, sString);
        ResizeColumn(BroadcastColumn, sString);
        
        sString.Format(_T("%ld"), m_pEvent->GetCancelled());
        m_EventList.SetItemText(nItem, CancelColumn, sString);
        ResizeColumn(CancelColumn, sString);
        
        CNAVEventCommon* pNAVEvent = static_cast<CNAVEventCommon*>(m_pEvent);

        const char* szValue;
        long lBufSize = 0;
        pNAVEvent->props.GetData(AV_Event_Base_propUserName, &szValue, &lBufSize );

        sString.Format(_T("%s"), szValue );
        m_EventList.SetItemText(nItem, UserNameColumn, sString);
        ResizeColumn(UserNameColumn, sString);

        pNAVEvent->props.GetData(AV_Event_Base_propComputerName, &szValue, &lBufSize );

        sString.Format(_T("%s"), szValue );
        m_EventList.SetItemText(nItem, ComputerNameColumn, sString);
        ResizeColumn(ComputerNameColumn, sString);

        // Only allow further processing if this event is 
        // not cancelled or broadcast
        if (m_pEvent->GetCancelled() == false &&
            m_pEvent->GetBroadcast() == false)
        {
            GetDlgItem(IDC_COMMIT_BUTTON)->EnableWindow(TRUE);
            GetDlgItem(IDC_CANCEL_BUTTON)->EnableWindow(TRUE);
        }
        else
        {
            m_Subscriber.DeleteEvent(m_pEvent);
        }
    }

    return 0;
}

void CTestSubscriberDlg::OnRegisterButton() 
{
    UpdateData(TRUE);

    if (m_Subscriber.Register(m_hWnd, 
                              m_nPriority,
                              AV_Event_ID_VirusLike,
                              m_nSubscriberId) == FALSE ||
        m_Subscriber.Register(m_hWnd, 
                              m_nPriority,
                              AV_Event_ID_SBAction,
                              m_nSubscriberId) == FALSE  ||
        m_Subscriber.Register(m_hWnd, 
                              m_nPriority,
                              AV_Event_ID_ScanAction,
                              m_nSubscriberId) == FALSE  ||
        m_Subscriber.Register(m_hWnd, 
                              m_nPriority,
                              AV_Event_ID_VirusAlert,
                              m_nSubscriberId) == FALSE )
    {
        MessageBox(_T("Failed to register subscriber"));
    }
    else
    {
        GetDlgItem(IDC_UNREGISTER_BUTTON)->EnableWindow(TRUE);
        GetDlgItem(IDC_REGISTER_BUTTON)->EnableWindow(FALSE);
        GetDlgItem(IDC_PRIORITY_EDIT)->EnableWindow(FALSE);
    }

    UpdateData(FALSE);
}

void CTestSubscriberDlg::OnUnregisterButton() 
{
    UpdateData(TRUE);

    m_Subscriber.Unregister();

    m_nSubscriberId = 0;

    GetDlgItem(IDC_UNREGISTER_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_REGISTER_BUTTON)->EnableWindow(TRUE);

    UpdateData(FALSE);
}

void CTestSubscriberDlg::OnConnectButton() 
{
	if (m_Subscriber.Connect() == FALSE)
    {
        MessageBox(_T("Failed to connect to Event Manager"));
        return;
    }
    GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_REGISTER_BUTTON)->EnableWindow(TRUE);
}