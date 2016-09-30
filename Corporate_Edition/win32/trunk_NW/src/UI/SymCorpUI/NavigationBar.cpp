#include "stdafx.h"
#include "NavigationBar.h"
#include "SymCorpUIStatusView.h"
#include "SymCorpUIScansView.h"
#include "MainFrm.h"
#include "UiUtil.h"
#include <list>
#include <windows.h>
#include <olectl.h>
#include <comdef.h>
#include <atlwin.h>


BEGIN_MESSAGE_MAP(CNavigationBar, CDialogBarEx)
    // Standard control command map entries
    ON_COMMAND(IDC_NAVBAR_STATUS, OnBtnStatus)
    ON_COMMAND(IDC_NAVBAR_SCAN, OnBtnScan)
    ON_COMMAND(IDC_NAVBAR_SETTINGS, OnBtnSettings)
    ON_COMMAND(IDC_NAVBAR_LIVEUPDATE, OnBtnLiveUpdate)
    ON_COMMAND(IDC_NAVBAR_QUARANTINE, OnBtnQuarantine)
    ON_COMMAND(IDC_NAVBAR_LOGS, OnBtnLogs)

    // Command UI mappings are required for elements to be enabled
    ON_UPDATE_COMMAND_UI(IDC_NAVBAR_STATUS, OnCommandUI)
    ON_UPDATE_COMMAND_UI(IDC_NAVBAR_SCAN, OnCommandUI)
    ON_UPDATE_COMMAND_UI(IDC_NAVBAR_SETTINGS, OnCommandUI)
    ON_UPDATE_COMMAND_UI(IDC_NAVBAR_LIVEUPDATE, OnCommandUI)
    ON_UPDATE_COMMAND_UI(IDC_NAVBAR_QUARANTINE, OnCommandUI)
    ON_UPDATE_COMMAND_UI(IDC_NAVBAR_LOGS, OnCommandUI)

    // Other
    ON_WM_CTLCOLOR()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


void CNavigationBar::OnCommandUI( CCmdUI* thisCommand )
{
    return;
}

void CNavigationBar::DoDataExchange( CDataExchange* pDX )
{
    DDX_Control(pDX, IDC_NAVBAR_STATUS, statusBtn);
    DDX_Control(pDX, IDC_NAVBAR_SCAN, scanBtn);
    DDX_Control(pDX, IDC_NAVBAR_SETTINGS, settingsBtn);
    DDX_Control(pDX, IDC_NAVBAR_LIVEUPDATE, liveUpdateBtn);
    DDX_Control(pDX, IDC_NAVBAR_QUARANTINE, quarantineBtn);
    DDX_Control(pDX, IDC_NAVBAR_LOGS, logsBtn);
    CDialogBarEx::DoDataExchange(pDX);
}

void CNavigationBar::OnPreInitDialogBar()
{
    // Create the brush for the background color.  Must be done before window is created
    // or we must invalidate the window to pickup this change, and then transparencies
    // are screwed up.
    backgroundBrush.CreateSolidBrush(COLOR_NAVBAR_BACKGROUND);
    // Load background bitmap
    backgroundImage.LoadFromResource(AfxGetResourceHandle(), IDB_NAVBAR_BACKGROUND);
}

void CNavigationBar::OnInitDialogBar()
{
    CButtonEx* buttons[] = {&statusBtn, &scanBtn, &settingsBtn, &quarantineBtn, &liveUpdateBtn, &logsBtn};
    CButtonEx* currButton = NULL;
    int currButtonNo = 0;

    // Setup buttons
    for (currButtonNo = 0; currButtonNo < sizeof(buttons)/sizeof(buttons[0]); currButtonNo++)
    {
        currButton = buttons[currButtonNo];

        currButton->LoadImages(IDR_NAVBAR_BUTTON_NORMAL, IDR_NAVBAR_BUTTON_NORMALSELECTED, IDR_NAVBAR_BUTTON_PUSHED, IDR_NAVBAR_BUTTON_ROLLOVER, CButtonEx::ScaleMode_StretchToFit);
        currButton->defaultFont.LoadFromString(IDS_NAVBAR_BUTTON_NORMAL_FONT);
        currButton->selectedFont.LoadFromString(IDS_NAVBAR_BUTTON_PUSHED_FONT);
        currButton->hoverFont = currButton->selectedFont;
        currButton->pressedFont = currButton->selectedFont;
        currButton->useSelectedFont = true;
        currButton->useHoverFont = true;
        currButton->usePressedFont = true;
        currButton->alignmentStop = 0.1;
        currButton->textPushDown = false;
        currButton->type = CButtonEx::ButtonType_RadioButton;
    }

    // Select Status button initially.
    statusBtn.SetCheck(BST_CHECKED);
    statusBtn.OnClicked();

    // For some reason, owner draw controls in a toolbar don't call DrawItem
    // on toolbar creation, but they draw normally otherwise.
    // No idea why.  Debug shows that parent window OnDrawItem correctly
    // tries to reflect the message to the control, but can't find it in
    // the map (???), and so the messages withers up and dies.  This is fixed
    // later...somehow.
    // Workaround for now is to force invalidate all owner draw controls here
    // so they draw.
    statusBtn.Invalidate();
    scanBtn.Invalidate();
    settingsBtn.Invalidate();
    quarantineBtn.Invalidate();
    liveUpdateBtn.Invalidate();
    logsBtn.Invalidate();
}

CNavigationBar::CNavigationBar()
{
    // Nothing for now
}

CNavigationBar::~CNavigationBar()
{
    // Nothing for now
}

void CNavigationBar::OnBtnStatus()
{
    CWnd*           activeWindow    = NULL;
    CMainFrame*     activeFrame     = NULL;

    activeWindow = AfxGetMainWnd();
    activeFrame = DYNAMIC_DOWNCAST(CMainFrame, activeWindow);
    SwitchViewInFrame(activeFrame, RUNTIME_CLASS(CSymCorpUIStatusView));
    activeFrame->GetHelpBar()->SetBackgroundImage(IDRHELPBAR_BACKGROUND_STATUS);
}

void CNavigationBar::OnBtnScan()
{
    CWnd*           activeWindow    = NULL;
    CMainFrame*     activeFrame     = NULL;

    activeWindow = AfxGetMainWnd();
    activeFrame = DYNAMIC_DOWNCAST(CMainFrame, activeWindow);
    SwitchViewInFrame(activeFrame, RUNTIME_CLASS(CSymCorpUIScansView));
    activeFrame->GetHelpBar()->SetBackgroundImage(IDRHELPBAR_BACKGROUND_SCAN);
}

void CNavigationBar::OnBtnSettings()
{
    CWnd*           activeWindow    = NULL;
    CMainFrame*     activeFrame     = NULL;

    activeWindow = AfxGetMainWnd();
    activeFrame = DYNAMIC_DOWNCAST(CMainFrame, activeWindow);
    activeFrame->GetHelpBar()->SetBackgroundImage(IDRHELPBAR_BACKGROUND_CHANGESETTINGS);
    MessageBox(_T("OnBtnSettings"), _T("Called"));
}

void CNavigationBar::OnBtnLiveUpdate()
{
    CWnd*           activeWindow    = NULL;
    CMainFrame*     activeFrame     = NULL;

    activeWindow = AfxGetMainWnd();
    activeFrame = DYNAMIC_DOWNCAST(CMainFrame, activeWindow);
    activeFrame->GetHelpBar()->SetBackgroundImage(IDRHELPBAR_BACKGROUND_LIVEUPDATE);
    MessageBox(_T("OnBtnLiveUpdate"), _T("Called"));
}

void CNavigationBar::OnBtnQuarantine()
{
    CWnd*           activeWindow    = NULL;
    CMainFrame*     activeFrame     = NULL;

    activeWindow = AfxGetMainWnd();
    activeFrame = DYNAMIC_DOWNCAST(CMainFrame, activeWindow);
    activeFrame->GetHelpBar()->SetBackgroundImage(IDRHELPBAR_BACKGROUND_VIEWQUARANTINE);
    MessageBox(_T("OnBtnQuarantine"), _T("Called"));
}

void CNavigationBar::OnBtnLogs()
{
    CWnd*           activeWindow    = NULL;
    CMainFrame*     activeFrame     = NULL;

    activeWindow = AfxGetMainWnd();
    activeFrame = DYNAMIC_DOWNCAST(CMainFrame, activeWindow);
    activeFrame->GetHelpBar()->SetBackgroundImage(IDRHELPBAR_BACKGROUND_VIEWLOGS);
    MessageBox(_T("OnBtnLogs"), _T("Called"));
}

HBRUSH CNavigationBar::OnCtlColor( CDC* drawDC, CWnd* thisWindow, UINT controlCode )
{
    // Specify dialog background color to use
    switch (controlCode)
    {
        // NOTE:  if BTN and STATIC support are removed, then at startup a grey box is
        // displayed for ViewLogs button background initially, so it's transparency support
        // is messed up.  Why??
    case CTLCOLOR_BTN:
    case CTLCOLOR_STATIC:
        drawDC->SetBkMode(TRANSPARENT);
        return static_cast<HBRUSH>(backgroundBrush.GetSafeHandle());
    case CTLCOLOR_DLG:
        return static_cast<HBRUSH>(backgroundBrush.GetSafeHandle());
    }

    return CDialogBarEx::OnCtlColor(drawDC, thisWindow, controlCode);
}

BOOL CNavigationBar::OnEraseBkgnd( CDC* paintDC )
{
    CRect originalSize;
	CRect clientRect;
    CRect scaledSize;

    GetClientRect(clientRect);
    originalSize.top    = 0;
    originalSize.left   = 0;
    originalSize.right  = backgroundImage.GetWidth();
    originalSize.bottom = backgroundImage.GetHeight();
    ScaleSizeFitKeepAspectRatio(originalSize, clientRect, &scaledSize);
//    backgroundImage.Draw(paintDC->GetSafeHdc(), 0, 0, scaledSize.Width(), scaledSize.Height());
    backgroundImage.Draw(paintDC->GetSafeHdc(), 0, 0);
    // Fill remainder with background brush?
    return TRUE;
}