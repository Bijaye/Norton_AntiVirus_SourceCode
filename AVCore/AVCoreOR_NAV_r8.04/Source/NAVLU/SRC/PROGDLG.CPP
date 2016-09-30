// Copyright 1996-1998 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/progdlg.cpv   1.2   28 Aug 1998 16:22:34   JBRENNA  $
//
// Description: Patching progress dialog.
//
// Contains:    CPatchProgressDlg class implementation
//
// See Also:
//****************************************************************************
// $Log:   S:/NAVLU/VCS/progdlg.cpv  $
// 
//    Rev 1.2   28 Aug 1998 16:22:34   JBRENNA
// Port changes from QAKN branch to trunk.
// 
//    Rev 1.1.1.1   20 Jul 1998 15:13:54   tcashin
// Tweaked the size and position of the patch progress dialog.
// 
//    Rev 1.1.1.0   22 Jun 1998 19:51:26   RFULLER
// Branch base for version QAKN
// 
//    Rev 1.1   26 May 1998 12:11:54   tcashin
// Center the progress dialog on the desktop to be consistant with the
// other NAVLU dialog boxes.
// 
//    Rev 1.0   20 May 1998 16:46:00   tcashin
// Initial revision.
// 
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ProgDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatchProgressDlg dialog

CPatchProgressDlg::CPatchProgressDlg(UINT nCaptionID)
{
	m_nCaptionID = CG_IDS_PROGRESS_CAPTION;
	if (nCaptionID != 0)
		m_nCaptionID = nCaptionID;

    m_bCancel=FALSE;
    m_nLower=0;
    m_nUpper=100;
    m_nStep=1;
    //{{AFX_DATA_INIT(CPatchProgressDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    m_bParentDisabled = FALSE;
}

CPatchProgressDlg::~CPatchProgressDlg()
{
    if(m_hWnd!=NULL)
      DestroyWindow();
}

BOOL CPatchProgressDlg::DestroyWindow()
{
    ReEnableParent();
    return CDialog::DestroyWindow();
}

void CPatchProgressDlg::ReEnableParent()
{
    if(m_bParentDisabled && (m_pParentWnd!=NULL))
      m_pParentWnd->EnableWindow(TRUE);
    m_bParentDisabled=FALSE;
}

BOOL CPatchProgressDlg::Create(CWnd *pParent)
{
    // Get the true parent of the dialog
    // m_pParentWnd = CWnd::GetSafeOwner(pParent);
    m_pParentWnd = pParent;

    // m_bParentDisabled is used to re-enable the parent window
    // when the dialog is destroyed. So we don't want to set
    // it to TRUE unless the parent was already enabled.

    if((m_pParentWnd!=NULL) && m_pParentWnd->IsWindowEnabled())
    {
      m_pParentWnd->EnableWindow(FALSE);
      m_bParentDisabled = TRUE;
    }

    if(!CDialog::Create(CPatchProgressDlg::IDD,pParent))
    {
      ReEnableParent();
      return FALSE;
    }

    return TRUE;
}

void CPatchProgressDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPatchProgressDlg)
    DDX_Control(pDX, CG_IDC_PROGDLG_PROGRESS, m_Progress);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPatchProgressDlg, CDialog)
    //{{AFX_MSG_MAP(CPatchProgressDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CPatchProgressDlg::OnCancel()
{
    m_bCancel=TRUE;
}

void CPatchProgressDlg::SetRange(int nLower,int nUpper)
{
    m_nLower = nLower;
    m_nUpper = nUpper;
    m_Progress.SetRange(nLower,nUpper);
}
  
int CPatchProgressDlg::SetPos(int nPos)
{
    PumpMessages();
    int iResult = m_Progress.SetPos(nPos);
    UpdatePercent(nPos);
    return iResult;
}

int CPatchProgressDlg::SetStep(int nStep)
{
    m_nStep = nStep; // Store for later use in calculating percentage
    return m_Progress.SetStep(nStep);
}

int CPatchProgressDlg::OffsetPos(int nPos)
{
    PumpMessages();
    int iResult = m_Progress.OffsetPos(nPos);
    UpdatePercent(iResult+nPos);
    return iResult;
}

int CPatchProgressDlg::StepIt()
{
    PumpMessages();
    int iResult = m_Progress.StepIt();
    UpdatePercent(iResult+m_nStep);
    return iResult;
}

void CPatchProgressDlg::PumpMessages()
{
    // Must call Create() before using the dialog
    ASSERT(m_hWnd!=NULL);

    MSG msg;
    // Handle dialog messages
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(!IsDialogMessage(&msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  
      }
    }
}

BOOL CPatchProgressDlg::CheckCancelButton()
{
    // Process all pending messages
    PumpMessages();

    // Reset m_bCancel to FALSE so that
    // CheckCancelButton returns FALSE until the user
    // clicks Cancel again. This will allow you to call
    // CheckCancelButton and still continue the operation.
    // If m_bCancel stayed TRUE, then the next call to
    // CheckCancelButton would always return TRUE

    BOOL bResult = m_bCancel;
    m_bCancel = FALSE;

    return bResult;
}

void CPatchProgressDlg::UpdatePercent(int nNewPos)
{
    CWnd *pWndPercent = GetDlgItem(CG_IDC_PROGDLG_PERCENT);
    int nPercent;
    
    int nDivisor = m_nUpper - m_nLower;
    ASSERT(nDivisor>0);  // m_nLower should be smaller than m_nUpper

    int nDividend = (nNewPos - m_nLower);
    ASSERT(nDividend>=0);   // Current position should be greater than m_nLower

    nPercent = nDividend * 100 / nDivisor;

    // Since the Progress Control wraps, we will wrap the percentage
    // along with it. However, don't reset 100% back to 0%
    if(nPercent!=100)
      nPercent %= 100;

    // Display the percentage
    CString strBuf;
    strBuf.Format(_T("%d%c"),nPercent,_T('%'));

	CString strCur; // get current percentage
    pWndPercent->GetWindowText(strCur);

	if (strCur != strBuf)
		pWndPercent->SetWindowText(strBuf);
}
    
/////////////////////////////////////////////////////////////////////////////
// CPatchProgressDlg message handlers

BOOL CPatchProgressDlg::OnInitDialog() 
{
    auto    WINDOWPLACEMENT     stWindowPlacement;

    CDialog::OnInitDialog();
    m_Progress.SetRange(m_nLower,m_nUpper);
    m_Progress.SetStep(m_nStep);
    m_Progress.SetPos(m_nLower);

	CString strCaption;
	VERIFY(strCaption.LoadString(m_nCaptionID));
    SetWindowText(strCaption);
    CenterWindow();

    // Move the dialog to the left and down to cover up some of LiveUpdate 1.4
    // wizard panel that says some confusing stuff. 
    GetWindowPlacement( &stWindowPlacement );

    stWindowPlacement.rcNormalPosition.left   -= 30;
    stWindowPlacement.rcNormalPosition.right  -= 30;
    stWindowPlacement.rcNormalPosition.top    += 65;
    stWindowPlacement.rcNormalPosition.bottom += 65;

    SetWindowPlacement( &stWindowPlacement );

    return TRUE;  
}
