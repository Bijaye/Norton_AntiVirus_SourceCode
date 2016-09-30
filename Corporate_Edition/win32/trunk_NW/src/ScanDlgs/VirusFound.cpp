// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// VirusFound.cpp : implementation file
//

#include "stdafx.h"
#include "scandlgs.h"
#include "VirusFound.h"
#include "user.h"
#include "OSUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVirusFound dialog


CVirusFound::CVirusFound(CWnd* pParent /*=NULL*/)
	: CDialog(CVirusFound::IDD, pParent),
	Head(NULL),CurrentVirus(NULL),m_Parent(0),hKey(0),m_bInitialized(FALSE)
{
	//{{AFX_DATA_INIT(CVirusFound)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CVirusFound::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVirusFound)
	DDX_Control(pDX, IDC_TEXT, m_Text);
	DDX_Control(pDX, IDC_KENS_DETAILS, m_KenD);
	//}}AFX_DATA_MAP
}

#define UWM_ADD WM_USER+1
#define UWM_SET WM_USER+2

BEGIN_MESSAGE_MAP(CVirusFound, CDialog)
	//{{AFX_MSG_MAP(CVirusFound)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_DETAILS, OnDetails)
	ON_BN_CLICKED(IDC_PREV, OnPrev)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_DESTROY()
	ON_MESSAGE(UWM_ADD,AddItem)
	ON_MESSAGE(UWM_SET,Set)
	//}}AFX_MSG_MAP
//	ON_NOTIFY_EX(TTN_NEEDTEXT,0,OnTTT)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnTTT)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnTTT)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVirusFound message handlers

void VFViewThread(CVirusFound *pThis)
{
	CoInitialize(NULL);
	AfxEnableControlContainer();

	pThis->DoModal();

	Sleep(128);

	// KAP - added code to handle deadlock condition -
	// KAP - 1-A370J, 1-CBVAJ - 06/26/2002
	//	Note:  This issue 
	//		if the caller attmempts to queue a message, while this dialog is exiting, deadlocks could occur.
	//		originally, the dialog message handler could hang invoking VirusFound::OnDestroy, which invoked ViewClosed(), resulting
	//		in a deadlock.  This happens because the Mutex in the ViewClosed() function is locked (which is happening) by another
	//		thread calling VirusFound::AddVirus().  The code in AddVirus() calls SendMessage(..) to the message handler which is blocked in
	//		OnDestroy().
	{
		// tell the ViewClosed(), calling threads, we are no longer here to receive messages.
		if( pThis->m_ViewClosed)
			pThis->m_ViewClosed(pThis->Context);

	}

	delete pThis;

	CoUninitialize();
}


DWORD CVirusFound::Open(PVIRUSFOUNDDLG VirusFound)
{

	DWORD cc = RV_SUCCESS;

	m_bInitialized = FALSE;

	if (!VirusFound || ( VirusFound->Size != sizeof(RESULTSVIEW)) )
		return RV_ERROR_BAD_SIZE;
	
	m_Flags          = VirusFound->Flags;
	m_Parent         = VirusFound->hWndParent;
	m_Modeless       = FALSE;
	m_ViewClosed     = VirusFound->ViewClosed;
	Context          = VirusFound->Context;
	hKey             = NULL;

	RegOpenKeyEx(VirusFound->hKey,NULL,0,KEY_ALL_ACCESS,&hKey);
	
	if (VirusFound->Title)
		m_Title=VirusFound->Title;

	if ((m_Flags&RV_FLAGS_CHILD) && m_Parent)
    {
		VirusFound->pVirusFoundDlg = this;
		Create(IDD,CWnd::FromHandle(m_Parent));
		m_Modeless = TRUE;
	}
	else
    {
		if (AfxBeginThread((AFX_THREADPROC)VFViewThread,this))
        {
			VirusFound->pVirusFoundDlg = this;
			for (int i=0;!m_bInitialized&&i<40;i++)
				Sleep(250);
		}
		else
			cc = RV_ERROR_NO_THREAD;
	}

	return cc;
}

BOOL CVirusFound::OnInitDialog() 
{
	HANDLE hAccessToken = 0;

	CDialog::OnInitDialog();
	
	SetWindowText(m_Title);

	m_Status.Create(this);
	UINT ind[2] = {0,0};
	m_Status.SetIndicators(ind,2);

	m_Status.SetPaneInfo(0,0,SBPS_NORMAL,200);
	m_Status.SetPaneInfo(1,0,SBPS_NORMAL,200);
	RepositionBars(AFX_IDW_STATUS_BAR, AFX_IDW_STATUS_BAR, 0);

	if (m_Parent) 
	{
		::SetParent( m_hWnd, m_Parent );

		if (m_Flags&RV_FLAGS_CHILD)
		{
			ASSERT(FALSE);
			DWORD style = ::GetWindowLong(m_hWnd,GWL_STYLE);
			style &= ~(WS_OVERLAPPEDWINDOW|WS_SIZEBOX|WS_POPUP);
			style |= WS_CHILD;
			SetWindowLong(m_hWnd,GWL_STYLE,style);
			RECT rect;	
			::GetClientRect(m_Parent,&rect);
			SetWindowPos(0,0,0,rect.right,rect.bottom,SWP_NOZORDER);
			UpdateWindow();

			::PostMessage( m_Parent, UWM_ADDCHILD, (WPARAM)m_hWnd, 0L );

		}
	}
	else {
		SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		SetParent(NULL);
		}

	//Create the toolbar
	m_toolBar.Create( this );
	m_toolBar.LoadToolBar( IDR_VIRUS_FOUND );

    // TCASHIN 06/03/2003   Ported CRT fix for Siebel Defect # 1-3S7H4 to 8.0
    // BALJIAN 03/22/2002.  Fix for 1-3S7H4.  Hide the help button if we
    //                      can't get the access token for the currently
    //                      logged on user (NT only).
    if (IsWinNT())
    {
        hAccessToken = GetAccessTokenForLoggedOnUser();
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
    // TCASHIN 06/03/2003   End CRT fix for Siebel Defect # 1-3S7H4 to 8.0
   
    m_toolBar.SetBarStyle(m_toolBar.GetBarStyle() &~CBRS_BORDER_TOP |	CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS);

	m_toolBar.ModifyStyle( 0, TBSTYLE_FLAT );
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	CurrentValue = 0;
	Total = 0;
	CurrentVirus = NULL;
	Head = NULL;

	m_bInitialized = TRUE;

	UpdateWindow();
	ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CVirusFound::OnDestroy() 
{
	// KAP - 1-A370J, 1-CBVAJ - 06/26/2002
	// KAP - indicate we are no longer ready to process any received messages.
	m_bInitialized = FALSE;

	CDialog::OnDestroy();
	
	if (hKey)
		RegCloseKey(hKey);

	PVIRUSFOUND cur = Head;
	Head = NULL;

	// KAP - 1-A370J, 1-CBVAJ - 06/26/2002
	// KAP - deadlock issue bug fix.  Do not call the following function, at this time, or deadlock, can occur.
	//		the Mutex used by ViewClosed may be locked by a Thread attempting to invoke AddVirus(...).  The thread
	//		invoking AddVirus(...) will be blocked in SendMessage(...) waiting for the messagehandler to finish processing
	//		this message handler (OnDestroy).
	//if( m_ViewClosed)
	//	m_ViewClosed(Context);

	//Context = NULL;

	while (cur) {
		PVIRUSFOUND last = cur;
		cur = cur->Next;
		delete last;
		}
}


DWORD CVirusFound::AddVirus(char * line, char * Description)
{
	// KAP - if we have not initialized, or we are exiting, dont try to post another message
	if ( FALSE == m_bInitialized )
		return 0;

	PVIRUSFOUND virus = NULL;
	try
	{
		virus = new VIRUSFOUND;
	}
	catch (std::bad_alloc &)
	{
		virus = NULL;
	}
	if (virus == NULL) 
		return RV_ERROR_NO_MEMORY;

	virus->LogLine = line;
	virus->Description = Description;

	SendMessage(UWM_ADD,0,(LPARAM)virus);

	return 0;
}



long CVirusFound::Set(UINT wParam,LPARAM lParam) 
{
	m_Text.SetWindowText( CurrentVirus->Description.c_str() );

	CString str;
 	str.Format( IDS_TOTAL_NOTIFICATIONS, Total );
	
	//EA 03/24/2000 setting status bar's font to a edit boxes font since it seems to have small
	//font so that when we have large font settings we will look ok
	m_Status.SetFont(m_Text.GetFont());
	//EA 03/24/2000
	m_Status.SetPaneText(0,str);

	str.Format( IDS_CURRENT_POS, CurrentValue );
	m_Status.SetPaneText(1,str);

	DWORD val = GetKenFlags();
	m_KenD.UpdateDialog( val, CurrentVirus->LogLine.c_str() );
	m_toolBar.SendMessage(TB_ENABLEBUTTON,IDC_NEXT,val&0xffff);
	m_toolBar.SendMessage(TB_ENABLEBUTTON,IDC_PREV,val>>1);
	return 0;
}


long CVirusFound::AddItem(UINT wParam,LPARAM lParam) 
{

	PVIRUSFOUND virus = (PVIRUSFOUND)lParam;

	virus->Prev = NULL;
	virus->Next = Head;
	Head = virus;
	if (virus->Next)
		virus->Next->Prev = virus;


	if (CurrentValue == 0) {
		CurrentVirus = virus;
		CurrentValue = 1;
		}

	Total++;

	SendMessage(UWM_SET,0,NULL);
	return 0;
}




void CVirusFound::OnDetails()
{
  m_KenD.ShowVirusDetails( GetKenFlags(), CurrentVirus->LogLine.c_str() );
}



void CVirusFound::OnNext()
{
	if (CurrentVirus->Prev) {
//		EnableWindow(GetDlgItem(hWnd,MMB_PREV),TRUE);
		CurrentVirus = CurrentVirus->Prev;
		CurrentValue++;
//		if (CurrentVirus->Prev == NULL)
//			EnableWindow(GetDlgItem(hWnd,MMB_NEXT),FALSE);
		SendMessage(UWM_SET,0,NULL);
		}
}


void CVirusFound::OnPrev()
{
	if (CurrentVirus->Next) {
		CurrentVirus = CurrentVirus->Next;
//		EnableWindow(GetDlgItem(hWnd,MMB_NEXT),TRUE);
		CurrentValue--;
//		if (cur->Next == NULL)
//			EnableWindow(GetDlgItem(hWnd,MMB_PREV),FALSE);
		SendMessage(UWM_SET,0,NULL);
		}
}

BEGIN_EVENTSINK_MAP(CVirusFound, CDialog)
    //{{AFX_EVENTSINK_MAP(CVirusFound)
	ON_EVENT(CVirusFound, IDC_KENS_DETAILS, 2 /* Previous */, OnPreviousKensDetails, VTS_NONE)
	ON_EVENT(CVirusFound, IDC_KENS_DETAILS, 3 /* Next */, OnNextKensDetails, VTS_NONE)
	ON_EVENT(CVirusFound, IDC_KENS_DETAILS, 1 /* DialogClosed */, OnDialogClosedKensDetails, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CVirusFound::OnPreviousKensDetails() 
{
	OnPrev();
}

void CVirusFound::OnNextKensDetails() 
{
	OnNext();
}

void CVirusFound::OnDialogClosedKensDetails() 
{
}

DWORD CVirusFound::GetKenFlags()
{
	DWORD ret=0;
	if (Total != CurrentValue)
		ret |= 1;
		
	if (CurrentValue > 1)
		ret |= 0x10000;

	return ret;
}


BOOL CVirusFound::OnTTT(UINT,NMHDR *pNMHDR,LRESULT *Result)  {

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
void CVirusFound::OnHelp() 
{
	AfxGetApp()->WinHelpInternal(CVirusFound::IDD);
}



