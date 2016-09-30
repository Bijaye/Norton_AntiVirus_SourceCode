#include "stdafx.h"
#include "HelpBar.h"
#include <list>
#include <windows.h>
#include <olectl.h>
#include <comdef.h>
#include <atlwin.h>


BEGIN_MESSAGE_MAP(CHelpBar, CDialogBarEx)
    // Standard control command map entries
    ON_COMMAND(IDC_HELPBAR_GO, OnBtnGo)
    ON_COMMAND(IDC_HELPBAR_HELPANDSUPPORT, OnBtnHelpAndSupport)
    ON_COMMAND(IDC_HELPBAR_QUESTION, OnQuestion)

    // Command UI mappings are required for elements to be enabled
    ON_UPDATE_COMMAND_UI(IDC_HELPBAR_GO, OnCommandUI)
    ON_UPDATE_COMMAND_UI(IDC_HELPBAR_HELPANDSUPPORT, OnCommandUI)

    // Other
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void CHelpBar::SetBackgroundImage( UINT backgroundID )
{
    if (static_cast<HBITMAP>(backgroundBitmap) != NULL)
        backgroundBitmap.Detach();
    LoadImageResource(AfxGetResourceHandle(), backgroundID, _T("BINARY"), &backgroundBitmap);
    Invalidate();
}

void CHelpBar::OnCommandUI( CCmdUI* thisCommand )
{
    return;
}

void CHelpBar::DoDataExchange( CDataExchange* pDX )
{
    DDX_Control(pDX, IDC_HELPBAR_QUESTION, helpQuestionCtrl);
    DDX_Control(pDX, IDC_HELPBAR_GO, goBtn);
    DDX_Control(pDX, IDC_HELPBAR_HELPANDSUPPORT, helpAndSupportBtn);
    CDialogBarEx::DoDataExchange(pDX);
}

void CHelpBar::OnPreInitDialogBar()
{
    // Create the brush for the background color.  Must be done before window is created
    // or we must invalidate the window to pickup this change, and then transparencies
    // are screwed up.
    backgroundBrush.CreateSolidBrush(COLOR_NAVBAR_BACKGROUND);
    helpQuestionCtrl.SetBorderColor(RGB(0,0,0));

    // Load the default background bitmap
    SetBackgroundImage(IDRHELPBAR_BACKGROUND_STATUS);
}

void CHelpBar::OnInitDialogBar()
{
    CButtonEx*      buttons[]           = {&goBtn, &helpAndSupportBtn};
    CButtonEx*      currButton          = NULL;
    int             currButtonNo        = 0;

    helpQuestionCtrl.SetWindowTextW(_T("Type a question for help"));
    helpQuestionCtrl.SetBackgroundColor(COLOR_WHITE);
    helpQuestionCtrl.displayFont.useDefaultColor = false;
    helpQuestionCtrl.displayFont.textColor = RGB(113,112,128);

    // Setup buttons
    for (currButtonNo = 0; currButtonNo < sizeof(buttons)/sizeof(buttons[0]); currButtonNo++)
    {
        currButton = buttons[currButtonNo];

        currButton->defaultFont.fontSize = 1;
        currButton->defaultFont.useDefaultColor = true;
    }

    goBtn.LoadImages(IDR_HELPBAR_GOBUTTON_NORMAL, IDR_HELPBAR_GOBUTTON_NORMAL, IDR_HELPBAR_GOBUTTON_NORMAL, IDR_HELPBAR_GOBUTTON_NORMAL, CButtonEx::ScaleMode_StretchToFit);
    helpAndSupportBtn.LoadImages(IDR_HELPBAR_HELPSUPPORT_NORMAL, IDR_HELPBAR_HELPSUPPORT_NORMAL, IDR_HELPBAR_HELPSUPPORT_NORMAL, IDR_HELPBAR_HELPSUPPORT_NORMAL, CButtonEx::ScaleMode_StretchToFit);

    // For some reason, owner draw controls in a toolbar don't call DrawItem
    // on toolbar creation, but they drawn normally otherwise.
    // No idea why.  Debug shows that parent window OnDrawItem correctly
    // tries to reflect the message to the control, but can't find it in
    // the map (???), and so the messages withers up and dies.  This is fixed
    // later...somehow.
    // Workaround for now is to force invalidate all owner draw controls here
    // so they draw.
    goBtn.Invalidate();
    helpAndSupportBtn.Invalidate();
    helpQuestionCtrl.Invalidate();
}

CHelpBar::CHelpBar()
{
    // Nothing for now
}

CHelpBar::~CHelpBar()
{
    // Nothing for now
}

void CHelpBar::OnBtnGo()
{
    OnQuestion();
}

void CHelpBar::OnBtnHelpAndSupport()
{
    CMenu helpMenu;
    CRect controlScreenRect;

    helpAndSupportBtn.GetWindowRect(controlScreenRect);
    helpMenu.LoadMenu(IDR_HELPBAR_HELP);
    helpMenu.GetSubMenu(0)->TrackPopupMenu(TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_CENTERALIGN, controlScreenRect.CenterPoint().x, controlScreenRect.CenterPoint().y, this);
}

void CHelpBar::OnQuestion()
{
    MessageBox(_T("Pending help implementation decision by InfoDev and possibly CHM merge research."), _T("Search for Help"));
}

BOOL CHelpBar::OnEraseBkgnd( CDC* paintDC )
{
    CDC             memoryDC;
    CRect           clientRect;
    CRect           fillRect;
    HBITMAP         oldBitmap           = NULL;
    DWORD           returnValDW         = ERROR_OUT_OF_PAPER;

    // Initialize
    memoryDC.CreateCompatibleDC(paintDC);
    GetClientRect(&clientRect);

    // Fill the remainder with the background color
    fillRect = clientRect;
    paintDC->FillRect(fillRect, &backgroundBrush);

    // Get bitmap size
    if (static_cast<HBITMAP>(backgroundBitmap) != NULL)
    {
        backgroundBitmap.Draw(paintDC->GetSafeHdc(), 0, 0, clientRect.Width(), clientRect.Height());
//        oldBitmap = (HBITMAP) memoryDC.SelectObject((HGDIOBJ) static_cast<HBITMAP>(backgroundBitmap));
//        paintDC->BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memoryDC, 0, 0, SRCCOPY);
//        memoryDC.SelectObject(oldBitmap);
    }
    return TRUE;
}

HBRUSH CHelpBar::OnCtlColor( CDC* drawDC, CWnd* thisWindow, UINT controlCode )
{
    // Specify background color to use
    switch (controlCode)
    {
    case CTLCOLOR_BTN:
        drawDC->SetBkMode(TRANSPARENT);
        return static_cast<HBRUSH>(backgroundBrush.GetSafeHandle());

    case CTLCOLOR_DLG:
        return static_cast<HBRUSH>(backgroundBrush.GetSafeHandle());
    }

    return CDialogBarEx::OnCtlColor(drawDC, thisWindow, controlCode);
}

void CHelpBar::OnPaint()
{
    CDialogBar::OnPaint();
}
